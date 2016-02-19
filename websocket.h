/******************************************************************************
* Copyright (C) 2008 - 2014 Andreas Öman
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to
* permit persons to whom the Software is furnished to do so, subject to
* the following conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
******************************************************************************/

#pragma once

#include <stdint.h>

struct htsbuf_queue;

typedef struct websocket_state {
  uint8_t opcode;
  int packet_size;
  uint8_t *packet;
} websocket_state_t;

void websocket_append_hdr(struct htsbuf_queue *q, int opcode, size_t len);

void websocket_free(websocket_state_t *state);

/**
 * Return-values
 *  0 - Not enough data in input buffer, call again when more is available
 *  1 - Fatal error, disconnect
 */
int websocket_parse(struct htsbuf_queue *q,
                    int (*cb)(void *opaque, int opcode,
                              uint8_t **data, int len),
                    void *opaque, websocket_state_t *state);
