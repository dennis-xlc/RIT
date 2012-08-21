#!/usr/bin/perl -w
#===============================================================================
#
#         FILE:  show_final_res.pl
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

my $NB_COSIM_LOG = 'NB_cosim_res.log';
my $NB_INTRP_LOG = 'NB_intrp_res.log';
my $NB_REL_TIA_LOG = 'NB_rel_tia_res.log';
my $NB_TIA_LOG = 'NB_tia_res.log';
my $NB_LOG_PAT = qr/^seed\s+(\w+)\.bin\s+(.*)$/;
my $NB_TOT_LOG = 'NB_tot_res.log';
my $NB_TOT_PAT = qr/^Tested\s+Seeds\s+Total\s+#:\s*<\s*(\d+)\s*>\s*$/;
my $LOC_COSIM_LOG = 'LOC_cosim_res.log';
my $LOC_INTRP_LOG = 'LOC_intrp_res.log';
my $LOC_TIA_LOG = 'LOC_tia_res.log';
my $LOC_LOG_PAT = qr/^(\w+)\s+(.*)$/;
my $LOC_REL_TIA_LOG = 'LOC_rel_tia_res.log';
#my $DEV_LOG_PREFIX = 'ARM_nat';
#my $DEV_LOG_PAT = qr/^(\w+)\s*--->\s*(.*)$/;
#my $PASS_ON_ARM_DIR = 'pass_on_arm';
my $FAIL_ON_ARM_DIR = 'fail_on_arm';
#my $LOC_VER_DIR = "$PASS_ON_ARM_DIR/verified";
my $UBT_BIN = undef;
my $LOC_RES_DIR = undef;
my $UBT_VER_PAT = qr/uBT\s+version\s+(\d+)/;
my $FIANL_RES = 'Final_res.log';
my $DIFF_LOG = 'Diff_res.log';
my $PRINT_FLAG = undef;
my $FAIL_ARM_NUM = undef;
my @NB_COSIM_FAIL_Q = ();
my @NB_INTRP_FAIL_Q = ();
my @NB_REL_TIA_FAIL_Q = ();
my @NB_TIA_FAIL_Q = ();
my @LOC_COSIM_FAIL_Q = ();
my @LOC_INTRP_FAIL_Q = ();
my @LOC_REL_TIA_FAIL_Q = ();
my @LOC_TIA_FAIL_Q = ();
my $NB_COSIM_FAIL_NUM = ();
my $NB_INTRP_FAIL_NUM = ();
my $NB_REL_TIA_FAIL_NUM = ();
my $NB_TIA_FAIL_NUM = ();
my $LOC_COSIM_FAIL_NUM = ();
my $LOC_INTRP_FAIL_NUM = ();
my $LOC_REL_TIA_FAIL_NUM = ();
my $LOC_TIA_FAIL_NUM = ();
my $TOT_NUM = undef;
my $UBT_REV = undef;
my $QUALIFIED_NUM = undef;
my $EMPTY_RES = 'EMPTY_RES';

#-----------------------------------------------------------------------------------------------------------------------
# Main Body
#-----------------------------------------------------------------------------------------------------------------------
# Get options
GetOptions( "dir=s" => \$LOC_RES_DIR,
            "prt!" => \$PRINT_FLAG,
            "bin=s" => \$UBT_BIN)
			or die "Fail to get options!\n";

if ( not defined($LOC_RES_DIR)) {
	warn "Lack of options!
Usage: show_final_res.pl [Options]
    -d  <Necessary> the Local Results dir 
    -b  ubt binary
    -p  print diff between NB and LOC into file $DIFF_LOG
    \n";
    exit 0;
}

die "Can not find local result dir <$LOC_RES_DIR>\n" if (not -d $LOC_RES_DIR);

if (defined($UBT_BIN)) {
    my $TMP_VER = `$UBT_BIN`;
    $UBT_REV = $1 if ($TMP_VER =~ /$UBT_VER_PAT/i);
}

