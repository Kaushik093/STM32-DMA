#include <stdio.h>
#include <uart_dma.h>

#define GPIOAEN		(1U<<0)
#define GPIOA_5		(1U<<5)
#define LED_PIN		GPIOA_5

//Interrupt action
static void DMA_callback(){

	GPIOA->ODR = LED_PIN;

}

//Interrupt handler
void DMA1_Stream6_IRQHandler(void) {


	//Check for transfer complete interrupt
	if(DMA1->HISR & HISR_TCIF6){

		//Clear interrupt flag
		DMA1->HIFCR |= HIFCR_CTCIF6;

		//Do something
		DMA_callback();
	}

}


int main(void){

	/**Initialize user LED GPIO**/

	//Enable clock access to GPIOA
	RCC->AHB1ENR |= GPIOAEN;

	//Set PA5 to output mode
	GPIOA->MODER |= (1U<<10);
	GPIOA->MODER &= ~(1U<<11);

	uart2_tx_init();

	char message[31]= "Hello I am Kaushik";

	dma1_stream6_init((uint32_t) message, (uint32_t) &USART2->DR,31);

	while(1){


	}
}
