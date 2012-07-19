#!/bin/ksh

# Branislav Blaskovic - xblask00 - 132966 - 2011

HELP()
{
echo "ardiff vypisuje zmeny archivu, vytvari rozdilovy archiv nebo aplikuje rozdilovy
archiv na zdrojovy archiv.
Pouziti: ardiff [volby] archiv1 archiv2
Volby:
  -o SOUBOR  Pokud je cilem skriptu vytvorit archiv, bude vytvoren do souboru
             se jmenem SOUBOR (plati pro -c a -p).
  -l         Vypis seznamu souboru, ktere se v zadanych archivech lisi.
  -c         Vytvoreni rozdiloveho archivu.
  -p         Aplikace rozdiloveho archivu (argument archiv2) na zdrojovy archiv
             (argument archiv1).
  -r         Prepnuti do reverzniho rezimu (plati pro -p)."
exit 1
}

UPRATUJ()
{
	cd ..
	if [ -d "$1" ];then
		zmaz="$1/"
		rm -fr ${zmaz}
	fi
}

ROZBAL()
{
	rozbalilsa=off
	tempik=`pwd`
	cd "$4"
	typ1=`file -h "$1"`
	cd "$tempik"
	
	hlavicka=`echo "$typ1" | expand | tr -s " " | sed "s/${1}: //"`

	# tar
	if [ `echo "$hlavicka" | grep -ic ^"POSIX tar archive"` -eq 1 ];then
		tar -xf "$4/$1" -C "$2"
		rozbalilsa=on
	fi
	# tgz
	if [ `echo "$hlavicka" | grep -ic ^"gzip compressed data"` -eq 1 ];then
		tar -xzf "$4/$1" -C "$2"
		rozbalilsa=on
	fi
	# zip
	if [ `echo "$hlavicka" | grep -ic ^"ZIP archive"` -eq 1 ];then
		unzip -q "$4/$1" -d "$2"
		rozbalilsa=on
	fi
	# tar.bz2
	if [ `echo "$hlavicka" | grep -ic ^"bzip2 compressed data"` -eq 1 ];then
		tar -xjf "$4/$1" -C "$2"
		rozbalilsa=on
	fi
	
	# nepodporovany archiv
	if [ $rozbalilsa = off ];then
		echo "Nepodporovany archiv: $1" 1>&2
		UPRATUJ
		exit 1
	fi
}

SUBORY()
{
	obsah=`find "./$1" -type f`

	arr=$(echo $obsah)

	for x in $arr
	do
		echo "$x" | sed -r "s/.\/${1}[/]{0,1}//"
	done
}

ZABAL()
{
	# zistenie pripony
	# .tar -> POSIX tar
	# .tgz nebo .tar.gz -> gzip zabaleny POSIX tar
	# .tar.bz2 -> bzip2 zabalený POSIX tar
	# .zip nebo .ZIP -> Zip archiv
	pripona=`echo $oparam | tail -c -4`
	
	cd "$1"
	rm -rf "$pracovnyadresar/$oparam"
	
	if [ $pripona = "tar" ];then
		tar -r -f "$pracovnyadresar/$oparam" *

	elif [ $pripona = "tgz" ] || [ $pripona = ".gz" ];then
		tar -czf "$pracovnyadresar/$oparam" *
	
	elif [ $pripona = "bz2" ];then
		tar -cjf "$pracovnyadresar/$oparam" *
	elif [ $pripona = "zip" ] || [ $pripona = "ZIP" ];then
		zip -q -r "$pracovnyadresar/$oparam" *
	fi
	
	cd ..
}

