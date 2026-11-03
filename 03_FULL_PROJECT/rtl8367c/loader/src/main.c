#pragma STRING (far)

#include <stdio.h>
#include <common.h>
#include <reg_sfr.h>
#include <SPIFlashDrv.h>
#include <upgrade_tftp.h>
#include <local/lwps_config.h>
#include <runtime_loader_msg.h>
#include <rtl8367c_base.h>

#define LOADER_ENTRY_ADDR   0xFD0;
extern uint16 phyAllPortMask;

#ifdef OLD_TFTP_FWUG
void upgrade_image_tftp(void) large {
	uint32       addr;
	uint8        i;
	
	OSAL_PRINTF("    Get RunTime From Server : %bu.%bu.%bu.%bu\n", SERVER_IP);
	/* Send ARP, get TFTP Server Mac */
	getServerMAC();
	OSAL_PRINTF("    [ServerMac = ");
	for(i=0;i<6;i++){
		OSAL_PRINTF("0x%02x ", (uint16)target_mac[i]);
	}
	OSAL_PRINTF("]\n");

	
	/* Erase Flash */
	/* Erase Run Time IROM Area, 4KB~16KB area */
	addr = CONFIG_FLASH_SECTOR_SIZE;
	for (i=1; i<=3; i++, addr+=CONFIG_FLASH_SECTOR_SIZE){
	        //OSAL_PRINTF("      Erasing 0x%08lx\n", addr);
		flash_erase(addr, FLASH_ERASE_BLOCK);
	}

    
	/* Erase Run Time Bank Area, 112KB~???KB area */
	addr = 112*1024UL;
	//for (i=27;i<=171;i++, addr+=CONFIG_FLASH_SECTOR_SIZE){
	for (addr = 112*1024UL;addr<(spi_chip0_chip_size);addr+=CONFIG_FLASH_SECTOR_SIZE){
	        //OSAL_PRINTF("      Erasing 0x%08lx\n", addr);
		flash_erase(addr, FLASH_ERASE_BLOCK);
	}
	OSAL_PRINTF("    [Flash Erase Done]\n");
    

	/* Update Run Time Image */
	while(1){
		if ( 0!=tftp_transfer() ){
			OSAL_PRINTF("Update Fail!!!\n");
		}else{
			break;
		}
		for(i=0;i<255;i++);
	}
}
#endif

static char _getchar_nb(void){
	char c;	
	while(!RI){
		return(0xFF);
	}
	RI=0; 
	c=SBUF0; 
	return c; 
}

extern int32 sal_sys_runtime_crc_set(uint8 error);

