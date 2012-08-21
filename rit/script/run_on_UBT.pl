#!/usr/bin/perl -w
#===============================================================================
#
#         FILE:  run_on_UBT.pl
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

my $RIT_BIN_DIR = &AbsPath(&dirname($0).'/../bin');
my $TIMEOUT_BIN = '';
my $TIMEOUT = 5;
my $SEED_DIR = undef;
my $UBT_DIR = undef;
my $UBT_BIN = 'cms.elf';
my $UBT_CFG = 'ubt.config';
my $OUT_LOG = 'ubt_res.log';

#-----------------------------------------------------------------------------------------------------------------------
# Main Body
#-----------------------------------------------------------------------------------------------------------------------
# Get options
GetOptions( "dir=s" => \$SEED_DIR,
            "bin=s" => \$UBT_DIR,
            "time=i" => \$TIMEOUT,
            "out=s" => \$OUT_LOG)
			or die "Fail to get options!\n";

if ( not defined($SEED_DIR)  or not defined($UBT_DIR)) {
	warn "Lack of options!
Usage: run_on_UBT.pl [Options]
    -b  <Necessary> the dir containing ubt binary and config
    -d  <Necessary> the dir containing failed seeds
    -o  <Default=ubt_res.log> the result log
    -t  <Default=5> the timeout to kill ubt (in sec)
    \n";
	exit 0;
}

die "Not find Seed dir <$SEED_DIR>\n" if ( not -d $SEED_DIR );
$SEED_DIR = &AbsPath( $SEED_DIR );
die "Not find ubt dir <$UBT_DIR>\n" if ( not -e $UBT_DIR );
die "Not find ubt bin <$UBT_DIR/$UBT_BIN>\n" if ( not -e "$UBT_DIR/$UBT_BIN" );
die "Not find ubt cfg <$UBT_DIR/$UBT_CFG>\n" if ( not -e "$UBT_DIR/$UBT_CFG" );
`which timeout > /dev/null`;
if ( $? != 0 and -e "$RIT_BIN_DIR/timeout" ) {
    $TIMEOUT_BIN = "$RIT_BIN_DIR/timeout -s 9 $TIMEOUT ";
} else {
    $TIMEOUT_BIN = "timeout -s 9 $TIMEOUT ";
}

foreach my $TEST (glob "$SEED_DIR/*.bin") {
    my ( $BASE_NAME, $DIR, $SUFFIX ) = fileparse ( $TEST,qr/\.[^.]*/ );
    warn "Testing seed <$BASE_NAME>\n";
    my $CWD = getcwd();
    chdir($UBT_DIR);
    `$TIMEOUT_BIN ./$UBT_BIN $TEST > /dev/null`;
    my $STAT = $?;
    chdir($CWD);
    if ( $STAT != 0 ) {
        `echo "$BASE_NAME failed error=$STAT" >> $OUT_LOG`;
        warn "$BASE_NAME failed error=$STAT\n";
    } else {
        `echo "$BASE_NAME passed" >> $OUT_LOG`;
        warn "$BASE_NAME passed\n";
    }
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
