// Sound.c
// This module contains the SysTick ISR that plays sound
// Runs on LM4F120 or TM4C123
// Program written by: Zachary Wang, Eric Hayes
// Date Created: 3/6/17 
// Last Modified: 3/5/18 
// Lab number: 6
// Hardware connections
// TO STUDENTS "REMOVE THIS LINE AND SPECIFY YOUR HARDWARE********

// Code files contain the actual implemenation for public functions
// this file also contains an private functions and private data
#include <stdint.h>
#include <stdlib.h>
#include "dac.h"
#include "../inc/tm4c123gh6pm.h"

#define NVIC_ST_CTRL_COUNT      0x00010000  // Count flag
#define NVIC_ST_CTRL_CLK_SRC    0x00000004  // Clock Source
#define NVIC_ST_CTRL_INTEN      0x00000002  // Interrupt enable
#define NVIC_ST_CTRL_ENABLE     0x00000001  // Counter mode
#define NVIC_ST_RELOAD_M        0x00FFFFFF  // Counter load value

const uint8_t wave[32] = 
	{8,9,11,12,13
	,14,14,15,15,15
	,14,14,13,12,11
	,9,8,7,5,4,3
	,2,2,1,1,1,2
	,2,3,4,5,7};

int i=0;
	
// Initialize SysTick with busy wait running at bus clock.
void SysTick_Init(void){
  NVIC_ST_CTRL_R = 0;                   
  NVIC_ST_RELOAD_R = 0x00FFFFFF;						
  NVIC_ST_CURRENT_R = 0;                
  NVIC_ST_CTRL_R = 7;
}

// **************Sound_Init*********************
// Initialize digital outputs and SysTick timer
// Called once, with sound/interrupts initially off
// Input: none
// Output: none
void Sound_Init(void){
	DAC_Init();
	SysTick_Init();
}

// **************Sound_Play*********************
// Start sound output, and set Systick interrupt period 
// Sound continues until Sound_Play called again
// This function returns right away and sound is produced using a periodic interrupt
// Input: interrupt period
//           Units of period to be determined by YOU
//           Maximum period to be determined by YOU
//           Minimum period to be determined by YOU
//         if period equals zero, disable sound output
// Output: none
void Sound_Play(uint32_t period){
		if(period==0) {
			DAC_Out(0);
			return;
		}	
		if(i>=32) {
			i=0;
		}	
		DAC_Out(wave[i]);
		i++;
		NVIC_ST_RELOAD_R = period;
		NVIC_ST_CURRENT_R = 0; 
}

