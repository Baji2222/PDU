//// Version: 1.4
//// Date: 08-07-2025
//// Description: Power over Ethernet (PoE) controller with UART, ADC, and 74HC595 shift register
////              Configures PE6 as input with internal pull-up
//#include "SYSCFG.h"
//#include <stdio.h>
//#include <string.h>
//#include <stdlib.h>
//#include <math.h>
////===========================================================
//// Macros
////===========================================================
//#define uint8_t     unsigned char
//#define uint16_t    unsigned int
//#define uint32_t    unsigned long
//// 74HC595 Shift Register Pins
//#define SCK_74HC595                 LATFbits.LATF1
//#define DATA_74HC595                LATFbits.LATF2
//#define LATCH_74HC595               LATFbits.LATF3
//#define SCK_74HC595_DIR             TRISFbits.TRISF1
//#define DATA_74HC595_DIR            TRISFbits.TRISF2
//#define LATCH_74HC595_DIR           TRISFbits.TRISF3
//// AC DETECTION PIN
//#define AC_DETECTION                LATEbits.LATE6
//#define AC_DETECTION_DIR            TRISEbits.TRISE6
//#define AC_DETECTION2                LATEbits.LATE7
//#define AC_DETECTION_DIR2            TRISEbits.TRISE7
//// Voltage Levels
//#define START_24_VOLT               24
//#define START_30_VOLT               30
//#define START_48_VOLT               48
//#define START_56_VOLT               56
//// Buffer Sizes
//#define BUFFER_MAX_SIZE             512
//#define FRAME_SIZE                  128
//// GPIO Definitions
//#define GPIO_30VOLT_SET             LATAbits.LATA4
//#define GPIO_48VOLT_SET             LATFbits.LATF5
//#define GPIO_56VOLT_SET             LATFbits.LATF4
//#define GPIO_30VOLT_SET_DIR         TRISAbits.TRISA4
//#define GPIO_48VOLT_SET_DIR         TRISFbits.TRISF5
//#define GPIO_56VOLT_SET_DIR         TRISFbits.TRISF4
//#define GPIO_AC_ON_AND_OFF          LATAbits.LATA1
//#define BATTERY_VOLTAGE_SENSE       PORTAbits.RA2
//#define BATTERY_CURRENT_SENSE       PORTAbits.RA3
//#define GPIO_DC_ON                  LATAbits.LATA0
//#define GPIO_DC_OUT_SENSE           PORTEbits.RE5
//#define GPIO_DC_OUT_DISCHARGE       LATAbits.LATA4
//#define GPIO_DC_DIS_CURRENT_SENSE   PORTEbits.RE3
//#define GPIO_POSITIVE_POE_CTRL      LATEbits.LATE2
//#define GPIO_NEGTIVE_POE_CTRL       LATEbits.LATE1
//#define GPIO_AC_ON_AND_OFF_DIR      TRISAbits.TRISA1
//#define BATTERY_VOLT_SENSE_DIR      TRISAbits.TRISA2
//#define BATTERY_CURRENT_SENSE_DIR   TRISAbits.TRISA3
//#define GPIO_DC_ON_DIR              TRISAbits.TRISA0
//#define GPIO_DC_OUT_SENSE_DIR       TRISEbits.TRISE5
//#define GPIO_DC_OUT_DISCHARGE_DIR   TRISEbits.TRISE4
//#define GPIO_DC_DIS_CURRENT_SENSE_DIR TRISEbits.TRISE3
//#define GPIO_POSITIVE_POE_CTRL_DIR  TRISEbits.TRISE2
//#define GPIO_NEGTIVE_POE_CTRL_DIR   TRISEbits.TRISE1
//// ADC Channel
//#define ADC_CHANNEL_AN2             0B000010
//#define AN33                        33
//#define TEMPERATURE_CHANNEL         AN33
//// Battery Percentage Thresholds
//#define BATT_10P                    2033
//#define BATT_25P                    2075
//#define BATT_50P                    2185
//#define BATT_75P                    2288
//#define BATT_100P                   2385
//#define TEMP_HIGH_V   0.650f   // 60°C
//#define TEMP_LOW_V    1.000f   // 40°C
//#define FAN_ON        1
//#define FAN_OFF       0
////===========================================================
//// Variables
////===========================================================
//volatile uint8_t usart_rx_buff[FRAME_SIZE] = {0};
//volatile uint8_t receive_complete_flag = 0;
//// UART2 buffers removed as not needed
//volatile uint16_t sec_1 = 0;
//volatile uint8_t ms_100 = 0;
//volatile uint8_t ms_1 = 0;
//volatile uint16_t adc_48v = 0;
//volatile uint8_t sn74hc595_buffer = 0;
//volatile uint8_t batt_10per_flag = 0;
//volatile uint16_t adcData = 0;
//volatile uint16_t theVoltage = 0;
//volatile uint8_t batteryPercentage = 0;
//volatile uint8_t new_ac_voltage = 0;
//volatile uint8_t ac_status = 0;
//volatile float temp_c = 0.0f;
//typedef struct {
//    float temp_c;
//    float volt;
//} temp_map_t;
//static const temp_map_t temp_map[] =
//{
//    {25, 1.534}, {26, 1.511}, {27, 1.488}, {28, 1.465},
//    {29, 1.431}, {30, 1.409}, {31, 1.386}, {32, 1.363},
//    {33, 1.341}, {34, 1.318}, {35, 1.295}, {36, 1.273},
//    {37, 1.250}, {38, 1.227}, {39, 1.204}, {40, 1.181},
//    {41, 1.158}, {42, 1.136}, {43, 1.113}, {44, 1.090},
//    {45, 1.067}, {46, 1.044}, {47, 1.022}, {48, 0.999},
//    {49, 0.976}, {50, 0.953}, {51, 0.931}, {52, 0.908},
//    {53, 0.885}, {54, 0.862}, {55, 0.840}, {56, 0.817},
//    {57, 0.794}, {58, 0.771}, {59, 0.749}, {60, 0.726},
//    {61, 0.703}, {62, 0.680}, {63, 0.658}, {64, 0.635},
//    {65, 0.612}, {66, 0.582}, {67, 0.564}, {68, 0.546},
//    {69, 0.528}, {70, 0.509}
//};
//#define TEMP_MAP_SIZE (sizeof(temp_map) / sizeof(temp_map[0]))
//// ADC Calibration Tables
//typedef enum {
//    E_VDD5_VREF5 = 0,
//    E_VDD5_VREF3,
//    E_VDD3_VREF3,
//    E_VDD5_VREF2,
//    E_OTHER,
//} ADC_SAMPLE_CONFIG_TYPE;
//const uint16_t c16_adc_rangeTable_VDD5_VREF5[64] = {
//    0x000, 0x017, 0x036, 0x05A, 0x07B, 0x0A3, 0x0CE, 0x0F2,
//    0x11D, 0x14F, 0x175, 0x19C, 0x1D2, 0x22F, 0x25C, 0x296,
//    0x2CE, 0x310, 0x350, 0x392, 0x3D8, 0x431, 0x496, 0x514,
//    0x594, 0x652, 0x7FF, 0x908, 0x9C7, 0xA7F, 0xAE7, 0xB3F,
//    0xB8F, 0xBEF, 0xC49, 0xCA2, 0xCE8, 0xD27, 0xD69, 0xDB7,
//    0xDF1, 0xDFF, 0xE13, 0xE4F, 0xE8D, 0xECF, 0xF05, 0xF41,
//    0xF7D, 0xFAB, 0xFE7, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
//    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF
//};
//const uint16_t c16_adc_rangeTable_VDD5_VREF3[64] = {
//    0x000, 0x01A, 0x054, 0x087, 0x0B8, 0x0F6, 0x13D, 0x179,
//    0x1B7, 0x24F, 0x297, 0x2DA, 0x337, 0x397, 0x3F7, 0x493,
//    0x51B, 0x5BA, 0x6B1, 0x7FE, 0x9C7, 0xAE5, 0xB6B, 0xC09,
//    0xC87, 0xCEF, 0xD47, 0xDA8, 0xDFA, 0xDFF, 0xE27, 0xE71,
//    0xEC3, 0xF07, 0xF4B, 0xF8D, 0xFE2, 0xFFFF, 0xFFFF, 0xFFFF,
//    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
//    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
//    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF
//};
//const uint16_t c16_adc_rangeTable_VDD3_VREF3[64] = {
//    0x000, 0x01A, 0x058, 0x097, 0x0DA, 0x12D, 0x16D, 0x1B8,
//    0x257, 0x2A7, 0x315, 0x375, 0x3DC, 0x476, 0x51F, 0x5E6,
//    0x7B6, 0x7FE, 0x908, 0xA85, 0xB41, 0xBE7, 0xC88, 0xD01,
//    0xD6D, 0xDE3, 0xDFF, 0xE0F, 0xE6F, 0xEC7, 0xF23, 0xF7D,
//    0xFCD, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
//    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
//    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
//    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF
//};
//const uint16_t c16_adc_rangeTable_VDD5_VREF2[64] = {
//    0x000, 0x023, 0x073, 0x0BF, 0x121, 0x179, 0x237, 0x298,
//    0x31A, 0x396, 0x438, 0x4E2, 0x5D5, 0x758, 0x7FE, 0xA3D,
//    0xB47, 0xC48, 0xCFF, 0xD81, 0xDFF, 0xE00, 0xE2C, 0xEA8,
//    0xF04, 0xF60, 0xFC7, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
//    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
//    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
//    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
//    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF
//};
//const uint16_t* RangeTable[4] = {
//    c16_adc_rangeTable_VDD5_VREF5,
//    c16_adc_rangeTable_VDD5_VREF3,
//    c16_adc_rangeTable_VDD3_VREF3,
//    c16_adc_rangeTable_VDD5_VREF2,
//};
//const uint8_t c8_adc_INL_VDD5_VREF5[64] = {
//    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
//    16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 24, 23, 22, 21, 20, 19,
//    18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3,
//    2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
//};
//const uint8_t c8_adc_INL_VDD5_VREF3[64] = {
//    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
//    16, 17, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5,
//    4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
//    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
//};
//const uint8_t c8_adc_INL_VDD3_VREF3[64] = {
//    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
//    16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1,
//    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
//    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
//};
//const uint8_t c8_adc_INL_VDD5_VREF2[64] = {
//    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 12, 11,
//    10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0,
//    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
//    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
//};
//const uint8_t* INL_Table[4] = {
//    c8_adc_INL_VDD5_VREF5,
//    c8_adc_INL_VDD5_VREF3,
//    c8_adc_INL_VDD3_VREF3,
//    c8_adc_INL_VDD5_VREF2
//};
////===========================================================
//// Structures
////===========================================================
//typedef struct {
//    long value;
//    uint8_t status_buffer[BUFFER_MAX_SIZE];
//    uint8_t set_ac;
//    uint8_t set_battery;
//    uint8_t set_mode;
//    uint16_t set_bat_per;
//    uint8_t voltage;
//    uint8_t address;
//    uint8_t ac_voltage;
//} Typedef_status_t;
//Typedef_status_t Typedef_status;
////===========================================================
//// Function Prototypes
////===========================================================
//void POWER_INITIAL(void);
//void GPIO_Initialize(void);
//void UART1_INITIAL(void);
//void TIMER2_INITIAL(void);
//void ADC_INITIAL(void);
//void SN74HC595_INITIAL(void);
//void Send_Character(uint8_t tchar);
//void Send_String(uint8_t *ptr);
//void DelayUs(uint8_t Time);
//void Delay10Us(void);
//void DelayMs(uint16_t Time);
//void DelayS(uint16_t Time);
//void Delay450Us(void);
//void Nop_Delay(void);
//uint8_t EEPROMread(uint8_t EEAddr);
//void EEPROMwrite(uint8_t EEAddr, uint8_t Data);
//uint16_t GET_ADC_DATA(uint8_t adcChannel);
//uint16_t ADC_Calibration(uint16_t val, ADC_SAMPLE_CONFIG_TYPE type);
//void shift_out_byte(uint8_t data);
//void send_data_to_ic(uint8_t data);
//void clear_sn74hc595(void);
//void sn74hc595_lastdata(Typedef_status_t *status);
//uint8_t calculate_checksum(uint8_t *data, uint8_t length);
//void send_reliable_message(uint8_t *data, uint8_t length);
//void sendPoeStatus(Typedef_status_t *status);
//void battery_percentage(void);
//long extract_and_convert(char *input_string);
//void AC_DETECTION_INITIAL(void);
//volatile uint8_t fan_timer_active = 0;
//volatile uint8_t timer_done = 0;
//volatile uint16_t ms_cnt = 0;
//volatile uint8_t sec_cnt = 0;
//volatile uint8_t min_cnt = 0;
//volatile uint8_t print_temp_flag = 0;
//uint8_t fan_state = FAN_OFF;
//uint8_t last_fan_state = 0xFF;
//uint8_t high_temp_seen = 0;
//#define TEMP_MIN_VALID   0.100f   // sensor minimum possible
//#define TEMP_MAX_VALID   3.000f   // sensor maximum possible
//#define STABLE_COUNT    5         // consecutive readings
//float temp_mv = 0.0f;
//float last_temp_mv = -1.0f;
//char buffer[64];
//volatile uint8_t print_timer_start_flag = 0;
//volatile uint8_t print_timer_restart_flag = 0;
//volatile uint8_t print_timer_done_flag = 0;
//volatile uint8_t print_countdown_flag = 0;
//uint8_t last_min = 0xFF;
//uint8_t last_sec = 0xFF;
////===========================================================
//// Interrupt Handlers
////===========================================================
//void interrupt high_priority InterruptHandlerHigh(void) {
//    static uint8_t receiveCnt = 0;
//    
//    // Check if UART1 receive interrupt is enabled and flag is set
//    if (UR1RXNEIE && UR1RXNEF) {
//        uint8_t temp = UR1DATL;
//        
//        // Add the received byte to buffer only if there's space and not already processing a message
//        if (receiveCnt < (FRAME_SIZE - 1) && receive_complete_flag == 0) {
//            usart_rx_buff[receiveCnt++] = temp;
//            
//            // Check for end of message marker
//            if (temp == '\n') {
//                // Null-terminate the string for safety
//                usart_rx_buff[receiveCnt] = '\0';
//                receiveCnt = 0;
//                receive_complete_flag = 1;
//            }
//        } else if (temp == '\n') {
//            // Reset if buffer overrun but still handle the end marker
//            receiveCnt = 0;
//            receive_complete_flag = 1;
//        }
//    }
//    if (T2CRbits.T2IE && T2CRbits.T2IF)
//    {
//        T2CRbits.T2IF = 0;
//        /* ---- 1 second tick ---- */
//        static uint16_t ms_1s = 0;
//        ms_1s++;
//        if (ms_1s >= 1000)
//        {
//            ms_1s = 0;
//            print_temp_flag = 1;
//            if (fan_timer_active)
//                print_countdown_flag = 1;
//        }
//        /* ---- Fan timer ---- */
//        if (fan_timer_active)
//        {
//            ms_cnt++;
//            if (ms_cnt >= 1000)
//            {
//                ms_cnt = 0;
//                sec_cnt++;
//                if (sec_cnt >= 60)
//                {
//                    sec_cnt = 0;
//                    min_cnt++;
//                    if (min_cnt >= 3)
//                    {
//                        min_cnt = 0;
//                        fan_timer_active = 0;
//                        timer_done = 1;
//                        print_timer_done_flag = 1;
//                    }
//                }
//            }
//        }
//    }
//    // UART2 code removed to improve interrupt processing time
//}
//void interrupt low_priority InterruptHandlerLow(void) {
//    // Empty low-priority ISR
//}
////===========================================================
//// Function Definitions
////===========================================================
//void POWER_INITIAL(void) {
//    OSCCON = 0B01110001;        // 16 MHz internal oscillator
//    INTCON1 = 0;                // Disable all interrupts
//    INTCON2 = 0;                //
//    INTCON3 = 0;                //
//    PCKEN2bits.IOCKEN = 1;      // Enable IO module clock
//                            
//    PORTA = 0x00;               // Initialize PORTA
//    TRISA = 0xFF;               // PORTA all inputs
//    PORTB = 0x00;               // Initialize PORTB
//    TRISB = 0xFF;               // PORTB all inputs
//    PORTC = 0x00;               // Initialize PORTC
//    TRISC = 0xFF;               // PORTC all inputs
//    PORTD = 0x00;               // Initialize PORTD
//    TRISD = 0xFF;               // PORTD all inputs
//    PORTE = 0x00;               // Initialize PORTE
//    TRISE = 0xFF;               // PORTE all inputs (including PE6)
//    PORTF = 0x00;               // Initialize PORTF
//    TRISF = 0xFF;               // PORTF all inputs
//    WPUEbits.WPUE6 = 1;         // Enable weak pull-up on PE6
//    //ANSELEbits.ANSE6 = 0;       // Configure PE6 as digital
//}
//void GPIO_Initialize(void) {
//    WPUAbits.WPUA1 = 1;         // Enable pull-up on PA1
//    GPIO_AC_ON_AND_OFF_DIR = 0; // PA1 output
//    GPIO_DC_ON_DIR = 0;         // PA0 output
//    GPIO_DC_OUT_SENSE_DIR = 1;  // PE5 input
//    GPIO_DC_OUT_DISCHARGE_DIR = 0; // PE4 output
//    GPIO_POSITIVE_POE_CTRL_DIR = 0; // PE2 output
//    GPIO_NEGTIVE_POE_CTRL_DIR = 1;  // PE1 input
//    BATTERY_CURRENT_SENSE_DIR = 1;  // PA3 input
//    BATTERY_VOLT_SENSE_DIR = 1;     // PA2 input
//    GPIO_30VOLT_SET_DIR = 0;        // PA4 output
//    GPIO_48VOLT_SET_DIR = 0;        // PF5 output
//    GPIO_56VOLT_SET_DIR = 0;        // PF4 output
//    GPIO_AC_ON_AND_OFF = 0;
//    GPIO_DC_ON = 0;
//    GPIO_DC_OUT_DISCHARGE = 0;
//    GPIO_POSITIVE_POE_CTRL = 0;
//    //GPIO_NEGTIVE_POE_CTRL = 0;
//}
//void UART1_INITIAL(void) {
//    PCKEN2bits.UR1CKEN = 1;     // Enable UART1 clock
//    TRISAbits.TRISA5 = 1;       // RA5 as RX input
//    TRISAbits.TRISA6 = 0;       // RA6 as TX output
//    LATAbits.LATA6 = 1;         // Set TX high initially
//    AFP2bits.TX1PO = 0;         // Map TX to RA6
//    AFP2bits.RX1PO = 0;         // Map RX to RA5
//    UR1CR1bits.UR1HDSEL = 0;    // Full-duplex
//    UR1CR1bits.UR1STOP = 0;     // 1 stop bit
//    UR1CR1bits.UR1MODE = 0;     // 8-bit data
//    UR1CR1bits.UR1PCEN = 0;     // No parity
//    UR1CR1bits.UR1RXEN = 1;     // Enable RX
//    UR1CR1bits.UR1TXEN = 1;     // Enable TX
//    UR1CR2bits.UR1BDM = 0;      // 16x baud rate
//    UR1CR2bits.UR1BRRH = 0;
//    UR1BRRL = 104;              // 9600 baud at 16 MHz
//    UR1CR2bits.UR1RXNEIE = 1;   // Enable RX interrupt
//    UR1STAT = 0;                // Clear status
//    INTCON1 = 0B11000000;       // Enable interrupts
//    IPEN = 0;                   // Disable priority
//    UR1CR1bits.UR1EN = 1;       // Enable UART1
//    GIE = 1;                    // Enable global interrupts
//}
//// UART2 initialization removed to reduce complexity and potential interrupt conflicts
//void TIMER2_INITIAL(void)
//{
//    PCKEN1bits.T2CKEN = 1;      // Enable TIM2 clock
//    T2CRbits.T2CKS = 0b00;     // System clock (16 MHz)
//    T2CRbits.T2PSC = 0b100;    // Prescaler = 16 ? 1 MHz timer clock
//    T2CNTRL = 0;
//    T2CNTRH = 0;
//    // 1 ms period
//    T2PRL = 0xE7;              // Low byte of 999
//    T2PRH = 0x03;              // High byte of 999
//    T2CRbits.T2IF = 0;
//    T2CRbits.T2IE = 1;
//    IPEN = 0;
//    PEIE = 1;
//    GIE  = 1;
//    T2CRbits.T2CEN = 1;        // Start timer
//}
//void DelayUs(uint8_t Time) {
//    for (uint8_t a = 0; a < Time; a++) {
//        NOP();
//    }
//}
//void Delay10Us(void) {
//    for (uint8_t i = 0; i < 17; i++) {
//        NOP(); NOP(); NOP(); NOP();
//    }
//}
//void DelayMs(uint16_t Time) {
//    for (uint16_t a = 0; a < Time; a++) {
//        for (uint8_t b = 0; b < 98; b++) {
//            Delay10Us();
//        }
//    }
//}
//void DelayS(uint16_t Time) {
//    for (uint16_t a = 0; a < Time; a++) {
//        for (uint16_t b = 0; b < 10; b++) {
//            DelayMs(100);
//        }
//    }
//}
//void Delay450Us(void) {
//    for (uint8_t i = 0; i < 45; i++) {
//        Delay10Us();
//    }
//}
//void Send_Character(uint8_t tchar) {
//    // Make sure TX buffer is empty before sending
//    while (!UR1TXEF) {
//        NOP();
//    }
//    
//    // Send the character
//    UR1DATL = tchar;
//    
//    // Wait for transmission to complete
//    while (!UR1TXEF) {
//        NOP();
//    }
//    
//    // Add a small delay for stability
//    DelayUs(50);
//}
//void Send_String(uint8_t *ptr) {
//    // First make sure we're not in the middle of another transmission
//    while (!UR1TXEF) { NOP(); }
//    DelayMs(5);
//    
//    // Send the entire string with careful timing between characters
//    while (*ptr) {
//        Send_Character(*ptr++);
//        DelayMs(1); // Substantial delay between characters
//    }
//    
//    // Add a pause after complete message to ensure it's fully received
//    DelayMs(5);
//}
//void Nop_Delay(void) {
//    NOP(); NOP(); NOP(); NOP(); NOP(); NOP();
//}
//uint8_t EEPROMread(uint8_t EEAddr) {
//    PCKEN3bits.DMCKEN = 1;  // Enable EEPROM clock
//    EECON1bits.EEPGD = 0;   // Access DROM
//    EECON1bits.CFGS = 0;    // Access PROM or DROM
//    EEADR = EEAddr;         // Set EEPROM address
//    EECON1bits.RD = 1;      // Start read
//    Nop_Delay();            // Ensure timing
//    uint8_t data = EEDATA;  // Read data
//    PCKEN3bits.DMCKEN = 0;  // Disable EEPROM clock
//    return data;
//}
//void EEPROMwrite(uint8_t EEAddr, uint8_t Data) {
//    // Removed UART2 disable/enable as UART2 is not used
//    PCKEN3bits.DMCKEN = 1;  // Enable EEPROM clock
//    EEADR = EEAddr;         // Set EEPROM address
//    EEDATA = Data;          // Set EEPROM data
//    EECON1bits.EEPGD = 0;   // Access DROM
//    EECON1bits.CFGS = 0;    // Access PROM or DROM
//    EECON1bits.WREN = 1;    // Enable write
//    GIE = 0;                // Disable interrupts
//    EECON2 = 0x55;          // Unlock sequence
//    EECON2 = 0xAA;
//    EECON1bits.WR = 1;      // Start write
//    while (EECON1bits.WR);  // Wait for completion
//    EECON1bits.WREN = 0;    // Disable write
//    PCKEN3bits.DMCKEN = 0;  // Disable EEPROM clock
//    DelayMs(1);
//    GIE = 1;                // Re-enable interrupts
//}
//uint16_t GET_ADC_DATA(uint8_t adcChannel) {
//    ADCON0bits.ADCHS = adcChannel & 0x3F; // Set ADC channel
//    Delay10Us(); Delay10Us();             // Stabilization delay
//    ADCON0bits.ADGO = 1;                  // Start ADC
//    NOP(); NOP();
//    while (ADCON0bits.ADGO);              // Wait for completion
//    return (uint16_t)(ADRESH << 8 | ADRESL); // Return 12-bit ADC value
//}
//uint16_t ADC_Calibration(uint16_t val, ADC_SAMPLE_CONFIG_TYPE type) {
//    uint8_t left = 0, right = 63, mid;
//    while (left <= right) {
//        mid = (left + right) >> 1;
//        if (RangeTable[type][mid] > val) {
//            right = mid - 1;
//        } else {
//            left = mid + 1;
//        }
//    }
//    return val + INL_Table[type][left - 1];
//}
//void ADC_INITIAL(void) {
//    PCKEN2bits.ADCCKEN = 1;     // Enable ADC clock
//    TRISEbits.TRISE2 = 1;       // PE2 as input (AN34)
//    TRISEbits.TRISE1 = 1;       // PE1 as input (AN33)
//    ANSEL4 = 0B00000110;        // PE2 and PE1 as analog
//    ADCKCONbits.ADCS = 0;       // ADC clock = SYSCLK
//    ADCKCONbits.ADCKDIV = 63;   // TAD = TADCS / (ADCKDIV + 1)
//    ADCMPLbits.ADCMPEN = 0;     // Disable comparator
//    ADCON1bits.ADVREFEN = 1;    // Enable voltage reference
//    ADCON1bits.ADVREFS = 0B10;  // Reference = 3.0V
//    ADCON1bits.ADPREF = 0B00;   // Positive ref = VADC_REF
//    ADCON1bits.ADNREF = 0B01;   // Negative ref = GND
//    ADCON2bits.ADTRGT = 0B00;   // No trigger
//    ADCON3bits.ADFM = 1;        // Right-justified result
//    ADCON2bits.ADDLY8 = 0;      // No delay
//    ADDLY = 0;
//    ADCON0bits.ADCHS = ADC_CHANNEL_AN2; // Select AN34 (PE2)
//    ADCON0bits.ADON = 1;        // Enable ADC
//    Delay450Us(); Delay450Us(); // Stabilization delay
//}
//void SN74HC595_INITIAL(void) {
//    SCK_74HC595_DIR = 0;        // SCK output
//    DATA_74HC595_DIR = 0;       // Data output
//    LATCH_74HC595_DIR = 0;      // Latch output
//}
//void AC_DETECTION_INITIAL(void) {
//    AC_DETECTION_DIR = 1;        // AC_DETECTION input
//}
//void shift_out_byte(uint8_t data) {
//    DATA_74HC595 = data;
//    SCK_74HC595 = 1;
//    DelayUs(1);
//    SCK_74HC595 = 0;
//}
//void send_data_to_ic(uint8_t data) {
//    LATCH_74HC595 = 0;
//    for (int i = 31; i >= 0; i--) {
//        shift_out_byte((data >> i) & 0x01);
//    }
//    LATCH_74HC595 = 1;
//    DelayUs(1);
//    LATCH_74HC595 = 0;
//}
//void clear_sn74hc595(void) {
//    sn74hc595_buffer = 0x00;
//    send_data_to_ic(sn74hc595_buffer);
//}
//void sn74hc595_lastdata(Typedef_status_t *status) {
//    status->voltage = EEPROMread(0x05);
//    switch (status->voltage) {
//        case 24: sn74hc595_buffer = 0x08; break;
//        case 30: sn74hc595_buffer = 0x04; break;
//        case 48: sn74hc595_buffer = 0x02; break;
//        case 56: sn74hc595_buffer = 0x01; break;
//        default: sn74hc595_buffer = 0x08; break;
//    }
//    send_data_to_ic(sn74hc595_buffer);
//    DelayMs(1);
//}
//// Calculate a simple checksum
//uint8_t calculate_checksum(uint8_t *data, uint8_t length) {
//    uint8_t checksum = 0;
//    for (uint8_t i = 0; i < length; i++) {
//        checksum ^= data[i]; // XOR checksum
//    }
//    return checksum;
//}
//// Send reliable data with retry mechanism
//void send_reliable_message(uint8_t *data, uint8_t length) {
//    // Add a solid delay before transmission
//    DelayMs(20);
//    
//    // Flush any pending transmissions
//    while (!UR1TXEF) { NOP(); }
//    
//    // Send start marker
//    Send_Character('$');
//    DelayMs(10);
//    
//    // Send data length
//    Send_Character(length);
//    DelayMs(10);
//    
//    // Send actual data bytes
//    for (uint8_t i = 0; i < length; i++) {
//        Send_Character(data[i]);
//        DelayMs(10);
//    }
//    
//    // Send checksum
//    uint8_t checksum = calculate_checksum(data, length);
//    Send_Character(checksum);
//    DelayMs(10);
//    
//    // Send end marker
//    Send_Character('#');
//    DelayMs(10);
//    Send_Character('\r');
//    DelayMs(5);
//    Send_Character('\n');
//    DelayMs(20);
//}
//void sendPoeStatus(Typedef_status_t *status) {
//    adcData = GET_ADC_DATA(ADC_CHANNEL_AN2);
//    theVoltage = (uint32_t)adcData * 3 * 1000 / 4096; // Convert to mV
//    if (adcData < BATT_10P) {
//        batteryPercentage = 0;
//        status->set_battery = 0;
//    } else if (adcData < BATT_25P) {
//        batteryPercentage = 10;
//        status->set_battery = 1;
//    } else if (adcData < BATT_50P) {
//        batteryPercentage = 25;
//        status->set_battery = 1;
//    } else if (adcData < BATT_75P) {
//        batteryPercentage = 50;
//        status->set_battery = 1;
//    } else if (adcData < BATT_100P) {
//        batteryPercentage = 75;
//        status->set_battery = 1;
//    } else {
//        batteryPercentage = 100;
//        status->set_battery = 1;
//    }
//    // Update LED status via shift register
//    sn74hc595_buffer &= 0x0F;
//    if (batteryPercentage == 10) {
//        batt_10per_flag = 1;
//    } else if (batteryPercentage == 25) {
//        batt_10per_flag = 0;
//        sn74hc595_buffer |= 0x80;
//    } else if (batteryPercentage == 50) {
//        batt_10per_flag = 0;
//        sn74hc595_buffer |= 0xC0;
//    } else if (batteryPercentage == 75) {
//        batt_10per_flag = 0;
//        sn74hc595_buffer |= 0xE0;
//    } else if (batteryPercentage == 100) {
//        batt_10per_flag = 0;
//        sn74hc595_buffer |= 0xF0;
//    }
//    send_data_to_ic(sn74hc595_buffer);
//    
//    // Simplify AC status check
//    status->set_ac = (PORTEbits.PE6 == 0) ? 1 : 0;
//    
//    // Update status variables
//    status->set_bat_per = batteryPercentage;
//    status->voltage = EEPROMread(0x05);
//    
//    // Prepare the binary data packet - more reliable than JSON
//    uint8_t data[5];
//    data[0] = status->set_ac;          // AC status
//    data[1] = status->set_battery;     // Battery status
//    data[2] = batteryPercentage;       // Battery percentage
//    data[3] = status->set_mode;        // Mode
//    data[4] = status->voltage;         // Voltage
//    
//    // Send the reliable message
//    send_reliable_message(data, 5);
//    
//    // Also send the traditional JSON format as a backup
//    // We'll format it as a single string to avoid partial transmission issues
//    char json_buffer[48];
//    sprintf(json_buffer, "{\"A\":%d,\"B\":%d,\"C\":%d,\"D\":%d,\"E\":%d}\r\n", 
//        status->set_ac, status->set_battery, batteryPercentage,
//        status->set_mode, status->voltage);
//    
//    // Small delay before sending the JSON backup
//    DelayMs(50);
//    
//    // Send the JSON - using traditional send function for compatibility
//    Send_String(json_buffer);
//}
// 
//void battery_percentage(void) {
//    adcData = GET_ADC_DATA(ADC_CHANNEL_AN2);
//    if (adcData < BATT_10P) {
//        batteryPercentage = 0;
//    } else if (adcData < BATT_25P) {
//        batteryPercentage = 10;
//    } else if (adcData < BATT_50P) {
//        batteryPercentage = 25;
//    } else if (adcData < BATT_75P) {
//        batteryPercentage = 50;
//    } else if (adcData < BATT_100P) {
//        batteryPercentage = 75;
//    } else {
//        batteryPercentage = 100;
//    }
//    sn74hc595_buffer &= 0x0F;
//    if (batteryPercentage == 10) {
//        sn74hc595_buffer |= 0x80;
//        send_data_to_ic(sn74hc595_buffer);
//        DelayMs(100);
//        sn74hc595_buffer &= ~0x80;
//        send_data_to_ic(sn74hc595_buffer);
//        DelayMs(100);
//    } else if (batteryPercentage == 25) {
//        batt_10per_flag = 0;
//        sn74hc595_buffer |= 0x80;
//    } else if (batteryPercentage == 50) {
//        batt_10per_flag = 0;
//        sn74hc595_buffer |= 0xC0;
//    } else if (batteryPercentage == 75) {
//        batt_10per_flag = 0;
//        sn74hc595_buffer |= 0xE0;
//    } else if (batteryPercentage == 100) {
//        batt_10per_flag = 0;
//        sn74hc595_buffer |= 0xF0;
//    }
//    send_data_to_ic(sn74hc595_buffer);
//}
//long extract_and_convert(char *input_string) {
//    char *start_ptr = strchr(input_string, ':');
//    if (!start_ptr) {
//        return 0; // Return default if ':' not found
//    }
//    
//    start_ptr++; // Move past the colon
//    
//    char *end_ptr = strchr(start_ptr, ';');
//    if (!end_ptr) {
//        // If no semicolon, look for end of line or null terminator
//        end_ptr = strchr(start_ptr, '\r');
//        if (!end_ptr) {
//            end_ptr = strchr(start_ptr, '\n');
//            if (!end_ptr) {
//                // If we still don't have an endpoint, use string length
//                end_ptr = start_ptr + strlen(start_ptr);
//            }
//        }
//    }
//    
//    // Save the character at end position
//    char saved_char = *end_ptr;
//    
//    // Temporarily null-terminate for conversion
//    *end_ptr = '\0';
//    
//    // Convert to integer
//    long result = atoi(start_ptr);
//    
//    // Restore the original character
//    *end_ptr = saved_char;
//    
//    return result;
//}
//#define ADC_SAMPLES      20
//#define FAN_PIN          PA3
//#define FAN_PIN_DIR      TRISA3
//float read_battery_voltage(uint8_t channel)
//{
//    unsigned long sum = 0;
//    for (unsigned char i = 0; i < ADC_SAMPLES; i++)
//    {
//        sum += GET_ADC_DATA(channel);
//        Delay10Us();   // small gap improves stability
//    }
//    float avg_raw = (float)sum / ADC_SAMPLES;
//    float adc_voltage = (avg_raw * 3.0f) / 4095.0f;
//    return adc_voltage;
//}
////===========================================================
////Function name£º	UART3_INITIAL ´®¿Ú3³õÊ¼»¯
////parameters£º		none   	UART3_RX:PD5
////							UART3_TX:PD4 
////returned value£º	none
////===========================================================  
//void UART3_INITIAL(void)
//{	
//	PCKEN2bits.UR3CKEN = 1;		// UART3 Ä£¿éÊ±ÖÓÊ¹ÄÜ
//    TRISDbits.TRISD5 = 1; 		// UART3_RXÊäÈë£¬PD5
//	TRISDbits.TRISD4 = 0; 		// UART3_TXÊä³ö£¬PD4    
//	LATDbits.LATD4  = 1;		
//    
//	AFP4bits.TX3PO = 0;			// UART3_TX ¹Ü½ÅÓ³ÉäÑ¡ÔñPD4
//    AFP4bits.RX3PO = 0;			// UART3_RX ¹Ü½ÅÓ³ÉäÑ¡ÔñPD5	
//    
//    UR3CR1bits.UR3HDSEL = 0;	// ¹Ø±Õ°ëË«¹¤
//	UR3CR1bits.UR3STOP = 0;		// 1bit Í£Ö¹Î»
//    UR3CR1bits.UR3MODE = 0;		// 8Î»Êý¾Ý
//    UR3CR1bits.UR3PCEN = 0;		// ÎÞÆæÅ¼Ð£Ñé
//    UR3CR1bits.UR3RXEN = 1;		// ½ÓÊÕÊ¹ÄÜ
//    UR3CR1bits.UR3TXEN = 1;		// ·¢ËÍÊ¹ÄÜ
//   
//	UR3CR2bits.UR3BDM = 0;		// 16 ±¶²¨ÌØÂÊ¹ý²ÉÑùÄ£Ê½ 
//    UR3CR2bits.UR3BRRH = 0; 
//    UR3BRRL =104;         		// 9600²¨ÌØÂÊ = Fosc/16*[URDLH:URDLL]
//	UR3CR2bits.UR3RXNEIE = 1;	// Ê¹ÄÜ½ÓÊÕ·Ç¿ÕÖÐ¶Ï
//    UR3STAT = 0;					
//	//INTCON1 = 0B11000000;
//    IPEN = 0;					// ½ûÖ¹ÖÐ¶ÏÓÅÏÈ¼¶    
//    UR3CR1bits.UR3EN=1;			// Ê¹ÄÜUART3Ä£¿é
//	//GIE = 1;					// Ê¹ÄÜÈ«¾Ö×ÜÖÐ¶Ï
//} 
//void UART3_SendChar(uint8_t tchar) {
//    // Make sure TX buffer is empty before sending
//    while (!UR3TXEF) {
//        NOP();
//    }
//    
//    // Send the character
//    UR3DATL = tchar;
//    
//    // Wait for transmission to complete
//    while (!UR3TXEF) {
//        NOP();
//    }
//}
//void UART3_SendString(uint8_t *ptr)
//{
//	    // Send the entire string with careful timing between characters
//    while (*ptr) 
//    {
//        UART3_SendChar(*ptr++);
//    }
//}
//#define TEMP_SPIKE_DELTA   2.0f     // max allowed jump (2°C)
//#define TEMP_CONFIRM_CNT  3
//uint8_t voltage_to_temperature(float volt, float *temp_out)
//{
//    /* ---- Clamp high voltage ? MIN temperature ---- */
//    if (volt > temp_map[0].volt)
//    {
//        *temp_out = temp_map[0].temp_c;   // 25°C
//        return 1;
//    }
//    /* ---- Clamp low voltage ? MAX temperature ---- */
//    if (volt < temp_map[TEMP_MAP_SIZE - 1].volt)
//    {
//        *temp_out = temp_map[TEMP_MAP_SIZE - 1].temp_c; // 70°C
//        return 1;
//    }
//    /* ---- Normal interpolation ---- */
//    for (uint8_t i = 0; i < TEMP_MAP_SIZE - 1; i++)
//    {
//        float v1 = temp_map[i].volt;
//        float v2 = temp_map[i + 1].volt;
//        if (volt <= v1 && volt >= v2)
//        {
//            float t1 = temp_map[i].temp_c;
//            float t2 = temp_map[i + 1].temp_c;
//            *temp_out = t1 + ((v1 - volt) * (t2 - t1)) / (v1 - v2);
//            return 1;
//        }
//    }
//    /* Should never happen */
//    *temp_out = temp_map[TEMP_MAP_SIZE - 1].temp_c;
//    return 1;
//}
//uint8_t get_filtered_temperature(float *out_temp)
//{
//    static float last_valid = 0.0f;
//    static float suspect_value = 0.0f;
//    static uint8_t suspect_cnt = 0;
//    static uint8_t initialized = 0;
//    float volt;
//    float temp;
//    /* Read ADC voltage */
//    volt = read_battery_voltage(33);
//    /* Convert voltage ? temperature */
//    if (!voltage_to_temperature(volt, &temp))
//        return 0;
//    /* First valid sample */
//    if (!initialized)
//    {
//        last_valid = temp;
//        *out_temp = temp;
//        initialized = 1;
//        return 1;
//    }
//    /* Normal small change */
//    if (fabs(temp - last_valid) <= TEMP_SPIKE_DELTA)
//    {
//        last_valid = temp;
//        suspect_cnt = 0;
//        *out_temp = temp;
//        return 1;
//    }
//    /* Possible spike */
//    if (suspect_cnt == 0 || fabs(temp - suspect_value) <= TEMP_SPIKE_DELTA)
//    {
//        suspect_value = temp;
//        suspect_cnt++;
//        if (suspect_cnt >= TEMP_CONFIRM_CNT)
//        {
//            last_valid = suspect_value;
//            suspect_cnt = 0;
//            *out_temp = last_valid;
//            return 1;
//        }
//    }
//    else
//    {
//        suspect_cnt = 0;
//    }
//    /* Spike ignored */
//    *out_temp = last_valid;
//    return 1;
//}
//#define TEMP_HIGH_C  60.0f
//#define TEMP_LOW_C   50.0f
//void fan_control_task(void)
//{
//    float temp;
//    if (!get_filtered_temperature(&temp))
//        return;
//    temp_c = temp;
//    /* -------- HIGH TEMP -------- */
//    if (temp_c >= TEMP_HIGH_C)
//    {
//        fan_state = FAN_ON;
//        high_temp_seen = 1;
//        fan_timer_active = 0;
//        timer_done = 0;
//        ms_cnt = sec_cnt = min_cnt = 0;
//    }
//    /* -------- LOW TEMP -------- */
//    else if (temp_c <= TEMP_LOW_C)
//    {
//        if (!fan_timer_active)
//        {
//            fan_state = FAN_OFF;
//            high_temp_seen = 0;
//            timer_done = 0;
//            ms_cnt = sec_cnt = min_cnt = 0;
//        }
//    }
//    /* -------- MID RANGE -------- */
//    else
//    {
//        if (high_temp_seen && !fan_timer_active)
//        {
//            fan_state = FAN_ON;
//            fan_timer_active = 1;
//            ms_cnt = sec_cnt = min_cnt = 0;
//            print_timer_start_flag = 1;
//        }
//        if (timer_done)
//        {
//            timer_done = 0;
//            if (temp_c <= TEMP_LOW_C)
//            {
//                fan_state = FAN_OFF;
//                high_temp_seen = 0;
//                fan_timer_active = 0;
//            }
//            else
//            {
//                fan_state = FAN_ON;
//                fan_timer_active = 1;
//                ms_cnt = sec_cnt = min_cnt = 0;
//                print_timer_restart_flag = 1;
//            }
//        }
//    }
//    FAN_PIN = fan_state;
//}
//void debug_print_task(void)
//{
//    char line[80];
//    /* Print only when flag is set (or call periodically) */
//    if (!print_temp_flag)
//        return;
//    print_temp_flag = 0;
//    if (fan_timer_active)
//    {
//        /* Timer running ? show countdown */
//        uint8_t rem_min = (uint8_t)(2 - min_cnt);
//        uint8_t rem_sec = (uint8_t)(59 - sec_cnt);
//        sprintf(line,
//                "T: %.2fC  F S: %s  T S: Run  T Rem %02u:%02u\r\n",
//                temp_c,
//                fan_state ? "On" : "Off",
//                rem_min,
//                rem_sec);
//    }
//    else
//    {
//        /* Timer stopped */
//        sprintf(line,
//                "T: %.2fC  F S: %s  T S: Stop\r\n",
//                temp_c,
//                fan_state ? "On" : "Off");
//    }
//    UART3_SendString((uint8_t *)line);
//}
//void main(void) {
//    GIE = 0; // Disable interrupts
//    POWER_INITIAL();
//    GPIO_Initialize();
//    SN74HC595_INITIAL();
//    AC_DETECTION_INITIAL();
//    clear_sn74hc595();
//    uint8_t existingVoltage = EEPROMread(0x05);
//    switch (existingVoltage) {
//        case 24:
//            GPIO_56VOLT_SET = 0;
//            GPIO_48VOLT_SET = 0;
//            GPIO_30VOLT_SET = 0;
//            GPIO_DC_ON = 0;
//            EEPROMwrite(0x05, 24);
//            sn74hc595_buffer &= 0xF0;
//            sn74hc595_buffer |= 0x08;
//            send_data_to_ic(sn74hc595_buffer);
//            DelayMs(500);
//            GPIO_DC_ON = 1;
//            break;
//        case 30:
//            GPIO_56VOLT_SET = 0;
//            GPIO_48VOLT_SET = 0;
//            GPIO_DC_ON = 0;
//            EEPROMwrite(0x05, 30);
//            sn74hc595_buffer &= 0xF0;
//            sn74hc595_buffer |= 0x04;
//            send_data_to_ic(sn74hc595_buffer);
//            DelayMs(500);
//            GPIO_30VOLT_SET = 1;
//            GPIO_DC_ON = 1;
//            break;
//        case 48:
//            GPIO_56VOLT_SET = 0;
//            GPIO_30VOLT_SET = 0;
//            GPIO_DC_ON = 0;
//            EEPROMwrite(0x05, 48);
//            sn74hc595_buffer &= 0xF0;
//            sn74hc595_buffer |= 0x02;
//            send_data_to_ic(sn74hc595_buffer);
//            DelayMs(500);
//            GPIO_48VOLT_SET = 1;
//            GPIO_DC_ON = 1;
//            break;
//        case 56:
//            GPIO_48VOLT_SET = 0;
//            GPIO_30VOLT_SET = 0;
//            GPIO_DC_ON = 0;
//            EEPROMwrite(0x05, 56);
//            sn74hc595_buffer &= 0xF0;
//            sn74hc595_buffer |= 0x01;
//            send_data_to_ic(sn74hc595_buffer);
//            DelayMs(500);
//            GPIO_56VOLT_SET = 1;
//            GPIO_DC_ON = 1;
//            break;
//        default:
//            GPIO_56VOLT_SET = 0;
//            GPIO_48VOLT_SET = 0;
//            GPIO_30VOLT_SET = 0;
//            GPIO_DC_ON = 0;
//            EEPROMwrite(0x05, 24);
//            DelayMs(10);
//            EEPROMwrite(0x02, 0);
//            DelayMs(10);
//            EEPROMwrite(0x03, 1);
//            DelayMs(10);
//            sn74hc595_buffer |= 0x08;
//            send_data_to_ic(sn74hc595_buffer);
//            GPIO_DC_ON = 1;
//            break;
//    }
//    
//    
//    UART1_INITIAL();
//    //UART3_INITIAL();
//    // UART2 initialization removed - PE6 used as input for AC detection
//    ADC_INITIAL();
//	FAN_PIN_DIR = 0;
//    uint8_t battery_update_counter = 0;
//    TIMER2_INITIAL();
//    PEIE = 1;
//    GIE = 1; // Enable interrupts
//    UART3_SendString((uint8_t *) "Systme Initialized\r\n" );
//    while (1) {
//    
//        // Update battery percentage less frequently to reduce processing load
//        if (++battery_update_counter >= 5) {
//            battery_percentage();
//            battery_update_counter = 0;
//        }
//        
//        // Shorter delay to improve responsiveness
//        DelayMs(20);
//        
//        // Process received commands immediately when they arrive
//        if (receive_complete_flag) {
//            receive_complete_flag = 0;
//            
//            // Use strncmp for safer string comparison - avoids potential buffer overrun
//            if (usart_rx_buff[0] == 'g' && usart_rx_buff[4] == 'S' && usart_rx_buff[5] == 'T') {
//                // Send status response with highest priority
//                sendPoeStatus(&Typedef_status);
//            } else if (usart_rx_buff[0] == 's' && usart_rx_buff[4] == 'P' && usart_rx_buff[5] == 'O' && usart_rx_buff[8] == 'V') {
//                // Process voltage change command
//                Typedef_status.value = extract_and_convert(usart_rx_buff);
//                EEPROMwrite(0x05, Typedef_status.value);
//                switch (Typedef_status.value) {
//                    case START_24_VOLT:
//                        sendPoeStatus(&Typedef_status);
//                        GPIO_56VOLT_SET = 0;
//                        GPIO_48VOLT_SET = 0;
//                        GPIO_30VOLT_SET = 0;
//                        GPIO_DC_ON = 0;
//                        sn74hc595_buffer &= 0xF0;
//                        sn74hc595_buffer |= 0x08;
//                        send_data_to_ic(sn74hc595_buffer);
//                        EEPROMwrite(0x05, 24);
//                        DelayMs(500);
//                        GPIO_DC_ON = 1;
//                        break;
//                    case START_30_VOLT:
//                        sendPoeStatus(&Typedef_status);
//                        GPIO_56VOLT_SET = 0;
//                        GPIO_48VOLT_SET = 0;
//                        GPIO_DC_ON = 0;
//                        sn74hc595_buffer &= 0xF0;
//                        sn74hc595_buffer |= 0x04;
//                        send_data_to_ic(sn74hc595_buffer);
//                        EEPROMwrite(0x05, 30);
//                        DelayMs(500);
//                        GPIO_30VOLT_SET = 1;
//                        GPIO_DC_ON = 1;
//                        break;
//                    case START_48_VOLT:
//                        sendPoeStatus(&Typedef_status);
//                        GPIO_56VOLT_SET = 0;
//                        GPIO_30VOLT_SET = 0;
//                        GPIO_DC_ON = 0;
//                        sn74hc595_buffer &= 0xF0;
//                        sn74hc595_buffer |= 0x02;
//                        send_data_to_ic(sn74hc595_buffer);
//                        EEPROMwrite(0x05, 48);
//                        DelayMs(500);
//                        GPIO_48VOLT_SET = 1;
//                        GPIO_DC_ON = 1;
//                        break;
//                    case START_56_VOLT:
//                        sendPoeStatus(&Typedef_status);
//                        GPIO_48VOLT_SET = 0;
//                        GPIO_30VOLT_SET = 0;
//                        GPIO_DC_ON = 0;
//                        sn74hc595_buffer &= 0xF0;
//                        sn74hc595_buffer |= 0x01;
//                        send_data_to_ic(sn74hc595_buffer);
//                        EEPROMwrite(0x05, 56);
//                        DelayMs(500);
//                        GPIO_56VOLT_SET = 1;
//                        GPIO_DC_ON = 1;
//                        break;
//                }
//            }
//        }
//        fan_control_task();
//		//debug_print_task();
//    }
//}


























