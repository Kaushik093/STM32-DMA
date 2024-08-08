#include <uart_dma.h>

#define UART2_CLK_EN        (1U<<17) //Enable clock for UART2 module
#define AHB1_CLK_EN			(1U<<0);

#define CR1_RE				(1U<<2)	 //Enable Receiver
#define CR1_TE				(1U<<3)  //Enable transmitter
#define CR1_UE				(1U<<13) //Enable USART

#define SYS_FREQ			16000000 //Default system frequency
#define APB1_CLK			SYS_FREQ //Verify and change code
#define BAUD_RATE			115200

#define DMA1EN		(1U<<21)
#define STR_DISABLE	(1U<<0)

#define CHSEL4		(1U<<27)
#define MEMINC		(1U<<10)
#define DMA_CR_TCIE	(1U<<4)
#define UART_DMA_TX	(1U<<7)

#define DMA_DIR_MEM_2_PERIPH	(1U<<6)


static uint16_t compute_divisor(uint32_t PeriphClk, uint32_t BaudRate){

	return ((PeriphClk + (BaudRate/2U))/BaudRate);
}

static void uart_set_baudrate(USART_TypeDef *USARTx, uint32_t PeriphClk, uint32_t BaudRate){

	USARTx->BRR = compute_divisor(PeriphClk,BaudRate);
}


void uart2_tx_init(void){

	/****Configure UART GPIO pin ****/

	//Enable clock access
	RCC->AHB1ENR |= AHB1_CLK_EN;

	//Set PA2 register mode to alternate function mode
	GPIOA->MODER &= ~(1U<<4);
	GPIOA->MODER |= (1U<<5);

	//Specify the alternate function for PA2 (Alternate function low register)
	GPIOA->AFR[0] |= (1U<<8);
	GPIOA->AFR[0] |= (1U<<9);
	GPIOA->AFR[0] |= (1U<<10);
	GPIOA->AFR[0] &= ~(1U<<11);

	/****Configure UART****/

	//Enable clock access to uart2
	RCC->APB1ENR |= UART2_CLK_EN;

	//Set baud rate
	uart_set_baudrate(USART2, APB1_CLK, BAUD_RATE);

	//Configure transfer direction (Enable transmitter and receiver)
	USART2->CR1 =  CR1_TE;

	//Enable UART module
	USART2->CR1 |= CR1_UE;
}


void dma1_stream6_init(uint32_t src, uint32_t dst, uint32_t len){

	//Enable clock access to DMA
	RCC->AHB1ENR |= DMA1EN;

	//Disable DMA1 Stream6
	DMA1_Stream6->CR &= ~STR_DISABLE;

	while(DMA1_Stream6->CR & STR_DISABLE){}

	//Clear all interrupt flags for stream6
	DMA1->HIFCR |= (1U<<16);
	DMA1->HIFCR |= (1U<<18);
	DMA1->HIFCR |= (1U<<19);
	DMA1->HIFCR |= (1U<<20);
	DMA1->HIFCR |= (1U<<21);

	//Set destination address
	DMA1_Stream6->PAR = dst;

	//Set source address
	DMA1_Stream6->M0AR = src;

	//Set length of data
	DMA1_Stream6->NDTR = len;

	//Select Stream6 CH4
	DMA1_Stream6->CR |= CHSEL4;

	//Enable memory increment (Try w/o enabling)
	DMA1_Stream6->CR |= MEMINC;

	//Enable transfer complete interrupt
	DMA1_Stream6->CR |= DMA_CR_TCIE;

	//Configure transfer direction (memory to peripheral
	DMA1_Stream6->CR |= DMA_DIR_MEM_2_PERIPH;

	//Enable Direct mode and disable FIFO
	DMA1_Stream6->FCR = 0;


	DMA1_Stream6->CR |= STR_DISABLE;//Enable DMA1 Stream6

	//Enable USART2 DMA Transmitter
	USART2->CR3 |= UART_DMA_TX;

	//Enable DMA interrupt in NVIC
	NVIC_EnableIRQ(DMA1_Stream6_IRQn);
}




