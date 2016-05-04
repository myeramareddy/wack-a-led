/*	Name & E-mail: Manisha Yeramareddy, myera001@ucr.edu
 *	Lab Section: 21
 *	Assignment: Custom Lab
 *	Exercise Description: 
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
 
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <avr/eeprom.h>
#include "timer.c"
#include "io.c"
#include "states.c"
#include "bit.c"
#include "keypad.c"
#include "usart.c"

void Custombuild(unsigned char location, unsigned char *shape)
{
	unsigned char i;
	if(location<8)
	{
		LCD_WriteCommand(0x40+(location*8));
		for(i=0;i<8;i++)
		{
			LCD_WriteData(shape[i]);
		}
	}

}

unsigned char instrCount = 0;
unsigned char molesPerSpeed = 3;
unsigned char numMolesForThisSpeed = 0;
unsigned char speedOfLEDMoles = 80; //start with 2 seconds
unsigned char previous_score = 0;

void restart_game()
{
	gameStarted = 0;
	instrCount = 120;
	start_instrState = ss;
	blinking_ledState = blinkss;
	comp_keyLEDState = compss;
	numMolesForThisSpeed = 0; previous_score = score;
	score = 0; lives = 2; missed_count = 0; speedOfLEDMoles = 80;
	sampled = 0; missed_sampled = 0; numMolesForThisSpeed = 0;
	streak = 0; dontHit_flag = 0; lives_streak = 0; hit_noHit = 0;
}

void adc_init()
{
	ADCSRA |= ((1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0)); //16Mhz/128 = 125Khz the ADC reference clock
	ADMUX |= (1<<REFS0); //Voltage reference from Avcc (5v)
	ADCSRA |= (1<<ADEN); //Turn on ADC
	ADCSRA |= (1<<ADSC); //Do an initial conversion because this one is the slowest and to ensure that everything is up and running
}


void main()
{
	DDRA = 0x00; PORTA = 0xFF; //INput - Photo-Resistors
	DDRB = 0xFF; PORTB = 0x00; //output - LCD Data
	DDRD = 0xFE; PORTD = 0x01; //output - LCD Control, data transfer
	DDRC = 0xF0; PORTC = 0x0F; //INput - Keypad

	unsigned char pad_press = 0;
	unsigned char blinkCount = 0;
	unsigned char compCount = 0;
	
	char your_score[10];

	unsigned char led[8]={0x04, 0x0E, 0x0E, 0x1F, 0x1F, 0x1f, 0x1F, 0x1F};
	Custombuild(1,led);
	unsigned char legs[8]={0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x02, 0x02};
	Custombuild(2,legs);
	unsigned char heart[8]={0x00, 0x1B, 0x1F, 0x1F, 0x1F, 0x0E, 0x04, 0x00};
	Custombuild(3,heart);
	unsigned char empty[8]={0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	Custombuild(4,empty);
	unsigned char trophy[8]={0x0E, 0x1F, 0x1F, 0x0E, 0x0E, 0x04, 0x0E, 0x1F};
	Custombuild(5,trophy);

	LCD_init();
	adc_init();
	initUSART();
	
	const unsigned char PeriodGCD = 25;
	TimerSet(PeriodGCD);
	TimerOn();

	start_instrState = ss;
	blinking_ledState = blinkss;
	comp_keyLEDState = compss;

	while(1)
	{
		if( !gameStarted && instrCount >= 120 ) //3 seconds
		{ //loop through instructions lost_flag=0;
			if(lost_flag == 1)
			{
				LCD_ClearScreen();
				sprintf(your_score,"%d", previous_score);
				LCD_DisplayString(1,"Final Score: "); LCD_DisplayString(14, your_score);
				LCD_DisplayString(17," Restarting Game");
				lost_flag = 0; instrCount = 0;
				//Calculate Top Scores
				SetTopScores(previous_score);
			}
			else
			{
				start_instr(pad_press);
				instrCount = 0;
			}
			
		}
		if(!gameStarted && pad_press == '*')
		{//starting game
			gameStarted = 1;
		}
		
		if(gameStarted && blinkCount >= speedOfLEDMoles)
		{ //blinking LED moles
			if(dontHit_flag == 1)
			{//missed, but they were supposed to
				missed_sampled = 0;
				LCD_ClearScreen();
				hit_noHit = 0;
			}
			else if(sampled == 0 && output != 0x00 && output != 0xFF && dontHit_flag != 1)
			{//if there is something on the output not sampled before it changes - missed
				missed_sampled = 1;
			}
			randBlink();
			if( USART_IsSendReady() )
			{
				USART_Send(output);
			}
			numMolesForThisSpeed += 1;
			if( (numMolesForThisSpeed >= molesPerSpeed) && speedOfLEDMoles >= 50)
			{
				speedOfLEDMoles -= 8; //lower by 200ms
				numMolesForThisSpeed = 0;
			}
			sampled = 0; missed_sampled = 0;
			blinkCount = 0;
		}
		
		pad_press = GetKeypadKey();
		
		if(gameStarted && compCount >= 1)
		{
			Key_LED_comp(pad_press);
			compCount = 0;
		}
		
		if( (gameStarted && pad_press == '#') || lost_flag==1)
		{ //reset the game if # in the middle of the game OR if game lost
			if( USART_IsSendReady() )
			{
				USART_Send(0x00);
			}
			restart_game();
		}
		
		//Pause/Start
		if(pad_press == 'C')
		{
			while(pad_press != 'D')
			{
				pad_press = GetKeypadKey();
			}
		}

		while(!TimerFlag){}
		TimerFlag = 0;
		instrCount++;
		blinkCount++;
		compCount++;
		timePassed += PeriodGCD;
	}
}