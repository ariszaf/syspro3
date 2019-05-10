#!/bin/bash

echo All Arguments = $*

if [ "$#" != "4" ]; then      # also: -ne, -gt -ge, -lt, -le
	echo "write ./script_name root_directory text_file w(integer) p(integer)"	
	echo "wrong parameters   "
	exit 1
fi

DIR="$1"

if [ -d $1 ]    # exists directory
   	then echo directory exist
else 
	echo "not directory found"
	exit 1
fi

if [ -e $2 ]    # exists file
  then if [ -f $2 ] # is a regular file
         then echo Regular File
	else
		echo "cant read the file"
	fi
else
	echo "not file found"
fi

if [[ ! "$3" =~ ^[0-9]+$ ]]; then
	echo "w must be int"	
	exit 1
fi

if (( "$3" < 2 )); then
	echo "w must be bigger than 1"	
	exit 1
fi

if [[ ! "$4" =~ ^[0-9]+$ ]]; then 
	echo "p must be int"	
	exit 1
fi

if (( "$4" < 3 )); then
	echo "p must be bigger than 2"	
	exit 1
fi

if [ `grep -c '' $2` -gt 10000 ]; then			
	echo "file its ok. Has more than 10000 lines"
else
	echo "file must have more than 10000 lines"
fi

INC_LINK_ARRAY=()
SITEARRAY=()
PAGEARRAY=()
n2=0;
#max2=$(($3))
while [ "$n2" -lt "$3" ]; do
	m2=0;
	#max2=$(($4-1))	
	SITEARRAY+=("$DIR/site"$n2"")
	while [ "$m2" -lt "$4" ]; do
		testing=$((RANDOM))
		PAGEARRAY+=("$DIR/site"$n2"/page"$m2"_"$testing".html")
		m2=`expr "$m2" + 1`;
	done	
	n2=`expr "$n2" + 1`;
done


