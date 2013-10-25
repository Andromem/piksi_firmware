/*
 * Copyright (C) 2011 Fergus Noble <fergusnoble@gmail.com>
 * Copyright (C) 2012 Colin Beighley <colin@swift-nav.com>
 *
 */

#include <stdio.h>
#include <libopencm3/stm32/f4/rcc.h>
#include <libopencm3/stm32/f4/flash.h>
#include <libopencm3/stm32/f4/gpio.h>

#include "main.h"
#include "sbp.h"
#include "acq.h"
#include "board/leds.h"
#include "board/m25_flash.h"
#include "board/nap/nap_common.h"
#include "peripherals/spi.h"
#include "peripherals/stm_flash.h"

int main(void)
{

  led_setup();
  led_on(LED_GREEN);
  led_on(LED_RED);

  /* Setup and hold the FPGA PROGRAM_B line low so that the FPGA does not
   * contest the flash SPI bus */
  nap_conf_b_setup();
  nap_conf_b_clear();

  spi_setup();
  sbp_setup(0);
  m25_register_callbacks();
  register_stm_flash_callbacks();

  for (u32 i = 0; i < 600000; i++)
    __asm__("nop");

  printf("\n\nFirmware info - git: " GIT_VERSION ", built: " __DATE__ " " __TIME__ "\n");
  printf("--- M25 FLASH TEST ---\n");

  while (1) {
    DO_EVERY(300,
      led_toggle(LED_GREEN);
      led_toggle(LED_RED);
    );
    sbp_process_messages();
  }

  return 0;
}
