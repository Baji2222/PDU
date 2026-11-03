#pragma STRING (far)
#include <local/config.h>
#include <reg_sfr.h>
#include <stdio.h>
#include <string.h>

#include <salutil.h>
#include <salsys.h>
#include <salflash.h>

#include "common.h"
#include "asicreg.h"
#include "web_user.h"
#include <web/web_api.h>


/* Extern Functions Declaration */
extern void send_str_web(uint8* tempData);
extern void Copy_RTC_DATA(uint8* tempData);
extern void Copy_Alarm_DATA(uint8* tempData);

extern void update_json_string(const char *new_json);
extern void Fetching_Set_Response(char* tempData);
/* Function Declaration */
void process_uart_string(void);

uint8  far m_u8_uart_str[234];
uint8 m_u8_temp_cntr = 0;


// Print Menu: Displays the main CLI menu with available commands and project name
void print_menu(void) large{
    OSAL_PRINTF("\n     ****************    %s    ****************\n", PROJECT_NAME);
    OSAL_PRINTF("     %c: Clear Screen\n", CLI_COMMAND_CLEAR);
    OSAL_PRINTF("     %c: Print IP\n", CLI_COMMAND_DUMP_IP);
    OSAL_PRINTF("     %c: Help Menu\n", CLI_COMMAND_HELP);
    OSAL_PRINTF("     %c: Enter Loader Mode\n", CLI_COMMAND_UPDATE);
    OSAL_PRINTF("     %c: Set MAC Address\n", CLI_COMMAND_MAC_SET);
    OSAL_PRINTF("     %c: Reboot\n", CLI_COMMAND_REBOOT);
    OSAL_PRINTF("     ************************************************\n\n");
}

// Clear Screen: ANSI escape codes, clear terminal and cursor to home
void clearScreen(void) large{
    putchar(0x1B);
    putchar('[');
    putchar('2');
    putchar('J');
    putchar(0x1B);
    putchar('[');
    putchar('1');
    putchar(';');
    putchar('1');
    putchar('H');
    putchar(0);
}

// Get MAC Address: input in XX:XX:XX:XX:XX:XX format, optional save to flash
void cliGetMacAddr(void) large{
    uint8      tmp;
    char      buffer[18];
    uint16    ptr;
    uint32    timeout;
    sal_mac_t mac;

    ptr = 0;
    timeout = 0xFFFFFFFFUL;

    while(1)
    {
        tmp = getchar_nb();

        /* no input, decrease timeout counter */
        if(0xFF == tmp)
        {
            timeout--;
            if(0 == timeout)
            {
                printf("Input timeout\n");
                break;                          /* FIX: do not hang the web server */
            }
        }
        else
        {
            if(0x0D == tmp)
            {
                buffer[17] = 0;
                buffer[ptr] = 0;
                if(0 == str_to_mac(mac.octet, buffer))
                {
                    sal_sys_mac_set(&mac);
                    printf("\nMAC address is set, save to flash?[y/n]: ");

                    timeout = 0xFFFFFFFFUL;
                    while(1)
                    {
                        tmp = getchar_nb();
                        if(0xFF == tmp)
                        {
                            timeout--;
                            if(0 == timeout)
                            {
                                printf("Input timeout, MAC address is not saved to flash\n");
                                break;
                            }
                        }
                        else
                        {
                            printf("%c \n", tmp);
                            if(tmp == 'y')
                            {
                                printf("MAC address is saved to Flash\n");
                                sal_flash_sysMac_set(&mac);
                            }
                            else
                            {
                                printf("MAC address is not saved to Flash\n");
                            }
                            break;
                        }
                    }
                }
                else
                {
                    printf("\nError input\n");
                }

                break;
            }
            else
            {
                if(ptr < 17)                    /* FIX: never overflow buffer[18] */
                {
                    printf("%c", tmp);
                    buffer[ptr++] = tmp;
                }
                timeout = 0xFFFFFFFFUL;
            }
        }
    }
}

