use strict;
#$L=<>;
#print $:L;

#open $F, "<", $ARGV[0];
our $R=<>;
chomp $R;
my @heading=split /\t/, $R;
$R=<>;
chomp $R;
our @format=split /\t/, $R;

#open $F, "<", $ARGV[0];
my $R=<>;

my $s_heading="";
my $s_header='\begin{tabular}{';
for my $i ( 0 .. $#format ) {
	my $s=$format[$i];
	if ($s=~/^[0-9]+[.][0-9]*$/) {
		$format[$i]="f";
		$s_heading.="\\multicolumn{2}{|c|}{$heading[$i]}&";
		$s_header.='|r@{.}l'
	#} elsif ($s=~/^[0-9]+$/)) {
	} else {
		$format[$i]='s';
		$s_heading.="$heading[$i]&";
		$s_header.="|l"
	}
}
$s_header.="|}\n";
print $s_header;
$s_heading=~s/_/\\_/;
$s_heading=~s/.$/\\\\\n/;
print "\\hline\n";
print $s_heading;
print "\\hline\n";
sub doline {
	my $W=$R;
	chomp $W;

	my $zeros="000";
	$W=~s/\b([0-9]+)[.]([0-9]{1,3})[0-9]+\b/\1.\2$zeros/g;
	$W=~s/\b([0-9]+)[.]([0-9]{1,3})[0-9]+\b/\1&\2/g;

	$W=~s/\t/ & /g;
	$W=~s/_/\\_/g;
	#$W=~s/$/\\\\/g;
	print "$W\\\\\n";
#	my @W=split /\t/, $R;
#	for my $i ( 0 .. $#format ) {
#		if ($format[$i]=='f') {
#			
#				
#		}
#	}
}

doline;
while ($R=<>) {
	doline;
}
print "\\hline\n";
print "\\end{tabular}\n";
