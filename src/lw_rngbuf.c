/*
 * lw_rngbuf.c
 *
 *  Created on: 28-Mar-2021
 *      Author: ABC
 */

#include <msp430.h>
#include <stdint.h>
#include "inc/lw_rngbuf.h"

/*********************************************************************//**
 * @brief		UART transmit function for interrupt mode (using ring buffers)

 * @param[out]  ring buffer pointer
 * @param[in]	txbuf Pointer to Transmit buffer
 * @param[in]	buflen Length of Transmit buffer
 * @return 		Number of bytes actually sent to the ring buffer
 **********************************************************************/
void fill_lw_rngbuf( LW_RING_BUFFER_T *ring, uint8_t *txbuf, uint8_t buflen)
{
	uint8_t *data = (uint8_t *) txbuf;
	uint16_t bytes = 0;
	/* Loop until transmit run buffer is full or until n_bytes
	   expires */
	while ((buflen > 0) && (!__BUF_IS_FULL(ring->tx_head, ring->tx_tail)))
	{
		/* Write data from buffer into ring buffer */
		ring->tx[ring->tx_head] = *data;
		data++;

		/* Increment head pointer */
		__BUF_INCR(ring->tx_head);

		/* Increment data count and decrement buffer size count */
		bytes++;
		buflen--;
	}
}


void insert_lw_rngbuf(LW_RING_BUFFER_T *ring, uint8_t tmp)
{
	if (!__BUF_IS_FULL(ring->rx_head,ring->rx_tail)){
		ring->rx[ring->rx_head] = tmp;
		__BUF_INCR(ring->rx_head);
	}
}


uint8_t pop_lw_rngbuf(LW_RING_BUFFER_T *ring)
{
	char temp = 0;
	if(__BUF_COUNT(ring->rx_head, ring->rx_tail) > 0)
	{
		temp = ring->rx[ring->rx_tail];
		__BUF_INCR(ring->rx_tail);
	}
	return temp;
}