// Version: 1.4
// Date: 08-07-2025
// Description: Power over Ethernet (PoE) controller with UART, ADC, and 74HC595 shift register
//              Configures PE6 as input with internal pull-up

#include "SYSCFG.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
//===========================================================
// Macros
//===========================================================
#define uint8_t     unsigned char
#define uint16_t    unsigned int
#define uint32_t    unsigned long

// 74HC595 Shift Register Pins
#define SCK_74HC595                 LATFbits.LATF1
#define DATA_74HC595                LATFbits.LATF2
#define LATCH_74HC595               LATFbits.LATF3
#define SCK_74HC595_DIR             TRISFbits.TRISF1
#define DATA_74HC595_DIR            TRISFbits.TRISF2
#define LATCH_74HC595_DIR           TRISFbits.TRISF3

// AC DETECTION PIN
#define AC_DETECTION                LATEbits.LATE6
#define AC_DETECTION_DIR            TRISEbits.TRISE6

#define AC_DETECTION2                LATEbits.LATE7
#define AC_DETECTION_DIR2            TRISEbits.TRISE7

// Voltage Levels
#define START_24_VOLT               24
#define START_30_VOLT               30
#define START_48_VOLT               48
#define START_56_VOLT               56
/* EEPROM 0x05: 0 = PoE output off (PA0=1); 24/30/48/56 = rail selected, PA0=0 enables output */
#define START_VOLT_OFF              0

