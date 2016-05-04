/*	Name & E-mail: Manisha Yeramareddy, myera001@ucr.edu
 *	Lab Section: 21
 *	Assignment: Custom Lab
 *	Exercise Description: Contains the function programmed onto the 
 *	other microcontroller which displays the LED’s using USART.
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */

#include <avr/io.h>
#include <util/delay.h>
#include "timer.c"
#include "usart.c"

int main(void)
{
	DDRB = 0xFF; PORTB = 0x00; // PORTB set to output - LED's
	DDRD = 0xFE; PORTD = 0x01; // PORTD - data transfer

	const unsigned long timerPeriod = 100;
	unsigned char got_data = 0;

	TimerSet(timerPeriod);
	TimerOn();
	initUSART();

	while(1)
	{
		if( USART_HasReceived() ) //checks if USART has received data
		{
			got_data = USART_Receive();
			PORTB = got_data;	
		} 

		while (!TimerFlag){}   // Wait for timer period
		TimerFlag = 0;         // Lower flag raised by timer
	}
}