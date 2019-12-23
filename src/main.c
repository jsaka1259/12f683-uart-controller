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

#ifdef DEBUG
  i2c_init();
  st7032i_init();
  st7032i_clear();
  st7032i_cmd(0x80);
  st7032i_puts("UART:");
#else
  uart_init();
#endif

  while(1) {
    SL = 0;
    SL = 1;

    c = 0;
    c = c | ((uint8_t)MISO << 7);
    for (i = 6; i >= 0; i--) {
      SCK = 1;
      SCK = 0;
      c = c | (MISO << i);
    }

    if (SW == 0) {
#ifdef DEBUG
      st7032i_cmd(0x86);
      st7032i_puts(itos(buf, c, 16, 2, '0'));
#else
      uart_putc(c);
#endif
      while (SW == 0);
      __delay_ms(10);
    }
  }
}