// Buffer Sizes
#define BUFFER_MAX_SIZE             512
#define FRAME_SIZE                  128

// GPIO Definitions
#define GPIO_30VOLT_SET             LATAbits.LATA4
#define GPIO_48VOLT_SET             LATFbits.LATF5
#define GPIO_56VOLT_SET             LATFbits.LATF4

#define GPIO_30VOLT_SET_DIR         TRISAbits.TRISA4
#define GPIO_48VOLT_SET_DIR         TRISFbits.TRISF5
#define GPIO_56VOLT_SET_DIR         TRISFbits.TRISF4

#define GPIO_AC_ON_AND_OFF          LATAbits.LATA1
#define BATTERY_VOLTAGE_SENSE       PORTAbits.RA2
#define BATTERY_CURRENT_SENSE       PORTAbits.RA3
/* PA0 (pin 11): 1 = DC output OFF, 0 = DC output enabled (then 24/30/48/56 rails apply) */
#define GPIO_DC_ON                  LATAbits.LATA0
#define GPIO_DC_OUT_SENSE           PORTEbits.RE5
#define GPIO_DC_OUT_DISCHARGE       LATAbits.LATA4
#define GPIO_DC_DIS_CURRENT_SENSE   PORTEbits.RE3
#define GPIO_POSITIVE_POE_CTRL      LATEbits.LATE2
#define GPIO_NEGTIVE_POE_CTRL       LATEbits.LATE1

