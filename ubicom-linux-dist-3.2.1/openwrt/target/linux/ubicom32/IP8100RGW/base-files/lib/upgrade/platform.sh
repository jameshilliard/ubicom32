PART_NAME=kernel

KERNEL_PART=kernel
ROOTFS_PART=rootfs
# Extract erase size and find kernel size
# kernel is on second line, rootfs is on third line (currently!)
KERNEL_SIZE_BYTE=0x$(sed -n '3p' /proc/mtd | cut -d' ' -f2)
ERASE_SIZE_BYTE=0x$(sed -n '3p' /proc/mtd | cut -d' ' -f3)
KERNEL_SIZE=$((KERNEL_SIZE_BYTE / ERASE_SIZE_BYTE))
ERASE_SIZE=$((ERASE_SIZE_BYTE / 0x400))k

platform_check_image() {
	[ "$ARGC" -gt 1 ] && return 1

	case "$(get_magic_word "$1")" in
		# .ub files
		2705)
			# AppIDs should match exactly
			image_appid=$(get_image "$1" | dd bs=1 skip=32 count=31 2>/dev/null)
			kernel_appid=$(dd if=/dev/mtd1 bs=1 skip=32 count=31 2>/dev/null)
			if [ "X${image_appid}" == "X${kernel_appid}" ]; then
				return 0
			fi
			echo "AppIDs do not match. Image: $image_appid. Kernel: $kernel_appid. Please use the same AppID as the installed kernel."
		;;
		*)
			echo "Invalid image type. Please use only .ub files"
		;;
	esac
	return 1
}

platform_do_upgrade() {
	sync
	if [ "$SAVE_CONFIG" -eq 1 -a -z "$USE_REFRESH" ]; then
		dd if="$1" bs="$ERASE_SIZE" count="$KERNEL_SIZE" | mtd write - "${KERNEL_PART:-kernel}"
		dd if="$1" bs="$ERASE_SIZE" skip="$KERNEL_SIZE"  | mtd write - "${ROOTFS_PART:-rootfs}"
		jffs2_copy_config
	else
		dd if="$1" bs="$ERASE_SIZE" count="$KERNEL_SIZE" | mtd write - "${KERNEL_PART:-kernel}"
		dd if="$1" bs="$ERASE_SIZE" skip="$KERNEL_SIZE"  | mtd write - "${ROOTFS_PART:-rootfs}"
	fi
}

disable_watchdog() {
	killall watchdog
	( ps | grep -v 'grep' | grep '/dev/watchdog' ) && {
		echo 'Could not disable watchdog'
		return 1
	}
}

append sysupgrade_pre_upgrade disable_watchdog

