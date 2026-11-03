#pragma STRING (far)

#include <local/config.h>
#include <reg_sfr.h>

#include <common.h>
#include <asicreg.h>
#include <image.h>

#include <stdio.h>


typedef void (*rt_entry)(void);
void print_runtime_header(void *p);
void *get_runtime_header(void);
u8_t check_header(u8_t *hdr_adr){
	u32_t sum, org_chksum;
	u16_t i;
	u8_t *u8_ptr;
	runTimeImageHeader_t *rt_header = (runTimeImageHeader_t *)hdr_adr;

	/* Check Magic Number */
	if( RUNTIME_MAGIC_NUMBER!=rt_header->magic_number ){
		/* TBC Magic Number Error */
		return 1;
	}

	/* Check Header Checksum */
	sum = 0;
	u8_ptr = (u8_t*)rt_header;
	org_chksum = rt_header->header_chksum;
	rt_header->header_chksum = 0;

	for (i=0; i<sizeof(runTimeImageHeader_t); i++, u8_ptr++){
		sum += *u8_ptr;
	}
	
	if( org_chksum!=sum ){
		/* TBC header_chksum Error */
		return 1;
	}
	rt_header->header_chksum = org_chksum;

	return 0;
}



// this function should be in irom
static uint32 sum_up_payload(uint32 len) large {
    uint32 sum=0;
    
    #pragma asm
        PUSH    CODE_BANK
        MOV     CODE_BANK, #(RUNTIME_HEADER_BANK)
    #pragma endasm
    
    while(len>=BANK_SIZE) {
        #pragma asm
            // R4/R5/R6: sum (R4: LSB, R6: MSB)
            CLR     A
            MOV     R4, A
            MOV     R5, A
            MOV     R6, A
            MOV     DPTR, #(IROM_size)
            // do {
?sumup_002:
                // R0:R1: sub-sum (R0: LSB, R1: MSB)
                // R2: 8-bit counter 
                CLR     A
                MOV     R0, A
                MOV     R1, A
                MOV     R2, A
    
                // do {(R0:R1) += *(dptr++); R2++;} while(R2!=0);
?sumup_003:
                    CLR     A
                    MOVC    A, @A+DPTR
                    ADD     A, R0
                    MOV     R0, A
                    CLR     A
                    ADDC    A, R1
                    MOV     R1, A
                    INC     DPTR
                    DJNZ    R2, ?sumup_003
                
                // (R4:R5:R6) += (R0:R1)
                MOV     A, R0
                ADD     A, R4
                MOV     R4, A
                MOV     A, R1
                ADDC    A, R5
                MOV     R5, A
                CLR     A
                ADDC    A, R6
                MOV     R6, A
            
            // while (dptr!=0);
            MOV     A, DPH
            JNZ     ?sumup_002
            
            // sum += (R4:R5:R6)
            MOV     DPTR, #(sum?147 + 3)
            MOVX    A, @DPTR
            ADD     A, R4
            MOVX    @DPTR, A
            MOV     DPTR, #(sum?147 + 2)
            MOVX    A, @DPTR
            ADDC    A, R5
            MOVX    @DPTR, A
            MOV     DPTR, #(sum?147 + 1)
            MOVX    A, @DPTR
            ADDC    A, R6
            MOVX    @DPTR, A
            MOV     DPTR, #(sum?147 + 0)
            MOVX    A, @DPTR
            ADDC    A, #0
            MOVX    @DPTR, A
            
            MOV     A, CODE_BANK 
            INC     A
            MOV     CODE_BANK, A
        #pragma endasm
        len -= BANK_SIZE;
    }        
    #pragma asm    
        // (R0:R1)=(unsigned int)len;
        MOV     DPTR, #(len?146 + 2)
        MOVX    A, @DPTR
        MOV     R1, A
        INC     DPTR
        MOVX    A, @DPTR
        MOV     R0, A
        
        // (R4:R5:R6:R7)=sum
        MOV     DPTR, #(sum?147)
        MOVX    A, @DPTR
        MOV     R7, A
        INC     DPTR
        MOVX    A, @DPTR
        MOV     R6, A
        INC     DPTR
        MOVX    A, @DPTR
        MOV     R5, A
        INC     DPTR
        MOVX    A, @DPTR
        MOV     R4, A
        
        // DPTR=IROM
        MOV     DPTR, #(IROM_size)
        
        // while ((len--)!=0) {
?sumup_012:
        MOV     A, R0
        ORL     A, R1
        JZ      ?sumup_011
        DEC     R0
        CJNE    R0, #(0xFF), ?sumup_013
        DEC     R1
?sumup_013:
        //      sum += *(dptr++)
            CLR     A
            MOVC    A, @A+DPTR
            ADD     A, R4
            MOV     R4, A
            CLR     A
            ADDC    A, R5
            MOV     R5, A
            CLR     A
            ADDC    A, R6
            MOV     R6, A
            CLR     A
            ADDC    A, R7
            MOV     R7, A
            
            INC     DPTR
        // }
        SJMP    ?sumup_012
?sumup_011:
        
        // sum=(R4:R5:R6:R7)
        MOV     DPTR, #(sum?147)
        MOV     A, R7
        MOVX    @DPTR, A
        INC     DPTR
        MOV     A, R6
        MOVX    @DPTR, A
        INC     DPTR
        MOV     A, R5
        MOVX    @DPTR, A
        INC     DPTR
        MOV     A, R4
        MOVX    @DPTR, A
        
        POP     CODE_BANK
    #pragma endasm    
    
    return sum;
}


