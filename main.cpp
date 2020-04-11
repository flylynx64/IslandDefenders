// main.cpp
// Runs on LM4F120/TM4C123
// Jonathan Valvano and Daniel Valvano
// This is a starter proj1ect for the EE319K Lab 10 in C++

// Last Modified: 4/19/2018 
// http://www.spaceinvaders.de/
// sounds at http://www.classicgaming.cc/classics/spaceinvaders/sounds.php
// http://www.classicgaming.cc/classics/spaceinvaders/playguide.php
/* This example accompanies the books
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2017

   "Embedded Systems: Introduction to Arm Cortex M Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2017

 Copyright 2018 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */
// * Possible Hardware I/O connections*******************
// Slide pot pin 1 connected to ground
// Slide pot pin 2 connected to PD2/AIN5
// Slide pot pin 3 connected to +3.3V 
// fire button connected to PE0
// special weapon fire button connected to PE1
// 8*R resistor DAC bit 0 on PB0 (least significant bit)
// 4*R resistor DAC bit 1 on PB1
// 2*R resistor DAC bit 2 on PB2
// 1*R resistor DAC bit 3 on PB3 (most significant bit)
// LED on PB4
// LED on PB5

// Backlight (pin 10) connected to +3.3 V
// MISO (pin 9) unconnected
// SCK (pin ?? connected to PA2 (SSI0Clk)
// MOSI (pin 7) connected to PA5 (SSI0Tx)
// TFT_CS (pin 6) connected to PA3 (SSI0Fss)
// CARD_CS (pin 5) unconnected
// Data/Command (pin 4) connected to PA6 (GPIO), high for data, low for command
// RESET (pin 3) connected to PA7 (GPIO)
// VCC (pin 2) connected to +3.3 V
// Gnd (pin 1) connected to ground

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "PLL.h"
#include "ST7735.h"
#include "SlidePot.h"
#include "Images.h"
#include "UART.h"
#include "Timer0.h"
#include "Timer1.h"
#include "DAC.h"
#include "Sound.h"

SlidePot my(138,0);

extern "C" void DisableInterrupts(void);
extern "C" void EnableInterrupts(void);
extern "C" void SysTick_Handler(void);

// Creating a struct for the Sprite.
typedef enum {dead,alive} status_t;
struct sprite{
  uint32_t x;      // x coordinate
  uint32_t y;      // y coordinate
  const unsigned short *image; // ptr->image
  status_t life;            // dead/alive
};          
typedef struct sprite sprite_t;

sprite_t enemies1[6]={{60, 20, Cruiser, alive}, {15, 20, Cruiser, alive}, {30, 20, Submarine, alive}, 
	{45, 20, Cruiser, alive}, {75, 20, Submarine, alive}, {90, 20, Cruiser, alive}};

sprite_t enemies2[12]={{60, 20, Submarine, alive}, {15, 20, Cruiser, alive}, {30, 20, Cruiser, alive}, 
	{45, 20, Submarine, alive}, {75, 20, Cruiser, alive}, {90, 20, Cruiser, alive}, {60, 0, Cruiser, alive}, 
  {15, 0, Submarine, alive}, {30, 0, Submarine, alive}, {45, 0, Submarine, alive}, {75, 0, Cruiser, alive}, 
	{90, 0, Cruiser, alive}};

sprite_t enemies3[12]={{60, 20, Cruiser, alive}, {15, 20, Cruiser, alive}, {30, 20, Submarine, alive}, 
	{45, 20, Cruiser, alive}, {75, 20, Submarine, alive}, {90, 20, Cruiser, alive}, {60, 0, Cruiser, alive}, 
	{15, 0, Cruiser, alive}, {30, 0, Submarine, alive}, {45, 0, Cruiser, alive}, {75, 0, Submarine, alive}, 
	{90, 0, Submarine, alive}};
sprite_t	proj1[100];
sprite_t	proj2[100];
sprite_t	proj3[100];
sprite_t main1={52, 149, BattleShip, alive};
int lives=3, level=1, delay=0, size1=6, size2=0, check1=0, temp=0;
status_t check=dead;

void slide(void){
	ST7735_DrawBitmap(main1.x,main1.y,mainfill,32,8);
	my.Save(ADC_In());
	main1.x=my.Distance();
	ST7735_DrawBitmap(main1.x,main1.y,main1.image,32,8);
}