chdir ($LOC_RES_DIR);
die "Can not create final_res.log <$FIANL_RES>" if (not open (FI,">$FIANL_RES"));
print FI "Houdini Revision: <$UBT_REV>\n" if (defined($UBT_REV));

if ( -e $NB_TOT_LOG and open (TOT, "<$NB_TOT_LOG")) {
    while (my $LINE = <TOT>) {
        if ($LINE =~ /$NB_TOT_PAT/i) {
            $TOT_NUM = $1;
            last;
        }
    }
    close (TOT);
    print FI "Total Seed #: <$TOT_NUM>\n" if (defined($TOT_NUM));
}

if (not -d $FAIL_ON_ARM_DIR) {
    warn "Can not find dir <$FAIL_ON_ARM_DIR>!!\n";
} else {
    $FAIL_ARM_NUM = 0;
    while (glob "$FAIL_ON_ARM_DIR/*.bin") {
        $FAIL_ARM_NUM ++;
    }
    print FI "Fail_on_ARM Seed #: <$FAIL_ARM_NUM>\n";
}

if (defined($TOT_NUM) and defined($FAIL_ARM_NUM)) {
    $QUALIFIED_NUM = $TOT_NUM - $FAIL_ARM_NUM;
    print FI "Qualified Seed #: <$QUALIFIED_NUM>\n";
}

if ( -e $NB_COSIM_LOG and open (NBC, "<$NB_COSIM_LOG")) {
    &Scan_Log(*NBC,$NB_LOG_PAT,'pass',\@NB_COSIM_FAIL_Q);
    close (NBC);
    $NB_COSIM_FAIL_NUM = @NB_COSIM_FAIL_Q;
    die "Get wrong result for NB_COSIM!!\n" if ($NB_COSIM_FAIL_NUM < 0);
    print FI "NB_Cosim_Fail Seed #: <$NB_COSIM_FAIL_NUM>\n";
}

if ( -e $NB_INTRP_LOG and open (NBI, "<$NB_INTRP_LOG")) {
    &Scan_Log(*NBI,$NB_LOG_PAT,'pass',\@NB_INTRP_FAIL_Q);
    close (NBI);
    $NB_INTRP_FAIL_NUM = @NB_INTRP_FAIL_Q;
    die "Get wrong result for NB_INTRP!!\n" if ($NB_INTRP_FAIL_NUM < 0);
    print FI "NB_INTRP_Fail Seed #: <$NB_INTRP_FAIL_NUM>\n";
}

if ( -e $NB_REL_TIA_LOG and open (NBR, "<$NB_REL_TIA_LOG")) {
    &Scan_Log(*NBR,$NB_LOG_PAT,'pass',\@NB_REL_TIA_FAIL_Q);
    close (NBR);
    $NB_REL_TIA_FAIL_NUM = @NB_REL_TIA_FAIL_Q;
    die "Get wrong result for NB_REL_TIA!!\n" if ($NB_REL_TIA_FAIL_NUM < 0);
    print FI "NB_REL_TIA_Fail Seed #: <$NB_REL_TIA_FAIL_NUM>\n";
}

if ( -e $NB_TIA_LOG and open (NBT, "<$NB_TIA_LOG")) {
    &Scan_Log(*NBT,$NB_LOG_PAT,'pass',\@NB_TIA_FAIL_Q);
    close (NBT);
    $NB_TIA_FAIL_NUM = @NB_TIA_FAIL_Q;
    die "Get wrong result for NB_TIA!!\n" if ($NB_TIA_FAIL_NUM < 0);
    print FI "NB_TIA_Fail Seed #: <$NB_TIA_FAIL_NUM>\n";
}

