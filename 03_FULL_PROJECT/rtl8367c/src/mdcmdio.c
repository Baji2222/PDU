#pragma STRING (far)

#include <stdio.h>
#include <common.h>
#include <reg_sfr.h>
#include <salgpio.h>


#define MDC_GPIO         1
#define MDIO_GPIO        0
#define I2C_SDA_GPIO     5
#define I2C_SCLK_GPIO    6
#define    HIGH        1
#define    LOW         0

void mdc_mdio_init(void)
{
    sal_gpio_init(MDC_GPIO);
    sal_gpio_init(MDIO_GPIO);
    sal_gpio_dir_set(MDC_GPIO, SAL_GPIO_DIR_OUTPUT);
    sal_gpio_dir_set(MDIO_GPIO, SAL_GPIO_DIR_OUTPUT);
}
void sethigh(void)
{                        
    sal_gpio_output_set(MDC_GPIO, LOW);
    sal_gpio_output_set(MDIO_GPIO, HIGH);
    sal_gpio_output_set(MDC_GPIO, HIGH);
    
}
void setlow(void)
{
    sal_gpio_output_set(MDC_GPIO, LOW);
    sal_gpio_output_set(MDIO_GPIO, LOW);   
    sal_gpio_output_set(MDC_GPIO, HIGH);
}
void w_mdc_mdio(uint8 PHY_ADDR,uint8 REG_ADDR,uint16 Odata)
{
	int i;
	uint8 ONE;
	uint8 w_prmb=32;
  //32 bit preamble
	for (i=0;i<w_prmb;i++)
		sethigh();

	setlow();  sethigh(); //start of frame
	setlow();  sethigh();  //operation code = 01 : write

	// output PHY address
	// printf("PHY_ADDR=%x",PHY_ADDR);
	if((PHY_ADDR & 0x10)>=1) sethigh(); else setlow();
	if((PHY_ADDR & 0x08)>=1) sethigh(); else setlow();
	if((PHY_ADDR & 0x04)>=1) sethigh(); else setlow();
	if((PHY_ADDR & 0x02)>=1) sethigh(); else setlow();
	if((PHY_ADDR & 0x01)>=1) sethigh(); else setlow();

	// output register address
	// printf("REG_ADDR=%x",REG_ADDR);
	if((REG_ADDR & 0x10)>=1) sethigh(); else setlow();
	if((REG_ADDR & 0x08)>=1) sethigh(); else setlow();
	if((REG_ADDR & 0x04)>=1) sethigh(); else setlow();
	if((REG_ADDR & 0x02)>=1) sethigh(); else setlow();
	if((REG_ADDR & 0x01)>=1) sethigh(); else setlow();

	sethigh(); //turn around1
	setlow();  //turn around2

	for(i=15;i>=0;i--) {
		ONE=((Odata>>i) & 0x1)>0?1:0;
		if(ONE) sethigh(); else setlow();
	}
  
} 

uint16 r_mdc_mdio(uint8 PHY_ADDR,uint8 REG_ADDR)
{
	int i;
	int Idata=0;
  uint8 r_prmb=31;
  uint8 tmp;
  //32 bit preamble
	for (i=0;i<r_prmb;i++) // output 32 bit preamble
		sethigh();

	setlow();  sethigh(); //start of frame = 01
	sethigh(); setlow();  //operation code = 10 : read

	// output PHY address
	// printf("PHY_ADDR=%x",PHY_ADDR);
	if((PHY_ADDR & 0x10)>=1) sethigh(); else setlow();
	if((PHY_ADDR & 0x08)>=1) sethigh(); else setlow();
	if((PHY_ADDR & 0x04)>=1) sethigh(); else setlow();
	if((PHY_ADDR & 0x02)>=1) sethigh(); else setlow();
	if((PHY_ADDR & 0x01)>=1) sethigh(); else setlow();

	// output Register address
	// printf("REG_ADDR=%x",REG_ADDR);
	if((REG_ADDR & 0x10)>=1) sethigh(); else setlow();
	if((REG_ADDR & 0x08)>=1) sethigh(); else setlow();
	if((REG_ADDR & 0x04)>=1) sethigh(); else setlow();
	if((REG_ADDR & 0x02)>=1) sethigh(); else setlow();
	if((REG_ADDR & 0x01)>=1) sethigh(); else setlow();


  sal_gpio_dir_set(MDIO_GPIO, SAL_GPIO_DIR_INPUT);
  sal_gpio_output_set(MDC_GPIO, 0); //turn around 1
  sal_gpio_output_set(MDC_GPIO, 1);
  sal_gpio_output_set(MDC_GPIO, 0); //turn around 2
  sal_gpio_output_set(MDC_GPIO, 1);
                   
	for(i=0;i<=15;i++) {
		Idata = Idata<<1;
            sal_gpio_output_set(MDC_GPIO, 0); 
        sal_gpio_input_get(MDIO_GPIO, &tmp);
		// printf("read data=0x%X ",tmp);
		//tmp = tmp & PC_DIN; // extract the bit of PC data in
		if(tmp>0) Idata = Idata | 0x1;
		sal_gpio_output_set(MDC_GPIO, 1); 
	}

  sal_gpio_dir_set(MDIO_GPIO, SAL_GPIO_DIR_OUTPUT);
  sal_gpio_output_set(MDC_GPIO, 1);
	return(Idata);
}

void test_mdio(uint8 PHY_ADDR,uint8 REG_ADDR)
{
    uint16 i,j;
    uint16 err=1;
    setReg(0x1f,0);
    setReg(0x1d,0);
    w_mdc_mdio(1,31,5);
    for(j=0;j<1000;j++)
    for(i=0;i<=0xffff;i++)
    {
        w_mdc_mdio(PHY_ADDR,REG_ADDR,i);
        if(r_mdc_mdio(PHY_ADDR,REG_ADDR)!=i)
            setReg(0x1f,err++);
        setReg(0x1d,i);
    }
}  