#define GPIO_AC_ON_AND_OFF_DIR      TRISAbits.TRISA1
#define BATTERY_VOLT_SENSE_DIR      TRISAbits.TRISA2
#define BATTERY_CURRENT_SENSE_DIR   TRISAbits.TRISA3
#define GPIO_DC_ON_DIR              TRISAbits.TRISA0
#define GPIO_DC_OUT_SENSE_DIR       TRISEbits.TRISE5
#define GPIO_DC_OUT_DISCHARGE_DIR   TRISEbits.TRISE4
#define GPIO_DC_DIS_CURRENT_SENSE_DIR TRISEbits.TRISE3
#define GPIO_POSITIVE_POE_CTRL_DIR  TRISEbits.TRISE2
#define GPIO_NEGTIVE_POE_CTRL_DIR   TRISEbits.TRISE1

// ADC Channel
#define ADC_CHANNEL_AN2             0B000010
#define AN33                        33
#define TEMPERATURE_CHANNEL         AN33
// Battery Percentage Thresholds
#define BATT_10P                    2033
#define BATT_25P                    2075
#define BATT_50P                    2185
#define BATT_75P                    2288
#define BATT_100P                   2385


#define TEMP_HIGH_V   0.650f   // 60?C
#define TEMP_LOW_V    1.000f   // 40?C

