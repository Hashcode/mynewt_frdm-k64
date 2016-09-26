/**
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include <assert.h>
#include <stdio.h>
#include <os/os.h>
#include <hal/hal_os_tick.h>

#include "mcu/fsl_debug_console.h"

/*
 * XXX implement tickless mode.
 */
void
os_tick_idle(os_time_t ticks)
{
    DEBUG_PRINTF("%s: ENTER\r\n", __func__);
    OS_ASSERT_CRITICAL();
    __DSB();
    __WFI();
    DEBUG_PRINTF("%s: EXIT\r\n", __func__);
}

void
os_tick_init(uint32_t os_ticks_per_sec, int prio)
{
    SysTick->LOAD = (os_ticks_per_sec & SysTick_LOAD_RELOAD_Msk) - 1; /* set reload register */
    SysTick->VAL = 0;                                                 /* Load the SysTick Counter Value */
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk |
                    SysTick_CTRL_ENABLE_Msk; /* Enable SysTick IRQ and SysTick Timer */

    /* Set the system tick priority */
    NVIC_SetPriority(SysTick_IRQn, prio);
}
