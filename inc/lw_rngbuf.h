/*
 * lw_rngbuf.h
 *
 *  Created on: 08-Dec-2020
 *      Author: ABC
 */

#ifndef LW_RNGBUF_H_
#define LW_RNGBUF_H_


/************************** PRIVATE DEFINTIONS *************************/
/* buffer size definition */
#define LW_RING_BUFSIZE 256

/** Type of data to store into the buffer. */
#define RingBuff_Data_t        uint8_t

/* Buf mask */
#define __BUF_MASK (LW_RING_BUFSIZE-1)
/* Check buf is full or not */
#define __BUF_IS_FULL(head, tail) ((tail&__BUF_MASK)==((head+1)&__BUF_MASK))
/* Check buf will be full in next receiving or not */
#define __BUF_WILL_FULL(head, tail) ((tail&__BUF_MASK)==((head+2)&__BUF_MASK))
/* Check buf is empty */
#define __BUF_IS_EMPTY(head, tail) ((head&__BUF_MASK)==(tail&__BUF_MASK))
/* get buf count */
#define __BUF_COUNT(head, tail) ((head&__BUF_MASK)-(tail&__BUF_MASK))
/* Reset buf */
#define __BUF_RESET(bufidx)	(bufidx=0)
#define __BUF_INCR(bufidx)	(bufidx=(bufidx+1)&__BUF_MASK)

#define     __IO    volatile             /*!< Defines 'read / write' permissions              */
/************************** PRIVATE TYPES *************************/

/** @brief UART Ring buffer structure */
typedef struct
{
    __IO uint32_t tx_head;                /*!< UART Tx ring buffer head index */
    __IO uint32_t tx_tail;                /*!< UART Tx ring buffer tail index */
    __IO uint32_t rx_head;                /*!< UART Rx ring buffer head index */
    __IO uint32_t rx_tail;                /*!< UART Rx ring buffer tail index */
    __IO uint8_t  tx[LW_RING_BUFSIZE];  /*!< UART Tx data ring buffer */
    __IO uint8_t  rx[LW_RING_BUFSIZE];  /*!< UART Rx data ring buffer */
} LW_RING_BUFFER_T;

void fill_lw_rngbuf( LW_RING_BUFFER_T *ring, uint8_t *txbuf, uint8_t buflen);
void insert_lw_rngbuf(LW_RING_BUFFER_T *ring, uint8_t tmp);
uint8_t pop_lw_rngbuf(LW_RING_BUFFER_T *ring);


#endif /* LW_RNGBUF_H_ */
