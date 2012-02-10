#!/bin/bash

[ -n "$1" ] && DIR=$1 || DIR=$(pwd)

CONFIG_FILE=config/Config.in

#DISTRO_TYPES="uClinux openwrt ultra u-boot toolchain"
DISTRO_TYPES="ultra"

echo "**********************************************"
echo "** Working under $DIR "
echo "**********************************************"

function GetDirType {
	case "$1" in
		uClinux) echo "DISTRO" ;;
		openwrt) echo "DISTRO" ;;
		ultra) echo "ULTRA" ;;
		u-boot) echo "UBOOT" ;;
		toolchain) echo "TOOLCHAIN" ;;
	esac
}

# append prologue
for distro_type in $DISTRO_TYPES
do
	# init board configs
	[ "$distro_type" = "ultra" ] && echo "" > $CONFIG_FILE.$distro_type.boards
	# start to create directory configs
#	echo "
#choice
#	prompt \"$distro_type\"
#	help
#	  Select your distro directory.
#" > $CONFIG_FILE.$distro_type
#	DIR_TYPE=$(GetDirType $distro_type)
#	echo "config ${DIR_TYPE}_DIR
#	string" >> $CONFIG_FILE.$distro_type.temp
	echo "" > $CONFIG_FILE.$distro_type
done

# append data
#for dir in `ls $DIR`
for dir in $DISTRO_TYPES
do
	if [ -d $dir ]; then
		for distro_type in $DISTRO_TYPES
		do
			if [ "${dir%%_*}" = "$distro_type" ]; then
#				echo "** $dir"
				DIR_TYPE=`GetDirType $distro_type`
#				echo "config ${DIR_TYPE}_DIR_$dir
#	bool \"$dir\"
#" >> $CONFIG_FILE.$distro_type
#				echo "	default \"$dir\" if ${DIR_TYPE}_DIR_$dir" >> $CONFIG_FILE.$distro_type.temp
				[ "$DIR_TYPE" = "ULTRA" ] && {
					./config/create_lpj_config.sh $dir
					./config/create_board_config.sh $dir
#					echo "if ${DIR_TYPE}_DIR_$dir
echo "source $CONFIG_FILE.$dir.boards
source $CONFIG_FILE.$dir.lpj
" >> $CONFIG_FILE.boards.$distro_type.temp
#endif
#" >> $CONFIG_FILE.boards.$distro_type.temp
				}
			fi
		done
	fi
done

# append epilogue
#for distro_type in $DISTRO_TYPES
#do
#	echo "
#endchoice
#" >> $CONFIG_FILE.$distro_type
#done

# combine two files
for distro_type in $DISTRO_TYPES
do
#cat $CONFIG_FILE.$distro_type.temp >> $CONFIG_FILE.$distro_type
#rm -f $CONFIG_FILE.$distro_type.temp

echo "" >> $CONFIG_FILE.$distro_type

[ "$distro_type" = "ultra" ] && {
	cat $CONFIG_FILE.boards.$distro_type.temp >> $CONFIG_FILE.$distro_type
	rm -f $CONFIG_FILE.boards.$distro_type.temp
}
done

echo ""
