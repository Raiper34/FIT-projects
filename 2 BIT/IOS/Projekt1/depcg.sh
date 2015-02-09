#!/usr/bin/env bash
#Vytvoril Filip Gulán (xgulan00)
#Projekt1 IOS skript 2
#popis viz.: http://www.fit.vutbr.cz/study/courses/IOS/public/Lab/projekty/projekt1/projekt1.html

#Premenne
#priecinok=${@: -1}
premd=0 #prepinac -d
premr=0 #prepinac -r
premp=0 #prepinac -p
premg=0 #prepinac -g

#Ziskanie argumentov
while getopts :d:r:gp o
do	case "$o" in
	d) 	premd=1; subor=$OPTARG;;
	r) 	premr=1; subor=$OPTARG;;
	p) 	premp=1;;
	g) 	premg=1;;
	*) 	#echo "Use options a, b with a parameter, or c." >&2
	exit 1;;
	esac
done
((OPTIND--))
shift $OPTIND
priecinok=$* #vsetko za argumentami do premenej subor (preco som to pomenoval priecinok? :-D)
#echo "Remaining arguments: ’$*’"

#Chyba ak chyba zadany subor
if [ "$priecinok" == "" ]
	then
	echo Chyba argumentov! >&2
	exit 1
fi

##################### Iba pre prepinac -d
if [ "$premd" -eq 1 ] && [ "$premr" -eq 0 ]
	then
	if [ "$premp" -eq 1 ]
		then 
		if [ "$premg" -eq 1 ]
			then ####### -g -p -d ###########
			echo digraph CG {
			objdump -d -j .text $priecinok | sed -n '/<'$subor'>:/,/^$/p' | grep "call.*<.*>" | sed  's/.*<//' | sed 's/>.*//'| sort -u | sed 's/^/'$subor' -> /' | sed 's/@plt/_PLT/' | sed 's/$/;/' #zoberem za call/callq teda pozdovanu hodnotu + nahradi plt a doda ;
			echo }
		else ####### -p -d ############
			objdump -d -j .text $priecinok | sed -n '/<'$subor'>:/,/^$/p' | grep "call.*<.*>" | sed  's/.*<//' | sed 's/>.*//'| sort -u | sed 's/^/'$subor' -> /' #zoberem za call/callq teda pozdovanu hodnotu + nahradi plt a doda ;
		fi
	else 
		if [ "$premg" -eq 1 ]
			then ######### -g -d ##########
			echo digraph CG {
			objdump -d -j .text $priecinok | sed -n '/<'$subor'>:/,/^$/p' | grep "call.*<.*>" | sed  's/.*<//' | sed 's/>.*//'| sort -u | sed 's/^/'$subor' -> /' | sed '/@plt/d'| sed 's/$/;/' #zoberem za call/callq teda pozdovanu hodnotu + odobere plt a doda ;
			echo }
		else ######### -d ##########
			objdump -d -j .text $priecinok | sed -n '/<'$subor'>:/,/^$/p' | grep "call.*<.*>" | sed  's/.*<//' | sed 's/>.*//'| sort -u | sed 's/^/'$subor' -> /' | sed '/@plt/d' #zoberem za call/callq teda pozdovanu hodnotu + odobere plt
		fi
	fi
fi | sed '/+0x/d' #vyfiltrujem nechcene

##################### Iba pre pre Prepinac -r
if [ "$premd" -eq 0 ] && [ "$premr" -eq 1 ]
	then
	pole=($(objdump -d -j .text $priecinok | grep "<.*>:" | sed 's/.*<//' | sed 's/>.*//')) #dam si do pola mena vsetkych funkcii
	dlzkapola=${#pole[*]}
	i=0
	if [ "$premg" -eq 1 ]
		then
		echo digraph CG {
	fi
	while [ $i -lt $dlzkapola ]
		do
		objdump -d -j .text $priecinok | sed -n "/<${pole[i]}>:/,/^$/p" | grep "call.*<.*>" | sed  's/.*<//' | sed 's/>.*//' | sed "s/^/${pole[i]} -> /" #zoberiem za call/callq 
		i=`expr $i + 1`
	done | grep ".*-> $subor$" | sort -u | if [ "$premp" -eq 0 ]
		then
		sed '/@plt/d' #odstranim plt riadky
		else 
		cat -
		fi | if [ "$premg" -eq 1 ]
		then
		sed 's/@plt/_PLT/' | sed 's/$/;/' #ak -g zmenim plt a pridam ;
		else
		cat -
		fi
	if [ "$premg" -eq 1 ]
		then
		echo }
	fi
fi | sed '/+0x/d' #vyfiltrujem nechcene

###################### Ak su tam obidva argumenty -r a -d
if [ "$premd" -eq 1 ] && [ "$premr" -eq 1 ]	
	then
	echo "Argumenty su v nespravnom formate!" >&2
	exit 1
fi

###################### Ak tam neni ziadny argument -d -r hlada vsetko
if [ "$premd" -eq 0 ] && [ "$premr" -eq 0 ]	
	then
	pole=($(objdump -d -j .text $priecinok | grep "<.*>:" | sed 's/.*<//' | sed 's/>.*//')) #dam si do ola mena vsetkych funkcii
	dlzkapola=${#pole[*]}
	i=0
	if [ "$premg" -eq 1 ]
		then
		echo digraph CG {
	fi
	while [ $i -lt $dlzkapola ]
		do
		objdump -d -j .text $priecinok | sed -n "/<${pole[i]}>:/,/^$/p" | grep "call.*<.*>" | sed  's/.*<//' | sed 's/>.*//' | sed "s/^/${pole[i]} -> /"  #zoberem vsetko za call/callq
		i=`expr $i + 1`
	done | sort -u | if [ "$premp" -eq 0 ]
		then
		sed '/@plt/d' #ak prepinac -p vymazem plt riadky
		else 
		cat -
		fi | if [ "$premg" -eq 1 ]
		then
		sed 's/@plt/_PLT/' | sed 's/$/;/' #ak prepinac -g nahradim plt + pridam ;
		else
		cat -
		fi
	if [ "$premg" -eq 1 ]
		then
		echo }
	fi
fi | sed '/+0x/d' #zbavim sa nechceneho
