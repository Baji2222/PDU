#ifndef __FIRMWARE_UPGRAGE__
#define __FIRMWARE_UPGRAGE__

extern void upgrade_init(uint32 imgLen);
extern int16 upgrade_write_img(uint8 *buf, uint16 len);
extern uint8 upgrade_upload_done();
extern void upgrade_terminate();

extern void upgrade_set_reboot_flag(uint8 reboot);
extern void upgrade_get_reboot_flag(uint8 *reboot);
extern void upgrade_set_img_len(uint32 len);
extern void upgrade_get_img_len(uint32 *len);
extern void upgrade_set_status(uint8 status);

#define FWUG_UNKNOWN_BIN_SIZE    0xFFFFFFFFUL

#define FWUPG_PRINTF 
//#define FWUPG_PRINTF OSAL_PRINTF

typedef enum upgrade_status_e
{
    UPGRADE_SUCCESS = 0,
    UPGRADE_NOT_START,
    UPGRADE_PROGRESSING,
    UPGRADE_FAIIL
} upgrade_status_t;

#endif