#define FAN_ON        1
#define FAN_OFF       0

//===========================================================
// Variables
//===========================================================
volatile uint8_t usart_rx_buff[FRAME_SIZE] = {0};
volatile uint8_t receive_complete_flag = 0;
// UART2 buffers removed as not needed

volatile uint16_t sec_1 = 0;
volatile uint8_t ms_100 = 0;
volatile uint8_t ms_1 = 0;
volatile uint16_t adc_48v = 0;
volatile uint8_t sn74hc595_buffer = 0;
volatile uint8_t batt_10per_flag = 0;
volatile uint16_t adcData = 0;
volatile uint16_t theVoltage = 0;
volatile uint8_t batteryPercentage = 0;
volatile uint8_t new_ac_voltage = 0;

volatile uint8_t ac_status = 0;

volatile float temp_c = 0.0f;

typedef struct {
    float temp_c;
    float volt;
} temp_map_t;

static const temp_map_t temp_map[] =
{
    {25, 1.534}, {26, 1.511}, {27, 1.488}, {28, 1.465},
    {29, 1.431}, {30, 1.409}, {31, 1.386}, {32, 1.363},
    {33, 1.341}, {34, 1.318}, {35, 1.295}, {36, 1.273},
    {37, 1.250}, {38, 1.227}, {39, 1.204}, {40, 1.181},
    {41, 1.158}, {42, 1.136}, {43, 1.113}, {44, 1.090},
    {45, 1.067}, {46, 1.044}, {47, 1.022}, {48, 0.999},
    {49, 0.976}, {50, 0.953}, {51, 0.931}, {52, 0.908},
    {53, 0.885}, {54, 0.862}, {55, 0.840}, {56, 0.817},
    {57, 0.794}, {58, 0.771}, {59, 0.749}, {60, 0.726},
    {61, 0.703}, {62, 0.680}, {63, 0.658}, {64, 0.635},
    {65, 0.612}, {66, 0.582}, {67, 0.564}, {68, 0.546},
    {69, 0.528}, {70, 0.509}
};

#define TEMP_MAP_SIZE (sizeof(temp_map) / sizeof(temp_map[0]))


// ADC Calibration Tables
typedef enum {
    E_VDD5_VREF5 = 0,
    E_VDD5_VREF3,
    E_VDD3_VREF3,
    E_VDD5_VREF2,
    E_OTHER,
} ADC_SAMPLE_CONFIG_TYPE;

const uint16_t c16_adc_rangeTable_VDD5_VREF5[64] = {
    0x000, 0x017, 0x036, 0x05A, 0x07B, 0x0A3, 0x0CE, 0x0F2,
    0x11D, 0x14F, 0x175, 0x19C, 0x1D2, 0x22F, 0x25C, 0x296,
    0x2CE, 0x310, 0x350, 0x392, 0x3D8, 0x431, 0x496, 0x514,
    0x594, 0x652, 0x7FF, 0x908, 0x9C7, 0xA7F, 0xAE7, 0xB3F,
    0xB8F, 0xBEF, 0xC49, 0xCA2, 0xCE8, 0xD27, 0xD69, 0xDB7,
    0xDF1, 0xDFF, 0xE13, 0xE4F, 0xE8D, 0xECF, 0xF05, 0xF41,
    0xF7D, 0xFAB, 0xFE7, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF
};

const uint16_t c16_adc_rangeTable_VDD5_VREF3[64] = {
    0x000, 0x01A, 0x054, 0x087, 0x0B8, 0x0F6, 0x13D, 0x179,
    0x1B7, 0x24F, 0x297, 0x2DA, 0x337, 0x397, 0x3F7, 0x493,
    0x51B, 0x5BA, 0x6B1, 0x7FE, 0x9C7, 0xAE5, 0xB6B, 0xC09,
    0xC87, 0xCEF, 0xD47, 0xDA8, 0xDFA, 0xDFF, 0xE27, 0xE71,
    0xEC3, 0xF07, 0xF4B, 0xF8D, 0xFE2, 0xFFFF, 0xFFFF, 0xFFFF,
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF
};

const uint16_t c16_adc_rangeTable_VDD3_VREF3[64] = {
    0x000, 0x01A, 0x058, 0x097, 0x0DA, 0x12D, 0x16D, 0x1B8,
    0x257, 0x2A7, 0x315, 0x375, 0x3DC, 0x476, 0x51F, 0x5E6,
    0x7B6, 0x7FE, 0x908, 0xA85, 0xB41, 0xBE7, 0xC88, 0xD01,
    0xD6D, 0xDE3, 0xDFF, 0xE0F, 0xE6F, 0xEC7, 0xF23, 0xF7D,
    0xFCD, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF
};

const uint16_t c16_adc_rangeTable_VDD5_VREF2[64] = {
    0x000, 0x023, 0x073, 0x0BF, 0x121, 0x179, 0x237, 0x298,
    0x31A, 0x396, 0x438, 0x4E2, 0x5D5, 0x758, 0x7FE, 0xA3D,
    0xB47, 0xC48, 0xCFF, 0xD81, 0xDFF, 0xE00, 0xE2C, 0xEA8,
    0xF04, 0xF60, 0xFC7, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF
};

