fw_get_wan_ifs() {
	wan_ifs=
	nws=$(uci_get_state firewall.core.wan_networks)
	for i in $nws
	do
		f=0
		for j in $wan_ifs
		do
		        [ "$j" == "$i" ] && {
		                f=1
		                break
		        }
		done
		[ $f -eq 0 ] && {
		        [ -n "$wan_ifs" ] && wan_ifs="$wan_ifs $i" || wan_ifs=$i
		}
	done
	echo $wan_ifs
}
