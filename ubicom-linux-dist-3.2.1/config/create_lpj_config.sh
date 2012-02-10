#!/bin/bash

[ -n "$1" ] && DIR=$1 || DIR=$(pwd)

CONFIG_FILE=config/Config.in

echo "
# **********************************************
# LPJ_x configs are used only internally
# **********************************************
" > $CONFIG_FILE.$DIR.lpj

echo "
choice
	prompt \"LPJ\"
	help
	  Select your lpj file.
" >> $CONFIG_FILE.$DIR.lpj

echo "config LPJ_NAME
	string" > $CONFIG_FILE.$DIR.lpj.temp

# append prologue
for lpj_file in `ls $DIR/projects/bootexec/config`
do
[ "${lpj_file##*.}" = "lpj" ] && {
	NAME=$(echo ${lpj_file%.*} | sed 's/[^A-Za-z0-9]//g')
	echo "config LPJ_${NAME}
	bool \"$lpj_file\"
" >> $CONFIG_FILE.$DIR.lpj
	echo "	default \"$lpj_file\" if LPJ_${NAME}" >> $CONFIG_FILE.$DIR.lpj.temp
}
done

	echo "
endchoice
" >> $CONFIG_FILE.$DIR.lpj

# combine two files
cat $CONFIG_FILE.$DIR.lpj.temp >> $CONFIG_FILE.$DIR.lpj
rm -f $CONFIG_FILE.$DIR.lpj.temp
