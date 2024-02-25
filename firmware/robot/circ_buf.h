#ifndef CIRC_BUF_H
#define CIRC_BUF_H
#include "Arduino.h"

typedef struct circular_buffer
{
  uint8_t * buf;
  uint32_t head;
  uint32_t tail;
  const uint32_t maxLen;
}circ_buf_t;


int8_t append_byte(circ_buf_t * buf, uint8_t val)
{
  uint32_t next;

  next = buf->head + 1;
  if (next >= buf->maxLen)
  {
    next = 0;
  }
  if (next == buf->tail)
  {
    return -1; // report that the buffer is full;.
  }

  buf->buf[buf->head] = val;
  buf->head = next;

  return 0;
}

uint32_t append_buffer(circ_buf_t * buf, uint8_t * bytes, uint32_t len)
{
  uint32_t retval = 0;

  for (int i = 0; i < len; i++)
  {
    retval =+ append_byte(buf, bytes[i]);
  }
  return retval;
}

uint8_t pop_byte(circ_buf_t * buf, uint8_t * data)
{
  uint32_t next;

  next = buf->tail + 1;
  if (next >= buf->maxLen)
  {
    next = 0;
  }
  if (next == buf->head)
  {
    return -1; // report that the buffer is full;.
  }

  *data = buf->buf[buf->tail];
  buf->tail = next;

  return 0;
}

uint32_t pop_bytes(circ_buf_t * buf, uint8_t * data, uint32_t len)
{
  uint32_t retval = 0;
  for (int i = 0; i < len; i++)
  {
    retval =+ pop_byte(buf, &data[i]);
  }
  return retval;
}

#endif