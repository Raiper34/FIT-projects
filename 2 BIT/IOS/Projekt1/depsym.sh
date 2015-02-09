#!/usr/bin/env bash
#Vytvoril Filip Gulán (xgulan00)
#Projekt1 IOS skript 2
#popis viz: http://www.fit.vutbr.cz/study/courses/IOS/public/Lab/projekty/projekt1/projekt1.html

#Premenne
#priecinok=${@: -1}
premd=0
premr=0
premg=0

#Ziskanie argumentov
while getopts :d:r:gp o
do	case "$o" in
	d) 	premd=1; subor=$OPTARG;;
	r) 	premr=1; subor=$OPTARG;;
	g) 	premg=1;;
	*) 	#echo "Use options a, b with a parameter, or c." >&2
	exit 1;;
	esac
done
((OPTIND--))
shift $OPTIND
priecinok=$*
#echo "Remaining arguments: ’$*’"

#Zle zadane argumenty
if [ "$premd" -eq 1 ] && [ "$premr" -eq 1 ]	
	then
	echo "Argumenty su v nespravnom formate!" >&2
	exit 1
fi

#Trapny Trap a trapnejsi TEMP!
TEMP=`mktemp /tmp/$USER.XXXXXX`
trap "rm $TEMP; exit" SIGHUP SIGINT SIGTERM

if [ $premg -eq 1 ]
then
	echo digraph GSYM {
	polesuborovgraf=($(nm $priecinok | grep ".*.o:" | sed 's/://' | sed 's/^.*\///' | sed 's/-/_/' | sed 's/\./D/' | sed 's/\+/P/')) #mena suborov spracovane pre graf vystup
fi

polesuborov=($(nm $priecinok | grep ".*.o:" | sed 's/://')) #pole normalnych mien suboro
polesuborovbezcesty=($(nm $priecinok | grep ".*.o:" | sed 's/://' | sed 's/^.*\///')) #pole mien suborov bez cesty
dlzkapolasuborov=${#polesuborov[*]}
i=0
while [ $i -lt $dlzkapolasuborov ] #idem po subore, for je nesympaticky! velka prasarna ale neni cas...
do
	polesymbolov=($(nm $priecinok | sed 's/^/#34#lom#34#/' | sed 's/#34#lom#34#.*\//#34#lom#34#/' | sed 's/^#34#lom#34#//' | sed -n '/^'${polesuborovbezcesty[$i]}':/,/^$/p' | grep ".* T .*\|.* B .*\|.* C .*\|.* D .*\|.* G .*" | sed 's/.* T //' | sed 's/.* B //' | sed 's/.* C //' | sed 's/.* D //' | sed 's/.* G //')) #ziskam si symboli s TBCDG
	dlzkapolasymbolov=${#polesymbolov[*]}
	k=0
	while [ $k -lt $dlzkapolasymbolov ] #idem po symbole
	do
		j=0
		while [ $j -lt $dlzkapolasuborov ] #idem znova po subore
		do
			pprem=($(nm $priecinok | sed 's/^/#34#lom#34#/' | sed 's/#34#lom#34#.*\//#34#lom#34#/' | sed 's/^#34#lom#34#//' | sed -n '/^'${polesuborovbezcesty[$j]}':/,/^$/p' | grep ".* U ${polesymbolov[k]}" | sed 's/.* U //')) #dam si symbol U suboru z 2 while do premenej
			if [ "$pprem" == "${polesymbolov[k]}" ] #a porovnavam, ak su rovnake z symbolmi TBCDG
			then
				if [ "$premg" == 1 ] #pre grafovy vystup + davanie do tempu iba to co potrebujem na hrany!
				then
					if [ "$premd" -eq 1 ] && [ "$premr" -eq 0 ] && [ "${polesuborovbezcesty[j]}" == "$subor" ]
					then
						echo "${polesuborovgraf[j]} [label=\"${polesuborov[j]}\"];" >>$TEMP
						echo "${polesuborovgraf[i]} [label=\"${polesuborov[i]}\"];" >>$TEMP
					elif [ "$premd" -eq 0 ] && [ "$premr" -eq 1 ] && [ "${polesuborovbezcesty[i]}" == "$subor" ]
					then
						echo "${polesuborovgraf[j]} [label=\"${polesuborov[j]}\"];" >>$TEMP
						echo "${polesuborovgraf[i]} [label=\"${polesuborov[i]}\"];" >>$TEMP
					elif [ "$premd" -eq 0 ] && [ "$premr" -eq 0 ]
					then
						echo "${polesuborovgraf[j]} [label=\"${polesuborov[j]}\"];" >>$TEMP
						echo "${polesuborovgraf[i]} [label=\"${polesuborov[i]}\"];" >>$TEMP
					fi
				echo "${polesuborovgraf[j]} -> ${polesuborovgraf[i]} [label=\"${polesymbolov[k]}\"];" #konecne vypis grafu
				else
				echo "${polesuborov[j]} -> ${polesuborov[i]} (${polesymbolov[k]})" #vypis bez grafu
				fi
			fi
			((j++))
		done
		((k++))
	done
	((i++))
done | sort -u | if [ "$premd" -eq 1 ] && [ "$premr" -eq 0 ] #vyberam podla prepinaca
		then
			grep -e "^$subor -> .*" -e "^.*/$subor -> .*"
		elif [ "$premd" -eq 0 ] && [ "$premr" -eq 1 ]
		then
			grep -e ".* -> $subor .*" -e ".* -> .*/$subor .*"
		elif [ "$premd" -eq 0 ] && [ "$premr" -eq 0 ]
		then
			cat -
		fi

if [ $premg -eq 1 ]
then
	cat $TEMP | sort -u
	echo }
fi
rm $TEMP
