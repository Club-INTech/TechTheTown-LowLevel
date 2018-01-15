#! /bin/bash

function pythonGraph {
	echo "DATAEND" >> "$3"
	python3 /home/trotfunky/Programmation/Git/Python/extraction_asserv.py "$2" "$1"
	exit
}

trap "pythonGraph $1 $outFile $fileName" INT
trap "pythonGraph $1 $outFile $fileName" TERM

stty -F /dev/ttyACM0 115200 raw -echo -echoe -echok

if [ $# -eq 3 ]; then
	fileName="./serialOutput/serialOutput-"$1","$2","$3".csv"
else
	fileName="./serialOutput/serialOutput.csv"
fi

if [ -e "$fileName" ]; then
	fileName=".""$(echo $fileName | cut -f 2 -d".")""-$(date +'%Y-%m-%d @ %Hh%M').csv"
fi

touch "$fileName"
outFile=$(echo $fileName | cut -f 3 -d"/")


while true; do
	read line < /dev/ttyACM0
	echo $line >> "$fileName"
	case "$line" in 
   		*"DATAEND"*)
   			 break;;
	esac
done

python3 /home/trotfunky/Programmation/Git/Python/extraction_asserv.py "$outFile" "$1"