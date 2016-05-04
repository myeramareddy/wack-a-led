/*	Name & E-mail: Manisha Yeramareddy, myera001@ucr.edu
 *	Lab Section: 21
 *	Assignment: Custom Lab
 *	Exercise Description: Contains the state machines for the game
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */

#include "bit.c"
#include <stdio.h>
#include <stdlib.h>

unsigned char gameStarted = 0;
unsigned short MAX_ADC_VAL = 512;
unsigned short adc_value = 0x0000; 
unsigned long timePassed = 0;

unsigned char highScore __attribute__((section (".eeprom"))) = 0x00;
unsigned char highScoreZero __attribute__((section (".eeprom"))) = 0x30;
unsigned char highScoreOne __attribute__((section (".eeprom"))) = 0x30;

unsigned char SecondhighScore __attribute__((section (".eeprom"))) = 0x00;
unsigned char SecondhighScoreZero __attribute__((section (".eeprom"))) = 0x30;
unsigned char SecondhighScoreOne __attribute__((section (".eeprom"))) = 0x30;

unsigned char ThirdhighScore __attribute__((section (".eeprom"))) = 0x00;
unsigned char ThirdhighScoreZero __attribute__((section (".eeprom"))) = 0x30;
unsigned char ThirdhighScoreOne __attribute__((section (".eeprom"))) = 0x30;

unsigned char getDigit(unsigned char place, unsigned char number)
{
	if(place == 0)
	{
		return (number % 10);
	}
	if(place == 1)
	{
		return (number / 10);
	}
}

void SetTopScores(unsigned char p)
{
	unsigned char zero = getDigit(0, p);
	unsigned char one = getDigit(1, p);
	if(p > eeprom_read_byte(&highScore) )
	{	
		eeprom_update_byte(&ThirdhighScoreZero, eeprom_read_byte(&SecondhighScoreZero) );
		eeprom_update_byte(&ThirdhighScoreOne, eeprom_read_byte(&SecondhighScoreOne) );
		eeprom_update_byte(&ThirdhighScore, eeprom_read_byte(&SecondhighScore) );
					
		eeprom_update_byte(&SecondhighScoreZero, eeprom_read_byte(&highScoreZero) );
		eeprom_update_byte(&SecondhighScoreOne, eeprom_read_byte(&highScoreOne) );
		eeprom_update_byte(&SecondhighScore, eeprom_read_byte(&highScore) );
					
		eeprom_write_byte(&highScoreZero, zero+'0');
		eeprom_write_byte(&highScoreOne, one+'0');
		eeprom_write_byte(&highScore, p);
	}
	else if( p > eeprom_read_byte(&SecondhighScore) && p != eeprom_read_byte(&highScore) )
	{
		eeprom_update_byte(&ThirdhighScoreZero, eeprom_read_byte(&SecondhighScoreZero) );
		eeprom_update_byte(&ThirdhighScoreOne, eeprom_read_byte(&SecondhighScoreOne) );
		eeprom_update_byte(&ThirdhighScore, eeprom_read_byte(&SecondhighScore) );
					
		eeprom_write_byte(&SecondhighScoreZero, zero+'0');
		eeprom_write_byte(&SecondhighScoreOne, one+'0');
		eeprom_write_byte(&SecondhighScore, p);
	}
	else if( p > eeprom_read_byte(&ThirdhighScore) && p != eeprom_read_byte(&SecondhighScore) 
			&& p != eeprom_read_byte(&highScore) )
	{
		eeprom_write_byte(&ThirdhighScoreZero, zero+'0');
		eeprom_write_byte(&ThirdhighScoreOne, one+'0');
		eeprom_write_byte(&ThirdhighScore, p);
	}
}

unsigned short read_adcFrom_channel(unsigned char channel)
{
	ADMUX &= 0xF0;                 //Clear the older channel that was read
	ADMUX |= channel;              //Defines the new ADC channel to be read
	ADCSRA |= (1<<ADSC);          //Starts a new conversion
	while(ADCSRA & (1<<ADSC));    //Wait until the conversion is done
	return ADC;                  //Returns the ADC value of the chosen channel
}

enum{ss, init, instrOne, instrTwo, instrThree, instrFour, begin} start_instrState;

