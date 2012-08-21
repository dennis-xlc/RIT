#!/usr/bin/perl -w
#===============================================================================
#
#         FILE:  run_on_NB.pl
#
#  DESCRIPTION:  
#
#        INPUT:  
#       OUTPUT:  
#       AUTHOR:  
#      COMPANY:  
#      VERSION:  
#      CREATED:  
#===============================================================================

use strict;
use warnings;
use File::Basename;
use File::Copy;
use File::Path;
use Getopt::Long;
use Time::HiRes qw(time);
use Sys::Hostname;
use Cwd;

# Remove line buffering from STDOUT
select(STDERR); $| = 1;
select(STDOUT); $| = 1;

#-----------------------------------------------------------------------------------------------------------------------
# Main Routine
#-----------------------------------------------------------------------------------------------------------------------

## Global variables

## Local variables

my $RIT_SVN = 'https://subversion.jf.intel.com/ssg/dpd/epl/ubt/cms7/mainline/branches/ubt-arm/dev-tools/rit';
my $UBT_CFG_SVN = 'https://sh-svn.sh.intel.com/ssg_repos/svn_Houdini/Houdini/mainline/trunk/src/scripts/ubt_config';
my $DB_DIR = '/nfs/shm/proj/epl/ubtusrs/SternGroveValidation/dailybuild/dailybuild_ubt-arm_binary_mainline';
my $DB_VER = undef;
my $PYBIN_VER = 'python2.7';
my $SCRIPT_DIR = &AbsPath(&dirname($0));
my $RIT_DIR = "$SCRIPT_DIR/..";
my $UBT_DIR = "$RIT_DIR/../ubt";
my $RES_POOL = "$RIT_DIR/../SeedRes";
my $GEN_RUN_SCPT = "$SCRIPT_DIR/gen_run_seeds.pl";
my $ARM_TOOL_BIN = "$RIT_DIR/../arm-2011.03/bin";
my $PY_PROC_SEEDS = "$SCRIPT_DIR/process_seeds.py";
my $PY_RIT_COV = "$SCRIPT_DIR/rit_cov.py";
my $PY_ART = "$RIT_DIR/art/art.py";
my $FAIL_DIR = "$RIT_DIR/../fail_seeds";
my $SEED_NUM = undef;
my $EXE_FLAG = undef;
my $COV_FLAG = undef;
my $VERI_FLAG = undef;
my $EXE_OPT = undef;
my $COV_OPT = undef;
my $VERI_OPT = undef;
my $UNIT_NUM = 1000;
my $UBT_BIN = undef;
my $RENEW_RIT = undef;
my $SEED_POOL = '/tmp/localdisk/SG_RIT_TMP';
my $NB_LOG_DIR = "$RIT_DIR/../NB_LOG";
my $NB_CMD_PRE = "nbq --log-file-dir $NB_LOG_DIR --priority 15";
my $UBT_NO_CFG = 'NO_CFG';
my @UBT_TEST_NAME= qw/intrp                         tia                     rel_tia                 cosim/;
my @UBT_BIN_NAME = qw/cms.elf.debug                 cms.elf.debug           cms.elf.release         cms.elf.debug/;
my @UBT_CFG_NAME = qw/ubt.config.always.interpreter ubt.config.always.TIA   ubt.config.always.TIA   ubt.config.always.TIA.cosim/;
my $UNIF_BIN_NAME = 'cms.elf';
my $UNIF_CFG_NAME = 'ubt.config';
my $SEED_START_NUM = undef;
my $SEED_START_OPT = '';
my $ISA_OPTS = undef;
my $FAIL_OPT = ''; 

#-----------------------------------------------------------------------------------------------------------------------
# Main Body
#-----------------------------------------------------------------------------------------------------------------------
# Get options
GetOptions( "arm=s" => \$ARM_TOOL_BIN,
            "seed=i" => \$SEED_NUM,
            "out=s" => \$RES_POOL,
			"fail=s" => \$FAIL_DIR,
            "bin=s" => \$UBT_BIN,
            "ren!" => \$RENEW_RIT,
            "unit=i" => \$UNIT_NUM,
            "exe!" => \$EXE_FLAG,
            "dir=s" => \$SEED_POOL,
            "veri!" => \$VERI_FLAG,
            "isa=s" => \$ISA_OPTS,
            "znum=i" => \$SEED_START_NUM,
            "cov!" => \$COV_FLAG
			 )
			or die "Fail to get options!\n";

