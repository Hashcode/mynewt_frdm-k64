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
#include <sys/types.h>
#include <stdio.h>
#include <hal/flash_map.h>

#include "mcu/fsl_device_registers.h"
#include "mcu/frdm-k64f_bsp.h"
#include "mcu/fsl_common.h"
#include "mcu/fsl_clock.h"
#include "mcu/fsl_port.h"
#include "mcu/fsl_debug_console.h"

#include "clock_config.h"

/* The UART to use for debug messages. */
#define BOARD_DEBUG_UART_TYPE DEBUG_CONSOLE_DEVICE_TYPE_UART
#define BOARD_DEBUG_UART_BASEADDR (uint32_t) UART0
#define BOARD_DEBUG_UART_CLKSRC SYS_CLK
#define BOARD_DEBUG_UART_CLK_FREQ CLOCK_GetCoreSysClkFreq()
#define BOARD_UART_IRQ UART0_RX_TX_IRQn
#define BOARD_UART_IRQ_HANDLER UART0_RX_TX_IRQHandler

#ifndef BOARD_DEBUG_UART_BAUDRATE
#define BOARD_DEBUG_UART_BAUDRATE 115200
#endif /* BOARD_DEBUG_UART_BAUDRATE */

void *_sbrk(int incr);
void _close(int fd);

/*
 * XXXX for now have it here.
 */
static struct flash_area bsp_flash_areas[] = {
    [FLASH_AREA_BOOTLOADER] = {
        .fa_flash_id = 0,       /* internal flash */
        .fa_off = 0x00000000,   /* beginning */
        .fa_size = (40 * 1024)
    },
    [FLASH_AREA_IMAGE_0] = {
        .fa_flash_id = 0,
        .fa_off = 0x0000A000,
        .fa_size = (484 * 1024)
    },
    [FLASH_AREA_IMAGE_1] = {
        .fa_flash_id = 0,
        .fa_off = 0x00083000,
        .fa_size = (484 * 1024)
    },
    [FLASH_AREA_IMAGE_SCRATCH] = {
        .fa_flash_id = 0,
        .fa_off = 0x000fc000,
        .fa_size = (8 * 1024)
    },
    [FLASH_AREA_NFFS] = {
        .fa_flash_id = 0,
        .fa_off = 0x000fe000,
        .fa_size = (8 * 1024)
    }
};

int
bsp_imgr_current_slot(void)
{
    return FLASH_AREA_IMAGE_0;
}

static void init_hardware(void)
{
    // Disable the MPU otherwise USB cannot access the bus
    MPU->CESR = 0;

    // Enable all the ports
    SIM->SCGC5 |= (SIM_SCGC5_PORTA_MASK | SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTC_MASK | SIM_SCGC5_PORTD_MASK |
                   SIM_SCGC5_PORTE_MASK);
}

void BOARD_InitPins(void)
{
    /* Debug uart port mux config */
    CLOCK_EnableClock(kCLOCK_PortD);
    PORT_SetPinMux(PORTD, 6U, kPORT_MuxAlt5);
    PORT_SetPinMux(PORTD, 7U, kPORT_MuxAlt3);

    /* Led pin mux Configuration */
    PORT_SetPinMux(PORTB, 22U, kPORT_MuxAsGpio);
}

/* Initialize debug console. */
void BOARD_InitDebugConsole(void)
{
    uint32_t uartClkSrcFreq = BOARD_DEBUG_UART_CLK_FREQ;
    DbgConsole_Init(BOARD_DEBUG_UART_BASEADDR, BOARD_DEBUG_UART_BAUDRATE, BOARD_DEBUG_UART_TYPE, uartClkSrcFreq);
}

extern void BOARD_BootClockRUN(void);

void
os_bsp_init(void)
{
    // Init pinmux and other hardware setup.
    init_hardware();

    BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();

    DEBUG_PRINTF("%s: POST BOARD_InitDebugConsole\r\n", __func__);

    /*
     * XXX these references are here to keep the functions in for libc to find.
     */
    _sbrk(0);
    _close(0);

    flash_area_init(bsp_flash_areas,
      sizeof(bsp_flash_areas) / sizeof(bsp_flash_areas[0]));
    DEBUG_PRINTF("%s: EXIT\r\n", __func__);
}
