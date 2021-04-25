#ifndef Touch_h
#define Touch_h

#include <Arduino.h>
#include <Homie.h>
#include <Adafruit_MPR121.h>

#define MPR121_AFE1_REG 0x5C
#define MPR121_FFI_6  (0<<6)
#define MPR121_FFI_10  (0<<6)
#define MPR121_FFI_18  (0<<6)
#define MPR121_FFI_34  (0<<6)
#define MPR121_CDC_MASK 0x3f

#define MPR121_AFE2_REG 0x5C
#define MPR121_CDT_0_5us (1<<5)
#define MPR121_CDT_1us (2<<5)
#define MPR121_CDT_2us (3<<5)
#define MPR121_CDT_4us (4<<5)
#define MPR121_CDT_8us (5<<5)
#define MPR121_CDT_16us (6<<5)
#define MPR121_CDT_32us (7<<5)
#define MPR121_SFI_4 (0<<3)
#define MPR121_SFI_6 (1<<3)
#define MPR121_SFI_10 (2<<3)
#define MPR121_SFI_18 (3<<3)
#define MPR121_ESI_1ms 0
#define MPR121_ESI_2ms 1
#define MPR121_ESI_4ms 2
#define MPR121_ESI_8ms 3
#define MPR121_ESI_16ms 4
#define MPR121_ESI_32ms 5
#define MPR121_ESI_64ms 6
#define MPR121_ESI_128ms 7


#define MPR121_PRO_T 			0x59 // Touch Threshold register address
#define MPR121_PRO_R 			0x5A // Touch Threshold register address

#define	MPR121_PROX_MHDR		0x36 // ELEPROX Max Half Delta Rising register address - 0xFF
#define	MPR121_PROX_NHDAR 		0x37 // ELEPROX Noise Half Delta Amount Rising register address - 0xFF
#define	MPR121_PROX_NCLR 		0x38 // ELEPROX Noise Count Limit Rising register address - 0x00
#define	MPR121_PROX_FDLR 		0x39 // ELEPROX Filter Delay Limit Rising register address - 0x00
#define	MPR121_PROX_MHDF 		0x3A // ELEPROX Max Half Delta Falling register address - 0x01
#define	MPR121_PROX_NHDAF		0x3B // ELEPROX Noise Half Delta Amount Falling register address - 0x01
#define	MPR121_PROX_NCLF  		0x3C // ELEPROX Noise Count Limit Falling register address - 0xFF
#define	MPR121_PROX_NDLF   	0x3D // ELEPROX Filter Delay Limit Falling register address - 0xFF
#define	MPR121_PROX_DEB     	0x5B // ELEPROX Debounce register address

#define	MPR121_PROX_NHDAT   	0x3E // ELEPROX Noise Half Delta Amount Touched register address - 0x00
#define	MPR121_PROX_NCLT    	0x3F // ELEPROX Noise Count Limit Touched register address - 0x00
#define	MPR121_PROX_FDLT    	0x40 // ELEPROX Filter Delay Limit Touched register address - 0x00

#define	MPR121_ATO_CFG0		0x7B
#define	MPR121_ATO_CFGU		0x7D
#define	MPR121_ATO_CFGL		0x7E
#define	MPR121_ATO_CFGT		0x7F



void touch_setup();
uint16_t read_touch();
#endif
