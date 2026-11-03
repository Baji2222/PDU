#include <common.h>
#include <sal/salflash.h>
#include <sal/salsys.h>
#include <sal/saltype.h>
//===========================
#include <flash_config.h>
#include <salflash.h>
#include <salutil.h>
#include <snmp/snmp_msg.h>

extern u16_t vlanIds;

/* Real subnet mask: defined in global_variable.c. Declaration only. */
extern sal_ipaddr_t netmask;

extern int add_gateway_over_vlan_acl_rule(uint16 rule_num, uint16 vlan_id, uint32 gateway_ip, uint32 subnetmask);
extern int32 sal_flash_vlan_id_get(uint32 addr, uint16 length, uint8 *pData);
extern err_t snmp_send_trap(s8_t generic_trap, struct snmp_obj_id *eoid, s32_t specific_trap);

/* Make sure the MAC starts with the A4:DA:22:52 prefix; if not, set it
 * (last two bytes zeroed). Locals live here, not in main(), so the C51
 * overlayer can share their XDATA with other init code. */
static void mac_prefix_fixup(void)
{
	sal_mac_t mac;

	sal_sys_mac_get(&mac);
	if (!(mac.octet[0] == 0xA4 && mac.octet[1] == 0xDA &&
	      mac.octet[2] == 0x22 && mac.octet[3] == 0x52)) {
		mac.octet[0] = 0xA4;
		mac.octet[1] = 0xDA;
		mac.octet[2] = 0x22;
		mac.octet[3] = 0x52;
		mac.octet[4] = 0x00;
		mac.octet[5] = 0x00;
		sal_sys_mac_set(&mac);
		sal_flash_sysMac_set(&mac);
	}
}

/* Read the provisioned VLAN ID from flash and install the gateway ACL rule. */
static void gateway_acl_init(void)
{
	uint8 getvlan_id[5] = {0, 0, 0, 0, 0};   /* initialised: safe if flash read fails */
	sal_ipaddr_t gateway = 0;

	if (sal_flash_vlan_id_get(FLSH_CONFIG_DUMMY_START, 4, getvlan_id) != 0)
		return;                              /* flash read failed: skip ACL */
	getvlan_id[4] = '\0';                    /* null-terminate */

	if (strncmp((char *)getvlan_id, "NEW ", 4) == 0)
		return;                              /* unprovisioned */

	sal_sys_gateway_get(&gateway);
	vlanIds = (u16_t)atoi((char *)getvlan_id);
	add_gateway_over_vlan_acl_rule(7, vlanIds, gateway, netmask);
}

// Main Function: Entry point for the switch firmware application
// Initializes system hardware, sets MAC address, configures ACL rules, and runs event dispatch loop
void main(void)
{
	setReg(0x1d32, 0x0004); // Enable 67C UART
	setReg(0x1d3f, 0x0100); // Force deglitch enable

	/* system initialization */
	all_init();

	mac_prefix_fixup();
	gateway_acl_init();

	while (1) {
		/* don't remove this two function calls */
		event_dispatch();
	}
}