// Command Select: reads one character without blocking, 0xFF = none
uint8 cmd_select(void) large
{
    uint8 xdata cmd;

    cmd = getchar_nb();
    return cmd;
}

// Change Clock: adjusts clock divider for SPI flash access
void change_clock(uint16 xdata value) large{
    volatile uint32 xdata i;

    for(i=0;i<0x100;i++);
    if(value < 0x04) //8051 Clock >100Mhz
    {
        setReg(DW8051_RDY, (DW8051_RDY_RATE(value)|(1<<8)) );
    }
    else
    {
        setReg(DW8051_RDY, (DW8051_RDY_RATE(value)) );
    }
    for(i=0;i<65535;i++);
    uart_init();
    timer_init();
    for(i=0;i<0x100;i++);
}

// Set 8051 Ready: sets bit 0 of DW8051_RDY
void set_8051RDY() large{
    volatile uint32 xdata tmp;
    tmp = getReg(DW8051_RDY);

    setReg(DW8051_RDY, tmp |0x1);
}


// System UART Command: drains the UART receive buffer, builds lines,
// handles CLI commands (help, clear, IP dump, MAC set).
// Bytes that belong to a JSON data line ('{' ... newline) are never
// treated as CLI commands.
// FIX: a '{' ALWAYS starts a new JSON line (buffer is reset), so a lost
// newline or stray byte can no longer let JSON letters trigger the CLI.
void sys_uart_cmd(void)
{
    uint8 cmd;
    uint8 n;
    static uint8 in_json_line = 0;

    for (n = 0; n < 200; n++)
    {
        cmd = cmd_select();
        if (cmd == 0xFF)
            break;                      /* nothing more buffered */

        if (cmd == '{')                 /* FIX: resync on every '{' */
        {
            m_u8_temp_cntr = 0;
            in_json_line = 1;
        }

        if (m_u8_temp_cntr < (sizeof(m_u8_uart_str) - 1))
        {
            m_u8_uart_str[m_u8_temp_cntr++] = cmd;

            if (cmd == 0x0A)
            {
                m_u8_uart_str[m_u8_temp_cntr] = '\0';
                process_uart_string();
                m_u8_temp_cntr = 0;
                in_json_line = 0;
            }
        }
        else
        {
            /* Overflow: discard incomplete message */
            m_u8_temp_cntr = 0;
            in_json_line = 0;
            memset(m_u8_uart_str, 0, sizeof(m_u8_uart_str));

            /* Ignore remaining bytes until newline */
            while (cmd != 0x0A)
            {
                cmd = cmd_select();
                if (cmd == 0xFF)
                    break;
            }
        }

        if (!in_json_line)
        {
            switch(cmd)
            {
                case CLI_COMMAND_HELP:
                    print_menu();
                    break;

                case CLI_COMMAND_CLEAR:
                    clearScreen();
                    break;

                case CLI_COMMAND_DUMP_IP:
                    printf("  IP:%d.%d.%d.%d\n",
                        (uint16)this_ip[0], (uint16)this_ip[1],
                        (uint16)this_ip[2], (uint16)this_ip[3]);
                    break;

                case CLI_COMMAND_MAC_SET:
                    printf("Format: XX:XX:XX:XX:XX:XX, press Enter to submit.\n");
                    printf("input: ");
                    cliGetMacAddr();
                    break;

                default:
                    break;
            }
        }
    }
}


uint8 m_u8_strlen;


/* Number of entries in the "P" and "I" arrays of a PDU frame.
   Set to 0 to disable the count check. */
#define PDU_CHANNEL_COUNT 8

uint16 m_u16_json_ok  = 0;   /* PDU status frames accepted           */
uint16 m_u16_json_bad = 0;   /* frames rejected as corrupted/unknown */
uint16 m_u16_json_rtc = 0;   /* RTC replies routed                   */
uint16 m_u16_json_alm = 0;   /* alarm replies routed                 */

