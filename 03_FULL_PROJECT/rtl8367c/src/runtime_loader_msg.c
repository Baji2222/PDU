#include <common.h>
#include <runtime_loader_msg.h>

/* loader cold/warm start magic number */
#define MAGIC_NUMBER_ADDR_LOADER(index)     (0x172e + (index))
#define MAGIC_NUMBER_0                      0x5555
#define MAGIC_NUMBER_1                      0x1234
#define MAGIC_NUMBER_2                      0x5678
#define MAGIC_NUMBER_3                      0xAAAA

/* register address of runtime cold/warm start indicator  */
#define MAGIC_NUMBER_ADDR_RUNTIME(index)    (0x172a + (index))

#define IP_ADDR_H                   0x1723
#define IP_ADDR_L                   0x1724
#define NETMASK_ADDR_H              0x1725
#define NETMASK_ADDR_L              0x1726
#define GATEWAY_ADDR_H              0x1727
#define GATEWAY_ADDR_L              0x1728

#if !defined(LWS_LOADER)
// Runtime IP Set: Stores IP, netmask and gateway addresses in registers for runtime use
// Converts byte arrays to 16-bit register values and writes to designated register addresses
void runtime_ip_set(uint8 *pIp, uint8 *pNetmask, uint8 *pGateway)
{
    setReg(IP_ADDR_H, (uint16)pIp[1] | ((uint16)pIp[0] << 8));
    setReg(IP_ADDR_L, (uint16)pIp[3] | ((uint16)pIp[2] << 8));
    setReg(NETMASK_ADDR_H, (uint16)pNetmask[1] | ((uint16)pNetmask[0] << 8));
    setReg(NETMASK_ADDR_L, (uint16)pNetmask[3] | ((uint16)pNetmask[2] << 8));
    setReg(GATEWAY_ADDR_H, (uint16)pGateway[1] | ((uint16)pGateway[0] << 8));
    setReg(GATEWAY_ADDR_L, (uint16)pGateway[3] | ((uint16)pGateway[2] << 8));
}
#endif

#if defined(LWS_LOADER)
// Runtime IP Get: Reads IP, netmask and gateway addresses from registers for loader use
// Extracts byte values from 16-bit register data and populates output arrays
void runtime_ip_get(uint8 *pIp, uint8 *pNetmask, uint8 *pGateway)
{
    uint16 tmp;

    tmp = getReg(IP_ADDR_H);
    pIp[0] = (tmp >> 8) & 0xFF;
    pIp[1] = tmp & 0xFF;    
    tmp = getReg(IP_ADDR_L);
    pIp[2] = (tmp >> 8) & 0xFF;
    pIp[3] = tmp & 0xFF;    

    tmp = getReg(NETMASK_ADDR_H);
    pNetmask[0] = (tmp >> 8) & 0xFF;
    pNetmask[1] = tmp & 0xFF;    
    tmp = getReg(NETMASK_ADDR_L);
    pNetmask[2] = (tmp >> 8) & 0xFF;
    pNetmask[3] = tmp & 0xFF;    

    tmp = getReg(GATEWAY_ADDR_H);
    pGateway[0] = (tmp >> 8) & 0xFF;
    pGateway[1] = tmp & 0xFF;    
    tmp = getReg(GATEWAY_ADDR_L);
    pGateway[2] = (tmp >> 8) & 0xFF;
    pGateway[3] = tmp & 0xFF;    
}
#endif
#if 0//NOT used

void runtime_start_mode_get(uint8* start_mode)
{
    if(getReg(MAGIC_NUMBER_ADDR_RUNTIME(0)) == MAGIC_NUMBER_0 && \
       getReg(MAGIC_NUMBER_ADDR_RUNTIME(1)) == MAGIC_NUMBER_1 && \
       getReg(MAGIC_NUMBER_ADDR_RUNTIME(2)) == MAGIC_NUMBER_2 && \       
       getReg(MAGIC_NUMBER_ADDR_RUNTIME(3)) == MAGIC_NUMBER_3) 
    {
        *start_mode = START_MODE_WARM;
    }
    else
        *start_mode = START_MODE_COLD;
}
#endif

// Loader Start Mode Get: Checks magic number pattern to determine if loader is warm or cold starting
// Returns START_MODE_WARM if magic pattern matches, otherwise START_MODE_COLD
void loader_start_mode_get(uint8* start_mode)
{
    if(getReg(MAGIC_NUMBER_ADDR_LOADER(0)) == MAGIC_NUMBER_0 && \
       getReg(MAGIC_NUMBER_ADDR_LOADER(1)) == MAGIC_NUMBER_1 && \
       getReg(MAGIC_NUMBER_ADDR_LOADER(2)) == MAGIC_NUMBER_2 && \       
       getReg(MAGIC_NUMBER_ADDR_LOADER(3)) == MAGIC_NUMBER_3) 
    {
        *start_mode = START_MODE_WARM;
    }
    else
        *start_mode = START_MODE_COLD;
}

#if 0
void runtime_start_mode_set(uint8 start_mode)
{
    if(START_MODE_WARM == start_mode)
    {
        setReg(MAGIC_NUMBER_ADDR_RUNTIME(0), MAGIC_NUMBER_0);
        setReg(MAGIC_NUMBER_ADDR_RUNTIME(1), MAGIC_NUMBER_1);        
        setReg(MAGIC_NUMBER_ADDR_RUNTIME(2), MAGIC_NUMBER_2);        
        setReg(MAGIC_NUMBER_ADDR_RUNTIME(3), MAGIC_NUMBER_3);        
    }
    else if(START_MODE_COLD == start_mode)
    {
        setReg(MAGIC_NUMBER_ADDR_RUNTIME(0), 0);
        setReg(MAGIC_NUMBER_ADDR_RUNTIME(1), 0);        
        setReg(MAGIC_NUMBER_ADDR_RUNTIME(2), 0);        
        setReg(MAGIC_NUMBER_ADDR_RUNTIME(3), 0);        
    }
}
#endif
#if !defined(LWS_LOADER)
// Loader Start Mode Set: Writes magic number pattern to indicate warm or cold start for loader
// Sets magic pattern for warm start or clears it for cold start to persist across reboot
void loader_start_mode_set(uint8 start_mode)
{
    if(START_MODE_WARM == start_mode)
    {
        setReg(MAGIC_NUMBER_ADDR_LOADER(0), MAGIC_NUMBER_0);
        setReg(MAGIC_NUMBER_ADDR_LOADER(1), MAGIC_NUMBER_1);        
        setReg(MAGIC_NUMBER_ADDR_LOADER(2), MAGIC_NUMBER_2);        
        setReg(MAGIC_NUMBER_ADDR_LOADER(3), MAGIC_NUMBER_3);        
    }
    else if(START_MODE_COLD == start_mode)
    {
        setReg(MAGIC_NUMBER_ADDR_LOADER(0), 0);
        setReg(MAGIC_NUMBER_ADDR_LOADER(1), 0);        
        setReg(MAGIC_NUMBER_ADDR_LOADER(2), 0);        
        setReg(MAGIC_NUMBER_ADDR_LOADER(3), 0);        
    }
}
#endif