if ( -e $LOC_COSIM_LOG and open (LCC, "<$LOC_COSIM_LOG")) {
    &Scan_Log(*LCC,$LOC_LOG_PAT,'pass',\@LOC_COSIM_FAIL_Q);
    close (LCC);
    $LOC_COSIM_FAIL_NUM = @LOC_COSIM_FAIL_Q;
    die "Get wrong result for LOC_COSIM!!\n" if ($LOC_COSIM_FAIL_NUM < 0);
    print FI "LOC_Cosim_Fail Seed #: <$LOC_COSIM_FAIL_NUM>\n";
    if (defined($QUALIFIED_NUM)) {
        my $LOC_COSIM_PASS_RATE = sprintf ("%.8f",($QUALIFIED_NUM-$LOC_COSIM_FAIL_NUM)/$QUALIFIED_NUM)*100;
        print FI "LOC_Cosim Passing Rate #: <$LOC_COSIM_PASS_RATE%>\n";
    }
}

if ( -e $LOC_INTRP_LOG and open (LCI, "<$LOC_INTRP_LOG")) {
    &Scan_Log(*LCI,$LOC_LOG_PAT,'pass',\@LOC_INTRP_FAIL_Q);
    close (LCI);
    $LOC_INTRP_FAIL_NUM = @LOC_INTRP_FAIL_Q;
    die "Get wrong result for LOC_INTRP!!\n" if ($LOC_INTRP_FAIL_NUM < 0);
    print FI "LOC_INTRP_Fail Seed #: <$LOC_INTRP_FAIL_NUM>\n";
    if (defined($QUALIFIED_NUM)) {
        my $LOC_INTRP_PASS_RATE = sprintf ("%.8f",($QUALIFIED_NUM-$LOC_INTRP_FAIL_NUM)/$QUALIFIED_NUM)*100;
        print FI "LOC_INTRP Passing Rate #: <$LOC_INTRP_PASS_RATE%>\n";
    }
}

if ( -e $LOC_REL_TIA_LOG and open (LCR, "<$LOC_REL_TIA_LOG")) {
    &Scan_Log(*LCR,$LOC_LOG_PAT,'pass',\@LOC_REL_TIA_FAIL_Q);
    close (LCR);
    $LOC_REL_TIA_FAIL_NUM = @LOC_REL_TIA_FAIL_Q;
    die "Get wrong result for LOC_REL_TIA!!\n" if ($LOC_REL_TIA_FAIL_NUM < 0);
    print FI "LOC_REL_TIA_Fail Seed #: <$LOC_REL_TIA_FAIL_NUM>\n";
    if (defined($QUALIFIED_NUM)) {
        my $LOC_REL_TIA_PASS_RATE = sprintf ("%.8f",($QUALIFIED_NUM-$LOC_REL_TIA_FAIL_NUM)/$QUALIFIED_NUM)*100;
        print FI "LOC_REL_TIA Passing Rate #: <$LOC_REL_TIA_PASS_RATE%>\n";
    }
}

if ( -e $LOC_TIA_LOG and open (LCT, "<$LOC_TIA_LOG")) {
    &Scan_Log(*LCT,$LOC_LOG_PAT,'pass',\@LOC_TIA_FAIL_Q);
    close (LCT);
    $LOC_TIA_FAIL_NUM = @LOC_TIA_FAIL_Q;
    die "Get wrong result for LOC_TIA!!\n" if ($LOC_TIA_FAIL_NUM < 0);
    print FI "LOC_TIA_Fail Seed #: <$LOC_TIA_FAIL_NUM>\n";
    if (defined($QUALIFIED_NUM)) {
        my $LOC_TIA_PASS_RATE = sprintf ("%.8f",($QUALIFIED_NUM-$LOC_TIA_FAIL_NUM)/$QUALIFIED_NUM)*100;
        print FI "LOC_TIA Passing Rate #: <$LOC_TIA_PASS_RATE%>\n";
    }
}

close (FI);