void start_instr(unsigned char ch)
{
	switch(start_instrState)
	{ //transitions
		case ss:
			start_instrState = init;
			break;
		case init:
			if(ch == '*') 
			{
				start_instrState = begin;
			}
			else if(ch != '*')
			{
				start_instrState = instrOne;
			}
			break;
		case instrOne:
			if(ch == '*') 
			{
				start_instrState = begin;
			}
			else if(ch != '*') 
			{
				start_instrState = instrTwo;
			}
			break;
		case instrTwo:
			if(ch == '*') 
			{
				start_instrState = begin;
			}
			else if(ch != '*')
			{
				start_instrState = instrThree;
			}
			break;
		case instrThree:
			if(ch == '*') 
			{
				start_instrState = begin;
			}
			else if(ch != '*')
			{
				start_instrState = instrFour;
			}
			break;
		case instrFour:
			if(ch == '*') 
			{
				start_instrState = begin;
			}
			else if(ch != '*')
			{
				start_instrState = init;
			}
			break;
		case begin:
			//do nothing
			break;
		default:
			start_instrState = init;
			break;
		
	}
	
	switch(start_instrState)
	{ //actions
		case init:
			LCD_ClearScreen();
			LCD_WriteCommand(0x80); //location where the char is going to be written.
			LCD_WriteData(1); //write to the screen the char stored in location
			LCD_WriteCommand(0x8F);
			LCD_WriteData(1);
			LCD_WriteCommand(0xC0); 
			LCD_WriteData(2);
			LCD_WriteCommand(0xCF); 
			LCD_WriteData(2);
	
			LCD_DisplayString(3, " Welcome to");
			LCD_DisplayString(19, " Wack-a-LED");
			break;
		case instrOne:
			LCD_ClearScreen();
			LCD_DisplayString(1, "Hit blinking LED");
			LCD_DisplayString(17, "Unless you see..");
			break;
		case instrTwo:
			LCD_ClearScreen();
			LCD_DisplayString(4, " Don't hit");
			LCD_DisplayString(21, " LED #");
			break;
		case instrThree:
			LCD_ClearScreen();
			LCD_WriteCommand(0x80); //location where the char is going to be written.
			LCD_WriteData(5); //write to the screen the char stored in location
			LCD_WriteCommand(0x8F); 
			LCD_WriteData(5); 
			LCD_DisplayString(2, " Top 3 Scores");
			LCD_Cursor(20);
			LCD_WriteData( eeprom_read_byte(&highScoreZero) );
			LCD_Cursor(19);
			LCD_WriteData( eeprom_read_byte(&highScoreOne) );
			LCD_Cursor(25);
			LCD_WriteData( eeprom_read_byte(&SecondhighScoreZero) );
			LCD_Cursor(24);
			LCD_WriteData( eeprom_read_byte(&SecondhighScoreOne) );
			LCD_Cursor(30);
			LCD_WriteData( eeprom_read_byte(&ThirdhighScoreZero) );
			LCD_Cursor(29);
			LCD_WriteData( eeprom_read_byte(&ThirdhighScoreOne) );
			LCD_Cursor(32);
			break;
		case instrFour:
			LCD_ClearScreen();
			LCD_DisplayString(1, "Press * to Start");
			break;
		case begin:
			srand(timePassed);
			gameStarted = 1;
		default:
			break;
	}
}

enum{blinkss, init_blink, randLED} blinking_ledState;
unsigned char light; 
unsigned char output = 0x00;
unsigned char prev_rand = 0;
unsigned char curr_rand = 0;
unsigned char missed_sampled = 0;
unsigned char missed_count = 0;
unsigned char first_time = 0;
unsigned char lives = 2;
unsigned char score = 0;
unsigned char streak = 0;
unsigned char lives_streak = 0;
unsigned char lost_flag = 0;
char final_score[10];
char final_streak[10];
char final_missed[10];
unsigned char prob_20[5]={0,0,1,0,0};
unsigned char dontHit_flag = 0;
unsigned char hit_noHit = 0;

