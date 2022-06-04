#include "stdint.h"
#include "stdio.h"
#include "string.h"

#include "irq.h"


static volatile char * const TERMINAL_ADDR = (char * const)0x20000000;

static volatile uint32_t * const DMA_SRC_ADDR  = (uint32_t * const)0x70000000;
static volatile uint32_t * const DMA_DST_ADDR  = (uint32_t * const)0x70000004;
static volatile uint32_t * const DMA_LEN_ADDR  = (uint32_t * const)0x70000008;
static volatile uint32_t * const DMA_OP_ADDR   = (uint32_t * const)0x7000000C;
static volatile uint32_t * const DMA_STAT_ADDR = (uint32_t * const)0x70000010;

static const uint32_t DMA_OP_NOP = 0;
static const uint32_t DMA_OP_MEMCPY = 1;


_Bool dma_completed = 0;

void dma_irq_handler() {
	dma_completed = 1;
}

void timer_irq_handler() {
	//Do nothing
	//printf("1ms reached\n.");
}

int main() {
	register_interrupt_handler(4, dma_irq_handler);
    register_interrupt_handler(3, timer_irq_handler);
	
	uint8_t src[32] = { [1]=1, 2, 3, 4, [28]=28, 29, 30, 31};
	uint8_t dst[32] = { 0 };

      
    char tmp[20];
	for (int i = 0; i < 32; ++i) { 
		printf("src[%d]=%d\n ", i, src[i]);
	}

	dma_completed = 0;
	*DMA_SRC_ADDR = (uint32_t)(&src[0]);
	*DMA_DST_ADDR = (uint32_t)(&dst[0]);
	*DMA_LEN_ADDR = 32;
	*DMA_OP_ADDR  = DMA_OP_MEMCPY;
	
	while (!dma_completed) {
		asm volatile ("wfi");
	}
    dma_completed = 0;

	char buf[20];
	for (int i = 0; i < 32; ++i) {
		printf("dst[%d]=%d\n ", i, dst[i]);
	}

	return 0;
}