PATCHUJ()
{
	
	for x in $1
	do
		adresar=${x%\/*}
	
		if [ -d "a/$x" ];then
			pom="$pom
$x"
		else
			
			if [ ! -f "a/$x" ];then
				
				if [ -f "a/$adresar" ];then
					pom="$pom
$x"
				else
					mkdir -p "$adresar"
					touch "a/$x" 2> "/dev/null"
					if [ $rparam = on ];then
						patch -s -u -f -R "a/$x" -i "b/$x.patch"
					else 
						patch -s -u -f "a/$x" -i "b/$x.patch"
					fi 
					
				fi
				
			else
				if [ $rparam = on ];then
					patch -s -u -f -R "a/$x" -i "b/$x.patch"
				else 
					patch -s -u -f "a/$x" -i "b/$x.patch"
				fi 
			fi
			
		fi
		
	done
	echo "$pom"
}

lparam=off
cparam=off
pparam=off
rparam=off
oparam=off

while getopts  :lo:cpr param
do
  case $param in
	l) lparam=on;;
	o) oparam="$OPTARG";;
	c) cparam=on;;
	p) pparam=on;;
	r) rparam=on;;
	\?) echo "Nespravny argument, precitajte si napovedu:" 1>&2; HELP; exit 1;;
	:)  echo "Chyba argument" 1>&2; exit 1;;
  esac
done

# osetrenie zlych parametrov
if [ $rparam = on ];then
	if [ $pparam = off ];then
		HELP
	fi
fi
if [ $cparam = on ] || [ $pparam = on ];then
	if [ $oparam = off ];then
		HELP
	fi
fi
if [ $oparam != off ];then
	if [ $cparam = off ] && [ $pparam = off ];then
		HELP
	fi 
fi
if [ $cparam = on ] && [ $pparam = on ];then
	HELP
fi


# dame prec parametre
shift $(($OPTIND-1));

# zly pocet archivov = DOROBIT

# ziadne argumenty = napoveda
if [ $# = 0 ];then
HELP
fi

archiv1=$1
archiv2=$2
# vytvorenie tmp
tempdir=`mktemp -d /tmp/ardiff.XXXXX`
pracovnyadresar=`pwd`
cd $tempdir
mkdir "a"
mkdir "b"
mkdir "pom"

# OSETRIT !!!
trap 'UPRATUJ "$tempdir"; echo "Necakane ukoncenie" 1>&2' INT TERM HUP

# existuju ?

if [ -f "$pracovnyadresar/$archiv1" ];then
	ROZBAL "$archiv1" "a" "$tempdir" "$pracovnyadresar"
else
	echo "$archiv1 nie je korektny subor"
	UPRATUJ "$tempdir"
	exit 1
fi
if [ -f "$pracovnyadresar/$archiv2" ];then
	ROZBAL "$archiv2" "b" "$tempdir" "$pracovnyadresar"
else
	echo "$archiv2 nie je korektny subor"
	UPRATUJ "$tempdir"
	exit 1
fi

# -l = Vypis seznamu souboru, ktere se v zadanych archivech lisi.
if [ $lparam = on ];then

	prve=`SUBORY "a"`
	druhe=`SUBORY "b"`
	vystup=""
	
	# prvy prechod
	arr=$(echo $prve)
	for x in $arr
	do
		jetam=off
		arr2=$(echo $druhe)
		for x2 in $arr2
		do			
			if [ "$x" = "$x2" ];then
				obsah1=`cat "a/$x"`
				obsah2=`cat "b/$x2"`
				if [ "$obsah1" = "$obsah2" ];then
					jetam=on
				fi
			fi
		done
		if [ $jetam = off ];then
		
			if [ "$vystup" = "" ];then
				vystup="$x"
			else
				vystup="$vystup
$x"
			fi
		fi
	done
	
	# druhy prechod
	arr=$(echo $druhe)
	for x in $arr
	do
		jetam=off
		arr2=$(echo $prve)
		for x2 in $arr2
		do			
			if [ "$x" = "$x2" ];then
				obsah1=`cat "b/$x"`
				obsah2=`cat "a/$x2"`
				if [ "$obsah1" = "$obsah2" ];then
					jetam=on
				fi
			fi
		done
		if [ $jetam = off ];then
			if [ "$vystup" = "" ];then
				vystup="$x"
			else
				vystup="$vystup
$x"
			fi
		fi
	done
	
	# vypisanie rozdielu
	if [ "$vystup" != "" ];then
		echo "$vystup" | sort | uniq
	fi
	
	

	
elif [ $cparam = on ];then
	
	cd $pracovnyadresar
	rozdielneSubory=`$pracovnyadresar/ardiff -l $archiv1 $archiv2`
	
	if [ $? -ne 0 ];then
		UPRATUJ $tempdir
		HELP
	fi
	cd $tempdir
	
	for x in $rozdielneSubory
	do
		adresar=${x%\/*}
		if [ $adresar != $x ];then
			mkdir -p "vystup/$adresar"
		fi
		
		if [ ! -f "a/$x" ];then

			mkdir -p "pom/b/$adresar"
			
			cp "b/$x" "pom/b/$x"
			
			cd pom
			diff -u -N -a "a/$x" "b/$x" > "$tempdir/vystup/$x.patch"
			rm -rf "b"
			cd ..
			
		elif [ ! -f "b/$x" ];then
			
			mkdir -p "pom/a/$adresar"
			
			cp "a/$x" "pom/a/$x"
			
			cd pom
			diff -u -N -a "a/$x" "b/$x" > "$tempdir/vystup/$x.patch"
			rm -rf "a"
			cd ..
			
		else
			diff -u -N -a "a/$x" "b/$x" > "vystup/$x.patch"
		fi
		
	done
	
	ZABAL "vystup"
	
elif [ $pparam = on ];then

	# aplikacia zmien
	subory=`SUBORY b | sed 's/.patch$//' | sort -r`
		
	zvysok=`PATCHUJ "$subory"`
	
	PATCHUJ "$zvysok" > "/dev/null" # pevne dufam, ze 3 prechody neni treba, rekurzie som sa mierne obaval
	
	ZABAL "a"

else
	UPRATUJ $tempdir
	HELP
fi

# zmazanie tmp - upratovanie
UPRATUJ	$tempdir

exit 0



