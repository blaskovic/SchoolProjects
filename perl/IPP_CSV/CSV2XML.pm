#!/usr/bin/perl

#CSV:xblask00

package CSV2XML;

use strict;
use Text::CSV;
use XML::Writer;

# Error codes constants
use constant {
	ERR_OK => 0,
	ERR_ARGS => 1,
	ERR_IN_FILE => 2,
	ERR_OUT_FILE => 3,
	ERR_BAD_FORMAT => 4,
	ERR_VALIDITY => 30,
	ERR_HEADER => 31,
	ERR_COLUMNS => 32,
};

sub new
{
	my $class = shift;
	my $self = {
		padding_width => 2,
		output_text => "",
		print_head => 1,
		separator => ",",
	};

	bless $self, $class;
	return $self;
}

sub set_h
{
	my($self) = @_;
	$self->{is_h} = 1;
}

sub set_input_file
{
	my($self, $file_name) = @_;
	$self->{input_file} = $file_name;
}

sub set_output_file
{
	my($self, $file_name) = @_;
	$self->{output_file} = $file_name;
}

sub disable_head
{
	my($self) = @_;
	$self->{print_head} = 0;
}

sub set_error_recovery
{
	my($self) = @_;
	$self->{error_recovery} = 1;
}

sub set_all_columns
{
	my($self) = @_;
	$self->{all_columns} = 1;
}

sub set_start_index
{
	my($self, $start_index) = @_;
	$self->{start_index} = $start_index;
}

sub set_index
{
	my($self) = @_;
	$self->{index} = 1;
}

sub set_missing_value
{
	my($self, $missing_value) = @_;
	$self->{missing_value} = $missing_value;
}

sub set_separator
{
	my($self, $separator) = @_;
	# Tabulator?
	if($separator eq "TAB") { $separator = "\t"; }
	if($separator eq "SPACE") { $separator = " "; }
	if($separator eq ";") { $separator = "\;"; }
	$self->{separator} = $separator;
}

sub set_root_element
{
	my($self, $root_element) = @_;
	$self->{root_element} = $root_element;
}

sub set_line_element
{
	my($self, $line_element) = @_;
	$self->{line_element} = $line_element;
}

sub validate
{
	my($self) = @_;
	my $error = ERR_OK;

	# valid root element
	if(defined($self->{root_element}) && !is_valid_tag($self, $self->{root_element})) { return ERR_VALIDITY; }
	if(defined($self->{line_element}) && !is_valid_tag($self, $self->{line_element})) { return ERR_VALIDITY; }
	
	# if -i, we need -l
	if(defined($self->{index}) && !defined($self->{line_element})) { $error = ERR_ARGS; }
	# if --start, we nee -i
	if(defined($self->{start_index}) && !defined($self->{index})) { $error = ERR_ARGS; }
	# if --missing-val, we need -e
	if(defined($self->{missing_value}) && !defined($self->{error_recovery})) { $error = ERR_ARGS; }
	# if --all-columns, we need -e 
	if(defined($self->{all_columns}) && !defined($self->{error_recovery})) { $error = ERR_ARGS; }
	
	# Default values
	if(!defined($self->{line_element})) { $self->{line_element} = "row"; }
	if(!defined($self->{start_index})) { $self->{start_index} = 1; }

	# Start index offset
	if($self->{is_h}) { $self->{start_index}--; }

	return $error;
}

sub validate_tag
{
	my($self, $text) = @_;
	utf8::decode($text);
	
	$text =~ s/[^:\-\w\x{C0}-\x{D6}\x{D8}-\x{F6}\x{F8}-\x{2FF}\x{370}-\x{37D}\x{37F}-\x{1FFF}\x{200C}-\x{200D}\x{2070}-\x{218F}\x{2C00}-\x{2FEF}\x{3001}-\x{D7FF}\x{F900}-\x{FDCF}\x{FDF0}-\x{FFFD}\x{10000}-\x{EFFFF}\x{C0}-\x{D6}\x{D8}-\x{F6}\x{F8}}\.0-9\x{B7}\x{0300}\x{036F}\x{203F}-\x{2040}]/-/g;

	utf8::encode($text);

	return $text;
}

sub is_valid_tag
{
	my($self, $text) = @_;

	utf8::decode($text);

	my $first = substr($text, 0, 1);
	my $other = substr($text, 1);


	if($text =~ m/[<>]+/) { return 0; }
	if($first =~ m/[:_a-zA-Z0-9\x{C0}-\x{D6}\x{D8}-\x{F6}\x{F8}-\x{2FF}\x{370}-\x{37D}\x{37F}-\x{1FFF}\x{200C}-\x{200D}\x{2070}-\x{218F}\x{2C00}-\x{2FEF}\x{3001}-\x{D7FF}\x{F900}-\x{FDCF}\x{FDF0}-\x{FFFD}\x{10000}-\x{EFFFF}]/i)
	{
		if($other =~ m/[:a-zA-Z\-\.0-9\x{C0}-\x{D6}\x{D8}-\x{F6}\x{F8}-\x{2FF}\x{370}-\x{37D}\x{37F}-\x{1FFF}\x{200C}-\x{200D}\x{2070}-\x{218F}\x{2C00}-\x{2FEF}\x{3001}-\x{D7FF}\x{F900}-\x{FDCF}\x{FDF0}-\x{FFFD}\x{10000}-\x{EFFFF}\x{B7}\x{0300}-\x{036F}\x{203F}-\x{2040}]*/i)
		{
			return 1;
		}
	}

	return 0;
}

