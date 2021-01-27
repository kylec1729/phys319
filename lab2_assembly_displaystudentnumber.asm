.include "msp430g2553.inc"
org 0xc000
START:                              ;This is a label we'll use later
                                    ;to refer to this position
    mov #0x0400, SP                 ;Initialize the stack pointer
    mov.w #WDTPW|WDTHOLD, &WDTCTL   ;Disable the watchdog, the symbol |
                                    ;indicates that values of WDTPW and WDTHOLD are bitwise OR'd
    mov.b #11110111b, &P1DIR        ;Set all Port 1 pins, except 1.3 as output

    ;Your code here

    ;Look back at the steps you followed to write to the display by hand.
    ;Convert these steps to assembly to complete the task.

    mov.b #00000000b, &P1OUT        ;Initialize P1OUT.
    mov.b #10100110b, &P1OUT
    mov.b #10100111b, &P1OUT        ;5 on display 1.
    mov.b #10110100b, &P1OUT
    mov.b #10110101b, &P1OUT        ;4 on display 1.
    mov.b #11110010b, &P1OUT
    mov.b #11110011b, &P1OUT        ;0 on display 3.
    mov.b #11110000b, &P1OUT
    mov.b #11110001b, &P1OUT        ;0 on display 4.

    bis.w #CPUOFF,SR                ;Disable the CPU (end program)
org 0xfffe                          ;Load the reset vector with the
     dw START                       ;location of the program start
                                    ;after power up or reset.