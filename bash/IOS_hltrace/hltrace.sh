#!/bin/ksh
export LC_ALL=C
HELP()
{
	echo "hltrace zvyrazni syntax stopy od strace.
	Pouziti: hltrace [volby] <stopa.strace >stopa.html
	Volby:
	  -s SYSCALL  Specialne zvyrazni volani SYSCALL."
	exit 1
}

HLAVICKA()
{
echo "<html>
<style>
.pid { color:darkred; }
.ts { color:navy; }
.number { color:red; }
.const { color:green; }
.string { color:blue; }
.hlcall { text-decoration:none; font-weight:bold; color:black; }
.call { text-decoration:none; color:olive; }

</style>
<body><pre>"
}
PATICKA()
{
echo "</pre></body></html>"
}

sparam=off
hparam=off

while getopts  :s:h param
do
  case $param in
	s) sparam="$OPTARG";;
	h) hparam=on;;
	\?) echo "Nespravny argument, precitajte si napovedu:" 1>&2; HELP; exit 1;;
	:)  echo "Chyba argument" 1>&2; exit 1;;
  esac
done

# napoveda
if [ $hparam = on ];then
HELP
fi

# program
HLAVICKA

sed "s/&/\&amp;/g" |
sed "s/</\&lt;/g" |
sed "s/>/\&gt;/g" |
sed 's/\\\"/\\\&quot\;/g' |

sed -r "s/\"((\\\")*[^\"]*)\"/<span class=\"string\">\"\1\"<\/span>/g" |
sed -r "s/([A-Z]+[A-Z0-9_]+)/<span class=\"const\">\1<\/span>/g" |
sed -r "s/([0-9]+)/<span class=\"pid\">\1<\/span>/" |
sed -r "s/([0-9]{1,}[\.]{1}[0-9]{2,})/<span class=\"ts\">\1<\/span>/" |

sed -r "s/${sparam}[(]/<a href=\"http\:\/\/www\.kernel\.org\/doc\/man-pages\/online\/pages\/man2\/${sparam}\.2\.html\" class=\"hlcall\">${sparam}<\/a>(/" |
sed -r "s/([a-z0-9_]{2,})[(]/<a href=\"http\:\/\/www\.kernel\.org\/doc\/man-pages\/online\/pages\/man2\/\1\.2\.html\" class=\"call\">\1<\/a>(/" |

sed -r "s/([([:space:]]{1})([-]{0,1}[0-9]+[x]{0,1}[0-9a-f]{0,})([),]{1})/\1<span class=\"number\">\2<\/span>\3/g" |

sed -r "s/= ([-]{0,1}[0-9]+[x]{0,1}[0-9a-f]{0,})/= <span class=\"number\">\1<\/span>/g" |

sed 's/\&quot\;/\"/g'
PATICKA

exit 0
