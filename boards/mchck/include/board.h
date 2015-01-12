/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    board_mchck mchck
 * @ingroup     boards
 * @brief       Board specific files for the mchck board
 * @{
 *
 * @file
 * @brief       Board specific definitions for the mchck board
 *
 * @author      Finn Wilke <finn.wilke@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef __BOARD_H
#define __BOARD_H

#include <periph/gpio.h>

#include "cpu.h"
#include "periph_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name LED GPIO pin
 */
#define LED_GPIO GPIO_26

/**
 * Define the nominal CPU core clock in this board
 */
#define F_CPU               CLOCK_CORECLOCK // typically ~48 Mhz

/**
 * @name Define UART device and baudrate for stdio
 * @{
 */
#define STDIO               UART_0
#define STDIO_BAUDRATE      (115200U)
#define STDIO_RX_BUFSIZE    (64U)
/** @} */

/**
 * @name Define LED commands for compatability to other boards
 * @{
 */
#define LED_GREEN_ON        PTB->PSOR = (1 << 16);
#define LED_GREEN_OFF       PTB->PCOR = (1 << 16);
#define LED_GREEN_TOGGLE    PTB->PTOR |= (1 << 16);

/* We don't have a second LED :( */
#define LED_RED_ON
#define LED_RED_OFF
#define LED_RED_TOGGLE
/** @} */

/**
 * @brief Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /** __BOARD_H */
/** @} */