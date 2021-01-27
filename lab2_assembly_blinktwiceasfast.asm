.include "msp430g2553.inc"
org 0xc000
start:
		mov.w #WDTPW|WDTHOLD, &WDTCTL	;halt watchdog timer
		mov.b #0x41, &P1DIR				;set P1.0 and P1.6 as output (P1.0 and P1.6 are the LEDs)
		mov.w #0x01, r8					;set 1 in register 8
repeat:
		mov.b r8, &P1out
		xor.b #0x41, r8
		mov.w #30000, r9
waiter:
		dec r9
		jnz waiter
		jmp restart
org 0xfffe
		dw start