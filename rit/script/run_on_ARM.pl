#!/usr/bin/perl -w
#===============================================================================
#
#         FILE:  run_on_ARM.pl
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
my $SEED_DIR = undef;
my $RES_DIR = undef;
my $REMOVE_FLAG = undef;
my $ARM_FAIL_DIR = undef;
my $ARM_PASS_DIR = undef;
my $SERIAL_NUM = undef;
my $ADB_RES = undef;
my $DEV_LOG = undef;
my $SN_OPT = '';
my $ERROR_CNT = undef;
my $ERROR_MAX = 3;
my @ABBR = qw/ Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec /;
my $TIMEOUT_BIN = '';
my $TIMEOUT = 5;
my $ARM_FILE_NAME = '/data/tmp.rit.bin';

#-----------------------------------------------------------------------------------------------------------------------
# Main Body
#-----------------------------------------------------------------------------------------------------------------------
# Get options
GetOptions( "out=s" => \$RES_DIR,
            "dir=s" => \$SEED_DIR,
            "snd=s" => \$SERIAL_NUM,
            "kill=i" => \$TIMEOUT,
            "file=s" => \$ARM_FILE_NAME,
            "log=s" => \$DEV_LOG,
            "rem!" => \$REMOVE_FLAG
			 )
			or die "Fail to get options!\n";

if ( not defined($SEED_DIR) ) {
	warn "Lack of options!
Usage: run_on_ARM.pl [Options]
    -d  <Necessary> the dir containing failed seeds
    -f  <Default=/data/tmp.rit.bin> the path and file name of seeds on ARM phone
    -k  <Default=5> the timeout to kill adb (in sec)
    -l  <Default=RES_DIR/ARM_nat_date.log> ARM test log file
    -o  the dir to store result log
    -r  remove seeds also failed on ARM phone
    -s  the serial number of the device
    \n";
	exit 0;
}

if (not defined($DEV_LOG)) {
    my ($SEC,$MIN,$HOUR,$MDAY,$MON,$YEAR,$WDAY,$YDAY,$ISDST) = localtime(time);
    $YEAR += 1900;
    $DEV_LOG = "$RES_DIR/ARM_nat_$YEAR-$ABBR[$MON]-$MDAY.log";
}

$SN_OPT = (defined($SERIAL_NUM))?"-s $SERIAL_NUM":'';
$RES_DIR = $SEED_DIR if ( not defined($RES_DIR) );
if ( not defined($REMOVE_FLAG) ) {
    $ARM_FAIL_DIR = "$RES_DIR/fail_on_arm" ;
    mkpath($ARM_FAIL_DIR);
}
$ARM_PASS_DIR = "$RES_DIR/pass_on_arm";
mkpath($ARM_PASS_DIR);
`which timeout > /dev/null`;
if ( $? != 0 and -e "$RIT_BIN_DIR/timeout" ) {
    $TIMEOUT_BIN = "$RIT_BIN_DIR/timeout -s 9 $TIMEOUT ";
} else {
    $TIMEOUT_BIN = "timeout -s 9 $TIMEOUT ";
}

foreach my $TEST (glob "$SEED_DIR/*.bin") {
    $ERROR_CNT = $ERROR_MAX;
    my ( $BASE_NAME, $DIR, $SUFFIX ) = fileparse ( $TEST,qr/\.[^.]*/ );
RE_TEST:
    $ADB_RES = undef;
    warn "Testing Seed <$BASE_NAME>\n";
    `adb $SN_OPT push $TEST $ARM_FILE_NAME`;
    $ADB_RES = `$TIMEOUT_BIN adb $SN_OPT shell $ARM_FILE_NAME`;
    `adb $SN_OPT shell rm $ARM_FILE_NAME`;
    if (not defined($ADB_RES) or $ADB_RES =~ /^$/i) {
        warn "ADB Error is encountered!\n";
        $ERROR_CNT --;
        goto RE_TEST if ($ERROR_CNT > 0);
        $ADB_RES = "ADB Error\n";
        goto FIN;
    }
    
    goto RE_TEST if ( $ADB_RES =~ /text\s+busy/i );
    $ERROR_CNT --;
    goto RE_TEST if( $ERROR_CNT > 0 and $ADB_RES !~ /pass/i );

FIN:
    `echo -n '$BASE_NAME ---> $ADB_RES' >> $DEV_LOG`;
    warn "$ADB_RES\n";
    if( $ADB_RES =~ /pass/i ) {
        `mv $TEST $ARM_PASS_DIR/`;
        next;
    }
    if (defined($REMOVE_FLAG)) {
        unlink $TEST;
        next;
    }
    `mv $TEST $ARM_FAIL_DIR/`;
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
# End of Subroutines
#-----------------------------------------------------------------------------------------------------------------------
