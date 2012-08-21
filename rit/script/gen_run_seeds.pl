#!/usr/bin/perl -w
#===============================================================================
#
#         FILE:  gen_run_seeds.pl
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
use Time::HiRes qw(time usleep);
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

my $SCRIPT_DIR = &AbsPath(&dirname($0));
my $NAME_PREFIX = &hostname."_$$";
my $PYBIN_VER = 'python2.7';
my $RIT_DIR = "$SCRIPT_DIR/..";
my $RIT_BIN = "$RIT_DIR/bin";
my $ARM_TOOL_BIN = "$RIT_DIR/../arm-2011.03/bin";
my $UBT_DIR = "$RIT_DIR/../ubt";
my $SEED_POOL = "$RIT_DIR/../SeedPool";
my $SEED_DIR_NAME = "$NAME_PREFIX".'_'.join('',(split(/\./, &time, 2)));
my $QEMU_ARM = 'qemu-arm';
my $QEMU_BIN = "$RIT_DIR/bin/$QEMU_ARM";
my $QEMU_COV = "$RIT_DIR/bin/$QEMU_ARM"."-cov";
my $PY_PROC_SEEDS = "$SCRIPT_DIR/process_seeds.py";
my $PY_RIT_COV = "$SCRIPT_DIR/rit_cov.py";
my $PY_ART = "$RIT_DIR/art/art.py";
my $FAIL_DIR = "$RIT_DIR/../fail_seeds";
my $SEED_NUM = undef;
my $EXE_FLAG = undef;
my $COV_FLAG = undef;
my $REL_TIA_OPT = '--with-rel-tia';
my $COV_DEF_NAME = 'rit.ilog';
my $COV_DIR = 'COV_DB';
my $REMOVE_FLAG = undef;
my $RES_POOL = "$RIT_DIR/../SeedRes";
my $TMP_SEED = 'tmp_seed';
my @INST_SET        =   qw/thumb2            ARMv7           VFP           excl                 aluwpc/;
my @ART_ISA_OPTS    =   qw/--with-Thumb-2    --with-ARMv7    --with-VFP    --with-excl-block    --with-aluwpc-block/;
my @ART_NO_ISA_OPTS =   qw/--without-Thumb-2 --without-ARMv7 --without-VFP --without-excl-block --without-aluwpc-block/;
my $ISA_OPTS = '';
my $MIN_INST_NUM = undef;
my $NEST_LEVEL = undef;
my $RAND_NUM = undef;
my $OPTS_ART = '';
my $RAND_OPT = '';
my $UBT_NO_CFG = 'NO_CFG';
my @UBT_TEST_NAME= qw/intrp                         tia                     rel_tia                 cosim/;
my @UBT_CFG_NAME = qw/ubt.config.always.interpreter ubt.config.always.TIA   ubt.config.always.TIA   ubt.config.always.TIA.cosim/;
my $UNIF_BIN_NAME = 'cms.elf';
my $UNIF_CFG_NAME = 'ubt.config';
my $SUCC_NUM = 0;
my $VERI_FLAG = undef;
my $DEST_OPT = '';
my $SEED_DIR = undef;
my $RES_DIR = undef;
my $TMP_DIR = undef;
my $ERROR_DIR = undef;
my $ASM_DIR = undef;
my $SEED_FAIL = undef;
#-----------------------------------------------------------------------------------------------------------------------
# Main Body
#-----------------------------------------------------------------------------------------------------------------------

# Get options
GetOptions( "dir=s" => \$SEED_POOL,
        "arm=s" => \$ARM_TOOL_BIN,
        "seed=i" => \$SEED_NUM,
        "out=s" => \$RES_POOL,
        "rem!" => \$REMOVE_FLAG,
        "exe!" => \$EXE_FLAG,
		"fail=s" => \$FAIL_DIR,
        "min=i" => \$MIN_INST_NUM,
        "tesn=i" => \$NEST_LEVEL,
        "isa=s" => \$ISA_OPTS,
        "znum=i" => \$RAND_NUM,
        "bin=s" => \$UBT_DIR,
        "verify!" => \$VERI_FLAG,
        "cov!" => \$COV_FLAG
        )
or die "Fail to get options!\n";

