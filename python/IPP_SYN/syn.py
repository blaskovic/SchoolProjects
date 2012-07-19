 #!/usr/bin/env python3
import re, argparse, codecs, sys
#SYN:xblask00

#
# Help string
#
HELP_STR = """
SYN - zvyrazneni syntaxe
	--input VSTUPNI_SOUBOR
	--output VYSTUPNI_SOUBOR
	--format FORMATOVACI_SOUBOR
	--br zalomovani radku
	--help tisk napovedy

Vytvoril Branislav Blaskovic (c) xblask00@stud.fit.vutbr.cz
"""

#
# My exceptions
#
class MyError(Exception):
	def __init__(self, value):
		self.value = value

	def __str__(self):
		return repr(self.value)

	def process_error(self):
		if self.value == "format":
			print("Error: format", file=sys.stderr)
			exit(4)
		elif self.value == "tag":
			print("Error: unknown tag", file=sys.stderr)
			exit(4)
		else:
			print("Error: unknown", file=sys.stderr)
			exit(100)

#
# Look for regex in text (return array)
#
def find_regular(text, regex_list):
	indexes = []
	for i in range(len(text)+1):
		indexes.append([-1, ""])

	for regex, tag in regex_list:
		for m in re.finditer(str(regex), text, re.DOTALL):
			if m.start() == m.end():
				continue
			indexes[m.start()][0] = m.start()
			indexes[m.start()][1] = indexes[m.start()][1] + get_tags(tag)
			indexes[m.end()][0] = m.end()
			indexes[m.end()][1] = get_tags(tag, False) + indexes[m.end()][1] 

	return indexes

#
# Get html tags from settings
#
def get_tags(settings, start = True):
	tags = ""
	for setting in settings.split(","):
		write = get_tag_from_setting(setting)
		if write == "":
			continue
		if start:
			tags += "<" + write + ">"
		else:
			tags = "</" + write.split(" ")[0] + ">" + tags
	return tags

#
# Get tag from setting
#
def get_tag_from_setting(setting):
	setting = setting.rstrip().lstrip()
	if setting == "italic":
		return "i"
	elif setting == "teletype":
		return "tt"
	elif setting == "underline":
		return "u"
	elif setting == "bold":
		return "b"
	elif re.match("color:[0-9A-Fa-f]{6}", setting):
		parts = setting.split(":")
		return "font color=#" + str(parts[1])
	elif re.match("size:[1-7]", setting):
		parts = setting.split(":")
		return "font size=" + str(parts[1])
	elif setting == "":
		return ""
	else:
		raise MyError("tag")

#
# Increment all indexes
#
def increment_list(list, increment, start = 0):
	for i in range(start, len(list)):
		if list[i][0] == -1:
			continue
		list[i][0] = list[i][0] + increment

#
# Insert tags
#
def insert_tags(list, text):
	for i in range(len(list)):
		index, tag = list[i]
		if index == -1:
			continue
		text = text[:index] + tag + text[index:]
		increment_list(list, len(tag), i)

	return text

