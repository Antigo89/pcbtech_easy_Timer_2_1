/*
File    : main.c
Software "Kurs STM32 PCBtech"
Lesson 7: Timers 1 and 2.
Student: antigo1989@gmail.com
*/

#include "main.h"

//global values
volatile uint32_t counter = 0;

/*********************************main************************************/
int main(void) {
  //System init
  SystemInit();
  RCC_Init();
  __enable_irq();
  //Timer init
  TIM1_Init();  
  TIM2_Init();
  //GPIO init
  key_led_Init();

 
  while(1){
    
    __NOP();
  }
}

/***********************interrupts function**************************/
//keys
void EXTI15_10_IRQHandler(void){
  switch(EXTI->PR & (EXTI_PR_PR10|EXTI_PR_PR11|EXTI_PR_PR12)){
    case EXTI_PR_PR10:
      TIM1->CR1 |= TIM_CR1_CEN;
      TIM1->EGR |= TIM_EGR_UG;

      TIM2->CR1 |= TIM_CR1_CEN;
      TIM2->EGR |= TIM_EGR_UG;
      break;
    case EXTI_PR_PR12:
      __NOP();
      break;
  }
  EXTI->PR |= EXTI_PR_PR10|EXTI_PR_PR12;
}

void TIM1_CC_IRQHandler(void){
  counter = TIM1->CCR2;
  TIM2->CR1 &= ~(TIM_CR1_CEN);
  TIM1->CR1 &= ~(TIM_CR1_CEN);
}

/****************************** function**********************************/
void TIM1_Init(void){
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;
  GPIOE->MODER |= GPIO_MODER_MODE11_1;
  GPIOE->AFR[1] |= (1<<GPIO_AFRH_AFSEL11_Pos);

  RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
  TIM1->SMCR &= ~(TIM_SMCR_TS);
  TIM1->SMCR |= (0b001<<TIM_SMCR_TS_Pos)|(0b111<<TIM_SMCR_SMS_Pos); //ITR1
  
  #ifdef TIM_MS
  TIM1->PSC = 1; //1kHz
  #else
  TIM1->PSC = 1000; //1Hz
  #endif
  TIM1->CR1 &= ~(TIM_CR1_CMS|TIM_CR1_DIR);
  //Input Capture
  TIM1->CCMR1 &= ~(TIM_CCMR1_IC2F|TIM_CCMR1_IC2PSC); //reset filter and prescaler
  TIM1->CCMR1 |= TIM_CCMR1_CC2S_0|(0b1001<<TIM_CCMR1_IC2F_Pos); //filter 64 pulse
  TIM1->CCER |= (0b01<<TIM_CCER_CC2P_Pos)|TIM_CCER_CC2E;
  TIM1->DIER |= TIM_DIER_CC2IE;
  NVIC_EnableIRQ(TIM1_CC_IRQn);

}

void TIM2_Init(void){
  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
  TIM2->PSC = 41;
  TIM2->CR1 |= TIM_CR1_URS;
  TIM2->CR1 &= ~(TIM_CR1_CMS|TIM_CR1_DIR);
  TIM2->ARR = 1999;
  TIM2->CR2 |= (0b010<<TIM_CR2_MMS_Pos); //master
}

void key_led_Init(void){
  // Clock BUS Initial
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;
  RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
  SYSCFG->EXTICR[2] |= SYSCFG_EXTICR3_EXTI10_PE;
  SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI12_PE;
  //GPIO 
  GPIOE->MODER |= GPIO_MODER_MODE13_0|GPIO_MODER_MODE14_0|GPIO_MODER_MODE15_0;
  GPIOE->MODER &= ~(GPIO_MODER_MODE10|GPIO_MODER_MODE12);
  //Interrupts keys
  EXTI->PR |= EXTI_PR_PR10|EXTI_PR_PR12;
  EXTI->FTSR |= EXTI_FTSR_TR10|EXTI_FTSR_TR12;
  EXTI->IMR |= EXTI_IMR_IM10|EXTI_IMR_IM12;
  //Interrupt NVIC Enable
  NVIC_EnableIRQ(EXTI15_10_IRQn);
  //LED turn off
  GPIOE->BSRR |= GPIO_BSRR_BS13|GPIO_BSRR_BS14|GPIO_BSRR_BS15;
}
/*************************** End of file ****************************/