//I2C Driver

void i2c_wait(void)
{
    uint8 i;
    for(i=0;i<10;i++);   
}

void i2c_wait2(void)
{
    uint8 i;
    for(i=0;i<5;i++);  
}
void i2c_init(void)
{
    sal_gpio_init(I2C_SDA_GPIO);
    sal_gpio_init(I2C_SCLK_GPIO);
    
    sal_gpio_dir_set(I2C_SDA_GPIO, SAL_GPIO_DIR_OUTPUT);
    sal_gpio_dir_set(I2C_SCLK_GPIO, SAL_GPIO_DIR_OUTPUT);
}

/*******************
 * Start signal
 * 
 *  SCL ________
 *              \_________
 *  SDA _____
 *           \____________
 *****************/  

void i2c_start(void)
{
    sal_gpio_output_set(I2C_SDA_GPIO, HIGH);
    sal_gpio_output_set(I2C_SCLK_GPIO, HIGH);
    i2c_wait();
    sal_gpio_output_set(I2C_SDA_GPIO, LOW);
    i2c_wait2();
    sal_gpio_output_set(I2C_SCLK_GPIO, LOW);
    
}
/*****************************************************************************
 * Stop signal
 *              ____________
 *  SCL _______/
 *                 _________
 *  SDA __________/
 *
 * assume I2C_SCL = LOW, I2C_SDA = LOW
 *****************************************************************************/
void i2c_stop(void)
{
    i2c_wait2();
    sal_gpio_output_set(I2C_SDA_GPIO,LOW);
    i2c_wait2();
    sal_gpio_output_set(I2C_SCLK_GPIO,HIGH);
    i2c_wait2();
    sal_gpio_output_set(I2C_SDA_GPIO,HIGH);
  
}

uint8 i2c_write(unsigned char value)
{
    uint8 i=9;
    uint8  ack;

    while(--i)
    {
        // upload data
        i2c_wait2();
        sal_gpio_output_set(I2C_SDA_GPIO,(value & 0x80) ? HIGH : LOW);
        i2c_wait2();
        // send data
        sal_gpio_output_set(I2C_SCLK_GPIO,HIGH);
        i2c_wait();
        value <<= 1;
        sal_gpio_output_set(I2C_SCLK_GPIO,LOW);
    }

    // get acknowledgement
    //i2c_wait2();
    sal_gpio_output_set(I2C_SDA_GPIO,HIGH);
    sal_gpio_dir_set(I2C_SDA_GPIO, SAL_GPIO_DIR_INPUT);
    i2c_wait2();
    sal_gpio_output_set(I2C_SCLK_GPIO,HIGH);
    i2c_wait2();
    sal_gpio_input_get(I2C_SDA_GPIO, &ack);
    i2c_wait2();
    sal_gpio_output_set(I2C_SCLK_GPIO,LOW);
    sal_gpio_dir_set(I2C_SDA_GPIO, SAL_GPIO_DIR_OUTPUT);

    sal_gpio_output_set(I2C_SDA_GPIO,HIGH);
    return ack;
}

uint8 i2c_read(bit acknowledge)
{
    uint8 value=0;
    uint8 Idata;
    uint8 i=9;

    sal_gpio_dir_set(I2C_SDA_GPIO, SAL_GPIO_DIR_INPUT);
    // read data
     while(--i)
    {
        // download data
        value <<= 1;
        i2c_wait();
        // get data
        sal_gpio_output_set(I2C_SCLK_GPIO,HIGH);
        i2c_wait2();
        sal_gpio_input_get(I2C_SDA_GPIO, &Idata);
        value |= Idata;
        i2c_wait2();
        sal_gpio_output_set(I2C_SCLK_GPIO,LOW);
    }
     sal_gpio_dir_set(I2C_SDA_GPIO, SAL_GPIO_DIR_OUTPUT);
    // send acknowledge
    i2c_wait2();
    sal_gpio_output_set(I2C_SDA_GPIO,acknowledge);
    i2c_wait2();
    sal_gpio_output_set(I2C_SCLK_GPIO,HIGH);
    i2c_wait();
    sal_gpio_output_set(I2C_SCLK_GPIO,LOW);

    // return data
    return value;
}


void i2c_write_byte(uint8 slave_addr, uint8 reg_addr, uint8 value)
{
    uint8 i=10;

    EA = 0;               // Disable All Interrupt
    while (--i)
    {
        i2c_start();                        // start I2C
        if(i2c_write(slave_addr)) continue; // slave address
        if(i2c_write(reg_addr))  continue;  // register address
        if(i2c_write(value))     continue;  // send data
        i2c_stop();                         // stop I2C
        break;
    }
    EA = 1;                // Enable All Interrupt
}
    
uint8 i2c_read_byte(uint8 slave_addr, uint8 reg_addr)
{
    uint8 i=10;
    uint8 value=0;

    EA = 0;               // Disable All Interrupt
    while (--i)
    {
        // send register address
        i2c_start();                                // start I2C
        if(i2c_write(slave_addr&0xfe)) continue;    // slave address
        if(i2c_write(reg_addr)) continue;           // register address
        // read data
        i2c_start();                                // restart I2C
        if(i2c_write(slave_addr | 1)) continue;     // slave address
        value = i2c_read(1);                        // read data
        i2c_stop();                                 // stop I2C
        break;
    }
    EA = 1;                // Enable All Interrupt

    return value;
} 


