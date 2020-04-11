// put implementations for functions, explain how it works
// put your names here, date
#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"

#include "DAC.h"

void DAC_Init(void){
	int delay=0;
	SYSCTL_RCGCGPIO_R |= 0x02;
	delay++;
	delay++;
	GPIO_PORTB_DEN_R |= 0x0F;	//4 bit DAC output
	GPIO_PORTB_DIR_R |= 0x3F;
}

void DAC_Out(uint32_t data){
	GPIO_PORTB_DATA_R &= 0;
	GPIO_PORTB_DATA_R = data;
}
