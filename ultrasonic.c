/*
/ Declare necessary header files.
/ Define trigger, echo pins.
*/

#include "msp430.h"

#define TRIG_PIN BIT1 // Corresponds to P2.1
#define ECHO_PIN BIT0 // Corresponds to P2.0

#define TXD BIT2 // TXD on P1.2 EnableCapture
#define RXD BIT1 // RXD on P1.1

volatile unsigned int overflow_counter = 0;

#include "ultrasonic_subroutines.h" // User-defined functions.

////////////////////////////////////////////////////////////////////////////////

/*
/ MAIN MODULE
*/

void main(void)
{

  WDTCTL = WDTPW + WDTHOLD;   // Stop Watch Dog Timer

  init_calibrated_clock();    // Initialize calibrated 1MHz clock.
  init_ultrasonic_pins();     // Initialize ECHO and TRIGGER pins.
  init_uart();                // Initialize the UART for serial communication.
  init_timer();               // Initialize the timer.

  // Global Interrupt Enable
  __enable_interrupt();

  unsigned int last_echo_value;
  unsigned int real_echo_value;
  unsigned int measurement;
  unsigned long start_time;
  unsigned long end_time;
  unsigned long distance;

  while (1)   // loop forever
  {
    P2OUT |= TRIG_PIN;  // Enable TRIGGER
    __delay_cycles(10); // Send pulse for 10us
    P2OUT &= ~TRIG_PIN; // Disable TRIGGER

    //enter while loop for measurement
    measurement = 1;

    while (measurement)   // loop while measuring
    {
      real_echo_value = P2IN & ECHO_PIN;    // ECHO_PIN is reference to if the ECHO is high,
                                            //  so therefore, if ECHO is high then real_echo_value
                                            //  is high since 1 & 1 = 1.
      // Rising edge
      if (real_echo_value > last_echo_value)  // If ECHO is high, start our stopwatch.
      {
        reset_timer();
        overflow_counter = 0;
        start_time = TAR;
      }
      // Falling edge
      else if (real_echo_value < last_echo_value)   // If ECHO is low, stop the stopwatch.
      {
        end_time = TAR;
        end_time += overflow_counter * 0xFFFF;      // Keeps count if timer goes to zero before
                                                     //   end_time is recorded, because TAR has
                                                     //   a memory allocation limit.
        distance = (unsigned long)((end_time - start_time) * 171.5);

        // HC-SR04 acceptable measure ranges only
        if (distance / 10000 >= 2.0 && distance / 10000 <= 400)
        {
          communicate_long_uart(distance);
        }

        measurement = 0;                              // If ECHO low, stop measuring.
      }
      last_echo_value = real_echo_value;    // The last ECHO value will be the last real ECHO value.
    }

    wait_ms(1000); // Wait a second before repeating measurement.
  }
}