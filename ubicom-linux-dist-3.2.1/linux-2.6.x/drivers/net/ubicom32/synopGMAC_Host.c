/**\file
 *  The top most file which makes use of synopsys GMAC driver code.
 *
 *  This file can be treated as the example code for writing a application driver
 *  for synopsys GMAC device using the driver provided by Synopsys.
 *  This exmple is for Linux 2.6.xx kernel
 *  - Uses Linux network driver and the TCP/IP stack framework
 *  - Uses the Device Specific Synopsys GMAC Kernel APIs
 *  \internal
 * ---------------------------REVISION HISTORY--------------------------------
 * Ubicom		01/Mar/2010			Modified for Ubicom32
 * Synopsys		01/Aug/2007			Created
 */

#include "synopGMAC_plat.h"
#include "synopGMAC_Dev.h"
#include "synopGMAC_banner.h"
#include "synopGMAC_network_interface.h"

/***************************************************/

int __init SynopGMAC_Host_Interface_init(void)
{
        int retval;

	TR0("**********************************************************\n");
	TR0("* Driver    :%s\n",synopGMAC_driver_string);
	TR0("* Version   :%s\n",synopGMAC_driver_version);
	TR0("* Copyright :%s\n",synopGMAC_copyright);
	TR0("**********************************************************\n");

        /* Initialize the Network dependent services */
	if((retval = synopGMAC_init_network_interface())){
		TR0("Could not initialize the Network interface.\n");
		return retval;
	}

	return 0 ;
}

void __exit SynopGMAC_Host_Interface_exit(void)
{

        TR0("Exiting synopsys GMAC interfaces ..\n") ;

        /* De-Initialize the Network dependent services */
        synopGMAC_exit_network_interface();
	TR0("Exiting synopGMAC_exit_network_interface\n");
}

module_init(SynopGMAC_Host_Interface_init);
module_exit(SynopGMAC_Host_Interface_exit);

MODULE_AUTHOR("Synopsys GMAC by Ubicom");
MODULE_LICENSE("GPL/BSD");
MODULE_DESCRIPTION("SYNOPSYS GMAC NETWORK DRIVER ON UBICOM32 PLATFORM");