void randBlink()
{
	switch(blinking_ledState)
	{
		case blinkss:
			blinking_ledState = init_blink;
			break;
		case init_blink:
			blinking_ledState = randLED;
			break;
		case randLED:
			blinking_ledState = randLED;
			break;
		default:
			blinking_ledState = blinkss;
			break;
	}
	
	switch(blinking_ledState)
	{ //actions
		case blinkss:
			break;
		case init_blink:
			LCD_ClearScreen();
			LCD_DisplayString(1, "GAME Starting...");
			first_time = 1;
			output = 0xFF;
			break;
		case randLED:
			if(first_time == 1)
			{
				LCD_ClearScreen();
				sprintf(final_score,"%d", score);
				LCD_DisplayString(1,"Score: "); LCD_DisplayString(8, final_score);
				LCD_DisplayString(11," Lives");
				LCD_WriteCommand(0xCD); //location where the char is going to be written.
				LCD_WriteData(3); //write to the screen the char stored in location
				LCD_WriteCommand(0xCE); 
				LCD_WriteData(3); 
				first_time = 0;
			}
			else if(dontHit_flag == 1 && missed_sampled == 0)
			{
				sprintf(final_score,"%d", score);
				LCD_DisplayString(1,"Score: "); LCD_DisplayString(8, final_score);
				sprintf(final_streak,"%d", streak);
				LCD_DisplayString(17,"Streak: "); LCD_DisplayString(25, final_streak);
				LCD_DisplayString(11," Lives");
				if(lives == 3)
				{
					LCD_DisplayString(11," Lives"); 
					LCD_WriteCommand(0xCF); 
					LCD_WriteData(3); 
					LCD_WriteCommand(0xCE); 
					LCD_WriteData(3);
					LCD_WriteCommand(0xCD); 
					LCD_WriteData(3); 
				}
				else if(lives == 2)
				{
					LCD_DisplayString(11," Lives"); 
					LCD_WriteCommand(0xCE); 
					LCD_WriteData(3); 
					LCD_WriteCommand(0xCD); 
					LCD_WriteData(3); 
				}
				else if(lives == 1)
				{
					LCD_DisplayString(11," Lives"); 
					LCD_WriteCommand(0xCD); 
					LCD_WriteData(3); 
				}
				dontHit_flag = 0;
			}
			else if(missed_sampled == 1)
			{ //decrement lives and running streak
				streak = 0; lives_streak = 0;
				sprintf(final_streak,"%d", streak);
				LCD_DisplayString(17,"Streak: "); LCD_DisplayString(25, final_streak);
				LCD_WriteCommand(0xC9); 
				LCD_WriteData(4); 
				if(lives == 3)
				{
					LCD_DisplayString(11," Lives"); 
					LCD_WriteCommand(0xCF); 
					LCD_WriteData(4); 
				}
				else if(lives == 2)
				{
					LCD_DisplayString(11," Lives"); 
					LCD_WriteCommand(0xCE); 
					LCD_WriteData(4); 
				}
				else if(lives == 1)
				{
					LCD_DisplayString(11," Lives"); 
					LCD_WriteCommand(0xCD); 
					LCD_WriteData(4); 
				}
				else if(lives <= 0)
				{
					lost_flag = 1;
					break;
				}
				lives -= 1;
			}
			prev_rand = curr_rand;
			light = rand() / (RAND_MAX / 5 ); // rand # between 0-4 inclusive
			while(prev_rand == light)
			{ //recalculate if previous output is same as output now
				light = rand() / (RAND_MAX / 4 );
			}
			curr_rand = light;
			output = 0x00;
			output = SetBit(output, light, 1);
			hit_noHit = prob_20[rand()/(RAND_MAX/5)];  //rand # between 0-4 inclusive
			if(hit_noHit == 1)
			{
				LCD_ClearScreen(); dontHit_flag = 1;
				LCD_DisplayString(1, " **Don't hit**");
				LCD_DisplayString(21, " LED!");	
			}
			else if(hit_noHit == 0)
			{ //carry on as usual
			}
			break;
		default:
			output = 0x00;
			break;
	}
}

enum{compss, init_comp, compKeyLED} comp_keyLEDState;
unsigned char sampled = 0;
unsigned char removed = 0; //so that they dont just hover at all times
//unsigned char work = 0;

void comp_Display()
{
	score += 1; 
	sampled = 1;
	sprintf(final_score,"%d", score);
	LCD_DisplayString(1,"Score: "); LCD_DisplayString(8, final_score);
	streak += 1; lives_streak +=1;
	sprintf(final_streak,"%d", streak);
	LCD_DisplayString(17,"Streak: "); LCD_DisplayString(25, final_streak);
}