if ( not defined($SEED_NUM) ) {
    warn "Lack of options!
Usage: gen_run_seeds.pl [Options]
    -a  <Default=RIT_DIR/../arm-2011.03/bin> the bin dir path of ARM tool chain
    -b  <Deault=RIT_DIR/../ubt> the path to ubt dir
    -c  dump the coverage file
    -d  the pool dir to store seeds and log files
    -e  test seeds imm after generated
    -f  <Default=RIT_DIR/../fail_seeds> the dir to contain failed seeds
    -i  do NOT contain ISA sets in seeds, e.g. [-i thumb2,ARMv7,VFP,excl,aluwpc]
    -m  minimal random instructions generated
    -o  the pool dir to store resluts
    -r  remove all files at last, expect for the result dir
    -s  <Necessary> the number of seeds to be generated
    -t  test case control flow nest level
    -v  verify the seed bin by QEMU after generated
    -z  specify the seed start number 
    \n";
    exit 0;
}

for (my $i=0; $i < @INST_SET; $i++) {
    $OPTS_ART .= ($ISA_OPTS =~ /.*\b$INST_SET[$i]\b.*/i)?" $ART_NO_ISA_OPTS[$i] ":" $ART_ISA_OPTS[$i] ";
}

$OPTS_ART .= " --min $MIN_INST_NUM " if (defined($MIN_INST_NUM));
$OPTS_ART .= " --nest $NEST_LEVEL " if (defined($NEST_LEVEL));
warn "The parameters to art.py are as below:\n-< $OPTS_ART >-\n";

$ARM_TOOL_BIN = &AbsPath($ARM_TOOL_BIN);
$RES_DIR = "$RES_POOL/$SEED_DIR_NAME";
mkpath ($RES_DIR);
$RES_DIR = &AbsPath($RES_DIR);
$COV_DIR = "$RES_POOL/$COV_DIR";
mkpath ($COV_DIR);
$COV_DIR = &AbsPath($COV_DIR);
$SEED_DIR = "$SEED_POOL/$SEED_DIR_NAME";
rmtree($SEED_DIR) if ( -d $SEED_DIR );
mkpath ($SEED_DIR);
$SEED_DIR = &AbsPath($SEED_DIR);
$TMP_DIR = "$SEED_DIR/TMP";
open (LOG,">$RES_DIR/rit.genseeds.log") or die "Fail to open log file <$RES_DIR/rit.genseeds.log>\n";
print LOG "Try to gen <$SEED_NUM> seeds @".localtime()."\n";
mkpath ($TMP_DIR);
if ( not defined($REMOVE_FLAG)) {
    $ERROR_DIR = "$SEED_DIR/ERROR";
    $ASM_DIR = "$SEED_DIR/ASM";
    mkpath ($ERROR_DIR);
    mkpath ($ASM_DIR);
    mkpath ("$SEED_DIR/DEST");
    $DEST_OPT = "-d $SEED_DIR/DEST";
}
die "Not find $PY_ART\n" if ( not -e $PY_ART);
die "Not find $QEMU_COV or $PY_RIT_COV\n" if ( defined($COV_FLAG) and (not -e $QEMU_COV or not -e $PY_RIT_COV));
if ( not -e "$RIT_BIN/librit.so" ) {
    if ( -d "$RIT_DIR/src" ) {
        warn "Compiling lib rit\n";
        my $CWD = getcwd();
        chdir("$RIT_DIR/src");
        `make librit.so`;
        chdir($CWD);
    }
    die "Not find librit.so\n" if ( not -e "$RIT_DIR/src/librit.so" );
    move "$RIT_DIR/src/librit.so", "$RIT_BIN/librit.so";
}

if (defined($EXE_FLAG)) {
    for (my $i=0; $i < @UBT_TEST_NAME; $i++) {
        my $CUR_DIR = "$UBT_DIR/$UBT_TEST_NAME[$i]";
        if ( not -e "$CUR_DIR/$UNIF_BIN_NAME") {
            if ($UBT_TEST_NAME[$i] =~ /rel_tia/i){
                $REL_TIA_OPT = '';
                next;
            }
            die "Please put ubt bin into <$CUR_DIR>\n";
        }
        `chmod 775 $CUR_DIR/$UNIF_BIN_NAME`;
        die "Please checkout $UBT_CFG_NAME[$i] into <$CUR_DIR> as $UNIF_CFG_NAME\n" if (( $UBT_CFG_NAME[$i] ne $UBT_NO_CFG ) and ( not -e "$CUR_DIR/$UNIF_CFG_NAME") );
    }
}