if ( not defined($SEED_NUM)
    or $SEED_NUM <= 0
    or $UNIT_NUM <= 0 ) {
	warn "Lack of options!
Usage: run_on_NB.pl [Options]
    -a  <Default=RIT_DIR/../arm-2011.03/bin> the bin dir path of ARM tool chain
    -b  provide uBT binary, otherwise test recent bin from dailybuild
    -c  dump the coverage file
    -d  the pool dir to store seeds and log files
    -e  test seeds imm after generated
    -f  <Default=RIT_DIR/../fail_seeds> the dir to contain failed seeds
    -i  do NOT contain ISA sets in seeds, e.g. [-i thumb2,ARMv7,VFP,excl,aluwpc]
    -o  the pool dir to store resluts
    -r  update rit scripts and re-compile librit binary
    -s  <Necessary> total number of seeds to be generated
    -u  <Default=1000> unit number of seeds in one NetBatch task
    -v  verify failed seeds with qemu
    -z  Specify the seed start number
    \n";
	exit 0;
}

if ( defined($EXE_FLAG) and not defined($UBT_BIN) ) {
    my ($SEC,$MIN,$HOUR,$MDAY,$MON,$YEAR,$WDAY,$YDAY,$ISDST) = localtime(time);
    $YEAR += 1900;
    $MON += 1;
    $MON = "0$MON" if ( $MON < 10 );
    while ($MDAY > 0) {
        my $XDAY = ($MDAY < 10)?"0$MDAY":"$MDAY";
        $DB_VER = 'binary_'.$YEAR.$MON.$XDAY;
        last if ( -e "$DB_DIR/$DB_VER/$UNIF_BIN_NAME.debug" and -e "$DB_DIR/$DB_VER/$UNIF_BIN_NAME.release");
        $MDAY --;
    }
die "Not find ubt for this month!\n" if ( not -e "$DB_DIR/$DB_VER/$UNIF_BIN_NAME.debug" or not -e "$DB_DIR/$DB_VER/$UNIF_BIN_NAME.release");
}

die "Not find ubt bin!\n" if ( not defined($UBT_BIN) and not defined($DB_VER));

if (defined($EXE_FLAG)) {
	mkpath($FAIL_DIR);
	$FAIL_DIR = &AbsPath($FAIL_DIR);
	$FAIL_OPT = "-f $FAIL_DIR";
}
rmtree($RES_POOL) if ( -d "$RES_POOL" );
mkpath($RES_POOL);
$RES_POOL = &AbsPath($RES_POOL);
rmtree($NB_LOG_DIR) if ( -d "$NB_LOG_DIR" );
mkpath($NB_LOG_DIR);
open (LOG,">$RES_POOL/TestBeg.log") or die "Fail to open log file <$RES_POOL/TestBeg.log>";
print LOG "Test begin @".localtime(),"\n";

if ( defined($RENEW_RIT) ) {
    print LOG "Updating rit src @".localtime(),"\n";
    warn "Updating rit src\n";
    if ( -d "$RIT_DIR/.svn" ) {
        `svn revert $RIT_DIR -R`;
        `svn up $RIT_DIR`;
    } else {
        print LOG "Fail to update rit src\n";
        close(LOG);
        die "No SVN info in dir <$RIT_DIR>\n";
    }
}

print LOG "Compile rit lib @".localtime(),"\n";
unlink "$RIT_DIR/bin/librit.so" if ( -e "$RIT_DIR/bin/librit.so" );
my $CWD = getcwd();
warn "Compiling rit lib\n";
chdir("$RIT_DIR/src");
`make librit.so`;
chdir($CWD);
if ( -e "$RIT_DIR/src/librit.so" ) {
    move "$RIT_DIR/src/librit.so", "$RIT_DIR/bin/librit.so";
} else {
    print LOG "Fail to make rit lib\n";
    close(LOG);
    die "Fail to make librit.so\n";
}

$ARM_TOOL_BIN = &AbsPath($ARM_TOOL_BIN);
die "Not find ARM tools dir @<$ARM_TOOL_BIN>\n" if ( not -d $ARM_TOOL_BIN );
die "Not find tool <$GEN_RUN_SCPT>\n" if ( not -e $GEN_RUN_SCPT );
die "Not find tool <$PY_ART>\n" if ( not -e $PY_ART );
die "Not find tool <$PY_PROC_SEEDS>\n" if ( defined($EXE_FLAG) and not -e $PY_PROC_SEEDS );
die "Not find tool <$PY_RIT_COV>\n" if ( defined($COV_FLAG) and not -e $PY_RIT_COV );