void Key_LED_comp(unsigned char inp)
{
	switch(comp_keyLEDState)
	{ //transitions
		case compss:
			comp_keyLEDState = init_comp;
			break;
		case init_comp:
			comp_keyLEDState = compKeyLED;
			break;
		case compKeyLED:
			comp_keyLEDState = compKeyLED;
			break;
		default:
			comp_keyLEDState = init_comp;
			break;
	}
	
	switch(comp_keyLEDState)
	{ //actions
		case compss:
			break;
		case init_comp:
			break;
		case compKeyLED:
			/*if(dontHit_flag == 1 && inp != '\0')
			{ //if input method is keypad
				lost_flag = 1;
				break;
			}*/
			if(dontHit_flag == 1 && output == 0x01 &&(read_adcFrom_channel(0) < 50) )
			{
				lost_flag = 1;
				break;
			}
			else if(dontHit_flag == 1 && output == 0x02 && (read_adcFrom_channel(2) < 50) )
			{
				lost_flag = 1;
				break;
			}
			else if(dontHit_flag == 1 && output == 0x04 && (read_adcFrom_channel(4) < 50) )
			{
				lost_flag = 1;
				break;
			}
			else if(dontHit_flag == 1 && output == 0x08 && (read_adcFrom_channel(1) < 50) )
			{
				lost_flag = 1;
				break;
			}
			else if(dontHit_flag == 1 && output == 0x10 && (read_adcFrom_channel(3) < 50) )
			{
				lost_flag = 1;
				break;
			}
			//else if(output == 0x01 && inp == '3' && sampled == 0)
			else if(output == 0x01 && (read_adcFrom_channel(0) < 50) && sampled == 0 &&
					(read_adcFrom_channel(1) > 75) && (read_adcFrom_channel(2) > 75) &&
					(read_adcFrom_channel(3) > 75) && (read_adcFrom_channel(4) > 75) )
			{
				comp_Display();
			}
			//else if(output == 0x02 && inp == '2' && sampled == 0)
			else if(output == 0x02 && (read_adcFrom_channel(2) < 50) && sampled == 0 &&
					(read_adcFrom_channel(0) > 65) && (read_adcFrom_channel(1) > 65) &&
					(read_adcFrom_channel(3) > 65) && (read_adcFrom_channel(4) > 65) )
			{
				comp_Display();
			}
			//else if(output == 0x04 && inp == '1' && sampled == 0)
			else if(output == 0x04 && (read_adcFrom_channel(4) < 50) && sampled == 0 &&
					(read_adcFrom_channel(0) > 75) && (read_adcFrom_channel(1) > 75) &&
					(read_adcFrom_channel(2) > 75) && (read_adcFrom_channel(3) > 75) )
			{
				comp_Display();
			}
			//else if(output == 0x08 && inp == '6' && sampled == 0)
			else if(output == 0x08 && (read_adcFrom_channel(1) < 50) && sampled == 0 &&
					(read_adcFrom_channel(0) > 75) && (read_adcFrom_channel(2) > 75) &&
					(read_adcFrom_channel(3) > 75) && (read_adcFrom_channel(4) > 75) )
			{
				comp_Display();
			}
			//else if(output == 0x10 && inp == '4' && sampled == 0)
			else if(output == 0x10 && (read_adcFrom_channel(3) < 50) && sampled == 0 &&
					(read_adcFrom_channel(0) > 75) && (read_adcFrom_channel(1) > 75) &&
					(read_adcFrom_channel(2) > 75) && (read_adcFrom_channel(4) > 75) )
			{
				comp_Display();
			}
			if(lives == 2 && lives_streak >= 10)
			{
				lives += 1; lives_streak = 0;
				LCD_WriteCommand(0xCF); //location where the char is going to be written.
				LCD_WriteData(3); //write to the screen the char stored in location
			}
			else if(lives == 1 && lives_streak >= 10)
			{
				lives += 1; lives_streak = 0;
				LCD_WriteCommand(0xCE); //location where the char is going to be written.
				LCD_WriteData(3); //write to the screen the char stored in location
			}
			break;
		default:
			score = 0;
			break;
	}
}