if (defined($PRINT_FLAG)) {
    die "Can not create file <$DIFF_LOG>\n" if (not open (DL, ">$DIFF_LOG"));

    for (my $i=0; $i<@NB_COSIM_FAIL_Q; $i++) {
        for (my $j=0; $j<@LOC_COSIM_FAIL_Q; $j++) {
            if ($NB_COSIM_FAIL_Q[$i] eq $LOC_COSIM_FAIL_Q[$j]) {
                $NB_COSIM_FAIL_Q[$i] = $EMPTY_RES;
                $LOC_COSIM_FAIL_Q[$j] = $EMPTY_RES;
            }
        }
    }

    print DL "[COSIM] Only Fail on NB:\n";
    foreach my $TEST (@NB_COSIM_FAIL_Q) {
        print DL $TEST,"\n" if ($TEST ne $EMPTY_RES);
    }

    print DL "[COSIM] Only Fail on LOC:\n";
    foreach my $TEST (@LOC_COSIM_FAIL_Q) {
        print DL $TEST,"\n" if ($TEST ne $EMPTY_RES);
    }

    for (my $i=0; $i<@NB_INTRP_FAIL_Q; $i++) {
        for (my $j=0; $j<@LOC_INTRP_FAIL_Q; $j++) {
            if ($NB_INTRP_FAIL_Q[$i] eq $LOC_INTRP_FAIL_Q[$j]) {
                $NB_INTRP_FAIL_Q[$i] = $EMPTY_RES;
                $LOC_INTRP_FAIL_Q[$j] = $EMPTY_RES;
            }
        }
    }

    print DL "[INTRP] Only Fail on NB:\n";
    foreach my $TEST (@NB_INTRP_FAIL_Q) {
        print DL $TEST,"\n" if ($TEST ne $EMPTY_RES);
    }

    print DL "[INTRP] Only Fail on LOC:\n";
    foreach my $TEST (@LOC_INTRP_FAIL_Q) {
        print DL $TEST,"\n" if ($TEST ne $EMPTY_RES);
    }

    for (my $m=0; $m<@NB_REL_TIA_FAIL_Q; $m++) {
        for (my $n=0; $n<@LOC_REL_TIA_FAIL_Q; $n++) {
            if ($NB_REL_TIA_FAIL_Q[$m] eq $LOC_REL_TIA_FAIL_Q[$n]) {
                $NB_REL_TIA_FAIL_Q[$m] = $EMPTY_RES;
                $LOC_REL_TIA_FAIL_Q[$n] = $EMPTY_RES;
            }
        }
    }

    print DL "[REL_TIA] Only Fail on NB:\n";
    foreach my $TEST (@NB_REL_TIA_FAIL_Q) {
        print DL $TEST,"\n" if ($TEST ne $EMPTY_RES);
    }

    print DL "[REL_TIA] Only Fail on LOC:\n";
    foreach my $TEST (@LOC_REL_TIA_FAIL_Q) {
        print DL $TEST,"\n" if ($TEST ne $EMPTY_RES);
    }

    for (my $m=0; $m<@NB_TIA_FAIL_Q; $m++) {
        for (my $n=0; $n<@LOC_TIA_FAIL_Q; $n++) {
            if ($NB_TIA_FAIL_Q[$m] eq $LOC_TIA_FAIL_Q[$n]) {
                $NB_TIA_FAIL_Q[$m] = $EMPTY_RES;
                $LOC_TIA_FAIL_Q[$n] = $EMPTY_RES;
            }
        }
    }

    print DL "[TIA] Only Fail on NB:\n";
    foreach my $TEST (@NB_TIA_FAIL_Q) {
        print DL $TEST,"\n" if ($TEST ne $EMPTY_RES);
    }

    print DL "[TIA] Only Fail on LOC:\n";
    foreach my $TEST (@LOC_TIA_FAIL_Q) {
        print DL $TEST,"\n" if ($TEST ne $EMPTY_RES);
    }

    close (DL);
}

exit 0;

#-----------------------------------------------------------------------------------------------------------------------

sub Scan_Log {
    my $LOG_FILE = shift;
    my $LOG_PAT = shift;
    my $PASS_PAT = shift;
    my $FAIL_Q = shift;

    while (my $LINE = <$LOG_FILE>) {
        if ($LINE =~ /$LOG_PAT/i) {
            my $name = $1;
            my $res = $2;
            push @$FAIL_Q, $name if ($res !~ /$PASS_PAT/i);
        }
    }
}

#-----------------------------------------------------------------------------------------------------------------------
# End of Subroutines
#-----------------------------------------------------------------------------------------------------------------------
