/*
 * Copyright (c) 2016 Zibin Zheng <znbin@qq.com>
 * All rights reserved
 */

#ifndef _MULTI_TIMER_H_
#define _MULTI_TIMER_H_

#include "public.h"


typedef struct Timer
{
    uint32_t timeout;
    uint32_t repeat;
    void (*timeout_cb)(void);
    struct Timer* next;
} Timer;

#ifdef __cplusplus
extern "C" {
#endif

void timer_init(struct Timer* handle, void(*timeout_cb)(), uint32_t timeout, uint32_t repeat);
int  timer_start(struct Timer* handle);
void timer_stop(struct Timer* handle);
void timer_ticks(void);
void timer_loop(void);


#ifdef __cplusplus
}
#endif

#endif
