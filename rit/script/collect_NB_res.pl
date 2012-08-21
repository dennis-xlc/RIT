#!/usr/bin/perl -w
#===============================================================================
#
#         FILE:  collect_NB_res.pl
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

my $SEED_RES_DIR = undef;
my $COSIM = undef;
my $INTRP = undef;
my $REL_TIA = undef;
my $TIA = undef;
my $COSIM_LOG = 'NB_cosim_res.log';
my $INTRP_LOG = 'NB_intrp_res.log';
my $REL_TIA_LOG = 'NB_rel_tia_res.log';
my $TIA_LOG = 'NB_tia_res.log';
my $TOT_LOG = 'NB_tot_res.log';
my $NB_LOG = 'rit.log';
my $PAT_COSIM_FAIL = qr/fail\s*\(\s*cosim-ubt\s*\)$/;
my $PAT_INTRP_FAIL = qr/fail\s*\(\s*intrp-ubt\s*\)$/;
my $PAT_REL_TIA_FAIL = qr/fail\s*\(\s*rel-tia-ubt\s*\)$/;
my $PAT_TIA_FAIL = qr/fail\s*\(\s*tia-ubt\s*\)$/;
my $TOTAL_NUM = 0;

#-----------------------------------------------------------------------------------------------------------------------
# Main Body
#-----------------------------------------------------------------------------------------------------------------------
# Get options
GetOptions( "dir=s" => \$SEED_RES_DIR,
            "cosim!" => \$COSIM,
            "intrp!" => \$INTRP,
            "rel!" => \$REL_TIA,
            "tia!" => \$TIA)
			or die "Fail to get options!\n";

if ( not defined($SEED_RES_DIR)) {
	warn "Lack of options!
Usage: collect_NB_res.pl [Options]
    -d  <Necessary> the SeedRes dir 
    -c  collect cosim failures
    -i  collect intrp failures
    -r  collect rel_tia failures
    -t  collect tia failures
    \n";
    exit 0;
}

die "Not find SeedRes dir <$SEED_RES_DIR>\n" if (not -d $SEED_RES_DIR);

if (defined($COSIM)) {
    unlink $COSIM_LOG if (-e $COSIM_LOG);
    die "Can't create cosim log <$COSIM_LOG>\n" if (not open (CL, ">$COSIM_LOG"));
}

if (defined($INTRP)) {
    unlink $INTRP_LOG if (-e $INTRP_LOG);
    die "Can't create intrp log <$INTRP_LOG>\n" if (not open (IL, ">$INTRP_LOG"));
}

if (defined($REL_TIA)) {
    unlink $REL_TIA_LOG if (-e $REL_TIA_LOG);
    die "Can't create rel_tia log <$REL_TIA_LOG>\n" if (not open (RL, ">$REL_TIA_LOG"));
}

if (defined($TIA)) {
    unlink $TIA_LOG if (-e $TIA_LOG);
    die "Can't create tia log <$TIA_LOG>\n" if (not open (TL, ">$TIA_LOG"));
}

foreach my $NB_TASK (glob "$SEED_RES_DIR/*") {
    next if (not -d $NB_TASK);
    next if (not -e "$NB_TASK/DONE");
    next if (not -e "$NB_TASK/$NB_LOG");
    next if (not open (DN,"$NB_TASK/DONE"));
    if (not open (NL,"$NB_TASK/$NB_LOG")) {
        close (DN);
        next;
    }
    my $NUM_FLAG = undef;
    while (my $LINE = <DN>) {
        if ($LINE =~ /Generated <(\d+)> seeds/i) {
            $TOTAL_NUM += $1;
            $NUM_FLAG = 1;
            last;
        }
    }
    close (DN);
    if (not defined($NUM_FLAG)) {
        close (NL);
        next;
    }
    while (my $LINE = <NL>) {
        if (defined($COSIM) and $LINE =~ $PAT_COSIM_FAIL) {
            print CL $LINE;
            next;
        }
        if (defined($INTRP) and $LINE =~ $PAT_INTRP_FAIL) {
            print IL $LINE;
            next;
        }
        if (defined($REL_TIA) and $LINE =~ $PAT_REL_TIA_FAIL) {
            print RL $LINE;
            next;
        }
        if (defined($TIA) and $LINE =~ $PAT_TIA_FAIL) {
            print TL $LINE;
            next;
        }
    }
    close (NL);
}

close (CL) if (defined($COSIM));
close (IL) if (defined($INTRP));
close (RL) if (defined($REL_TIA));
close (TL) if (defined($TIA));
warn "Tested Seeds Total #: <$TOTAL_NUM>\n";
die "Can't create TOT log <$TOT_LOG>\n" if (not open (TOT, ">$TOT_LOG"));
print TOT "Tested Seeds Total #: <$TOTAL_NUM>\n";
close (TOT);
exit 0;

#-----------------------------------------------------------------------------------------------------------------------

#-----------------------------------------------------------------------------------------------------------------------
# End of Subroutines
#-----------------------------------------------------------------------------------------------------------------------