static runTimeImageHeader_t xdata rt_header ; //_at_ 0x1400;
//static u8_t bank_reg_backup _at_ 0x13ff;
void check_boot(uint8 to_bootup) large {
	u32_t sum, org_chksum, org_length, irom_sum, banked_sum;
	u16_t i;
	u8_t xdata *u8_ptr;
	u8_t code *u8_code_ptr;
	rt_entry fp;
	
	get_runtime_header();
        //print_runtime_header(NULL);
        
	if ( 0!=check_header((u8_t*)&rt_header) ){
		OSAL_PRINTF("    Hdr Chksum Error\n");
		return ;
	}

	/* Check Image Payload Checksum */	
	u8_code_ptr = (u8_t code *)0x1000;
	//OSAL_PRINTF("    First 4 bytes of iROM Part: %02bx:%02bx:%02bx:%02bx\n", 
	    //u8_code_ptr[0], u8_code_ptr[1], u8_code_ptr[2], u8_code_ptr[3]);
	
	// irom checksum
	sum = 0;
	for (i=0; i<(RUNTIME_IROM_SIZE-2); i++, u8_code_ptr++){
		sum += *u8_code_ptr;
	}
	irom_sum=sum;
	
	// compensate checksum
	banked_sum=0;
	u8_ptr = (u8_t xdata *)(&rt_header);
	for(i=0; i<sizeof(runTimeImageHeader_t); i++, u8_ptr++){
		banked_sum += 0xFFUL - *u8_ptr;
	}
	org_length = rt_header.length - (RUNTIME_IROM_SIZE - 2);

        banked_sum+=sum_up_payload(org_length);
    
        sum+=banked_sum;
	
	org_chksum = rt_header.payload_chksum;
	if( org_chksum!=sum ){
		OSAL_PRINTF("    PayLoad Chksum Error\n");
		//OSAL_PRINTF("      Org=0x%08lx, sum=0x%08lx\n", org_chksum, sum);
		//OSAL_PRINTF("      iROM sum=0x%08lx\n", irom_sum);
		//OSAL_PRINTF("      banked area sum=0x%08lx\n", banked_sum);
		return;
	}
	OSAL_PRINTF("    Chksum Correct!\n");
    if (to_bootup) {
	OSAL_PRINTF("    RunTime Kernel Starting....  \n");
	fp = (rt_entry)(0x1000);
	fp();
    }
}

void *get_runtime_header(void) {
    //runTimeImageHeader_t xdata *rt_header_far = (runTimeImageHeader_t *)(RUNTIME_HEADER_ADDRESS&0xFFFF);
    #define rt_header_code ((runTimeImageHeader_t code *)(RUNTIME_HEADER_ADDRESS&0xFFFF))
    #pragma asm
        PUSH    CODE_BANK
        MOV     CODE_BANK, #(RUNTIME_HEADER_BANK)
    #pragma endasm

    rt_header.magic_number   = rt_header_code->magic_number;
    rt_header.length         = rt_header_code->length;
    rt_header.header_chksum  = rt_header_code->header_chksum;
    rt_header.payload_chksum = rt_header_code->payload_chksum;
    rt_header.reserved       = rt_header_code->reserved;
    
    #pragma asm
        POP     CODE_BANK
    #pragma endasm
    
    return &rt_header;
}

void print_runtime_header(void *p) {
    runTimeImageHeader_t *q=(p!=NULL)?(runTimeImageHeader_t *)p:&rt_header;
    OSAL_PRINTF("    magic_number:  0x%08lx\n", q->magic_number);
    OSAL_PRINTF("    length:        0x%08lx (%ld)\n", q->length, q->length);
    OSAL_PRINTF("    header_chksum: 0x%08lx\n", q->header_chksum);
    OSAL_PRINTF("    payload_chksum:0x%08lx\n", q->payload_chksum);
    OSAL_PRINTF("    reserved:      0x%08lx\n", q->reserved);
}
