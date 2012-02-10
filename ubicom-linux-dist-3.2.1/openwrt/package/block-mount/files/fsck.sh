#!/bin/sh
# Copyright 2010 Vertical Communications
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

force_fsck() {
	local dev_full=$1
	local fstype=$2
	
	local tmp_cmd="awk -F\"=\" 'match(\$0, \"$dev_full\") != 0 {print \$3}'"
	[ -z "$fstype" ] && fstype=$(blkid | eval "$tmp_cmd" | sed "s/\"//g")
	
	[ -x /sbin/fsck ] && {
		fsck -a -t "$fstype" "$dev_full" > /dev/null
	
		local status="$?"
		case "$status" in
		0|1)
			# Successfully
			# 0- No errors
			# 1- File system errors corrected
			return 0 ;;
		*)
			# Other errors
			#
			# 2- System should be rebooted
			# 4- File system errors left uncorrected
			# 8- Operational error
			# 16- Usage or syntax error
			# 32- Fsck cancelled by user request
			# 128- Shared library error
			return 1 ;;
		esac
	}
}

libmount_fsck() {
	local device="$1"
	local fsck_fstype="$2"
	local fsck_enabled="$3"
	local known_type
	local found_fsck=0
	

	grep -q "$device" /proc/swaps || grep -q "$device" /proc/mounts || {
		[ -e "$device" ] && [ "$fsck_enabled" -eq 1 ] && {
			for known_type in $libmount_known_fsck; do
				if [ "$known_type" = "$fsck_fstype" ]; then
					fsck_${known_type} "$device"
					found_fsck=1
					break
				fi
			done
			if [ "$found_fsck" -ne 1 ]; then
				logger -t 'fstab' "Unable to check/repair $device; no known fsck for filesystem type $fstype"
			fi
		}
	}
}

libmount_known_fsck=""

pi_include /lib/functions/fsck

