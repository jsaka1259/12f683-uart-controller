#include <xc.h>
#include <stdint.h>

#include "common/common.h"

// CONFIG
#pragma config FOSC = INTOSCIO  // Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA4/OSC2/CLKOUT pin, I/O function on RA5/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // MCLR Pin Function Select bit (MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Detect (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)

#define SL   GP0
#define SCK  GP1
#define MISO GP2
#define SW   GP3

#define SW_ON 0x01

#define CHATT_CNT 10

uint8_t read_sw(void) {
  static uint8_t sw;
  static uint8_t sw_cnt[2];

  if ((sw & SW_ON) == 0) {
    if (SW == 0)
      sw_cnt[0]++;
    else
      sw_cnt[0] = 0;

    if (sw_cnt[0] > CHATT_CNT) {
      sw_cnt[0] = 0;
      sw |= SW_ON;
      return sw;
    }
  } else {
    if (SW == 1)
      sw_cnt[1]++;
    else
      sw_cnt[1] = 0;

    if (sw_cnt[1] > CHATT_CNT) {
      sw_cnt[1] = 0;
      sw &= ~SW_ON;
    }
  }

  return 0;
}

void main(void) {
  OSCCON = 0x70;
  GPIO   = 0x00;
  TRISIO = 0x0c;
  ANSEL  = 0x00;
  CMCON0 = 0x07;
  WPU    = 0x04;
  nGPPU  = 0;

  char buf[3] = {};
  uint8_t c;
  int8_t  i;
  uint8_t sw;

  uart_init();

  while(1) {
    SL = 0;
    SL = 1;

    c  = 0;
    c |= ((uint8_t)MISO << 7);
    for (i = 6; i >= 0; i--) {
      SCK = 1;
      SCK = 0;
      c  |= (MISO << i);
    }

    sw = read_sw();
    if (sw & SW_ON)
      uart_putc(c);
  }
}
