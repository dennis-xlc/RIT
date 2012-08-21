#!/usr/bin/perl -w
#===============================================================================
#
#         FILE:  local_verify.pl
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

my $RIT_BIN_DIR = &dirname($0).'/../bin';
my $RIT_SCPT_DIR = &dirname($0).'/../script';
my $PL_ARM = "$RIT_SCPT_DIR/run_on_ARM.pl";
my $PL_UBT = "$RIT_SCPT_DIR/run_on_UBT.pl";
my $SEED_DIR = undef;
my $RES_DIR = undef;
my $SCAN_SPAN = 2;
my $LOG_ARM_PREFIX = 'ARM_nat';
my $LOG_COSIM_PREFIX = 'LOC_cosim';
my $LOG_INTRP_PREFIX = 'LOC_intrp';
my $LOG_REL_TIA_PREFIX = 'LOC_rel_tia';
my $LOG_TIA_PREFIX = 'LOC_tia';
my $SERIAL_NUM = undef;
my $ARM_PASS_DIR = 'pass_on_arm';
my $ARM_FAIL_DIR = 'fail_on_arm';
my $AMR_LOG = $LOG_ARM_PREFIX.'_res.log';
my $COSIM_LOG = $LOG_COSIM_PREFIX.'_res.log';
my $INTRP_LOG = $LOG_INTRP_PREFIX.'_res.log';
my $REL_TIA_LOG = $LOG_REL_TIA_PREFIX.'_res.log';
my $TIA_LOG = $LOG_TIA_PREFIX.'_res.log';
my $UBT_BIN = 'cms.elf';
my $UBT_CFG = 'ubt.config';
my $TMP_RES_DIR = 'TMP_RES';
my $UBT_DIR = undef;
my $CUR_UNIT_DIR = undef;
my $TIME_SPAN = 1800;
my $UNIT_MAX = 100;
my $SPAN_CNT = 0;
my $UNIT_CNT = 0;
my $UNIT_NUM = 0;
my $REL_TIA_FLAG = undef;

#-----------------------------------------------------------------------------------------------------------------------
# Main Body
#-----------------------------------------------------------------------------------------------------------------------
# Get options
GetOptions( "out=s" => \$RES_DIR,
            "dir=s" => \$SEED_DIR,
            "snd=s" => \$SERIAL_NUM,
            "bin=s" => \$UBT_DIR,
            "time=i" => \$SCAN_SPAN,
			 )
			or die "Fail to get options!\n";

if ( not defined($SEED_DIR)
    or not defined($UBT_DIR) ) {
	warn "Lack of options!
Usage: local_verify.pl [Options]
    -b  <Necessary> the dir containing dir-s within ubt bin-s and config-s for tia and cosim tests
    -d  <Necessary> the dir containing failed seeds
    -o  the dir to store result log
    -s  the serial number of the device
    -t  <Default=2> the time span to scan fail_seeds dir (in sec)
    \n";
	exit 0;
}

my $UBT_COSIM = "$UBT_DIR/cosim";
my $UBT_INTRP = "$UBT_DIR/intrp";
my $UBT_REL_TIA = "$UBT_DIR/rel_tia";
my $UBT_TIA = "$UBT_DIR/tia";
die "Can not find ubt bin and config files in $UBT_DIR/tia, intrp or cosim\n"
if (not -e "$UBT_COSIM/$UBT_BIN"
    or not -e "$UBT_COSIM/$UBT_CFG"
    or not -e "$UBT_INTRP/$UBT_BIN"
    or not -e "$UBT_INTRP/$UBT_CFG"
    or not -e "$UBT_TIA/$UBT_BIN"
    or not -e "$UBT_TIA/$UBT_CFG");


if (-d $UBT_REL_TIA) {
    die "Can not find ubt bin and config in $UBT_REL_TIA\n" if (not -e "$UBT_REL_TIA/$UBT_BIN" or not -e "$UBT_REL_TIA/$UBT_CFG");
    $REL_TIA_FLAG = 1;
}

die "Can not find $PL_ARM\n" if (not -e $PL_ARM);
die "Can not find $PL_UBT\n" if (not -e $PL_UBT);
my $SN_OPT = (defined($SERIAL_NUM))?"-s $SERIAL_NUM":'';
$RES_DIR = $SEED_DIR if (not defined($RES_DIR));
mkpath ("$RES_DIR/$ARM_PASS_DIR");
mkpath ("$RES_DIR/$ARM_FAIL_DIR");
mkpath ("$RES_DIR/$TMP_RES_DIR");
my $VER_DIR = "$RES_DIR/verifying";
my $lt = localtime();
#`echo ~~~~ $lt ~~~~ > $RES_DIR/$AMR_LOG`;
#`echo ~~~~ $lt ~~~~ > $RES_DIR/$COSIM_LOG`;
#`echo ~~~~ $lt ~~~~ > $RES_DIR/$TIA_LOG`;