$ENV{'PATH'} = "$ARM_TOOL_BIN:$RIT_BIN:$ENV{'PATH'}";
my $STAT = undef;
for my $i (1..$SEED_NUM) {
    my $SEED_NAME = undef;
    if ( not defined($RAND_NUM) ) {
        $SEED_NAME = $NAME_PREFIX.'_t'.join('',(split(/\./, &time, 2)));
        $RAND_OPT = '';
    } else {
        $SEED_NAME = $NAME_PREFIX.'_r'.$RAND_NUM;
        $RAND_OPT = "--seed $RAND_NUM";
        $RAND_NUM ++;
    }
    my $CWD = getcwd();
    chdir($SEED_DIR);
    unlink $TMP_SEED.'s' if ( -e $TMP_SEED.'s');
    unlink $TMP_SEED.'bin' if ( -e $TMP_SEED.'bin');
    `$PYBIN_VER $PY_ART $OPTS_ART $RAND_OPT -o $TMP_SEED.s > /dev/null`;
    my $STAT = $?;
    chdir($CWD);
    if ( $STAT != 0 ) {
        print LOG "Fail to gen seed <$SEED_NAME> @".localtime()."\n";
        move "$SEED_DIR/$TMP_SEED.s","$ERROR_DIR/$SEED_NAME.s" if ( not defined($REMOVE_FLAG) and -e "$SEED_DIR/$TMP_SEED.s" );
        unlink "$SEED_DIR/$TMP_SEED.bin" if ( -e "$SEED_DIR/$TMP_SEED.bin" );
        next;
    }

    if ( defined($VERI_FLAG) ) {
        my $V_RES = `$QEMU_BIN $SEED_DIR/$TMP_SEED.bin`;
        my $STAT = $?;
        my $BIN_SIZE = (-s "$SEED_DIR/$TMP_SEED.bin");
        if ($STAT != 0 or $V_RES !~ /Pass/i or $BIN_SIZE <= 0) {
            print LOG "<Verified> Fail to gen seed <$SEED_NAME> @".localtime()."\n";
            move "$SEED_DIR/$TMP_SEED.s","$ERROR_DIR/$SEED_NAME.s" if (  not defined($REMOVE_FLAG) and -e "$SEED_DIR/$TMP_SEED.s" );
            unlink "$SEED_DIR/$TMP_SEED.bin" if ( -e "$SEED_DIR/$TMP_SEED.bin" );
            next;
        }
    }
    $SUCC_NUM += 1;

    if ( defined($COV_FLAG) ) {
        chdir($SEED_DIR);
        `$QEMU_COV -singlestep -d in_asm $TMP_SEED.bin > /dev/null`;
        $STAT = $?;
        if ( $STAT != 0 ) {
            print LOG "<$SEED_NAME> qemu failed\n";
            next;
        }
#`nm $TMP_SEED.bin > tmp.covinfo`;
#`cat qemu.log >> tmp.covinfo`;
        `$PYBIN_VER $PY_RIT_COV -d $SEED_DIR/ qemu.log > /dev/null`;
        chdir($CWD);
    }
    move "$SEED_DIR/$TMP_SEED.bin","$TMP_DIR/$SEED_NAME.bin" if ( -e "$SEED_DIR/$TMP_SEED.bin" );
    move "$SEED_DIR/$TMP_SEED.s","$ASM_DIR/$SEED_NAME.s" if (  not defined($REMOVE_FLAG) and -e "$SEED_DIR/$TMP_SEED.s" );
}

if ( defined($COV_FLAG) and -e "$SEED_DIR/$COV_DEF_NAME" ) {
    my $CWD = getcwd();
    chdir("$SEED_DIR");
    move "$COV_DEF_NAME","$SEED_DIR_NAME.ilog";
    `$PYBIN_VER $PY_RIT_COV -m $SEED_DIR_NAME.ilog > /dev/null`;
    chdir($CWD);
    copy "$SEED_DIR/$COV_DEF_NAME","$COV_DIR/$SEED_DIR_NAME.ilog" if (-e "$SEED_DIR/$COV_DEF_NAME");
}