const uint16_t* RangeTable[4] = {
    c16_adc_rangeTable_VDD5_VREF5,
    c16_adc_rangeTable_VDD5_VREF3,
    c16_adc_rangeTable_VDD3_VREF3,
    c16_adc_rangeTable_VDD5_VREF2,
};

const uint8_t c8_adc_INL_VDD5_VREF5[64] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
    16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 24, 23, 22, 21, 20, 19,
    18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3,
    2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

const uint8_t c8_adc_INL_VDD5_VREF3[64] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
    16, 17, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5,
    4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

const uint8_t c8_adc_INL_VDD3_VREF3[64] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
    16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

const uint8_t c8_adc_INL_VDD5_VREF2[64] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 12, 11,
    10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

const uint8_t* INL_Table[4] = {
    c8_adc_INL_VDD5_VREF5,
    c8_adc_INL_VDD5_VREF3,
    c8_adc_INL_VDD3_VREF3,
    c8_adc_INL_VDD5_VREF2
};

//===========================================================
// Structures
//===========================================================
typedef struct {
    long value;
    uint8_t status_buffer[BUFFER_MAX_SIZE];
    uint8_t set_ac;
    uint8_t set_battery;
    uint8_t set_mode;
    uint16_t set_bat_per;
    uint8_t voltage;
    uint8_t address;
    uint8_t ac_voltage;
} Typedef_status_t;

Typedef_status_t Typedef_status;

//===========================================================
// Function Prototypes
//===========================================================
void POWER_INITIAL(void);
void GPIO_Initialize(void);
void UART1_INITIAL(void);
void TIMER2_INITIAL(void);
void ADC_INITIAL(void);
void SN74HC595_INITIAL(void);

void Send_Character(uint8_t tchar);
void Send_String(uint8_t *ptr);
void DelayUs(uint8_t Time);
void Delay10Us(void);
void DelayMs(uint16_t Time);
void DelayS(uint16_t Time);
void Delay450Us(void);
void Nop_Delay(void);
uint8_t EEPROMread(uint8_t EEAddr);
void EEPROMwrite(uint8_t EEAddr, uint8_t Data);
uint16_t GET_ADC_DATA(uint8_t adcChannel);
uint16_t ADC_Calibration(uint16_t val, ADC_SAMPLE_CONFIG_TYPE type);
void shift_out_byte(uint8_t data);
void send_data_to_ic(uint8_t data);
void clear_sn74hc595(void);
void sn74hc595_lastdata(Typedef_status_t *status);
uint8_t calculate_checksum(uint8_t *data, uint8_t length);
void send_reliable_message(uint8_t *data, uint8_t length);
void sendPoeStatus(Typedef_status_t *status);
void battery_percentage(void);
long extract_and_convert(char *input_string);
void AC_DETECTION_INITIAL(void);

volatile uint8_t fan_timer_active = 0;
volatile uint8_t timer_done = 0;

volatile uint16_t ms_cnt = 0;
volatile uint8_t sec_cnt = 0;
volatile uint8_t min_cnt = 0;

volatile uint8_t print_temp_flag = 0;

uint8_t fan_state = FAN_OFF;
uint8_t last_fan_state = 0xFF;
uint8_t high_temp_seen = 0;


#define TEMP_MIN_VALID   0.100f   // sensor minimum possible
#define TEMP_MAX_VALID   3.000f   // sensor maximum possible

#define STABLE_COUNT    5         // consecutive readings




float temp_mv = 0.0f;
float last_temp_mv = -1.0f;

char buffer[64];

volatile uint8_t print_timer_start_flag = 0;
volatile uint8_t print_timer_restart_flag = 0;
volatile uint8_t print_timer_done_flag = 0;
volatile uint8_t print_countdown_flag = 0;

uint8_t last_min = 0xFF;
uint8_t last_sec = 0xFF;



//===========================================================
// Interrupt Handlers
//===========================================================
void interrupt high_priority InterruptHandlerHigh(void) {
    static uint8_t receiveCnt = 0;
    
    // Check if UART1 receive interrupt is enabled and flag is set
    if (UR1RXNEIE && UR1RXNEF) {
        uint8_t temp = UR1DATL;
        
        // Add the received byte to buffer only if there's space and not already processing a message
        if (receiveCnt < (FRAME_SIZE - 1) && receive_complete_flag == 0) {
            usart_rx_buff[receiveCnt++] = temp;
            
            // Check for end of message marker
            if (temp == '\n') {
                // Null-terminate the string for safety
                usart_rx_buff[receiveCnt] = '\0';
                receiveCnt = 0;
                receive_complete_flag = 1;
            }
        } else if (temp == '\n') {
            // Reset if buffer overrun but still handle the end marker
            receiveCnt = 0;
            receive_complete_flag = 1;
        }
    }
    if (T2CRbits.T2IE && T2CRbits.T2IF)
    {
        T2CRbits.T2IF = 0;

        /* ---- 1 second tick ---- */
        static uint16_t ms_1s = 0;
        ms_1s++;

        if (ms_1s >= 1000)
        {
            ms_1s = 0;
            print_temp_flag = 1;

            if (fan_timer_active)
                print_countdown_flag = 1;
        }

        /* ---- Fan timer ---- */
        if (fan_timer_active)
        {
            ms_cnt++;

            if (ms_cnt >= 1000)
            {
                ms_cnt = 0;
                sec_cnt++;

                if (sec_cnt >= 60)
                {
                    sec_cnt = 0;
                    min_cnt++;

                    if (min_cnt >= 3)
                    {
                        min_cnt = 0;
                        fan_timer_active = 0;
                        timer_done = 1;
                        print_timer_done_flag = 1;
                    }
                }
            }
        }
    }


    // UART2 code removed to improve interrupt processing time
}

void interrupt low_priority InterruptHandlerLow(void) {
    // Empty low-priority ISR
}

//===========================================================
// Function Definitions
//===========================================================
void POWER_INITIAL(void) {
    OSCCON = 0B01110001;        // 16 MHz internal oscillator
    INTCON1 = 0;                // Disable all interrupts
    INTCON2 = 0;                //
    INTCON3 = 0;                //
    PCKEN2bits.IOCKEN = 1;      // Enable IO module clock
    GPIO_DC_ON_DIR = 0;         // PA0 output   
    GPIO_DC_ON = 1;                     
    PORTA = 0x01;               // Initialize PORTA
    TRISA = 0xFF;               // PORTA all inputs
    PORTB = 0x00;               // Initialize PORTB
    TRISB = 0xFF;               // PORTB all inputs
    PORTC = 0x00;               // Initialize PORTC
    TRISC = 0xFF;               // PORTC all inputs
    PORTD = 0x00;               // Initialize PORTD
    TRISD = 0xFF;               // PORTD all inputs
    PORTE = 0x00;               // Initialize PORTE
    TRISE = 0xFF;               // PORTE all inputs (including PE6)
    PORTF = 0x00;               // Initialize PORTF
    TRISF = 0xFF;               // PORTF all inputs

    WPUEbits.WPUE6 = 1;         // Enable weak pull-up on PE6

    //ANSELEbits.ANSE6 = 0;       // Configure PE6 as digital
}

void GPIO_Initialize(void) {
    WPUAbits.WPUA1 = 1;         // Enable pull-up on PA1
    GPIO_AC_ON_AND_OFF_DIR = 0; // PA1 output
    GPIO_DC_ON_DIR = 0;         // PA0 output
    GPIO_DC_OUT_SENSE_DIR = 1;  // PE5 input
    GPIO_DC_OUT_DISCHARGE_DIR = 0; // PE4 output
    GPIO_POSITIVE_POE_CTRL_DIR = 0; // PE2 output
    GPIO_NEGTIVE_POE_CTRL_DIR = 1;  // PE1 input
    BATTERY_CURRENT_SENSE_DIR = 1;  // PA3 input
    BATTERY_VOLT_SENSE_DIR = 1;     // PA2 input
    GPIO_30VOLT_SET_DIR = 0;        // PA4 output
    GPIO_48VOLT_SET_DIR = 0;        // PF5 output
    GPIO_56VOLT_SET_DIR = 0;        // PF4 output

    GPIO_AC_ON_AND_OFF = 0;
    GPIO_DC_ON = 1;             /* safe: output off until voltage path configured */
    GPIO_DC_OUT_DISCHARGE = 0;
    GPIO_POSITIVE_POE_CTRL = 0;
    //GPIO_NEGTIVE_POE_CTRL = 0;
}

void UART1_INITIAL(void) {
    PCKEN2bits.UR1CKEN = 1;     // Enable UART1 clock
    TRISAbits.TRISA5 = 1;       // RA5 as RX input
    TRISAbits.TRISA6 = 0;       // RA6 as TX output
    LATAbits.LATA6 = 1;         // Set TX high initially
    AFP2bits.TX1PO = 0;         // Map TX to RA6
    AFP2bits.RX1PO = 0;         // Map RX to RA5
    UR1CR1bits.UR1HDSEL = 0;    // Full-duplex
    UR1CR1bits.UR1STOP = 0;     // 1 stop bit
    UR1CR1bits.UR1MODE = 0;     // 8-bit data
    UR1CR1bits.UR1PCEN = 0;     // No parity
    UR1CR1bits.UR1RXEN = 1;     // Enable RX
    UR1CR1bits.UR1TXEN = 1;     // Enable TX
    UR1CR2bits.UR1BDM = 0;      // 16x baud rate
    UR1CR2bits.UR1BRRH = 0;
    UR1BRRL = 104;              // 9600 baud at 16 MHz
    UR1CR2bits.UR1RXNEIE = 1;   // Enable RX interrupt
    UR1STAT = 0;                // Clear status
    INTCON1 = 0B11000000;       // Enable interrupts
    IPEN = 0;                   // Disable priority
    UR1CR1bits.UR1EN = 1;       // Enable UART1
    GIE = 1;                    // Enable global interrupts
}

// UART2 initialization removed to reduce complexity and potential interrupt conflicts

void TIMER2_INITIAL(void)
{
    PCKEN1bits.T2CKEN = 1;      // Enable TIM2 clock

    T2CRbits.T2CKS = 0b00;     // System clock (16 MHz)
    T2CRbits.T2PSC = 0b100;    // Prescaler = 16 ? 1 MHz timer clock

    T2CNTRL = 0;
    T2CNTRH = 0;

    // 1 ms period
    T2PRL = 0xE7;              // Low byte of 999
    T2PRH = 0x03;              // High byte of 999

    T2CRbits.T2IF = 0;
    T2CRbits.T2IE = 1;

    IPEN = 0;
    PEIE = 1;
    GIE  = 1;

    T2CRbits.T2CEN = 1;        // Start timer
}

void DelayUs(uint8_t Time) {
    for (uint8_t a = 0; a < Time; a++) {
        NOP();
    }
}

void Delay10Us(void) {
    for (uint8_t i = 0; i < 17; i++) {
        NOP(); NOP(); NOP(); NOP();
    }
}

void DelayMs(uint16_t Time) {
    for (uint16_t a = 0; a < Time; a++) {
        for (uint8_t b = 0; b < 98; b++) {
            Delay10Us();
        }
    }
}

void DelayS(uint16_t Time) {
    for (uint16_t a = 0; a < Time; a++) {
        for (uint16_t b = 0; b < 10; b++) {
            DelayMs(100);
        }
    }
}

void Delay450Us(void) {
    for (uint8_t i = 0; i < 45; i++) {
        Delay10Us();
    }
}

void Send_Character(uint8_t tchar) {
    // Make sure TX buffer is empty before sending
    while (!UR1TXEF) {
        NOP();
    }
    
    // Send the character
    UR1DATL = tchar;
    
    // Wait for transmission to complete
    while (!UR1TXEF) {
        NOP();
    }
    
    // Add a small delay for stability
    DelayUs(50);
}

