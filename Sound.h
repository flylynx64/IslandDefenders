// Sound.h
// Runs on TM4C123 or LM4F120
// Prototypes for basic functions to play sounds from the
// original Space Invaders.
// Jonathan Valvano
// November 17, 2014
#ifndef __SOUND_H__ // do not include more than once
#define __SOUND_H__
#include <stdint.h>

void Sound_Init(void(*task)(void), uint32_t period);
void Play_Intro(void);
void Play_Music(void);
void Play_Bullet(void);
void Play_Explosion(void);
extern int count, introsize, shoot1size, shoot2size, expsize, bgmsize;
extern void (*sound)(void);

#endif