sub print_output
{
	my($self) = @_;

	$self->{xml_writer}->end();
	close $self->{output_handler};
}

sub read_file
{
	my($self) = @_;
	my $csv = Text::CSV->new ({
		quote_char          => '"',
		escape_char         => '"',
		sep_char            => $self->{separator},
		eol                 => "\015\012",
		always_quote        => 0,
		quote_space         => 1,
		#quote_null          => 1,
		binary              => 1,
		keep_meta_info      => 0,
		allow_loose_quotes  => 1,
		allow_loose_escapes => 0,
		allow_whitespace    => 0,
		blank_is_undef      => 0,
		empty_is_undef      => 0,
		verbatim            => 0,
		auto_diag           => 0,
	});

	my $file;

	if(!defined($self->{input_file}))
	{
		open $file, "<-" or return ERR_IN_FILE;
	}
	else
	{
		open $file, "<:encoding(utf8)", $self->{input_file} or return ERR_IN_FILE;
	}

	
	# XML Writer 
	if(!defined($self->{output_file}))
	{
		$self->{xml_writer} = XML::Writer->new(
			DATA_INDENT => $self->{padding_width},
			DATA_MODE => 1,
			UNSAFE => 1
		);
	}
	else
	{
		open $self->{output_handler}, ">", $self->{output_file} or return ERR_OUT_FILE;
		$self->{xml_writer} = XML::Writer->new(
			DATA_INDENT => $self->{padding_width},
			DATA_MODE => 1,
			UNSAFE => 1,
			OUTPUT => $self->{output_handler}
		);
	}

	if($self->{print_head})
	{
		$self->{xml_writer}->xmlDecl("UTF-8");
	}

	my $row = 0;
	my $actual_row = "";
	my $head;
	my $column_text;
	my $padding_offset;
	my $line_element_text;
	my $num_of_columns = 0;
	my $column_num;
	my @head;

	# Root element
	if(defined($self->{root_element}))
	{
		$self->{xml_writer}->startTag($self->{root_element});
	}
	
	my $line;
	while($line = $csv->getline($file))
	{
		push my @columns, @$line; 
		
		$column_num = 0;
		$actual_row = "";

		if(!($self->{is_h} == 1 && $row == 0))
		{
			if(defined($self->{index}))
			{
				$self->{xml_writer}->startTag($self->{line_element}, 'index' => ($self->{start_index} + $row));
			}
			else
			{
				$self->{xml_writer}->startTag($self->{line_element});
			}
		}

		# Go through all columns
		foreach my $column (@columns)
		{
			if($self->{is_h} == 1 && $row == 0)
			{
				# Read head - first row
				$column = validate_tag($self, $column);
				if(!is_valid_tag($self, $column)) { return ERR_HEADER; }
				$head[$column_num] = $column;
			}
			else
			{
				# Set name of tag
				if($self->{is_h} == 1)
				{
					$column_text = $head[$column_num];
				}
				else
				{
					$column_text = "col" . ($column_num+1);
				}

				# Set text to tag
				if(!($column_num >= $num_of_columns && defined($self->{error_recovery})) || $row == 0 || defined($self->{all_columns}))
				{
					if($column_num >= $num_of_columns) { $column_text = "col" . ($column_num+1); }
					$self->{xml_writer}->startTag($column_text);
					$self->{xml_writer}->characters($column);
					$self->{xml_writer}->endTag($column_text);
				}
			}
			
			# Get num of columns of first row
			if($row == 0)
			{
				$num_of_columns++;
			}

			$column_num++;

		} # End column cycle

		# On not first row, check errors
		if($row != 0)
		{
			# If havent got error-recovery
			if(!defined($self->{error_recovery}) && $column_num != $num_of_columns)
			{
				return ERR_COLUMNS;
			}

			# If we've got error-recovery, add some columns (with missing-vals, if specified)
			if(defined($self->{error_recovery}) && $num_of_columns > $column_num)
			{
				for(my $i = $column_num; $i < $num_of_columns; $i++)
				{
					if($self->{is_h} == 1)
					{
						$column_text = $head[$i];
					}
					else
					{
						$column_text = "col" . ($i+1);
					}
					$self->{xml_writer}->startTag($column_text);
					$self->{xml_writer}->characters($self->{missing_value});
					$self->{xml_writer}->endTag($column_text);
				}
			}
		} # End if $row != 0
		
		if(!($self->{is_h} == 1 && $row == 0))
		{
			$self->{xml_writer}->endTag($self->{line_element});
		}

		$row++;

	} # End line cycle

	# Root element
	if(defined($self->{root_element}))
	{
		$self->{xml_writer}->endTag($self->{root_element});
	}

	close CSV;
	
	return ERR_OK;
}

1;