void Send_String(uint8_t *ptr) {
    // First make sure we're not in the middle of another transmission
    while (!UR1TXEF) { NOP(); }
    DelayMs(5);
    
    // Send the entire string with careful timing between characters
    while (*ptr) {
        Send_Character(*ptr++);
        DelayMs(1); // Substantial delay between characters
    }
    
    // Add a pause after complete message to ensure it's fully received
    DelayMs(5);
}

void Nop_Delay(void) {
    NOP(); NOP(); NOP(); NOP(); NOP(); NOP();
}

uint8_t EEPROMread(uint8_t EEAddr) {
    PCKEN3bits.DMCKEN = 1;  // Enable EEPROM clock
    EECON1bits.EEPGD = 0;   // Access DROM
    EECON1bits.CFGS = 0;    // Access PROM or DROM
    EEADR = EEAddr;         // Set EEPROM address
    EECON1bits.RD = 1;      // Start read
    Nop_Delay();            // Ensure timing
    uint8_t data = EEDATA;  // Read data
    PCKEN3bits.DMCKEN = 0;  // Disable EEPROM clock
    return data;
}

void EEPROMwrite(uint8_t EEAddr, uint8_t Data) {
    // Removed UART2 disable/enable as UART2 is not used
    PCKEN3bits.DMCKEN = 1;  // Enable EEPROM clock
    EEADR = EEAddr;         // Set EEPROM address
    EEDATA = Data;          // Set EEPROM data
    EECON1bits.EEPGD = 0;   // Access DROM
    EECON1bits.CFGS = 0;    // Access PROM or DROM
    EECON1bits.WREN = 1;    // Enable write
    GIE = 0;                // Disable interrupts
    EECON2 = 0x55;          // Unlock sequence
    EECON2 = 0xAA;
    EECON1bits.WR = 1;      // Start write
    while (EECON1bits.WR);  // Wait for completion
    EECON1bits.WREN = 0;    // Disable write
    PCKEN3bits.DMCKEN = 0;  // Disable EEPROM clock
    DelayMs(1);
    GIE = 1;                // Re-enable interrupts
}

uint16_t GET_ADC_DATA(uint8_t adcChannel) {
    ADCON0bits.ADCHS = adcChannel & 0x3F; // Set ADC channel
    Delay10Us(); Delay10Us();             // Stabilization delay
    ADCON0bits.ADGO = 1;                  // Start ADC
    NOP(); NOP();
    while (ADCON0bits.ADGO);              // Wait for completion
    return (uint16_t)(ADRESH << 8 | ADRESL); // Return 12-bit ADC value
}

uint16_t ADC_Calibration(uint16_t val, ADC_SAMPLE_CONFIG_TYPE type) {
    uint8_t left = 0, right = 63, mid;
    while (left <= right) {
        mid = (left + right) >> 1;
        if (RangeTable[type][mid] > val) {
            right = mid - 1;
        } else {
            left = mid + 1;
        }
    }
    return val + INL_Table[type][left - 1];
}

void ADC_INITIAL(void) {
    PCKEN2bits.ADCCKEN = 1;     // Enable ADC clock
    TRISEbits.TRISE2 = 1;       // PE2 as input (AN34)
    TRISEbits.TRISE1 = 1;       // PE1 as input (AN33)
    ANSEL4 = 0B00000110;        // PE2 and PE1 as analog
    ADCKCONbits.ADCS = 0;       // ADC clock = SYSCLK
    ADCKCONbits.ADCKDIV = 63;   // TAD = TADCS / (ADCKDIV + 1)
    ADCMPLbits.ADCMPEN = 0;     // Disable comparator
    ADCON1bits.ADVREFEN = 1;    // Enable voltage reference
    ADCON1bits.ADVREFS = 0B10;  // Reference = 3.0V
    ADCON1bits.ADPREF = 0B00;   // Positive ref = VADC_REF
    ADCON1bits.ADNREF = 0B01;   // Negative ref = GND
    ADCON2bits.ADTRGT = 0B00;   // No trigger
    ADCON3bits.ADFM = 1;        // Right-justified result
    ADCON2bits.ADDLY8 = 0;      // No delay
    ADDLY = 0;
    ADCON0bits.ADCHS = ADC_CHANNEL_AN2; // Select AN34 (PE2)
    ADCON0bits.ADON = 1;        // Enable ADC
    Delay450Us(); Delay450Us(); // Stabilization delay
}

void SN74HC595_INITIAL(void) {
    SCK_74HC595_DIR = 0;        // SCK output
    DATA_74HC595_DIR = 0;       // Data output
    LATCH_74HC595_DIR = 0;      // Latch output
}

void AC_DETECTION_INITIAL(void) {
    AC_DETECTION_DIR = 1;        // AC_DETECTION input
}


void shift_out_byte(uint8_t data) {
    DATA_74HC595 = data;
    SCK_74HC595 = 1;
    DelayUs(1);
    SCK_74HC595 = 0;
}

void send_data_to_ic(uint8_t data) {
    LATCH_74HC595 = 0;
    for (int i = 31; i >= 0; i--) {
        shift_out_byte((data >> i) & 0x01);
    }
    LATCH_74HC595 = 1;
    DelayUs(1);
    LATCH_74HC595 = 0;
}

void clear_sn74hc595(void) {
    sn74hc595_buffer = 0x00;
    send_data_to_ic(sn74hc595_buffer);
}

void sn74hc595_lastdata(Typedef_status_t *status) {
    status->voltage = EEPROMread(0x05);
    switch (status->voltage) {
        case 0:  sn74hc595_buffer = 0x00; break;
        case 24: sn74hc595_buffer = 0x08; break;
        case 30: sn74hc595_buffer = 0x04; break;
        case 48: sn74hc595_buffer = 0x02; break;
        case 56: sn74hc595_buffer = 0x01; break;
        default: sn74hc595_buffer = 0x08; break;
    }
    send_data_to_ic(sn74hc595_buffer);
    DelayMs(1);
}

// Calculate a simple checksum
uint8_t calculate_checksum(uint8_t *data, uint8_t length) {
    uint8_t checksum = 0;
    for (uint8_t i = 0; i < length; i++) {
        checksum ^= data[i]; // XOR checksum
    }
    return checksum;
}

// Send reliable data with retry mechanism
void send_reliable_message(uint8_t *data, uint8_t length) {
    // Add a solid delay before transmission
    DelayMs(20);
    
    // Flush any pending transmissions
    while (!UR1TXEF) { NOP(); }
    
    // Send start marker
    Send_Character('$');
    DelayMs(10);
    
    // Send data length
    Send_Character(length);
    DelayMs(10);
    
    // Send actual data bytes
    for (uint8_t i = 0; i < length; i++) {
        Send_Character(data[i]);
        DelayMs(10);
    }
    
    // Send checksum
    uint8_t checksum = calculate_checksum(data, length);
    Send_Character(checksum);
    DelayMs(10);
    
    // Send end marker
    Send_Character('#');
    DelayMs(10);
    Send_Character('\r');
    DelayMs(5);
    Send_Character('\n');
    DelayMs(20);
}

void sendPoeStatus(Typedef_status_t *status) {
    adcData = GET_ADC_DATA(ADC_CHANNEL_AN2);
    theVoltage = (uint32_t)adcData * 3 * 1000 / 4096; // Convert to mV
    if (adcData < BATT_10P) {
        batteryPercentage = 0;
        status->set_battery = 0;
    } else if (adcData < BATT_25P) {
        batteryPercentage = 10;
        status->set_battery = 1;
    } else if (adcData < BATT_50P) {
        batteryPercentage = 25;
        status->set_battery = 1;
    } else if (adcData < BATT_75P) {
        batteryPercentage = 50;
        status->set_battery = 1;
    } else if (adcData < BATT_100P) {
        batteryPercentage = 75;
        status->set_battery = 1;
    } else {
        batteryPercentage = 100;
        status->set_battery = 1;
    }

    // Update LED status via shift register
    sn74hc595_buffer &= 0x0F;
    if (batteryPercentage == 10) {
        batt_10per_flag = 1;
    } else if (batteryPercentage == 25) {
        batt_10per_flag = 0;
        sn74hc595_buffer |= 0x80;
    } else if (batteryPercentage == 50) {
        batt_10per_flag = 0;
        sn74hc595_buffer |= 0xC0;
    } else if (batteryPercentage == 75) {
        batt_10per_flag = 0;
        sn74hc595_buffer |= 0xE0;
    } else if (batteryPercentage == 100) {
        batt_10per_flag = 0;
        sn74hc595_buffer |= 0xF0;
    }
    send_data_to_ic(sn74hc595_buffer);
    
    // Simplify AC status check
    status->set_ac = (PORTEbits.PE6 == 0) ? 1 : 0;
    
    // Update status variables
    status->set_bat_per = batteryPercentage;
    status->voltage = EEPROMread(0x05);
    
    // Prepare the binary data packet - more reliable than JSON
    uint8_t data[5];
    data[0] = status->set_ac;          // AC status
    data[1] = status->set_battery;     // Battery status
    data[2] = batteryPercentage;       // Battery percentage
    data[3] = status->set_mode;        // Mode
    data[4] = status->voltage;         // Voltage
    
    // Send the reliable message
    send_reliable_message(data, 5);
    
    // Also send the traditional JSON format as a backup
    // We'll format it as a single string to avoid partial transmission issues
    char json_buffer[48];
    sprintf(json_buffer, "{\"A\":%d,\"B\":%d,\"C\":%d,\"D\":%d,\"E\":%d}\r\n", 
        status->set_ac, status->set_battery, batteryPercentage,
        status->set_mode, status->voltage);
    
    // Small delay before sending the JSON backup
    DelayMs(50);
    
    // Send the JSON - using traditional send function for compatibility
    Send_String(json_buffer);
}
 
void battery_percentage(void) {
    adcData = GET_ADC_DATA(ADC_CHANNEL_AN2);
    if (adcData < BATT_10P) {
        batteryPercentage = 0;
    } else if (adcData < BATT_25P) {
        batteryPercentage = 10;
    } else if (adcData < BATT_50P) {
        batteryPercentage = 25;
    } else if (adcData < BATT_75P) {
        batteryPercentage = 50;
    } else if (adcData < BATT_100P) {
        batteryPercentage = 75;
    } else {
        batteryPercentage = 100;
    }

    sn74hc595_buffer &= 0x0F;
    if (batteryPercentage == 10) {
        sn74hc595_buffer |= 0x80;
        send_data_to_ic(sn74hc595_buffer);
        DelayMs(100);
        sn74hc595_buffer &= ~0x80;
        send_data_to_ic(sn74hc595_buffer);
        DelayMs(100);
    } else if (batteryPercentage == 25) {
        batt_10per_flag = 0;
        sn74hc595_buffer |= 0x80;
    } else if (batteryPercentage == 50) {
        batt_10per_flag = 0;
        sn74hc595_buffer |= 0xC0;
    } else if (batteryPercentage == 75) {
        batt_10per_flag = 0;
        sn74hc595_buffer |= 0xE0;
    } else if (batteryPercentage == 100) {
        batt_10per_flag = 0;
        sn74hc595_buffer |= 0xF0;
    }
    send_data_to_ic(sn74hc595_buffer);
}

long extract_and_convert(char *input_string) {
    char *start_ptr = strchr(input_string, ':');
    if (!start_ptr) {
        return 0; // Return default if ':' not found
    }
    
    start_ptr++; // Move past the colon
    
    char *end_ptr = strchr(start_ptr, ';');
    if (!end_ptr) {
        // If no semicolon, look for end of line or null terminator
        end_ptr = strchr(start_ptr, '\r');
        if (!end_ptr) {
            end_ptr = strchr(start_ptr, '\n');
            if (!end_ptr) {
                // If we still don't have an endpoint, use string length
                end_ptr = start_ptr + strlen(start_ptr);
            }
        }
    }
    
    // Save the character at end position
    char saved_char = *end_ptr;
    
    // Temporarily null-terminate for conversion
    *end_ptr = '\0';
    
    // Convert to integer
    long result = atoi(start_ptr);
    
    // Restore the original character
    *end_ptr = saved_char;
    
    return result;
}