#
# Converts regex to python-like regex
#
def convert_regex(file):
	regex_list = []

	for line in file.splitlines():
		line = line.replace("\n$", "")
		try:
			regex, sep, tag = line.partition("\t")
			if sep == "":
				raise MyError("format")
		except:
			raise MyError("format")
		# Per cents
		count = 0
		new_regex = ""
		neg = ""
		last_special = ""
		last_bracket = ""
		# print("parse: " + regex)
		for i in range(len(regex)):
			# %
			if regex[i] == "%":
				if count == 1:
					new_regex += "%"
					count = 0
				else:
					count += 1
				last_special = ""
			# Another
			else:
				if count == 1:
					if regex[i] in "sadlLwWtn.|!*+()":
						if regex[i] == "w":
							new_regex += "["+neg+"a-zA-Z]"
						elif regex[i] == "W":
							new_regex += "["+neg+"a-zA-Z0-9]"
						elif regex[i] == "l":
							new_regex += "["+neg+"a-z]"
						elif regex[i] == "L":
							new_regex += "["+neg+"A-Z]"
						elif regex[i] == "t":
							new_regex += "["+neg+"\t]"
						elif regex[i] == "n":
							new_regex += "["+neg+"\n]"
						elif regex[i] == "s":
							new_regex += "["+neg+" \t\n\r\f\v]"
						elif regex[i] == "d":
							new_regex += "["+neg+"0-9]"
						elif regex[i] == "a":
							if neg != "^":
								new_regex += "."
						elif regex[i] in ".|!*+()":
							new_regex += "\\"+regex[i]
						else:
							raise MyError("format")
						neg = ""
					else:
						raise MyError("format")
					last_special = ""
					last_bracket = ""
				else:
					ins = regex[i]
					# More than one special char is not allowed
					if ins in "+*|.!":
						# Two equal special chars in a row? Impossibru!
						if last_special == ins:
							raise MyError("format")
						# Some special sequences
						if last_special != "":
							if last_special in ".|" and ins in ".|":
								raise MyError("format")
							if ins in "+*" and last_special in "|.":
								raise MyError("format")
						# These men cannot be next to left bracket
						if ins in "+*|." and last_bracket == "(":
							raise MyError("format")
						last_special = ins
					else:
						if ins == ")" and last_special in ".|" and last_special != "":
							raise MyError("format")
						last_special = ""
					# Escape vulnerable chars
					if ins in "[]^\\{}$?":
						ins = "\\" + ins
					# Negotion
					if regex[i] == "!":
						neg = "^"
					# We don't want dots
					elif neg != "^" and regex[i] != ".":
						new_regex += ins
						if ins in "()":
							if last_bracket == "(" and ins == ")":
								raise MyError("format")
							last_bracket = ins
						else:
							last_bracket = ""
					# Not correct regex
					elif neg == "^" and regex[i] in "+*|.":
						raise MyError("format")
					# Cannot negate everything
					elif neg == "^" and ins in "()+*|.":
						raise MyError("format")
					# Negate char
					elif neg == "^":
						new_regex += "[^" + ins + "]"
						neg = ""

				count = 0
				if not regex[i] in "()":
					last_bracket = ""

		try:
			re.compile(new_regex)
		except:
			raise MyError("format")
		regex_list.append([new_regex, tag])

	return regex_list
#
# Arguments
#
parser = argparse.ArgumentParser(description='Syntax highlighter', add_help = False)
parser.add_argument('--input', action="store", dest='input_file', default=False)
parser.add_argument('--output', action="store", dest='output_file', default=False)
parser.add_argument('--format', action="store", dest='format_file', default=None)
parser.add_argument('--br', action="store_true", dest='break_line', default=False)
parser.add_argument('--help', action="store_true", dest='help', default=False)
try:
	results = parser.parse_args()
except:
	print("Error: bad arguments", file=sys.stderr)
	exit(1)

#
# Want help?
#
if results.help:
	if len(sys.argv) != 2:
		print("Error: bad arguments", file=sys.stderr)
		exit(1)
	print(HELP_STR.rstrip().lstrip())
	exit(0)


#
# Read files
#
try:
	if not results.input_file:
		input_content = sys.stdin.read()
	else:
		input_fd = codecs.open(results.input_file, 'r', 'utf-8')
		input_content = input_fd.read()
		input_fd.close()
except:
	print("Error: bad input file", file=sys.stderr)
	exit(2)

try:
	format_fd = codecs.open(results.format_file, 'r', 'utf-8')
	format_content = format_fd.read()
except:
	format_content = ""

#
# Parse files
#
try:
	regex_list = convert_regex(format_content)
	indexes = find_regular(input_content, regex_list)
	input_content = insert_tags(indexes, input_content)
except MyError as e:
	e.process_error()
	traceback.print_exc(file=sys.stdout)

# Want break-lines?
if results.break_line:
	input_content = input_content.replace("\n", "<br />\n")

#
# Print it
#
try:
	if not results.output_file:
		sys.stdout.write(input_content)
	else:
		output_fd = codecs.open(results.output_file, 'w', 'utf-8')
		output_fd.write(input_content)
		output_fd.close()
except:
	print("Error: bad output file", file=sys.stderr)
	exit(3)
