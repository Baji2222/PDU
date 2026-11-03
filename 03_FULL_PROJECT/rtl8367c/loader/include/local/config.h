#ifndef __GLOBAL_CONFIG_H__
#define __GLOBAL_CONFIG_H__

#include <common_types.h>
extern uint8 interfaceNumber;
extern uint8 sysMaxPhyPort;
extern uint32 cpuPortPortmask;


/* to define the target is for run-time or loader */
#define CONFIG_FOR_LOADER

#define COMBO_PORT_NUM                        (18)//for 5G & 8G to use same image file

#define LWPS_ENABLE                 (1)
#define LWPS_SNMP                   (0)
#define LWPS_SYSLOG                 (0)
#define MIB_ENABLE                  (0)
#define WEB_ENABLE                  (1)  
#define LLDP_ENABLE                 (0)

/* runtime or loader */
#define RUNTIME_MODE                (1)
#define LOADER_MODE                 (0)

// self-check for conflict 
#if (defined(CONFIG_FOR_RUNTIME) && defined(CONFIG_FOR_LOADER))
    #error "conflict options: both CONFIG_FOR_RUNTIME and CONFIG_FOR_LOADER were defined!"
#endif
#if (!defined(CONFIG_FOR_RUNTIME) && !defined(CONFIG_FOR_LOADER))
    #error "conflict options: both CONFIG_FOR_RUNTIME and CONFIG_FOR_LOADER were NOT defined!"
#endif

#include <local/flash_config.h>


/* System Configuration */
#define PROJECT_NAME                        "RTL8370S"


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
#define CONFIG_TOTAL_IROM_SIZE              (16*1024UL)

#if defined(DW8051_CLK_200)
#define DW8051_CLK    (DW8051_RDY_200MHz)
#elif defined(DW8051_CLK_100)
#define DW8051_CLK    (DW8051_RDY_100MHz)
#elif defined(DW8051_CLK_125)
#define DW8051_CLK    (DW8051_RDY_125MHz)
#elif defined(DW8051_CLK_625)
#define DW8051_CLK    (DW8051_RDY_625MHz)
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
#define CONFIG_USE_FLASH_VIEWER



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
#define CLI_COMMAND_HELP                    ('h')
#define CLI_COMMAND_EXIT                    ('q')
#define CLI_COMMAND_RFC                     ('r') /*Read Flash Content*/
#define CLI_COMMAND_UPDATE                  ('u')



/* UART Configuration */
#define BAUD_RATE                           (57600)
//#define BAUD_RATE                           (1200)

/* Time Configuration */
#define CONFIG_TIMER_DIVISOR                (200UL)

/* RTK API Configuration */
#define RTK_API_LWPS_BASE                   (1)

/* Others */
#ifndef MAC_ADDR_LEN
#define MAC_ADDR_LEN (6)
#endif

/* Secondary stack */
#define SECOND_STACK_SZ     (64)
extern unsigned char secondary_stack[SECOND_STACK_SZ];


#endif /* End of __GLOBAL_CONFIG_H__ */

