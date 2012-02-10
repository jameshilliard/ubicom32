#!/bin/bash

[ -n "$1" ] && DIR=$1 || DIR=$(pwd)

CONFIG_FILE=config/Config.in

echo "
# **********************************************
# BOARD_x configs are used only internally
# **********************************************
" > $CONFIG_FILE.$DIR.boards

# append prologue
echo "choice
	prompt \"Board\"
	help
	  Select your board.
" >> $CONFIG_FILE.$DIR.boards

echo "config BOARD_NAME
	string" > $CONFIG_FILE.boards.$DIR.temp

for board_file in `ls $DIR/boards`
do
[ "${board_file##*.}" = "brd" ] && {
	NAME=$(echo ${board_file%.*} | sed 's/[^A-Za-z0-9]//g')
	[ "${board_file%%8*}" != "IP" -a "${board_file%%7*}" != "IP" -a "${board_file%%5*}" != "IP" ] && continue
	# Write help message: name and description of board
	old_ifs=$IFS
	IFS=$'\n'
	DESC_FOUND=0
	help_message=""
	for line in `cat $DIR/boards/$board_file`
	do
		# Check for Description = 'xxxx'
		t=${line% =*\'*}
		if [ $t = "  Description" ] ; then
			DESC_FOUND=1
			line=${line/  Description = /}
		else 
			# Check for Description = \n '....'
			t=${line% =*}
			if [ $t = "  Description" ] ; then
				DESC_FOUND=1
				continue
			fi
		fi

		len=${#line}
		len=$((len-1))
		lastchar=${line:$len:1}
		[ $DESC_FOUND -eq 1 ] && {
			filtered=$line
			filtered=${filtered//\'#9/#9}
			filtered=${filtered//\'#13/#13}
			filtered=${filtered//\'#10/#10}
			filtered=${filtered//\#9\'/#9}
			filtered=${filtered//\#13\'/#13}
			filtered=${filtered//\#10\'/#10}
			filtered=${filtered//#10/$'\n\t\t'}
			filtered=${filtered//#13/}
			filtered=${filtered//#9/$'\t'}
			[ $lastchar != "+" ] && {
				help_message=$help_message$filtered
				break
			}
			help_message=$help_message$filtered
		}
	done
	IFS=$old_ifs

	# Write board config
echo "config BOARD_${NAME}
	bool \"$board_file\"
	help
	  $help_message.
" >> $CONFIG_FILE.$DIR.boards
echo "	default \"$board_file\" if BOARD_${NAME}" >> $CONFIG_FILE.boards.$DIR.temp
}
done

echo "endchoice
" >> $CONFIG_FILE.$DIR.boards

cat $CONFIG_FILE.boards.$DIR.temp >> $CONFIG_FILE.$DIR.boards
rm -f $CONFIG_FILE.boards.$DIR.temp