print LOG "<$SUCC_NUM> seeds generated @".localtime()."\n";
if ( not defined($EXE_FLAG) ) {
    close (LOG);
    &TestDone($RES_DIR, $RES_POOL, $SUCC_NUM);
    rmtree($SEED_DIR) if ( defined($REMOVE_FLAG) );
    exit 0;
}

die "Not find $PY_PROC_SEEDS\n" if ( not -e $PY_PROC_SEEDS);

$SEED_FAIL = "$FAIL_DIR/$SEED_DIR_NAME";
mkpath($SEED_FAIL);
my $UBT_BIN = "$SEED_DIR/ubt";
mkpath ($UBT_BIN);
`cp $UBT_DIR/* $UBT_BIN/ -r`;
my $CWD = getcwd();
chdir($SEED_DIR);
`$PYBIN_VER $PY_PROC_SEEDS -l $RES_DIR/rit.log -s $TMP_DIR $DEST_OPT -u $UBT_BIN -f $SEED_FAIL $REL_TIA_OPT --with-intrp > /dev/null`;
#if ( defined($COV_FLAG )) {
#    `cat tmp.cov.log | sort | uniq >> $RES_DIR/rit-cov.log`;
#    `cat tmp.cov.fsm.log | sort | uniq >> $RES_DIR/rit-cov.fsm.log`;
#    `rm -f tmp.cov.log`;
#    `rm -f tmp.cov.fsm.log`;
#}
close (LOG);
chdir($CWD);
&TestDone($RES_DIR, $RES_POOL, $SUCC_NUM);
rmtree($SEED_DIR) if ( defined($REMOVE_FLAG) );
`echo Done > $SEED_FAIL/Done`;
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

sub TestDone {
    my $RES_DIR = shift;
    my $RES_POOL = shift;
    my $SUCC_NUM = shift;
#    my $TOTAL_NUM = 0;
#    my $RET_STR = '';

    if ( open (DONE,">$RES_DIR/DONE") ) {
        print DONE "Generated <$SUCC_NUM> seeds\nTest done @".localtime();
        close(DONE);
    }

#    unlink "$RES_POOL/TestEnd.log" if ( -e "$RES_POOL/TestEnd.log" );
#    foreach my $TASK_DIR (glob "$RES_POOL/*") {
#        next if ( not -d "$TASK_DIR" );
#        next if ( "$TASK_DIR" =~ /COV_DB$/i );
#        return if ( not -e "$TASK_DIR/DONE");
#        my $PRE_FILE_SIZE = (-s "$TASK_DIR/DONE");
#        my $WAIT_CNT = 10;
#        while ($WAIT_CNT--) {
#            usleep(1000);
#            my $CUR_FILE_SIZE = (-s "$TASK_DIR/DONE");
#            last if ($PRE_FILE_SIZE == $CUR_FILE_SIZE);
#            $PRE_FILE_SIZE = $CUR_FILE_SIZE;
#        }
#        if ( $WAIT_CNT <= 0 or not open (TD, "$TASK_DIR/DONE") ) {
#            $RET_STR .= "Fail to get seed num in <$TASK_DIR>\n";
#            last;
#        }
#        my $NUM_FLAG = undef;
#        while (my $LINE = <TD>) {
#            if ($LINE =~ /Generated <(\d+)> seeds/i) {
#                $TOTAL_NUM += $1;
#                $NUM_FLAG = 1;
#                last;
#            }
#        }
#        close(TD);
#        $RET_STR .= "Fail to get seed num in <$TASK_DIR>\n" if ( not defined($NUM_FLAG));
#    }
#
#    if ( open (ALLDONE,">$RES_POOL/TestEnd.log") ) {
#        print ALLDONE "$RET_STR"."Totally generated <$TOTAL_NUM> seeds\nTest end @".localtime();
#        close(ALLDONE);
#    }
}

#-----------------------------------------------------------------------------------------------------------------------
# End of Subroutines
#-----------------------------------------------------------------------------------------------------------------------