void main()
{
    uint8 cmd_downcount, cmd, start_mode;
    uint32 wait_cmd_delay;
    uint16 tmp;
#if defined(PARALLEL_MODE)
        uint16 tmpreg, ledValue;
#endif

    setReg(0x1d32, 0x0004); //Enable 67C UART
    setReg(0x1d3f, 0x0100); //Force deglitch enable

    /*  LED setting
    0x1b03 [3:0]  Group 0
    0x1b03 [7:4]  Group 1
    0x1b03 [11:8] Group 2
    
    0000 LED_Off
    0001 Dup/Col
    0010 Link/Act
    0011 Spd1000
    0100 Spd100
    0101 Spd10
    0110 Spd1000/Act 6
    0111 Spd100/Act
    1000 Spd10/Act
    1001 Spd100 (10)/Act 9
    1010 xx
    1011 xx
    1100 Link/Rx
    1101 Link/Tx
    1110 xx
    1111 Act
    */
        
/* LED setting */
#if 1							// put 1 for 10 port and 0 for 8 port (change 1 of 3)
setReg(0x1b00, 0x14a1);//LED1 Power on light
//setReg(0x1b03, 0x690); // for change yellow to green...
setReg(0x1b03, 0x0069);
setReg(0x1b3a, 0x3fff);   
#endif
#if 0						// put 0 for 10 port and 1 for 8 port	(change 2 of 3)
/* LED setting */
#if defined(SERIAL_MODE_LED)
    /* LED setting */ /*serial mode, group 0:Spd1000/Act, group 1:Spd100 (10)/Act, group 2:off */
    setReg(0x1b26, 3);
    setReg(0x1b24, 0);
    setReg(0x1b25, 0);
    setReg(0x1B00, 0x14F1);
    setReg(0x1B03, 0x0096);
    setReg(0x1B3A, 0x3fff);    
#else    
#if defined(PARALLEL_MODE)
    /* set LED to Parallel mode */
    setReg(0x1b00, 0x1410);
    /* enable group 0 ~ 1 */
    //setReg(0x1b24, 0x1f1f);
    /* enable group 2 */
    //setReg(0x1b25, 0x001f);
    /* group 0*/
    tmpreg = getReg(0x1b03);
    tmpreg &= ~(0xF << 0);
    ledValue = 6;//Spd1000/Act
    tmpreg |= (ledValue << 0);
    setReg(0x1b03, tmpreg);

    /* group 1*/
    tmpreg= getReg(0x1b03);
    tmpreg &= ~(0xF << 4);
    ledValue = 9;//Spd100 (10)/Act
    tmpreg |= (ledValue << 4);
    setReg(0x1b03, tmpreg);

    tmpreg = getReg(0x1b03);
    tmpreg &= ~(0xF << 8);
    ledValue = 0;//off
    tmpreg |= (ledValue << 8);
    setReg(0x1b03, tmpreg);
    
#else //SCAN mode
    setReg(0x1B00, 0x54F2);
    setReg(0x1B03, 0x0069);   // put 0096 for 10 port and 0960 for 8 port  (change 3 of 3)
    setReg(0x1B24, 0);
		setReg(0x1B25, 0);

#endif	
#endif


    /* power on blinking */
    tmp = getReg(0x130c);
    tmp |= 0x0040;
    setReg(0x130c, tmp);
    /*
    0x1b00 [6]  Group 2 blinking
    0x1b00 [5]  Group 1 blinking
    0x1b00 [4]  Group 0 blinking
    */
    /* blink LED 0 */
    tmp = getReg(0x1B00);
    tmp &= ~(0x0070);
    tmp |= 0x0020;				// blinking group 0 = 0010, group 1 = 0020, group 2 = 0040, 8 port =0020
    setReg(0x1B00, tmp);
#endif
    loader_start_mode_get(&start_mode);
    setReg(0x130c, 0x1470);

	all_init();
        
    
	
    if(START_MODE_WARM == start_mode)
    {
        OSAL_PRINTF("Loader warm start\n");
        loader_main();
    }	  
	
    while(1) 
    {
        OSAL_PRINTF("\n\n==========Loader start===========\n"
            "Press any key to start the normal procedure.\n"
#ifdef CONFIG_USE_FLASH_VIEWER
            "To run SPI flash viewer, press [v]\n"
#endif
            "To enforce the download of the runtime kernel, press [ESC] ");
            
            
        for (cmd_downcount=5;cmd_downcount>0;--cmd_downcount) {
            OSAL_PRINTF(".");
            for (wait_cmd_delay=0;wait_cmd_delay<15000UL;wait_cmd_delay++) ;
            cmd=_getchar_nb();
            if (cmd!=0xff) break;
         }
         OSAL_PRINTF("\n");

            
#ifdef CONFIG_USE_FLASH_VIEWER
         if ((cmd=='v')||(cmd=='V')) {
             if (spi_flash_viewer()==0) continue;
             setReg(0x1322, 1);
             while(1);
         }
#endif

        break;
    }

    if (cmd!=27) {
        OSAL_PRINTF("    Check Runtime Image.....\n");
        /* switch isr to runtime isr */
        loader_isr_set(FALSE);
        check_boot(1);
    }

    sal_sys_runtime_crc_set(1); //runtime checksum error
    
#ifdef LOADER_FWUG_OLD
    OSAL_PRINTF("    Download Runtime Image.....\n");
    upgrade_image_tftp();	
	setReg(0x1322, 1);
#else

    loader_main();
    
#endif    
    
	while(1);
}

