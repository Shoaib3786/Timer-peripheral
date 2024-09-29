

#include <stdint.h>
#include "timer.h"
#include "stdio.h"

#if !defined(__SOFT_FP__) && defined(__ARM_FP)
  #warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif


void GPIO_init(void);
void delayTIM4(uint16_t);


int main(void)
{

	GPIO_init();

	while(1){

		GPIOA->ODR |= 1<<5;			// SET output data register at pin-5.
		delayTIM4(100); 			// Generate delay of 20ms

		GPIOA->ODR &= ~(1<<5);		// CLEAR output data register at pin-5.
		delayTIM4(100);
	}
}


void GPIO_init(void){

	RCC->AHB1ENR |= 1<<0;	// ENABLE AHB1 Bus clock

	GPIOA->MODER |= 1<<10;	// SET the moder as output
	GPIOA->PUPDR |= 0<<10;	// SET no push-pull

}



void delayTIM4(uint16_t delay){  			//parameter must be 16-bit, because the TIM4 is 16bit.

	//By Default; HSI is the source of SYSCLK, thus HCLK = 16MHz,
	//TIM4 Clock = APB1 Bus Clock.
	//Need to Replace Prescaler of APB1 Bus to 2 from (default 4) to produce APB1 TIMER Clock or (may be also PCLK) frequecy as 16MHz.
	/***[WE NEED APB1 Timer clock]***/
	RCC->CFGR |= 0<<10; 		//for both APB1 TIMER Clock + APB1 Peripheral clock to be 16Mhz.
	/*[or]*/
	//RCC->CFGR |= 4<<10; 		//for just APB1 TIMER Clock to be 16MHz.

	RCC->CFGR |= 0<<10;

	RCC->APB1ENR |= 1<<2; 		// ENABLE APB! bus Clock

	TIM4->PSC = 16000-1;		// SET PRE-SCALER (16-1) to bring 1us timeperiod 1us (if want delay in ms then PSC=16000-1  to produce freq 1KHz and time period as 1ms)

	TIM4->CNT = 0;				// SET counter

	if (delay<65535){  						// 16 bit -> 2^16 = 65536

		TIM4->ARR = (delay);				// SET Event reset/overflow value as user input delay but of 16bits
		TIM4->CR1 |= 1;  					// START Time

		while(!(TIM4->SR & TIM_SR_UIF)){} 	//Wait until Status Reg and UIF is True
		TIM4->SR &= ~(0x0001); 				//RESET Update flag in SR, once counter reached ARR.
	}
	else{
		printf("Insert delay from 0 to 65535");
	}

}