# init
# look for empty dir
if [ "$(ls -A $DIR)" ]; then
	rm -rf $DIR/*
	echo "Warning: directory $DIR is full, purging ..."
fi

for ((i = 0; i < "${#SITEARRAY[@]}"; i++))
        do
	echo "Creating web site $i ..."
	mkdir ${SITEARRAY[$i]}

	z=$(($i*$4))

	l=$(($z+$4))

	while [ "$z" -lt "$l" ]; do
		nblines=$(grep -c '' $2)
		nblines=$(($nblines-2000))
		k=$(( $RANDOM % nblines + 1 ))

		
		m=$((1000 + RANDOM%(1+2000-1000)))

		
		f=$((($4/2)+1))
	
		F_ARRAY=()
		RAND_VAL_ARRAY=()
		
		min_value=$(($i*$4))

		max_value=$(($min_value+$4-1))
		
		i1=0;
		while [ "$i1" -lt "$f" ]; do
			random_value=$(($min_value + RANDOM%(1+$max_value-$min_value)))
			if [[ ! " ${RAND_VAL_ARRAY[@]} " =~ " ${random_value} " ]]; then
    				if [ "$random_value" != "$z" ]; then
					
					FILE=${PAGEARRAY[$random_value]}
					FILE=${FILE#*/} #removes stuff from / all the way to end

					FILE=${FILE#*/} #removes stuff from / all the way to end
					F_ARRAY+=($FILE)
					if [[ ! " ${INC_LINK_ARRAY[@]} " =~ " ${PAGEARRAY[$random_value]} " ]]; then
					INC_LINK_ARRAY+=(${PAGEARRAY[$random_value]})
					fi	

					#F_ARRAY+=(${PAGEARRAY[$random_value]})
					RAND_VAL_ARRAY+=("$random_value")
					i1=`expr "$i1" + 1`;

				fi
			fi
		done

		q=$((($3/2)+1))

		i2=0;
		while [ "$i2" -lt "$q" ]; do
			array_index=$((${#PAGEARRAY[@]}-1))
			random_value=$((0 + RANDOM%(1+$array_index-0)))

			if [[ ! " ${RAND_VAL_ARRAY[@]} " =~ " ${random_value} " ]]; then
    				if [[ "$random_value" -lt "$min_value" || "$random_value" -gt "$max_value" ]]; then
					FILE=${PAGEARRAY[$random_value]}
					FILE=${FILE#*/} #removes stuff from / all the way to end

					#FILE=${FILE#*/} #removes stuff from / all the way to end
					n='../'
					nFILE=$n$FILE
					F_ARRAY+=($nFILE)
					
					#F_ARRAY+=(${PAGEARRAY[$random_value]})
					if [[ ! " ${INC_LINK_ARRAY[@]} " =~ " ${PAGEARRAY[$random_value]} " ]]; then
					INC_LINK_ARRAY+=(${PAGEARRAY[$random_value]})
					fi
					RAND_VAL_ARRAY+=("$random_value")				
					i2=`expr "$i2" + 1`;
				fi
			fi
		done	

	
		#oso to m / (f + q) einai diaforetiko apo 0
		#grapse to keimeno ksekinontas apo to k gia m / (f + q) grammes
		#vale to epomeno diathesimo link kai aferese to apo ton pinaka an o pinakas exei 			diathesima link
		#enimerwse m = m-m / (f + q)
		#kai k = k+ m / (f + q)
		
		echo "Creating page ${PAGEARRAY[$z]} with $m lines starting at line $k ..."
		echo -e	'<!DOCTYPE html> \n <html> \n <body>' > ${PAGEARRAY[$z]}

		#echo F_ARRAY
		#echo ${F_ARRAY[@]}
		#echo ${#F_ARRAY[@]}

		lines_to_write=$(($m/($f+$q)))
		while [ "$lines_to_write" -gt 0 ]; do
			echo -e '<p>\c' >> ${PAGEARRAY[z]}
			tail -n +$k $2 | head -n $lines_to_write >> ${PAGEARRAY[$z]}		
			if [ "${#F_ARRAY[@]}" -gt 0 ]; then
				echo "Adding link to ${PAGEARRAY[z]} "



				echo -e '<a href="\c' >> ${PAGEARRAY[z]}
				echo -e "${F_ARRAY[0]}\c" >> ${PAGEARRAY[z]}
				echo -e '">\c' >> ${PAGEARRAY[z]}
				echo -e 'link_text</a></p>' >> ${PAGEARRAY[z]}				
				#if [[ ! " ${INC_LINK_ARRAY[@]} " =~ " ${F_ARRAY[0]} " ]]; then
				#	INC_LINK_ARRAY+=(${F_ARRAY[0]})
				#fi					
  				unset F_ARRAY[0]      # remove the undesired item
  				F_ARRAY=( "${F_ARRAY[@]}" ) # renumber the indexes
			else
				echo -e 'link_text</p>' >> ${PAGEARRAY[z]}							
			fi

			m=`expr "$m" - "$lines_to_write"`;	

			let "k=k+lines_to_write"

			lines_to_write=$(($m/($f+$q)))

			if [[ "$lines_to_write" == 0 && "$m" -gt 0 ]]; then
				lines_to_write=$m
				#echo "assignment ln"
			fi
		done	

		#sed -n '1,10p' $2 >> ${PAGEARRAY[$z]}
		#sed -n "$k,10p" $2 >> ${PAGEARRAY[$z]}
		#head -n10 $2 >> ${PAGEARRAY[$z]}

		echo -e	'</body>\n</html>' >> ${PAGEARRAY[$z]}

		z=`expr "$z" + 1`;
	done
done
#echo ${#PAGEARRAY[@]}
#echo ${#INC_LINK_ARRAY[@]}
if [ "${#PAGEARRAY[@]}" == "${#INC_LINK_ARRAY[@]}" ]; then
	echo "All pages have at least one incoming link"
else
	echo "NOT All pages have at least one incoming link"
fi
echo "done"
#https://stackoverflow.com/questions/29851918/bash-if-variable-is-an-integer 
#https://stackoverflow.com/questions/12022319/bash-echo-number-of-lines-of-file-given-in-a-bash-variable-without-the-file-name
#https://stackoverflow.com/questions/8988824/generating-random-number-between-1-and-10-in-bash-shell-script
#https://stackoverflow.com/questions/43533439/generating-random-numbers-that-conform-to-a-range-in-bash
#https://stackoverflow.com/questions/4140822/creating-multiple-files-with-content-from-shell
#https://stackoverflow.com/questions/33370890/bash-find-a-list-of-files-with-more-than-3-lines
#https://www.youtube.com/watch?v=nWylbGC2KnQ&list=PLW_lDd8kNgAF9HFKMiM3A3WfvOhlqoKSW&index=48
#https://unix.stackexchange.com/questions/48750/creating-numerous-directories-using-mkdir
#https://stackoverflow.com/questions/24666330/bash-command-to-create-a-new-file-and-its-parent-directories-if-necessary
#https://www.thegeekstuff.com/2010/06/bash-array-tutorial
#https://stackoverflow.com/questions/1951506/add-a-new-element-to-an-array-without-specifying-the-index-in-bash
#https://stackoverflow.com/questions/13181725/bash-append-file-contents-to-the-bottom-of-existing-file
#https://stackoverflow.com/questions/3685970/check-if-a-bash-array-contains-a-value