if (defined($EXE_FLAG)) {
    if (defined($UBT_BIN)) {
		warn "Copying ubt bin from <$UBT_BIN>\n";
		print LOG "Copying ubt bin from <$UBT_BIN>\n";
    } elsif ( defined($DB_VER) ) {
		warn "Copying ubt bin from dailybuild <$DB_VER>\n";
		print LOG "Copying ubt bin from dailybuild <$DB_VER>\n";
	} else {
        die "No ubt bin found!\n";
	}
	for (my $i=0; $i < @UBT_TEST_NAME; $i++) {
        next if ($UBT_TEST_NAME[$i] =~ /rel_tia/i and defined($UBT_BIN));
		my $CUR_DIR = "$UBT_DIR/$UBT_TEST_NAME[$i]";
		rmtree("$CUR_DIR") if ( -d "$CUR_DIR" );
		mkpath("$CUR_DIR");
        if (defined($UBT_BIN)) {
            copy $UBT_BIN, "$CUR_DIR/$UNIF_BIN_NAME";
        } else {
            copy "$DB_DIR/$DB_VER/$UBT_BIN_NAME[$i]", "$CUR_DIR/$UNIF_BIN_NAME";
        }
		`chmod 775 $CUR_DIR/$UNIF_BIN_NAME`;
		if ( $UBT_CFG_NAME[$i] ne $UBT_NO_CFG ) {
			`svn cat $UBT_CFG_SVN/$UBT_CFG_NAME[$i] > $CUR_DIR/$UNIF_CFG_NAME`;
			warn "Check out ubt cfg <$UBT_CFG_NAME[$i]>\n";
			print LOG "Check out ubt cfg <$UBT_CFG_NAME[$i]>\n";
		}
	}
}

$EXE_OPT = (defined($EXE_FLAG))?'-e':'';
$COV_OPT = (defined($COV_FLAG))?'-c':'';
$VERI_OPT = (defined($VERI_FLAG))?'-v':'';
$ISA_OPTS = (defined($ISA_OPTS))?"-i $ISA_OPTS":'';
print LOG "Submit NetBatch tasks @".localtime(),"\n\n";
warn "Submitting NetBatch tasks\n";
while ( $SEED_NUM >= $UNIT_NUM ) {
    if ( defined($SEED_START_NUM)) {
        $SEED_START_OPT = " -z $SEED_START_NUM ";
        $SEED_START_NUM += $UNIT_NUM;
    }
    my $NB_CMD = "$NB_CMD_PRE perl $GEN_RUN_SCPT -a $ARM_TOOL_BIN -d $SEED_POOL $EXE_OPT $COV_OPT $VERI_OPT $SEED_START_OPT $ISA_OPTS -o $RES_POOL -s $UNIT_NUM $FAIL_OPT -r";
    print LOG "[*NB_CMD*]: $NB_CMD\n";
    my $NB_RET = `$NB_CMD`;
    print LOG "[*NB_RET*]: $NB_RET\n";
    $SEED_NUM -= $UNIT_NUM;
}

if ( $SEED_NUM > 0 ) {
    $SEED_START_OPT = " -z $SEED_START_NUM " if ( defined($SEED_START_NUM));
    my $NB_CMD = "$NB_CMD_PRE perl $GEN_RUN_SCPT -a $ARM_TOOL_BIN -d $SEED_POOL $EXE_OPT $COV_OPT $VERI_OPT $SEED_START_OPT $ISA_OPTS -o $RES_POOL -s $SEED_NUM $FAIL_OPT -r";
    print LOG "[*NB_CMD*]: $NB_CMD\n";
    my $NB_RET = `$NB_CMD`;
    print LOG "[*NB_RET*]: $NB_RET\n";
}

print LOG "Self exit @".localtime(),"\n";
close(LOG);
exit 0;

#-----------------------------------------------------------------------------------------------------------------------

sub AbsPath {
    my $CUR_PH = shift;
    chomp($CUR_PH);

    my $CWD = getcwd();
    chdir($CUR_PH);
    $CUR_PH = getcwd();
    chdir($CWD);
    return $CUR_PH;
}

#-----------------------------------------------------------------------------------------------------------------------
# End of Subroutines
#-----------------------------------------------------------------------------------------------------------------------
