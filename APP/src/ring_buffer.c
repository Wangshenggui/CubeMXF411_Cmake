/*
 * ring_buffer.c
 *
 *  Created on: Jun 22, 2025
 *      Author: 10534
 */
#include "ring_buffer.h"

void RingBuffer_Init(RingBuffer *rb) {
    rb->head = 0;
    rb->tail = 0;
    rb->size = 0;
}

bool RingBuffer_IsEmpty(RingBuffer *rb) {
    return rb->size == 0;
}

bool RingBuffer_IsFull(RingBuffer *rb) {
    return rb->size == RING_BUFFER_SIZE;
}

bool RingBuffer_Put(RingBuffer *rb, uint8_t data) {
    if (RingBuffer_IsFull(rb)) {
        return false;
    }
    rb->buffer[rb->head] = data;
    rb->head = (rb->head + 1) % RING_BUFFER_SIZE;
    rb->size++;
    return true;
}

bool RingBuffer_Get(RingBuffer *rb, uint8_t *data) {
    if (RingBuffer_IsEmpty(rb)) {
        return false;
    }
    *data = rb->buffer[rb->tail];
    rb->tail = (rb->tail + 1) % RING_BUFFER_SIZE;
    rb->size--;
    return true;
}