#define ADC_SAMPLES      20
#define FAN_PIN          PA3
#define FAN_PIN_DIR      TRISA3
float read_battery_voltage(uint8_t channel)
{
    unsigned long sum = 0;

    for (unsigned char i = 0; i < ADC_SAMPLES; i++)
    {
        sum += GET_ADC_DATA(channel);
        Delay10Us();   // small gap improves stability
    }

    float avg_raw = (float)sum / ADC_SAMPLES;

    float adc_voltage = (avg_raw * 3.0f) / 4095.0f;

    return adc_voltage;
}

#define TEMP_SPIKE_DELTA   2.0f     // max allowed jump (2?C)
#define TEMP_CONFIRM_CNT  3


uint8_t voltage_to_temperature(float volt, float *temp_out)
{
    /* ---- Clamp high voltage ? MIN temperature ---- */
    if (volt > temp_map[0].volt)
    {
        *temp_out = temp_map[0].temp_c;   // 25?C
        return 1;
    }

    /* ---- Clamp low voltage ? MAX temperature ---- */
    if (volt < temp_map[TEMP_MAP_SIZE - 1].volt)
    {
        *temp_out = temp_map[TEMP_MAP_SIZE - 1].temp_c; // 70?C
        return 1;
    }

    /* ---- Normal interpolation ---- */
    for (uint8_t i = 0; i < TEMP_MAP_SIZE - 1; i++)
    {
        float v1 = temp_map[i].volt;
        float v2 = temp_map[i + 1].volt;

        if (volt <= v1 && volt >= v2)
        {
            float t1 = temp_map[i].temp_c;
            float t2 = temp_map[i + 1].temp_c;

            *temp_out = t1 + ((v1 - volt) * (t2 - t1)) / (v1 - v2);
            return 1;
        }
    }

    /* Should never happen */
    *temp_out = temp_map[TEMP_MAP_SIZE - 1].temp_c;
    return 1;
}


uint8_t get_filtered_temperature(float *out_temp)
{
    static float last_valid = 0.0f;
    static float suspect_value = 0.0f;
    static uint8_t suspect_cnt = 0;
    static uint8_t initialized = 0;

    float volt;
    float temp;

    /* Read ADC voltage */
    volt = read_battery_voltage(33);

    /* Convert voltage ? temperature */
    if (!voltage_to_temperature(volt, &temp))
        return 0;

    /* First valid sample */
    if (!initialized)
    {
        last_valid = temp;
        *out_temp = temp;
        initialized = 1;
        return 1;
    }

    /* Normal small change */
    if (fabs(temp - last_valid) <= TEMP_SPIKE_DELTA)
    {
        last_valid = temp;
        suspect_cnt = 0;
        *out_temp = temp;
        return 1;
    }

    /* Possible spike */
    if (suspect_cnt == 0 || fabs(temp - suspect_value) <= TEMP_SPIKE_DELTA)
    {
        suspect_value = temp;
        suspect_cnt++;

        if (suspect_cnt >= TEMP_CONFIRM_CNT)
        {
            last_valid = suspect_value;
            suspect_cnt = 0;
            *out_temp = last_valid;
            return 1;
        }
    }
    else
    {
        suspect_cnt = 0;
    }

    /* Spike ignored */
    *out_temp = last_valid;
    return 1;
}

#define TEMP_HIGH_C  60.0f
#define TEMP_LOW_C   50.0f

void fan_control_task(void)
{
    float temp;

    if (!get_filtered_temperature(&temp))
        return;

    temp_c = temp;

    /* -------- HIGH TEMP -------- */
    if (temp_c >= TEMP_HIGH_C)
    {
        fan_state = FAN_ON;
        high_temp_seen = 1;

        fan_timer_active = 0;
        timer_done = 0;

        ms_cnt = sec_cnt = min_cnt = 0;
    }

    /* -------- LOW TEMP -------- */
    else if (temp_c <= TEMP_LOW_C)
    {
        if (!fan_timer_active)
        {
            fan_state = FAN_OFF;
            high_temp_seen = 0;

            timer_done = 0;
            ms_cnt = sec_cnt = min_cnt = 0;
        }
    }

    /* -------- MID RANGE -------- */
    else
    {
        if (high_temp_seen && !fan_timer_active)
        {
            fan_state = FAN_ON;
            fan_timer_active = 1;

            ms_cnt = sec_cnt = min_cnt = 0;
            print_timer_start_flag = 1;
        }

        if (timer_done)
        {
            timer_done = 0;

            if (temp_c <= TEMP_LOW_C)
            {
                fan_state = FAN_OFF;
                high_temp_seen = 0;
                fan_timer_active = 0;
            }
            else
            {
                fan_state = FAN_ON;
                fan_timer_active = 1;

                ms_cnt = sec_cnt = min_cnt = 0;
                print_timer_restart_flag = 1;
            }
        }
    }

    FAN_PIN = fan_state;
}



void main(void) {
    GIE = 0; // Disable interrupts
    POWER_INITIAL();
    GPIO_Initialize();
    SN74HC595_INITIAL();
    AC_DETECTION_INITIAL();
    clear_sn74hc595();

    uint8_t existingVoltage = EEPROMread(0x05);
    switch (existingVoltage) {
        case START_VOLT_OFF:
			GPIO_DC_ON = 1;             /* output off */
            GPIO_56VOLT_SET = 0;
            GPIO_48VOLT_SET = 0;
            GPIO_30VOLT_SET = 0;
            EEPROMwrite(0x05, 0);
            sn74hc595_buffer &= 0xF0;
            send_data_to_ic(sn74hc595_buffer);
            GPIO_DC_ON = 1;             /* output off */
            break;
        case 24:
            GPIO_56VOLT_SET = 0;
            GPIO_48VOLT_SET = 0;
            GPIO_30VOLT_SET = 0;
            GPIO_DC_ON = 1;             /* off while selecting rail */
            EEPROMwrite(0x05, 24);
            sn74hc595_buffer &= 0xF0;
            sn74hc595_buffer |= 0x08;
            send_data_to_ic(sn74hc595_buffer);
            DelayMs(500);
            GPIO_DC_ON = 0;             /* 24 V: enable output only */
            break;
        case 30:
            GPIO_56VOLT_SET = 0;
            GPIO_48VOLT_SET = 0;
            GPIO_DC_ON = 1;
            EEPROMwrite(0x05, 30);
            sn74hc595_buffer &= 0xF0;
            sn74hc595_buffer |= 0x04;
            send_data_to_ic(sn74hc595_buffer);
            DelayMs(500);
            GPIO_30VOLT_SET = 1;
            GPIO_DC_ON = 0;
            break;
        case 48:
            GPIO_56VOLT_SET = 0;
            GPIO_30VOLT_SET = 0;
            GPIO_DC_ON = 1;
            EEPROMwrite(0x05, 48);
            sn74hc595_buffer &= 0xF0;
            sn74hc595_buffer |= 0x02;
            send_data_to_ic(sn74hc595_buffer);
            DelayMs(500);
            GPIO_48VOLT_SET = 1;
            GPIO_DC_ON = 0;
            break;
        case 56:
            GPIO_48VOLT_SET = 0;
            GPIO_30VOLT_SET = 0;
            GPIO_DC_ON = 1;
            EEPROMwrite(0x05, 56);
            sn74hc595_buffer &= 0xF0;
            sn74hc595_buffer |= 0x01;
            send_data_to_ic(sn74hc595_buffer);
            DelayMs(500);
            GPIO_56VOLT_SET = 1;
            GPIO_DC_ON = 0;
            break;
        default:
            GPIO_56VOLT_SET = 0;
            GPIO_48VOLT_SET = 0;
            GPIO_30VOLT_SET = 0;
            GPIO_DC_ON = 1;
            EEPROMwrite(0x05, 0);
            DelayMs(10);
            EEPROMwrite(0x02, 0);
            DelayMs(10);
            EEPROMwrite(0x03, 1);
            DelayMs(10);
            sn74hc595_buffer |= 0xF0;
            send_data_to_ic(sn74hc595_buffer);
            DelayMs(10);
            GPIO_DC_ON = 1;
            break;
    }

    
    
    UART1_INITIAL();
    // UART2 initialization removed - PE6 used as input for AC detection
    ADC_INITIAL();
	FAN_PIN_DIR = 0;
    uint8_t battery_update_counter = 0;
    TIMER2_INITIAL();
    PEIE = 1;
    GIE = 1; // Enable interrupts
    //UART3_SendString((uint8_t *) "Systme Initialized\r\n" );
    while (1) {
    

        // Update battery percentage less frequently to reduce processing load
        if (++battery_update_counter >= 5) {
            battery_percentage();
            battery_update_counter = 0;
        }
        
        // Shorter delay to improve responsiveness
        DelayMs(20);
        
        // Process received commands immediately when they arrive
        if (receive_complete_flag) {
            receive_complete_flag = 0;
            
            // Use strncmp for safer string comparison - avoids potential buffer overrun
            if (usart_rx_buff[0] == 'g' && usart_rx_buff[4] == 'S' && usart_rx_buff[5] == 'T') {
                // Send status response with highest priority
                sendPoeStatus(&Typedef_status);
            } else if (usart_rx_buff[0] == 's' && usart_rx_buff[4] == 'P' && usart_rx_buff[5] == 'O' && usart_rx_buff[8] == 'V') {
                // Process voltage change command (set_POE_VOLT:<n>;)
                Typedef_status.value = extract_and_convert(usart_rx_buff);
                EEPROMwrite(0x05, (uint8_t)Typedef_status.value);
                switch (Typedef_status.value) {
                
                    case START_VOLT_OFF:
						GPIO_DC_ON = 1;
                        GPIO_56VOLT_SET = 0;
                        GPIO_48VOLT_SET = 0;
                        GPIO_30VOLT_SET = 0;
                        sn74hc595_buffer &= 0xF0;
                        send_data_to_ic(sn74hc595_buffer);
                        GPIO_DC_ON = 1;
                        sendPoeStatus(&Typedef_status);
                        break;
                    case START_24_VOLT:
                        sendPoeStatus(&Typedef_status);
                        GPIO_56VOLT_SET = 0;
                        GPIO_48VOLT_SET = 0;
                        GPIO_30VOLT_SET = 0;
                        GPIO_DC_ON = 1;
                        sn74hc595_buffer &= 0xF0;
                        sn74hc595_buffer |= 0x08;
                        send_data_to_ic(sn74hc595_buffer);
                        DelayMs(500);
                        GPIO_DC_ON = 0;
                        break;
                    case START_30_VOLT:
                        sendPoeStatus(&Typedef_status);
                        GPIO_56VOLT_SET = 0;
                        GPIO_48VOLT_SET = 0;
                        GPIO_DC_ON = 1;
                        sn74hc595_buffer &= 0xF0;
                        sn74hc595_buffer |= 0x04;
                        send_data_to_ic(sn74hc595_buffer);
                        DelayMs(500);
                        GPIO_30VOLT_SET = 1;
                        GPIO_DC_ON = 0;
                        break;
                    case START_48_VOLT:
                        sendPoeStatus(&Typedef_status);
                        GPIO_56VOLT_SET = 0;
                        GPIO_30VOLT_SET = 0;
                        GPIO_DC_ON = 1;
                        sn74hc595_buffer &= 0xF0;
                        sn74hc595_buffer |= 0x02;
                        send_data_to_ic(sn74hc595_buffer);
                        DelayMs(500);
                        GPIO_48VOLT_SET = 1;
                        GPIO_DC_ON = 0;
                        break;
                    case START_56_VOLT:
                        sendPoeStatus(&Typedef_status);
                        GPIO_48VOLT_SET = 0;
                        GPIO_30VOLT_SET = 0;
                        GPIO_DC_ON = 1;
                        sn74hc595_buffer &= 0xF0;
                        sn74hc595_buffer |= 0x01;
                        send_data_to_ic(sn74hc595_buffer);
                        DelayMs(500);
                        GPIO_56VOLT_SET = 1;
                        GPIO_DC_ON = 0;
                        break;
                }
            }
        }
        fan_control_task();
    }
}