uint32_t time = 0;
volatile uint32_t flag;
void background1(void){
  flag = 1; // semaphore
	check=dead;
	for(int i=0; i<size2; i++){
		if(proj1[i].life==alive){
			ST7735_DrawBitmap(proj1[i].x, proj1[i].y,projfill,2,4);
			proj1[i].y--;	
			if(proj1[i].y==0){
				proj1[i].life=dead;
				continue;
			}	
			for(int j=0; j<size1; j++){
					if((enemies1[j].life==alive) && ((proj1[i].y-12)<=enemies1[j].y) && 
						(((proj1[i].x<enemies1[j].x) && (proj1[i].x+2>enemies1[j].x)) || ((proj1[i].x<enemies1[j].x+8) && (proj1[i].x+2>enemies1[j].x+8))
						|| (proj1[i].x==enemies1[j].x) || (proj1[i].x+2==enemies1[j].x+8) || ((proj1[i].x>enemies1[j].x) && (proj1[i].x+2<enemies1[j].x+8)))){
							sound=&Play_Explosion;
							enemies1[j].life=dead;
							proj1[i].life=dead;
							ST7735_DrawBitmap(proj1[i].x, proj1[i].y-1,projfill,2,4);
							ST7735_DrawBitmap(enemies1[i].x,enemies1[i].y,enemyfill,8,24);
							break;
					}
			}	
		}	
	}	
	for(int i=0; i<size1; i++){
		if(enemies1[i].life == alive){
			check=alive;
			enemies1[i].y++;
			if(enemies1[i].y>155){
				sound=&Play_Explosion;
				main1.life = dead;
				delay=20000;
				while(delay){delay--;}
			}
			if(enemies1[i].y > main1.y-8 && enemies1[i].y < main1.y && enemies1[i].x > main1.x && enemies1[i].x < main1.x+32){
				sound=&Play_Explosion;
				main1.life=dead;
				delay=20000;
				while(delay){delay--;}
			}
		}
		else {
			ST7735_DrawBitmap(enemies1[i].x,enemies1[i].y,enemyfill,8,24);
		}	
	}
}

void background2(void){
  flag = 1; // semaphore
	check=dead;
	for(int i=0; i<size2; i++){
		if(proj2[i].life==alive){
			ST7735_DrawBitmap(proj2[i].x, proj2[i].y,projfill,2,4);
			proj2[i].y--;	
			if(proj2[i].y==0){
				proj2[i].life=dead;
				continue;
			}	
			for(int j=0; j<size1; j++){
				if((enemies2[j].life==alive) && ((proj2[i].y-12)<=enemies2[j].y) && 
					(((proj2[i].x<enemies2[j].x) && (proj2[i].x+2>enemies2[j].x)) || ((proj2[i].x<enemies2[j].x+8) && (proj2[i].x+2>enemies2[j].x+8))
					|| (proj2[i].x==enemies2[j].x) || (proj2[i].x+2==enemies2[j].x+8) || ((proj2[i].x>enemies2[j].x) && (proj2[i].x+2<enemies2[j].x+8)))){
					sound=&Play_Explosion;
					enemies2[j].life=dead;
					proj2[i].life=dead;
					ST7735_DrawBitmap(proj2[i].x, proj2[i].y-1,projfill,2,4);
					ST7735_DrawBitmap(enemies2[i].x,enemies2[i].y,enemyfill,8,24);
					break;
				}
			}
		}	
	}	
	for(int i=0; i<size1; i++){
		if(enemies2[i].life == alive){
			check=alive;
			enemies2[i].y++;
			if(enemies2[i].y>155){
				sound=&Play_Explosion;
				main1.life = dead;
				delay=20000;
				while(delay){delay--;}
			}
			if(enemies2[i].y > main1.y-8 && enemies2[i].y < main1.y && enemies2[i].x > main1.x && enemies2[i].x < main1.x+32){
				sound=&Play_Explosion;
				main1.life=dead;
				delay=20000;
				while(delay){delay--;}
			}
		}
		else {
			ST7735_DrawBitmap(enemies2[i].x,enemies2[i].y,enemyfill,8,24);
		}	
	}
}

