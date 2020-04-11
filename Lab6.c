// Lab6.c
// Runs on LM4F120 or TM4C123
// Use SysTick interrupts to implement a 3-key digital piano
// MOOC lab 13 or EE319K lab6 starter
// Program written by: Zachary Wang, Eric Hayes
// Date Created: 3/6/17 
// Last Modified: 3/5/18 
// Lab number: 6
// Hardware connections
// TO STUDENTS "REMOVE THIS LINE AND SPECIFY YOUR HARDWARE********

#define B 2531
#define A 2841 	
#define G 3189	
#define F 3579
#define	E	3792
#define	D	4257
#define	C 4778

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "Sound.h"
#include "Piano.h"
#include "TExaS.h"

// basic functions defined at end of startup.s
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

int counter=0;
int twinkle[42]=
	{D, D, A, A, B, B, A,
	 G, G, F, F, E, E, D,
	 A, A, G, G, F, F, E,
	 A, A, G, G, F, F, E,
	 D, D, A, A, B, B, A,
	 G, G, F, F, E, E, D};	//change the note HOW TO CHANGE WHEN THE NOTE CHANGES??
int reload[42]=
	{D, D, A, A, B, B, A,
	 G, G, F, F, E, E, D,
	 A, A, G, G, F, F, E,
	 A, A, G, G, F, F, E,
	 D, D, A, A, B, B, A,
	 G, G, F, F, E, E, D};	//change the amount of time between timer interrupts 

void heartbeat(){
	GPIO_PORTF_DATA_R ^= 0x04;
}	

void stop(void) {
	counter=0;
	Sound_Play(0);
}	

void song(void) {
	if(counter==42) {
		stop();
		return;
	}	
	Sound_Play(twinkle[counter]);
	counter++;
}	

void SysTick_Handler(void) {
	heartbeat();
	int data=Piano_In();
	if((data & 0x07)==0x01) {
		Sound_Play(B); 
	}
	else if((data & 0x07)==0x02) {
		Sound_Play(A); 
	}
	else if((data & 0x07)==0x04) {
		Sound_Play(G); 
	}
	/*else if() {// button pressed
		song();
	}*/	
	else {
		Sound_Play(0);
		stop();
	}	
}

/*void Timer0A_Init(){
	long sr;
  sr = StartCritical(); 
  SYSCTL_RCGCTIMER_R |= 0x01;   // 0) activate TIMER0
  TIMER0_CTL_R = 0x00000000;    // 1) disable TIMER0A during setup
  TIMER0_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
  TIMER0_TAMR_R = 0x00000002;   // 3) configure for periodic mode, default down-count settings
  TIMER0_TAILR_R = 0x00FFFFFF;    // 4) reload value
  TIMER0_TAPR_R = 0;            // 5) bus clock resolution
  TIMER0_ICR_R = 0x00000001;    // 6) clear TIMER0A timeout flag
  TIMER0_IMR_R = 0x00000001;    // 7) arm timeout interrupt
  NVIC_PRI4_R = (NVIC_PRI4_R&0x00FFFFFF)|0x80000000; // 8) priority 4
// interrupts enabled in the main program after all devices initialized
// vector number 35, interrupt number 19
  NVIC_EN0_R = 1<<19;           // 9) enable IRQ 19 in NVIC
  TIMER0_CTL_R = 0x00000001;    // 10) enable TIMER0A
  EndCritical(sr);
}

void Timer0A_Handler(void){
  TIMER0_ICR_R = TIMER_ICR_TATOCINT;// acknowledge timer0A timeout
  if((GPIO_PORTF_DATA_R & 0x01) == 1){ //button pressed
		song();
	}
	else {
		stop();
	}	
	//set reload value to something
}*/

int main(void){      
  TExaS_Init(SW_PIN_PE3210,DAC_PIN_PB3210,ScopeOn);    // bus clock at 80 MHz
	Piano_Init();
  Sound_Init();
	//Timer0A_Init();
  int delay=0;
	SYSCTL_RCGCGPIO_R |= 0x28;
	delay++;
	delay++;
	GPIO_PORTF_DEN_R |= 0x05;	//PF2 LED output
	GPIO_PORTF_DIR_R |= 0x04;
	GPIO_PORTD_DEN_R |= 0x08;	//some other button
	GPIO_PORTD_DIR_R &= 0x00;
	// other initialization
  EnableInterrupts();
  while(1){ 
  }    
}


