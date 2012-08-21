#!/usr/bin/perl -w
#===============================================================================
#
#         FILE:  run_on_QEMU.pl
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

my $QEMU_BIN = &dirname($0).'/../bin/qemu-arm';
my $SEED_DIR = undef;
my $OUT_LOG = 'qemu_res.log';

#-----------------------------------------------------------------------------------------------------------------------
# Main Body
#-----------------------------------------------------------------------------------------------------------------------
# Get options
GetOptions( "dir=s" => \$SEED_DIR,
            "bin=s" => \$QEMU_BIN,
            "out=s" => \$OUT_LOG)
			or die "Fail to get options!\n";

if ( not defined($SEED_DIR) ) {
	warn "Lack of options!
Usage: run_on_QEMU.pl [Options]
    -b  <Default=RIT_DIR/bin/qemu-arm> qemu-arm binary
    -d  <Necessary> the dir containing failed seeds
    -o  <Default=qemu_res.log> the result log
    \n";
	exit 0;
}

die "Not find Seed dir <$SEED_DIR>\n" if ( not -d $SEED_DIR );
die "Not find qemu-arm bin <$QEMU_BIN>\n" if ( not -e $QEMU_BIN );

foreach my $TEST (glob "$SEED_DIR/*.bin") {
    my ( $BASE_NAME, $DIR, $SUFFIX ) = fileparse ( $TEST,qr/\.[^.]*/ );
    warn "Testing seed <$BASE_NAME>\n";
    `$QEMU_BIN $TEST > /dev/null`;
    my $STAT = $?;
    if ( $STAT != 0 ) {
        `echo "$BASE_NAME failed error=$STAT" >> $OUT_LOG`;
        warn "$BASE_NAME failed error=$STAT\n";
    } else {
        `echo "$BASE_NAME passed" >> $OUT_LOG`;
        warn "$BASE_NAME passed\n";
    }
}

#-----------------------------------------------------------------------------------------------------------------------

#-----------------------------------------------------------------------------------------------------------------------
# End of Subroutines
#-----------------------------------------------------------------------------------------------------------------------
