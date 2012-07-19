#!/usr/bin/perl

#CSV:xblask00

use strict;

use CSV2XML;
use Getopt::Long qw(:config gnu_compat no_auto_abbrev);
use Switch;

BEGIN { $SIG{'__WARN__'} = sub { }}

# Help text
sub print_help
{
	my $help = "";
	$help .= "CSV2XML - Branislav Blaskovic xblask00\n";
	$help .= "  -- help            napoveda\n";
	$help .= "  --input=soubor     vstupni soubor\n";
	$help .= "  --output=soubor    vystupni soubor\n";
	$help .= "  -n                 negenerovat XML hlavicku\n";
	$help .= "  -r=nazev           jmeno korenoveho paroveho elementu\n";
	$help .= "  -s=separator       nazev separatoru\n";
	$help .= "  -h                 prvni radek slouzi jako hlavicka\n";
	$help .= "  -l=nazev           jmeno radkoveho elementu\n";
	$help .= "  -i                 atribut index\n";
	$help .= "  -start=n           zacatek indexu\n";
	$help .= "  -e                 zotaveni z chybniho poctu sloupcu\n";
	$help .= "  --missing-value=s  doplneny text do prazdnej bunky\n";
	$help .= "  --all-columns      sloupce navyse nejsou ignorovany, ale jsou vlozeny\n";
	print $help;
}

# Error texts
sub write_error_message
{
	my($error) = @_;

	switch($error)
	{
		case 1  { print STDERR "Err: Spatne parametry\n"; }
		case 2  { print STDERR "Err: Chyba vstupniho souboru\n"; }
		case 3  { print STDERR "Err: Chyba vystupniho souboru\n"; }
		case 30 { print STDERR "Err: Nevalidni znacka\n"; }
		case 31 { print STDERR "Err: Nevalidni root elem\n"; }
		case 32 { print STDERR "Err: Spatny pocet stloupcu\n"; }
	}
}


# arguments
my $help = 0;
my @input;
my @output;
my $disable_head = 0;
my @root_element;
my @separator;
my $heading = 0;
my @line_element;
my $index = 0;
my $start_index;
my $error_recovery = 0;
my @missing_value;
my $all_columns = 0;
my $num_of_args = scalar(@ARGV);
my $error;

GetOptions (
	'help+' => \$help, 
	'input:s' => \@input, 
	'output:s' => \@output, 
	'n+' => \$disable_head,
	'r:s' => \@root_element,
	's:s' => \@separator,
	'h+' => \$heading,
	'l:s' => \@line_element,
	'i+' => \$index,
	'start:i' => \$start_index,
	'e|error-recovery+' => \$error_recovery,
	'missing-value:s' => \@missing_value,
	'all-columns+' => \$all_columns,
) or exit 1;

if($help == 1)
{
	if($num_of_args != 1) { write_error_message(1); exit 1; }
	print_help();
	exit 0;
}


# Double args aid
if(
	$help > 1 || $disable_head > 1 || $heading > 1 || $index > 1 || $error_recovery > 1 || $all_columns > 1 ||
	scalar(@input) > 1 || scalar(@output) > 1 || scalar(@root_element) > 1 || scalar(@separator) > 1 || scalar(@line_element) > 1 || scalar(@missing_value) > 1
	)
	{
		write_error_message(1);
		exit 1;
	}

my $my_csv = new CSV2XML();

# Set my library options
if($heading == 1) { $my_csv->set_h(); }
if($disable_head == 1) { $my_csv->disable_head(); }
if($index == 1) { $my_csv->set_index(); }
if($error_recovery == 1) { $my_csv->set_error_recovery(); }
if($all_columns == 1) { $my_csv->set_all_columns(); }
if(defined(@root_element[0])) { $my_csv->set_root_element(@root_element[0]); }
if(defined(@line_element[0])) { $my_csv->set_line_element(@line_element[0]); }
if(defined($start_index)) { $my_csv->set_start_index($start_index); }
if(defined(@missing_value[0])) { $my_csv->set_missing_value(@missing_value[0]); }
if(defined(@separator[0])) { $my_csv->set_separator(@separator[0]); }

$my_csv->set_input_file(@input[0]);
$my_csv->set_output_file(@output[0]);

# Validate input data and prepare it
if(($error = $my_csv->validate()) != 0)
{
	write_error_message($error);
	exit $error;
}

# Read file and preparing for dispatching
if(($error = $my_csv->read_file()) != 0)
{
	write_error_message($error);
	exit $error;
}

# Writing the output
$my_csv->print_output();
exit 0;