while (1) {
    foreach my $RES_FILE (glob "$RES_DIR/$TMP_RES_DIR/*") {
        warn "Merging result files!\n";
        `cat $RES_FILE >> $RES_DIR/$AMR_LOG` if ($RES_FILE =~ /$LOG_ARM_PREFIX/i);
        `cat $RES_FILE >> $RES_DIR/$COSIM_LOG` if ($RES_FILE =~ /$LOG_COSIM_PREFIX/i);
        `cat $RES_FILE >> $RES_DIR/$INTRP_LOG` if ($RES_FILE =~ /$LOG_INTRP_PREFIX/i);
        `cat $RES_FILE >> $RES_DIR/$REL_TIA_LOG` if ($RES_FILE =~ /$LOG_REL_TIA_PREFIX/i);
        `cat $RES_FILE >> $RES_DIR/$TIA_LOG` if ($RES_FILE =~ /$LOG_TIA_PREFIX/i);
        unlink $RES_FILE;
    }

    $CUR_UNIT_DIR = $VER_DIR.'_'.$UNIT_NUM;
    foreach my $F_DIR (glob "$SEED_DIR/*") {
        next if (not -d $F_DIR);
        next if (not -e "$F_DIR/Done");
        while (glob "$F_DIR/*.bin") {
            mkpath($CUR_UNIT_DIR) if (not -d $CUR_UNIT_DIR);
            goto SBM_TASK if ($UNIT_CNT >= $UNIT_MAX);
            move $_, "$CUR_UNIT_DIR/";
            $UNIT_CNT ++;
        }
        rmtree ($F_DIR);
    }

SBM_TASK:
    if ($SPAN_CNT >= $TIME_SPAN or $UNIT_CNT >= $UNIT_MAX) {
        $SPAN_CNT = 0;
        next if ($UNIT_CNT <= 0);
        warn "Submitting a task!\n";
        my $RET_STR = &VerThd ($CUR_UNIT_DIR, $UNIT_NUM);
        next if ($RET_STR !~ /OK/i);
        $UNIT_CNT = 0;
        $UNIT_NUM ++;
    }

    sleep($SCAN_SPAN);
    $SPAN_CNT += $SCAN_SPAN;
    warn "Current task num: <$UNIT_NUM>\n";
}


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

sub VerThd {
    my $CUR_SEED_DIR = shift;
    my $CUR_UNIT_NUM = shift;

    `chmod 775 $CUR_SEED_DIR/*`;
    my $CUR_TEST_BIN = "/data/tmp_ver_rit_$CUR_UNIT_NUM.bin";
    my $CUR_PASS_DIR = "$CUR_SEED_DIR/$ARM_PASS_DIR";
    my $CUR_FAIL_DIR = "$CUR_SEED_DIR/$ARM_FAIL_DIR";
    my $CUR_ARM_LOG = $LOG_ARM_PREFIX."_$CUR_UNIT_NUM.log";
    my $CUR_COSIM_LOG = $LOG_COSIM_PREFIX."_$CUR_UNIT_NUM.log";
    my $CUR_INTRP_LOG = $LOG_INTRP_PREFIX."_$CUR_UNIT_NUM.log";
    my $CUR_REL_TIA_LOG = $LOG_REL_TIA_PREFIX."_$CUR_UNIT_NUM.log";
    my $CUR_TIA_LOG = $LOG_TIA_PREFIX."_$CUR_UNIT_NUM.log";

    my $CH_PID = fork();
    if (not defined($CH_PID)) {
        return 'Error';
    } elsif ($CH_PID == 0) {
        my $ADB_NUM = 0;
        while (1) {
            $ADB_NUM = int(`pgrep -c adb`);
            sleep(1);
            $ADB_NUM += int(`pgrep -c adb`);
            last if ( $ADB_NUM <= 10 );
            sleep(2);
        }

        `$PL_ARM -d $CUR_SEED_DIR -f $CUR_TEST_BIN -k 5 $SN_OPT -l $CUR_SEED_DIR/$CUR_ARM_LOG`;
        move "$CUR_SEED_DIR/$CUR_ARM_LOG", "$RES_DIR/$TMP_RES_DIR/" if (-e "$CUR_SEED_DIR/$CUR_ARM_LOG");

        `$PL_UBT -b $UBT_DIR/cosim -d $CUR_PASS_DIR -o $CUR_SEED_DIR/$CUR_COSIM_LOG -t 5`;
        move "$CUR_SEED_DIR/$CUR_COSIM_LOG", "$RES_DIR/$TMP_RES_DIR/" if (-e "$CUR_SEED_DIR/$CUR_COSIM_LOG");

        `$PL_UBT -b $UBT_DIR/intrp -d $CUR_PASS_DIR -o $CUR_SEED_DIR/$CUR_INTRP_LOG -t 5`;
        move "$CUR_SEED_DIR/$CUR_INTRP_LOG", "$RES_DIR/$TMP_RES_DIR/" if (-e "$CUR_SEED_DIR/$CUR_INTRP_LOG");

        if (defined($REL_TIA_FLAG)) {
            `$PL_UBT -b $UBT_DIR/rel_tia -d $CUR_PASS_DIR -o $CUR_SEED_DIR/$CUR_REL_TIA_LOG -t 5`;
            move "$CUR_SEED_DIR/$CUR_REL_TIA_LOG", "$RES_DIR/$TMP_RES_DIR/" if (-e "$CUR_SEED_DIR/$CUR_REL_TIA_LOG");
        }

        `$PL_UBT -b $UBT_DIR/tia -d $CUR_PASS_DIR -o $CUR_SEED_DIR/$CUR_TIA_LOG -t 5`;
        move "$CUR_SEED_DIR/$CUR_TIA_LOG", "$RES_DIR/$TMP_RES_DIR/" if (-e "$CUR_SEED_DIR/$CUR_TIA_LOG");

        while (glob "$CUR_FAIL_DIR/*.bin") {
            move $_, "$RES_DIR/$ARM_FAIL_DIR/";
        }

        while (glob "$CUR_PASS_DIR/*.bin") {
            move $_, "$RES_DIR/$ARM_PASS_DIR/";
        }

        rmtree ($CUR_SEED_DIR);
        exit 0;

    } else {
        return 'OK';
    }
}

#-----------------------------------------------------------------------------------------------------------------------
# End of Subroutines
#-----------------------------------------------------------------------------------------------------------------------
