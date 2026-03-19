/*
 * ring_buffer.h
 *
 *  Created on: Jun 22, 2025
 *      Author: 10534
 */

#ifndef RING_BUFFER_H_
#define RING_BUFFER_H_

#include <stdint.h>
#include <stdbool.h>

#define RING_BUFFER_SIZE 4096

typedef struct {
    uint8_t buffer[RING_BUFFER_SIZE];
    uint16_t head;
    uint16_t tail;
    uint16_t size;
} RingBuffer;

void RingBuffer_Init(RingBuffer *rb);
bool RingBuffer_IsEmpty(RingBuffer *rb);
bool RingBuffer_IsFull(RingBuffer *rb);
bool RingBuffer_Put(RingBuffer *rb, uint8_t data);
bool RingBuffer_Get(RingBuffer *rb, uint8_t *data);

#endif // RING_BUFFER_H

