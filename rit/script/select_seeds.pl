#!/usr/bin/perl -w
#===============================================================================
#
#         FILE:  select_seeds.pl
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

my $LOC_LOG_PAT = qr/^(\w+)\s+(.*)$/;
my $NB_LOG_PAT = qr/^seed\s+(\w+)\.bin\s+(.*)$/;
my $ARM_LOG_PAT = qr/^(\w+)\s*--->\s*(.*)$/;
my $ARM_PREFIX = 'ARM_nat';
my $LOC_PREFIX = 'LOC_';
my $NB_PREFIX = 'NB_';

my $SEED_DIR = undef;
my $LOG_RES = undef;
my $OUT_DIR = undef;
my $CUR_PAT = undef;
my $PASS_FLAG = undef;
my $MAT_STR = undef;
my $SEED_CNT = 0;

#-----------------------------------------------------------------------------------------------------------------------
# Main Body
#-----------------------------------------------------------------------------------------------------------------------
# Get options
GetOptions( "dir=s" => \$SEED_DIR,
            "log=s" => \$LOG_RES,
            "pass!" => \$PASS_FLAG,
            "out=s" => \$OUT_DIR)
			or die "Fail to get options!\n";

if ( not defined($SEED_DIR)
    or not defined($LOG_RES)
    or not defined($OUT_DIR)) {
	warn "Lack of options!
Usage: select_seeds.pl [Options]
    -d  <Necessary> the seed dir 
    -l  <Necessary> Log res file
    -o  <Necessary> output dir
    -p  <Default=failed seeds> Select passing seeds instead of failed seeds
    \n";
    exit 0;
}

die "Can not find seed dir or log file\n" if ((not -d $SEED_DIR) or (not -e $LOG_RES));
die "Fail to open log file\n" if (not open (LOG, "<$LOG_RES"));

if ($LOG_RES =~ /$ARM_PREFIX/i) {
    $CUR_PAT = $ARM_LOG_PAT;
} elsif ($LOG_RES =~ /$LOC_PREFIX/i) {
    $CUR_PAT = $LOC_LOG_PAT;
} elsif ($LOG_RES =~ /$NB_PREFIX/i) {
    $CUR_PAT = $NB_LOG_PAT;
} else {
    $CUR_PAT = $LOC_LOG_PAT;
}

mkpath($OUT_DIR);
while (my $LINE = <LOG>) {
    next if ($LINE !~ /$CUR_PAT/i);
    my $SEED_N = "$SEED_DIR/$1.bin";
    my $RES = $2;

    if (defined($PASS_FLAG)) {
        next if ($RES !~ /pass/i);
    } else {
        next if ($RES =~ /pass/i);
    }

    if (not -e $SEED_N) {
        warn "Can not find seed <$SEED_N>\n";
        next;
    }
    warn "Copying seed <$SEED_N>\n";
    copy $SEED_N, "$OUT_DIR/";
    $SEED_CNT ++;
}

warn "Totally find <$SEED_CNT> seeds\n";
close (LOG);
exit 0;

#-----------------------------------------------------------------------------------------------------------------------
# End of Subroutines
#-----------------------------------------------------------------------------------------------------------------------
