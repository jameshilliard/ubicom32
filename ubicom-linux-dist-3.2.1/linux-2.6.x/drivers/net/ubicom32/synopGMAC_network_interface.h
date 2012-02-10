/** \file
 * Header file for the nework dependent functionality.
 * The function prototype listed here are linux dependent.
 *
 * \internal
 * ---------------------------REVISION HISTORY-------------------
 * Ubicom		01/Mar/2010		Modified for Ubicom32
 * Synopsys		01/Aug/2007		Created
 */

#ifndef SYNOP_GMAC_NETWORK_INTERFACE_H
#define SYNOP_GMAC_NETWORK_INTERFACE_H 1

#define NET_IF_TIMEOUT (10*HZ)
#define CHECK_TIME (HZ)

s32   synopGMAC_init_network_interface(void);
void  synopGMAC_exit_network_interface(void);

#endif /* End of file */