/* Returns number of items in the array that follows key (e.g. "\"P\""),
   or 0xFF if malformed. */
static uint8 json_array_items(char *s, const char *key)
{
    char *p;
    uint8 items = 1;

    p = strstr(s, key);
    if (p == NULL) return 0xFF;
    p += strlen(key);
    while (*p == ' ') p++;
    if (*p != ':') return 0xFF;
    p++;
    while (*p == ' ') p++;
    if (*p != '[') return 0xFF;
    p++;
    if (*p == ']') return 0;

    while (*p != ']')
    {
        if (*p == '\0') return 0xFF;
        if (*p == ',')
            items++;
        else if (!((*p >= '0' && *p <= '9') || *p == '.' || *p == '-' || *p == '+' || *p == ' '))
            return 0xFF;                /* unexpected character inside array */
        p++;
    }
    return items;
}

/* Returns 1 if the number after key (e.g. "\"V\"") looks valid. */
static uint8 json_number_ok(char *s, const char *key)
{
    char *p = strstr(s, key);
    if (p == NULL) return 0;
    p += strlen(key);
    while (*p == ' ') p++;
    if (*p != ':') return 0;
    p++;
    while (*p == ' ') p++;
    return (uint8)((*p >= '0' && *p <= '9') || *p == '-');
}

// Process UART String:
//  1. PDU status frame {"V":..,"P":[8],"I":[8]}  -> validated, then sent to
//     the web page and the SNMP JSON store.
//  2. GET_ALARM reply {"alarms":[...]}           -> Copy_Alarm_DATA()
//  3. GET_RTC / SETRTC reply {"H":..,"MO":..}    -> Copy_RTC_DATA()
//  4. Anything else (ALARM_SET, ALARM_FIRED, RTC_SET ...) is ignored.
// Routing is by CONTENT, not by start_xxx flags, so a status poll can
// never make an RTC or alarm reply get lost.
void process_uart_string(void)
{
    uint8 temp_var;
    uint8 valid = 0;
    uint8 len;
    char *s = (char *)m_u8_uart_str;

    m_u8_strlen = strlen(s);

    /* ignore trailing CR / LF */
    len = m_u8_strlen;
    while (len > 0 && (s[len - 1] == '\r' || s[len - 1] == '\n'))
        len--;

    if (len >= 10 && s[0] == '{' && s[len - 1] == '}')
    {
        if (json_number_ok(s, "\"V\"") &&
            json_array_items(s, "\"P\"") != 0xFF &&
            json_array_items(s, "\"I\"") != 0xFF)
        {
            valid = 1;
#if PDU_CHANNEL_COUNT
            if (json_array_items(s, "\"P\"") != PDU_CHANNEL_COUNT ||
                json_array_items(s, "\"I\"") != PDU_CHANNEL_COUNT)
            {
                valid = 0;
            }
#endif
        }
    }

    if (valid)
    {
        m_u16_json_ok++;
        send_str_web(m_u8_uart_str);
        update_json_string((char *)m_u8_uart_str);
    }
    else if (len >= 2 && s[0] == '{' && s[len - 1] == '}')
    {
        if (strstr(s, "\"alarms\""))
        {
            m_u16_json_alm++;
            Copy_Alarm_DATA(m_u8_uart_str);     /* GET_ALARM reply */
        }
        else if (strstr(s, "\"MO\""))
        {
            m_u16_json_rtc++;
            Copy_RTC_DATA(m_u8_uart_str);       /* GET_RTC / SETRTC reply */
        }
        else if (strstr(s, "\"ALARM_") || strstr(s, "\"RTC_SET\""))
        {
            /* short acknowledgements: nothing to store */
        }
        else
        {
            m_u16_json_bad++;
        }
    }
    else
    {
        m_u16_json_bad++;
    }

    /* Clear receive buffer */
    for (temp_var = 0; temp_var < sizeof(m_u8_uart_str); temp_var++)
    {
        m_u8_uart_str[temp_var] = '\0';
    }
}