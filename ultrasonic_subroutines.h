/*
/ Define subroutines necessary for main.
*/

// Include standard C libraries so we can actually write stuff with standard C.
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void init_calibrated_clock(void) {
  /* Use internally calibrated 1MHz clock: */
  BCSCTL1 = CALBC1_1MHZ; // Set range
  DCOCTL = CALDCO_1MHZ;
  BCSCTL2 &= ~(DIVS_3); // SMCLK = DCO = 1MHz
}

/* Setup TRIGGER and ECHO pins */
void init_ultrasonic_pins(void)
{
  P2DIR &= ~ECHO_PIN; // Set ECHO (P2.0) pin as INPUT
  P2DIR |= TRIG_PIN;  // Set TRIGGER (P2.1) pin as OUTPUT
  P2OUT &= ~TRIG_PIN; // Set TRIGGER (P2.1) pin to LOW
}

/* Setup UART */
void init_uart(void)
{

  P1DIR |= TXD;
  P1OUT |= TXD;

  /* P1.1 = RXD, P1.2=TXD and special function enable */
  P1SEL = RXD + TXD;
  P1SEL2 = RXD + TXD;

  UCA0CTL1 |= UCSSEL_2; // Use SMCLK - 1MHz clock
  UCA0BR0 = 104;        // Set baud rate to 9600 with 1MHz clock (Data Sheet 15.3.13)
  UCA0BR1 = 0;          // Set baud rate to 9600 with 1MHz clock
  UCA0MCTL = UCBRS0;    // Modulation UCBRSx = 1
  UCA0CTL1 &= ~UCSWRST; // Initialize USCI state machine - enable
}

void init_timer(void)
{
  TACCTL1 = CCIE + OUTMOD_3; // TACCTL1 Capture Compare
  TACTL |= TASSEL_2 + MC_2 + ID_0;
}

void reset_timer(void)
{
  TACTL |= TACLR;
}

void communicate_byte_uart(char value)
{
  while (!(IFG2 & UCA0TXIFG));
  // wait for TX buffer to be ready for new data
  // UCA0TXIFG register will be truthy when available to recieve new data to computer.
  UCA0TXBUF = value;
}

void communicate_string_uart(char *str)
{
  unsigned int i = 0;
  while (str[i] != '\0')
  {
    communicate_byte_uart(str[i++]);
  }
}

void communicate_long_uart(unsigned long l)
{
  char str[10];
  sprintf(str, "%ld\n", l);
  communicate_string_uart(str);
}

/* If Timer counts to zero before end_time recorded */
#if defined(__TI_COMPILER_VERSION__)
#pragma vector = TIMER0_A1_VECTOR
__interrupt void ta1_isr(void)
#else
void __attribute__((interrupt(TIMER0_A1_VECTOR))) ta1_isr(void)
#endif
{
  overflow_counter++;
  TACCTL1 &= ~CCIFG; // reset the interrupt flag
}

void wait_ms(unsigned int ms)
{
  unsigned int i;
  for (i = 0; i <= ms; i++)
  {
    __delay_cycles(1000); // Clock is ~1MHz so 1E3/1E6 = 1E-3 (1ms) seconds
  }
}