void background3(void){
  flag = 1; // semaphore
	check=dead;
	for(int i=0; i<size2; i++){
		if(proj3[i].life==alive){
			ST7735_DrawBitmap(proj3[i].x, proj3[i].y,projfill,2,4);
			proj3[i].y--;	
			if(proj3[i].y==0){
				proj3[i].life=dead;
				continue;
			}	
			for(int j=0; j<size1; j++){
				if((enemies3[j].life==alive) && ((proj3[i].y-12)<=enemies3[j].y) && 
					(((proj3[i].x<enemies3[j].x) && (proj3[i].x+2>enemies3[j].x)) || ((proj3[i].x<enemies3[j].x+8) && (proj3[i].x+2>enemies3[j].x+8))
					|| (proj3[i].x==enemies3[j].x) || (proj3[i].x+2==enemies3[j].x+8) || ((proj3[i].x>enemies3[j].x) && (proj3[i].x+2<enemies3[j].x+8)))){
					sound=&Play_Explosion;
					enemies3[j].life=dead;
					proj3[i].life=dead;
					ST7735_DrawBitmap(proj3[i].x, proj3[i].y-1,projfill,2,4);
					ST7735_DrawBitmap(enemies3[i].x,enemies3[i].y,enemyfill,8,24);
					break;
				}
			}
		}	
	}	
	for(int i=0; i<size1; i++){
		if(enemies3[i].life == alive){
			check=alive;
			enemies3[i].y++;
			if(enemies3[i].y>155){
				sound=&Play_Explosion;
				main1.life = dead;
				delay=20000;
				while(delay){delay--;}
			}
			if(enemies3[i].y > main1.y-8 && enemies3[i].y < main1.y && enemies3[i].x > main1.x && enemies3[i].x < main1.x+32){
				sound=&Play_Explosion;
				main1.life=dead;
				delay=20000;
				while(delay){delay--;}
			}
		}
		else {
			ST7735_DrawBitmap(enemies3[i].x,enemies3[i].y,enemyfill,8,24);
		}	
	}
}
void SysTick_Handler(void){
	(*sound)(); 
}	
void PortE_Init(){
	int delay=0;
	SYSCTL_RCGCGPIO_R |= 0x010;
	delay++;
	delay++;
	GPIO_PORTE_DEN_R |= 0x03;
	GPIO_PORTE_DIR_R &= 0;
}
void PortF_Init(void){
  SYSCTL_RCGCGPIO_R |= 0x020;
	int delay=10;
	GPIO_PORTF_DIR_R |= 0x0E;
	GPIO_PORTF_DEN_R |= 0x0E;
}
void Game_Over(void){
	ST7735_FillScreen(0x0000);            // set screen to black
  ST7735_SetCursor(7, 7);
  ST7735_OutString((char*)"GAME OVER");
  while(1){}
}
void Win(void){
	ST7735_FillScreen(0x0000);            // set screen to black
  ST7735_SetCursor(7, 7);
  ST7735_OutString((char*)"YOU WIN!");
  while(1){}
}
int main(void){
  PLL_Init(Bus80MHz);       // Bus clock is 80 MHz 
  Output_Init();
	PortE_Init();
	PortF_Init();
	Sound_Init(&Play_Intro, 7652);
	ADC_Init();
  EnableInterrupts();
	ST7735_DrawBitmap(2, 80, StartScreen, 128, 64);
	ST7735_SetCursor(4, 10);
  ST7735_OutString((char*)"Press Button 1");
	ST7735_SetCursor(7, 11);
  ST7735_OutString((char*)"to Start");
	while((GPIO_PORTE_DATA_R&1)==0){}
	ST7735_FillScreen(0x0000);
	ST7735_SetCursor(7, 7);
	ST7735_OutString((char*)"LEVEL 1");
	delay=3000000;
	while(delay>0){delay--;}
	ST7735_FillScreen(0xF800); //FFE0
	Timer1_Init(&background1,7500000); // 50 Hz
	Sound_Init(&Play_Music, 7652);
  while(main1.life == alive){
    while(flag==0){};
    flag = 0;
		//my.Sync();
		Timer0_Init(&slide,1333333);
		for(int x=0; x<size1; x++){
			if(enemies1[x].life==alive){
				ST7735_DrawBitmap(enemies1[x].x,enemies1[x].y,enemies1[x].image,8,24);
			}
		}	
		for(int x=0; x<size2; x++){
			if(proj1[x].life==alive){
				ST7735_DrawBitmap(proj1[x].x,proj1[x].y,proj1[x].image,2,4);
			}	
		}	
		if((GPIO_PORTE_DATA_R&1)==1){
			DisableInterrupts();
			while((GPIO_PORTE_DATA_R&1)==1){}
			EnableInterrupts();
			for(int x=0; x<size2; x++){
				if(proj1[x].life==dead){
					check1++;
					sprite a={main1.x+14, main1.y-8, Bullet, alive};
					proj1[x]=a;
					sound=&Play_Bullet;
					ST7735_DrawBitmap(proj1[x].x,proj1[x].y,proj1[x].image,2, 4);
					break;
				}	
			}
			if(check1==0){
				size2++;
				sprite a={main1.x+14, main1.y-8, Bullet, alive};
				proj1[size2-1]=a;
				sound=&Play_Bullet;
				ST7735_DrawBitmap(proj1[size2].x,proj1[size2].y,proj1[size2].image,2, 4);
			}
			check1=0;
		}
		if((GPIO_PORTE_DATA_R&2)==2){
			DisableInterrupts();
			while((GPIO_PORTE_DATA_R&2)){}
			while(!(GPIO_PORTE_DATA_R&2)){}
			delay=1000000;
			while(delay){delay--;}
			EnableInterrupts();
		}
		if(check==dead){
			level++;
			break;
		}	
  }
	DisableInterrupts();
	if(main1.life==dead){
		Game_Over();
	}	
	ST7735_FillScreen(0x0000);
	ST7735_SetCursor(7, 7);
	ST7735_OutString((char*)"LEVEL 2");
	main1.life = alive;
	size1 = 12;
	delay=3000000;
	while(delay>0){delay--;}
	ST7735_FillScreen(0xF800); //FFE0
	EnableInterrupts();
	Timer1_Init(&background2,5000000); // 50 Hz
	Sound_Init(&Play_Music, 7652);
  while(main1.life == alive){
    while(flag==0){};
    flag = 0;
		//my.Sync();
		Timer0_Init(&slide,1333333);
		for(int x=0; x<size1; x++){
			if(enemies2[x].life==alive){
				ST7735_DrawBitmap(enemies2[x].x,enemies2[x].y,enemies2[x].image,8,24);
			}
		}	
		for(int x=0; x<size2; x++){
			if(proj2[x].life==alive){
				ST7735_DrawBitmap(proj2[x].x,proj2[x].y,proj2[x].image,2,4);
			}	
		}	
		if((GPIO_PORTE_DATA_R&1)==1){
			DisableInterrupts();
			while((GPIO_PORTE_DATA_R&1)==1){}
			EnableInterrupts();
			for(int x=0; x<size2; x++){
				if(proj2[x].life==dead){
					check1++;
					sprite a={main1.x+14, main1.y-8, Bullet, alive};
					proj2[x]=a;
					sound=&Play_Bullet;
					ST7735_DrawBitmap(proj2[x].x,proj2[x].y,proj2[x].image,2, 4);
					break;
				}	
			}
			if(check1==0){
				size2++;
				sprite a={main1.x+14, main1.y-8, Bullet, alive};
				proj2[size2-1]=a;
				sound=&Play_Bullet;
				ST7735_DrawBitmap(proj2[size2].x,proj2[size2].y,proj2[size2].image,2, 4);
			}
			check1=0;
		}
		if((GPIO_PORTE_DATA_R&2)==2){
			DisableInterrupts();
			while((GPIO_PORTE_DATA_R&2)){}
			while(!(GPIO_PORTE_DATA_R&2)){}
			delay=1000000;
			while(delay){delay--;}
			EnableInterrupts();
		}
		if(check==dead){
			level++;
			break;
		}	
  }
	DisableInterrupts();
	if(main1.life==dead){
		Game_Over();
	}
	ST7735_FillScreen(0x0000);
	ST7735_SetCursor(7, 7);
	ST7735_OutString((char*)"LEVEL 3");
	main1.life = alive;
	size1 = 12;
	delay=3000000;
	while(delay>0){delay--;}
	ST7735_FillScreen(0xF800); //FFE0
	Timer1_Init(&background3,2500000); // 50 H
	EnableInterrupts();
	Sound_Init(&Play_Music, 7652);
  while(main1.life == alive){
    while(flag==0){};
    flag = 0;
		//my.Sync();
		Timer0_Init(&slide,1333333);
		for(int x=0; x<size1; x++){
			if(enemies3[x].life==alive){
				ST7735_DrawBitmap(enemies3[x].x,enemies3[x].y,enemies3[x].image,8,24);
			}
		}	
		for(int x=0; x<size2; x++){
			if(proj3[x].life==alive){
				ST7735_DrawBitmap(proj3[x].x,proj3[x].y,proj3[x].image,2,4);
			}	
		}	
		if((GPIO_PORTE_DATA_R&1)==1){
			DisableInterrupts();
			while((GPIO_PORTE_DATA_R&1)==1){}
			EnableInterrupts();
			for(int x=0; x<size2; x++){
				if(proj3[x].life==dead){
					check1++;
					sprite a={main1.x+14, main1.y-8, Bullet, alive};
					proj3[x]=a;
					sound=&Play_Bullet;
					
					ST7735_DrawBitmap(proj3[x].x,proj3[x].y,proj3[x].image,2, 4);
					break;
				}	
			}
			if(check1==0){
				size2++;
				sprite a={main1.x+14, main1.y-8, Bullet, alive};
				proj3[size2-1]=a;
				sound=&Play_Bullet;
				ST7735_DrawBitmap(proj3[size2].x,proj3[size2].y,proj3[size2].image,2, 4);
			}
			check1=0;
		}
		if((GPIO_PORTE_DATA_R&2)==2){
			DisableInterrupts();
			while((GPIO_PORTE_DATA_R&2)){}
			while(!(GPIO_PORTE_DATA_R&2)){}
			delay=1000000;
			while(delay){delay--;}
			EnableInterrupts();
		}
		if(check==dead){
			level++;
			break;
		}	
  }
	DisableInterrupts();
	if(main1.life==dead){
		Game_Over();
	}
	Win();
}