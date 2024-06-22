#ifndef H_MAIN
#define H_MAIN 1

#include <stdio.h>
#include <stdlib.h>
#include "stm32f4xx.h"

//Settings
//Input Capture in miliseconds
//#define TIM_MS 1

//Constants

//prototype function
void RCC_Init(void);
void key_led_Init(void);
void TIM1_Init(void);
void TIM2_Init(void);

#endif