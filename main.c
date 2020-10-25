#include "state.h"
#include "stm32l476xx.h"
#include "SysClock.h"
#include "SysTimer.h"
#include "LED.h"

theSTATE = -1;
char data[5] = {1,0,1,1,1};
char currentBit = 0;

int main() {
	System_Clock_Init(); // Switch System Clock = 80 MHz
	TIM2_Init();

	LED_Init();
	EXTI_Init();
	SysTick_Init();

	while(1){
		switch(theSTATE){
		case STANDBY:
			break;
		case DATA_SENT:
			currentBit = 0;
			Green_LED_Off();
			theSTATE = STANDBY;
			break;
		default:
			break;
		}
	}
}

void TIM2_Init(void){
    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN; // Enable TIMER 2 clock
	TIM2->CR1  &= ~TIM_CR1_DIR;           // Counting direction: 0 = up-counting, 1 = down-counting
    TIM2->PSC 	= 7999;                   // Prescaler < 65536.  Prescaler = 7999, timer clock 80MHz/(7999 + 1) = 10 KHz
    TIM2->ARR   = 10000 - 1;              // Auto-reload: Upcouting (0..ARR), Downcouting (ARR..0)
    TIM2->DIER |= TIM_DIER_UIE;           // Enable UIE interrupts
	TIM2->CR1  |= TIM_CR1_CEN;            // Enable counter

	NVIC_EnableIRQ(TIM2_IRQn);
	NVIC_SetPriority(TIM2_IRQn, 2);
}

void TIM2_IRQHandler(void){
	  if ((theSTATE==SEND_DATA) & (TIM2->SR & TIM_SR_UIF) != 0) {
		    if(currentBit==0)
		    {
		    	delay(2000); // allow for hardware to stabalize
		    }

		  	if(currentBit>=5)
		    {
		    	theSTATE = DATA_SENT;
		    }else{
		    	sendData(data[currentBit]);
		    	currentBit++;
		    }
			TIM2->SR &= ~TIM_SR_UIF; // Clear UIF flag
		}
}

void sendData(char x){
	if(x==1)
	{
		Green_LED_On();
	}
	else
	{
		Green_LED_Off();
	}
}
