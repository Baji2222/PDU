#ifndef __GLOBAL_CONFIG_H__
#define __GLOBAL_CONFIG_H__

#include <common_types.h>
extern uint8 interfaceNumber;
extern uint8 sysMaxPhyPort;
extern uint32 cpuPortPortmask;

#define COMBO_PORT_NUM                        (18)//for 5G & 8G to use same image file

/* feature start */
#ifdef LWS_A_SET
#define LWPS_ENABLE                 (1)
#define LWPS_SNMP                   (1)
#define LWPS_SYSLOG                 (0)
#define MIB_ENABLE                  (1)
#define WEB_ENABLE                  (1)  
#define TIME_LIB_ENABLE             (0)
#else
#define LWPS_ENABLE                 (1)
#define LWPS_SYSLOG                 (0)
#define LWPS_SNMP                   (1)
#define MIB_ENABLE                  (1)
#define WEB_ENABLE                  (1)  
#define TIME_LIB_ENABLE             (1)
#endif
/* feature end */

/* runtime or loader */
#define RUNTIME_MODE                (1)
#define LOADER_MODE                 (0)


#define CONFIG_FOR_RUNTIME          (1)
#define CONFIG_STANDALONE           (1)
#define STANDARD_MEM_LIB            (1)

// self-check for conflict 
#if (defined(CONFIG_FOR_RUNTIME) && defined(CONFIG_FOR_LOADER))
    #error "conflict options: both CONFIG_FOR_RUNTIME and CONFIG_FOR_LOADER were defined!"
#endif
#if (!defined(CONFIG_FOR_RUNTIME) && !defined(CONFIG_FOR_LOADER))
    #error "conflict options: both CONFIG_FOR_RUNTIME and CONFIG_FOR_LOADER were NOT defined!"
#endif

/* System Configuration */
#define PROJECT_NAME                        "RTL8367C"
#define SWITCH_NAME                         "OPL SWITCH"


/* CPU Configuration */
#ifndef LITTLE_ENDIAN
  #define LITTLE_ENDIAN                       1234
#endif /* end of LITTLE_ENDIAN */
#ifndef BIG_ENDIAN
  #define BIG_ENDIAN                          4321
#endif /* end of BIG_ENDIAN */
#ifndef BYTE_ORDER
  #define BYTE_ORDER                          LITTLE_ENDIAN
#endif /* end of BYTE_ORDER */
#define CONFIG_INTERNAL_CPU_ENDIAN_BIG      (0)
#define CONFIG_INTERNAL_CPU_ENDIAN_LITTLE   (1)
#define CONFIG_INTERNAL_CPU_ENDIAN          (CONFIG_INTERNAL_CPU_ENDIAN_BIG)

#if defined(DW8051_CLK_100)
#define DW8051_CLK    (DW8051_RDY_100MHz)
#elif defined(DW8051_CLK_625)
#define DW8051_CLK    (DW8051_RDY_625MHz)
#elif defined(DW8051_CLK_200)
#define DW8051_CLK    (DW8051_RDY_200MHz)
#elif defined(DW8051_CLK_125)
#define DW8051_CLK    (DW8051_RDY_125MHz)
#elif defined(DW8051_CLK_208)
#define DW8051_CLK    (DW8051_RDY_20_8MHz)
#else
#define DW8051_CLK    (DW8051_RDY_20_8MHz)
#endif


/* Flash Configuration */
#define CONFIG_FLASH_PROBE                  (0)
#define FLASH_TYPE_SPI                      (1)
#define FLASH_TYPE_NOR                      (1)
//#undef  FLASH_TYPE_SPI
#undef  FLASH_TYPE_NOR
#define CONFIG_UPDATE_FIRMWARE_MAX_SIZE     (256*1024UL)  // It works on update firmware & SPI Flash
#if defined(FLASH_2M) //2M flash
#define FLASH_SIZE    (2048UL*1024UL)
#else //1M flash
#define FLASH_SIZE    (1024UL*1024UL)
#endif
#if 0==CONFIG_FLASH_PROBE
  #define CONFIG_DEFAULT_FLASH_SIZE         (FLASH_SIZE)
#endif /* end of 0==CONFIG_FLASH_PROBE */

#define INTERFACE_NUMBER                    (interfaceNumber)

/* NIC Configuration */
#define RXBUFFERSIZE                        (6 * 1024)
#define TXBUFFERSIZE                        (2 * 1024 -8)
#define PEDMA_ERAM_OFFSET                   (8)
#define FRAME_INFO_SIZE                     (8)
#define CPU_TAG_SIZE                        (8)
#define VLAN_TAG_SIZE                       (4)
#define TAG_SIZE                            (4+4+CPU_TAG_SIZE)
#define CONFIG_NIC_CHECKSUM_OFFLOAD         (1)
#define CONFIG_MAX_RX_PER_INTR              (4)
#define P_BUFFER_SIZE                       (1518+FRAME_INFO_SIZE+CPU_TAG_SIZE+32)
#define SYS_MAX_PHY_PORT                    (sysMaxPhyPort)
#define CPU_PORT_PORTMASK                   (cpuPortPortmask) //70B physical CPU port is 10(from 0), 67C physical CPU port is 7(from 0)

/* CLI command list */
#define CLI_COMMAND_CLEAR                   ('c')
#define CLI_COMMAND_DEBUG                   ('d')
#define CLI_COMMAND_HELP                    ('h')
#define CLI_COMMAND_DUMP_IP                 ('i')
#define CLI_COMMAND_DUMP_IGMP               ('g')
#define CLI_COMMAND_EXIT                    ('q')
#define CLI_COMMAND_SNTP                    ('t')
#define CLI_COMMAND_UPDATE                  ('u')
#define CLI_COMMAND_DUMP_VLAN_INFO          ('v')
#define CLI_COMMAND_MAC_SET                 ('m')
#define CLI_COMMAND_TRAP_IP_SET             ('p')
#define CLI_COMMAND_REBOOT                  ('r')

#define FIRMWARE_VER                        ("1.0.0")

/* UART Configuration */
#define BAUD_RATE                           (9600)

/* Time Configuration */
#define CONFIG_TIMER_DIVISOR                (200UL)

/* RTK API Configuration */
#define RTK_API_LWPS_BASE                   (1)

/* define GPIO pressed time for factory reset */
#define GPIO_RESET_TIME         30          /* 3s */

#ifndef MAC_ADDR_LEN
#define MAC_ADDR_LEN (6)
#endif

/* Secondary stack */
#define SECOND_STACK_SZ     (24)
extern unsigned char secondary_stack[SECOND_STACK_SZ];

#endif /* End of __GLOBAL_CONFIG_H__ */

