/*
 * bootloader for the Swift Navigation Piksi GPS Receiver
 *
 * Copyright (C) 2010 Gareth McMullin <gareth@blacksphere.co.nz>
 * Copyright (C) 2011 Piotr Esden-Tempski <piotr@esden.net>
 * Copyright (C) 2013 Swift Navigation Inc <www.swift-nav.com>
 *
 * Contact: Colin Beighley <colin@swift-nav.com>
 *
 * Based on luftboot, a bootloader for the Paparazzi UAV project.
 *
 * This source is subject to the license found in the file 'LICENSE' which must
 * be be distributed together with this source. All other rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 */

#include <stdio.h>
#include <libopencm3/cm3/scb.h>

#include "main.h"
#include "sbp.h"
#include "board/leds.h"
#include "peripherals/stm_flash.h"
#include "peripherals/usart.h"
#include "peripherals/spi.h"
#include "board/m25_flash.h"
#include "board/nap/nap_common.h"

#define APP_ADDRESS	0x08010000
#define STACK_ADDRESS 0x10010000

u8 pc_wants_bootload = 0;
u8 current_app_valid = 0;

void jump_to_app_callback(u8 buff[] __attribute__((unused)))
{
  /* Disable peripherals used in the bootloader */
  sbp_disable();
  spi_deactivate();
  nap_conf_b_set();
  /* Set vector table base address */
  SCB_VTOR = APP_ADDRESS & 0x1FFFFF00;
  /* Initialise master stack pointer */
  asm volatile ("msr msp, %0"::"g"(*(volatile u32*)APP_ADDRESS));
  /* Jump to application */
  (*(void(**)())(APP_ADDRESS + 4))();
}

void pc_wants_bootload_callback(u8 buff[] __attribute__((unused)))
{
  /* Disable FPGA configuration and set up SPI in case we want to flash M25 */
  nap_conf_b_setup();
  nap_conf_b_clear();
  spi_setup();
  m25_setup();
  pc_wants_bootload = 1;
}

int main(void)
{
  /* Force FPGA to reconfigure in case we've had a warm reset */
  nap_conf_b_setup();
  nap_conf_b_clear();
  nap_conf_b_set();

  /* Setup and turn on LEDs */
  led_setup();
  led_off(LED_GREEN);
  led_off(LED_RED);

  /* Setup UART and SBP interface for transmitting and receiving callbacks */
  sbp_setup(0);

  /* STM flash erase/write/read callbacks */
  register_stm_flash_callbacks();

  /* Add callback for jumping to application after bootloading is finished */
  static msg_callbacks_node_t jump_to_app_node;
  sbp_register_callback(MSG_BOOTLOADER_JUMP_TO_APP, &jump_to_app_callback,
                          &jump_to_app_node);

  /* Add callback for PC to tell bootloader it wants to load program */
  static msg_callbacks_node_t pc_wants_bootload_node;
  sbp_register_callback(MSG_BOOTLOADER_HANDSHAKE,&pc_wants_bootload_callback,
                          &pc_wants_bootload_node);

  /* Send message to PC indicating bootloader is ready to load program */
  sbp_send_msg(MSG_BOOTLOADER_HANDSHAKE,0,0);

  /* Is current application we are programmed with valid? Check this by seeing
   * if the first address of the application contains the correct stack address
   */
  current_app_valid = (*(volatile u32*)APP_ADDRESS == STACK_ADDRESS) ? 1:0;

  /*
   * Wait a bit for response from PC. If it doesn't respond by calling
   * pc_wants_bootload_callback and we have a valid application, then boot the
   * application.
   * TODO : might as well make this as long as FPGA takes to configure itself
   *        from the configuration flash, as it doesn't add to the startup time
   */
	for (u64 i=0; i<200000; i++){
    DO_EVERY(3000,
      led_toggle(LED_RED);
      sbp_send_msg(MSG_BOOTLOADER_HANDSHAKE,0,0);
    );
    sbp_process_messages(); /* to service pc_wants_bootload_callback */
    if (pc_wants_bootload) break;
  }
  led_off(LED_GREEN);
  led_off(LED_RED);
  if ((pc_wants_bootload) || !(current_app_valid)){
    /*
     * We expect PC application passing firmware data to call
     * jump_to_app_callback to break us out of this while loop after it has
     * finished sending flash programming callbacks
     */
    while(1){
      sbp_process_messages();
      DO_EVERY(3000,
        led_toggle(LED_GREEN);
        led_toggle(LED_RED);
        /*
         * In case PC application was started after we entered the loop. It is
         * expecting to get a bootloader handshake message before it will send
         * flash programming callbacks
         */
        DO_EVERY(10,
          sbp_send_msg(MSG_BOOTLOADER_HANDSHAKE,0,0);
        );
      );
    }
  }

  /* Looks like the PC didn't want to update - boot the existing application */
  jump_to_app_callback(NULL);

  return 0;
}
