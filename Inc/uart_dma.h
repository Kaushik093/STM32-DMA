#ifndef UART_DMA_H_
#define UART_DMA_H_

#include "stm32f4xx.h"
#include <stdint.h>
#include <stdio.h>

#define HISR_TCIF6		(1U<<21)
#define HIFCR_CTCIF6 	(1U<<21)

void uart2_tx_init(void);
void dma1_stream6_init(uint32_t src, uint32_t dst, uint32_t len);

#endif /* UART_DMA_H_ */
