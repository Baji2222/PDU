#include <stdio.h>
#include <stdlib.h>
#include <lwps\lwps.h>
#include "SPIFlashDrv.h"
extern void check_boot(uint8 to_bootup) large;
extern void print_runtime_header(void *p);
extern void *get_runtime_header(void);
//void upgrade_image_tftp(void) large;

#define CMDLINE_SIZE 100
#ifdef CONFIG_USE_FLASH_VIEWER
void dump_xdata_buf(uint8 *buf, uint16 l) large {
    uint16 i;
    OSAL_PRINTF("       00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n");
    for (i=0;i<l;i+=16, buf+=16) {
        OSAL_PRINTF("[%04x] "
            "%02bx %02bx %02bx %02bx %02bx %02bx %02bx %02bx "
            "%02bx %02bx %02bx %02bx %02bx %02bx %02bx %02bx\n",
            (uint16)i,
            buf[ 0], buf[ 1], buf[ 2], buf[ 3], buf[ 4], buf[ 5], buf[ 6], buf[ 7],
            buf[ 8], buf[ 9], buf[10], buf[11], buf[12], buf[13], buf[14], buf[15]);
    }
    //OSAL_PRINTF("length=%d.\n", l);
}

static void sfv_print_menu() large {
    OSAL_PRINTF(
    "=========================SPI FLASH VIEWER=============================\n"
    "    b: reboot\n"
    "    e <addr>: Erase flash with the address of <addr>\n"
    "    ev <addr>: Erase flash with the address of <addr> and then Verify\n"
    "    r <addr> <len>: read flash from the address of <addr> and then dump\n"
    "    c: check runtime kernel without boot\n"
    "    cb: check runtime kernel and boot if checksum is pass\n"
    "    h: print header\n"
    "    l: load runtime kernel\n"
    "    v: show verobose information\n"
    "    m: print this menu\n"
    "    q: quit from spi flash view\n");

}
static char *sfv_find_word(char *inp) {
    while (((*inp)==' ')||((*inp)=='\t')) ++inp;
    return inp;
}
static char *sfv_find_delimiter(char *inp) {
    while (1) {
        if (((*inp)==' ')||((*inp)=='\t')||((*inp)==0)) return inp;
        ++inp;
    }
}
static char sfv_cook_char(char c) {
    if (c<32) return '.';
    return c;
}
static void sfv_dump_spi_flash(uint32 addr, uint16 l) large {
    uint16 i;
    uint8 buf[64], *p, j;
    
    OSAL_PRINTF("         00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n");
    for (i=0;i<l;i+=64) {
        spi_flash_read(addr, buf, 64);
        for (j=0, p=buf;j<4;++j, p+=16) {
            OSAL_PRINTF("[%06lx] ", addr);
            OSAL_PRINTF(
                "%02bx %02bx %02bx %02bx %02bx %02bx %02bx %02bx "
                "%02bx %02bx %02bx %02bx %02bx %02bx %02bx %02bx  ",
                p[ 0], p[ 1], p[ 2], p[ 3], p[ 4], p[ 5], p[ 6], p[ 7],
                p[ 8], p[ 9], p[10], p[11], p[12], p[13], p[14], p[15]
                );
            OSAL_PRINTF("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n",
                sfv_cook_char(p[ 0]), sfv_cook_char(p[ 1]), sfv_cook_char(p[ 2]), sfv_cook_char(p[ 3]), 
                sfv_cook_char(p[ 4]), sfv_cook_char(p[ 5]), sfv_cook_char(p[ 6]), sfv_cook_char(p[ 7]),
                sfv_cook_char(p[ 8]), sfv_cook_char(p[ 9]), sfv_cook_char(p[10]), sfv_cook_char(p[11]), 
                sfv_cook_char(p[12]), sfv_cook_char(p[13]), sfv_cook_char(p[14]), sfv_cook_char(p[15])
                );
            addr+=16;
        }
    }
    OSAL_PRINTF("length=0x%x(%d).\n", l, l);
}
static void sfv_erase_spi_flash(uint32 addr, uint8 to_verify) large {
    uint8 buf[64];
    uint8 l=(uint8)(CONFIG_SPI_SECTOR_SIZE/64);
    uint8 *p, *q=buf+64;
    
    if (spi_flash_erase(addr, SPI_ERASE_BLOCK)==FALSE) {
        OSAL_PRINTF("Erase 0x%06lx: FAIL (may due to protected regions)\n", addr);
        return;
    }

    OSAL_PRINTF("Erase 0x%06lx: done\n", addr);
    if (!to_verify) return;
    for (;l>0;--l, addr+=64) {
        spi_flash_read(addr, buf, 64);
        for (p=buf; p!=q; ++p){
            if (0xff!=*p) {
                OSAL_PRINTF("Verify the erase: FAIL at 0x%06lx\n", addr);
                OSAL_PRINTF("data read back:\n"); 
                dump_xdata_buf(buf, 64);
                return;
            }
        }
    }
    OSAL_PRINTF("Erase and verify: SUCCESS\n");
}
static void sfv_show_verobose_infor(void) {
    uint8 i;
    
    // show proected regsion
    OSAL_PRINTF("There %bd protected region(s).\n", num_flash_protected_area>>1);
    for (i=0; i<num_flash_protected_area; i+=2){
        OSAL_PRINTF("    Portected region %bd: 0x%06lx-0x%06lx\n", 
            i>>1, flash_protect_area[i], flash_protect_area[i+1]);
    }
}
uint8 spi_flash_viewer() large {
    char cmdline[CMDLINE_SIZE], *s, *e;
    uint32 addr;
    uint16 len;
    uint8 flag;
    uint8 last_is_read=0, last_is_erase=0;
    uint8 write_buffer;
    sfv_print_menu();
    while(1) {
        flag=0;
        OSAL_PRINTF(">"); gets(cmdline, CMDLINE_SIZE-1);
        s=sfv_find_word(cmdline);
        if (*s==0) {
            if (last_is_read) {
                addr += len;
                sfv_dump_spi_flash(addr, len);
            } else if (last_is_erase) {
                addr += CONFIG_SPI_SECTOR_SIZE;
                sfv_erase_spi_flash(addr, flag);
            }
            continue;
        }
        last_is_read=0;
        last_is_erase=0;
        e=sfv_find_delimiter(s);
        if (s[0]=='e') {
            if (s[1]=='v') flag=1;
            s=sfv_find_word(e);
            if (*s==0) goto unkown_message;
            addr=strtoul(s, &e, 0);
            addr &= (0x1000000 - CONFIG_SPI_SECTOR_SIZE);
            sfv_erase_spi_flash(addr, flag);
            last_is_erase=1;
            continue;
        } else if (s[0]=='w'){
            if (e!=s+1) goto unkown_message;
            s=sfv_find_word(e);
            if (*s==0) goto unkown_message;
            addr=strtoul(s, &e, 0);
            s=sfv_find_word(e);
            if (*s==0) goto unkown_message;
            write_buffer=(uint8)strtoul(s, &e, 0);
            spi_flash_write(addr, &write_buffer, 1);
            continue;
        } else if (s[0]=='r'){
            if (e!=s+1) goto unkown_message;
            s=sfv_find_word(e);
            if (*s==0) goto unkown_message;
            addr=strtoul(s, &e, 0);
            s=sfv_find_word(e);
            if (*s==0) goto unkown_message;
            len=strtoul(s, &e, 0);
            len=(len+(uint8)(addr&0x0f)+15)&0xfff0;
            addr &= 0xffff0;
            sfv_dump_spi_flash(addr, len);
            last_is_read=1;
            continue;
        } else if (s[0]=='h'){
            if (s[1]==0) {
                get_runtime_header();
                print_runtime_header(NULL);
                continue;
            }
        } else if (s[0]=='c'){
            if (s[1]=='b') flag=1;
            check_boot(flag);
            continue;
/*            
        } else if (s[0]=='l'){
            if (s[1]==0) {
                upgrade_image_tftp();
                continue;
            }
*/            
        } else if (s[0]=='v'){
            if (s[1]==0) {
                sfv_show_verobose_infor();
                continue;
            }
        } else if (s[0]=='m'){
            if (s[1]==0) {
                sfv_print_menu();
                continue;
            }
        } else if (s[0]=='b'){
            if (s[1]==0) return 1;
        } else if (s[0]=='q'){
            if (s[1]==0) return 0;
        } 
unkown_message:
        OSAL_PRINTF("unknown command: %s\n", cmdline); 
    }
}
#endif
