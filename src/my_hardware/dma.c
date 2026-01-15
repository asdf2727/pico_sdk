#include "dma.h"

#include <stdio.h>
#include <stdlib.h>

#include <hardware/regs/dma.h>
#include <hardware/structs/dma.h>

#include "utils.h"

#include "../logging.h"

struct dma_pm_t {
	dma_channel_hw_t *channel;
	void *ring;
};

dma_pm_t *dma_pm_create(void *from, int log_count, int log_bytes, int treq) {
	dma_pm_t *dma = malloc(sizeof(dma_pm_t));
	
	dma->channel = NULL;
	int i;
	for (i = 0; i < sizeof(dma_hw->ch) / sizeof(dma_channel_hw_t); i++) {
		if (REG_GET(dma_hw->ch[i].al1_ctrl, DMA_CH0_CTRL_TRIG_EN)) continue;
		dma->channel = &dma_hw->ch[i];
		break;
	}
	if (dma->channel == NULL) return NULL;

	dma->ring = aligned_alloc(1u << (log_bytes + log_count), 1u << (log_bytes + log_count));

	dma->channel->read_addr = (uint32_t)from;
	dma->channel->write_addr = (uint32_t)dma->ring;
	dma->channel->transfer_count = 1u << log_count;

	uint32_t reg = 0;
	reg |= REG_VAL(DMA_CH0_CTRL_TRIG_TREQ_SEL, treq);
	reg |= REG_VAL(DMA_CH0_CTRL_TRIG_CHAIN_TO, i);
	reg |= REG_VAL(DMA_CH0_CTRL_TRIG_RING_SEL, 1);
	reg |= REG_VAL(DMA_CH0_CTRL_TRIG_RING_SIZE, log_count + log_bytes);
	reg |= REG_VAL(DMA_CH0_CTRL_TRIG_INCR_WRITE, 0);
	reg |= REG_VAL(DMA_CH0_CTRL_TRIG_INCR_READ, 0);
	reg |= REG_VAL(DMA_CH0_CTRL_TRIG_DATA_SIZE, log_bytes);
	reg |= REG_VAL(DMA_CH0_CTRL_TRIG_EN, 1);
	dma->channel->ctrl_trig = reg;
	return dma;
}

void dma_pm_delete(dma_pm_t *dma) {
	if (dma == NULL) return;
	while (REG_GET(dma->channel->al1_ctrl, DMA_CH0_CTRL_TRIG_BUSY)) {}
	dma->channel->al1_ctrl = 0;
	free(dma->ring);
	free(dma);
}

void *dma_pm_value(dma_pm_t *dma, uint32_t offset) {
	if (dma == NULL) {
		log("DMA is null!");
		return NULL;
	}
	//while (REG_GET(dma->channel->al1_ctrl, DMA_CH0_CTRL_TRIG_BUSY)) {
	//	logf("%u %u %u", dma->channel->transfer_count,
	//		REG_GET(*(uint32_t *)((void *)dma + 0x800), DMA_CH0_DBG_CTDREQ),
	//		REG_GET(*(uint32_t *)((void *)dma + 0x804), DMA_CH0_DBG_TCR));
	//}
	logf("%x %x", dma->channel->transfer_count, (void *)dma->channel->write_addr - dma->ring);
	return dma->ring + offset;
}
