

#include <avr/io.h>
#include <avr/interrupt.h>
#include "io.c"


volatile unsigned char TimerFlag = 0;
unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

void TimerOn() {
	TCCR1B = 0x0B;
	OCR1A = 125;
	TIMSK1 = 0x02;
	TCNT1=0;
	_avr_timer_cntcurr = _avr_timer_M;
	SREG |= 0x80;
}
void TimerOff() {
	TCCR1B = 0x00;
}
void TimerISR() {
	TimerFlag = 1;
}
ISR(TIMER1_COMPA_vect) {
	_avr_timer_cntcurr--;
	if (_avr_timer_cntcurr == 0) {
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}
void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

void set_PWM(double frequency) {
	static double current_frequency;
	if (frequency != current_frequency) {
		if (!frequency) { TCCR3B &= 0x08; }
		else { TCCR3B |= 0x03; }
		if (frequency < 0.954) { OCR3A = 0xFFFF; }
		else if (frequency > 31250) { OCR3A = 0x0000; }
		else { OCR3A = (short)(8000000 / (128 * frequency)) - 1; }
		TCNT3 = 0;
		current_frequency = frequency;
	}
}
void PWM_on() {
	TCCR3A = (1 << COM3A0);
	TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);
	set_PWM(0);
}
void PWM_off() {
	TCCR3A = 0x00;
	TCCR3B = 0x00;
}

unsigned char song_num = 0x00;
const unsigned char max_songs = 0x04;
unsigned char old_songnum = 0x01;
unsigned char song_size = 0x5A;
unsigned char flaggor = 0x00;
unsigned char j= 0x00;
unsigned char displayonce = 0x00;
enum Menu_states {welcome, start, next, previous, select, pause_play}Menu_state;
void Jukebox (){
	const double A = 27.5;
	const double B = 30.87;
	const double C = 16.35;
	const double D = 18.35;
	const double E0 = 20.6;
	const double F = 21.83;
	const double G = 24.50;

	const double A1 = 55.0;
	const double B1 = 61.74;
	const double C1 = 32.7;
	const double D1 = 36.71;
	const double E1 = 41.20;
	const double F1 = 43.65;
	const double G1 = 49.00;

	const double A2 = 110.0;
	const double B2 = 123.5;
	const double C2 = 65.41;
	const double D2 = 73.42;
	const double E2 = 82.41;
	const double F2 = 87.31;
	const double G2 = 98;

	const double A3 = 220.0;
	const double B3 = 246.9;
	const double C3 = 130.8;
	const double D3 = 146.8;
	const double E3 = 164.8;
	const double F3 = 185;
	const double G3 = 196;


	const double A4 = 440.0;
	const double B4 = 493.9;
	const double C4 = 261.6;
	const double D4 = 293.7;
	const double E4 = 329.6;
	const double F4 = 349.2;
	const double G4 = 392;
	
	const double all_song[4][76] = {
		{C3,D3,E0,F3,G,A,B3, B,B3,B,B,A2,A2,A,A2,A4,C4,A2,C1,A2,B3,A2,D,B,A3,D3,E0,E0,F4,A4,F1,A2,C3,A4,D1,D,A2,D,B1,B,C2,D3,F,A,G1,F2,F,A1,A2,G,G},
		{ C,D,E0,F,G,A,B, B,B,B,B,A,A,A,A,A,C,A,C,A,B,A,D,B,A,D,E0,E0,F,A,F,A,C,A,D,D,A,D,B,B,A,D,F,A,G,F,F,A,A,G,G},
		{ A2, G2, F2, D2, B2, A2, G2, F2, D2, B2, A2, G2, F2, D2, B2, A2, G2, F2, D2, B2, A2, G2, F2, D2, B2, A2, G2, F2, D2, B2, A2, G2, F2, D2, B2, A2, G2, F2, D2, B2, A2, G2, F2, D2, B2, A2, G2, F2, D2},
		{C2, B2, A2, C2, B2,A2, G2, A2, B2,	C2, B2, G2, D2, C2, B2, D2, C2, B2, D2, C2, B2, A2, B2, C2, D2, C2, B2, D2, D2, C2, C2, B2, B2, A2, C4, B4, A4, C4, B4,A4, G4, A4, B4,C4, B4, G4, D4, C4, B4, D4, C4, B4, D4, C4, B4, A4, B4, C4, D4, C4, B4, D4, D4,D4, C4,C4, C4, B4,B4, B4, B3,B3,B2,B2,B1,B1,0}
	};
	unsigned char sizeof_songs[4]= {51, 51,49, 76};
	unsigned char tempA = ~PINA & 0x0F;
	
	switch(Menu_state){
		case welcome:
		if(tempA > 0x00){ Menu_state = start;}
		break;
		
		case start:
		flaggor = 0;
		j = 0x00;
		if(tempA == 0x00){ Menu_state = start;}
		else if(tempA == 0x01) {Menu_state = next;}
		else if(tempA == 0x02) {Menu_state = previous;}
		else if(tempA == 0x04) {Menu_state = select;}
		break;
		
		case next:
		Menu_state = start;
		break;
		
		case previous:
		Menu_state = start;
		break;
		
		case select:
		if(flaggor){Menu_state = start;}
		if(!flaggor && !(tempA == 0x04) ){Menu_state = select;}
		if(tempA == 0x04 && !flaggor){Menu_state = pause_play; set_PWM(0); LCD_DisplayString(5,"PAUSED");}
		break;
		
		case pause_play:
		if(tempA == 0x04){ Menu_state = select; }
		else if(tempA == 0x08) {Menu_state = start;old_songnum = 0xFF;}
		break;
	}
	switch(Menu_state){
		case welcome:
		if(!flaggor){LCD_DisplayString(1,"WELCOME 2 JUKE[]PRESS ANY BUTTON"); flaggor = 0x01;}
		break;
		
		case start:
		if(old_songnum != song_num){
			if(song_num == 0x00) {LCD_DisplayString(4,"SELECTSONG      | SONG 1 >" );}
		else if(song_num == 0x01) {LCD_DisplayString(4,"SELECTSONG      < SONG 2 >");}
		else if(song_num == 0x02) {LCD_DisplayString(4,"SELECTSONG      < SONG 3 >");}
		else if(song_num == 0x03) {LCD_DisplayString(4,"SELECTSONG      < SONG 4 |");}
		old_songnum = song_num;
		}
		
		
		break;
		
		case next:
		if(song_num < (max_songs - 1)){song_num++;};

		break;
		
		case previous:
		if(song_num > 0x00){song_num--;}
		break;
		
		case select:
		if (displayonce == 0x00){
		LCD_ClearScreen();
		LCD_DisplayString(2,"Playing Song ");
		LCD_Cursor(0x0F);
		LCD_WriteData(song_num + 0x01 + '0');
		displayonce = 0x01;
		}
		set_PWM(all_song[song_num][j]);
		displaynote();
		j++;
		if((tempA == 0x08)||(j >= sizeof_songs[song_num]))
		 {flaggor = 1; 
		  set_PWM(0); 
		  old_songnum = 0xFF;
		  displayonce = 0;}
		break;
		
		case pause_play:
		displayonce = 0x00;
		break; 
		
	}
	
};

	
	
void displaynote(){
		const double A = 27.5;
		const double B = 30.87;
		const double C = 16.35;
		const double D = 18.35;
		const double E0 = 20.6;
		const double F = 21.83;
		const double G = 24.50;

		const double A1 = 55.0;
		const double B1 = 61.74;
		const double C1 = 32.7;
		const double D1 = 36.71;
		const double E1 = 41.20;
		const double F1 = 43.65;
		const double G1 = 49.00;

		const double A2 = 110.0;
		const double B2 = 123.5;
		const double C2 = 65.41;
		const double D2 = 73.42;
		const double E2 = 82.41;
		const double F2 = 87.31;
		const double G2 = 98;

		const double A3 = 220.0;
		const double B3 = 246.9;
		const double C3 = 130.8;
		const double D3 = 146.8;
		const double E3 = 164.8;
		const double F3 = 185;
		const double G3 = 196;


		const double A4 = 440.0;
		const double B4 = 493.9;
		const double C4 = 261.6;
		const double D4 = 293.7;
		const double E4 = 329.6;
		const double F4 = 349.2;
		const double G4 = 392;
		
		const double all_song[4][93] = {
			{C3,D3,E0,F3,G,A,B3, B,B3,B,B,A2,A2,A,A2,A4,C4,A2,C1,A2,B3,A2,D,B,A3,D3,E0,E0,F4,A4,F1,A2,C3,A4,D1,D,A2,D,B1,B,C2,D3,F,A,G1,F2,F,A1,A2,G,G},
			{  C,D,E0,F,G,A,B, B,B,B,B,A,A,A,A,A,C,A,C,A,B,A,D,B,A,D,E0,E0,F,A,F,A,C,A,D,D,A,D,B,B,A,D,F,A,G,F,F,A,A,G,G},
			{ A2, G2, F2, D2, B2, A2, G2, F2, D2, B2, A2, G2, F2, D2, B2, A2, G2, F2, D2, B2, A2, G2, F2, D2, B2, A2, G2, F2, D2, B2, A2, G2, F2, D2, B2, A2, G2, F2, D2, B2, A2, G2, F2, D2, B2, A2, G2, F2, D2},
			{C2, B2, A2, C2, B2,A2, G2, A2, B2,	C2, B2, G2, D2, C2, B2, D2, C2, B2, D2, C2, B2, A2, B2, C2, D2, C2, B2, D2, D2, C2, C2, B2, B2, A2, C4, B4, A4, C4, B4,A4, G4, A4, B4,C4, B4, G4, D4, C4, B4, D4, C4, B4, D4, C4, B4, A4, B4, C4, D4, C4, B4, D4, D4,D4, C4,C4, C4, B4,B4, B4, B3,B3,B2,B2,B1,B1}
		};
	if(all_song[song_num][j] == A || all_song[song_num][j] == A1 || all_song[song_num][j] == A2 || all_song[song_num][j] == A3 ||all_song[song_num][j] == A4  )
	{
		LCD_Cursor(0x11); //17
		LCD_WriteData('A');
		LCD_WriteData(' ');
		LCD_WriteData(' ');
		LCD_WriteData(' ');
		LCD_WriteData(' ');
		LCD_WriteData(' ');
		LCD_WriteData(' ');
		LCD_WriteData(' ');
		LCD_WriteData(' ');
		LCD_WriteData(' ');
		LCD_WriteData(' ');
		LCD_WriteData(' ');
		LCD_WriteData(' ');
		LCD_WriteData(' ');
		LCD_WriteData(' ');
		LCD_WriteData(' ');		
	}
		if(all_song[song_num][j] == B || all_song[song_num][j] == B1 || all_song[song_num][j] == B2 || all_song[song_num][j] == B3 ||all_song[song_num][j] == B4  )
		{
			LCD_Cursor(0x11); //17
			LCD_WriteData(' ');
			LCD_WriteData(' ');
			LCD_WriteData('B');
			LCD_WriteData(' ');
			LCD_WriteData(' ');
			LCD_WriteData(' ');
			LCD_WriteData(' ');
			LCD_WriteData(' ');
			LCD_WriteData(' ');
			LCD_WriteData(' ');
			LCD_WriteData(' ');
			LCD_WriteData(' ');
			LCD_WriteData(' ');
			LCD_WriteData(' ');
			LCD_WriteData(' ');
			LCD_WriteData(' ');
			
		}
		if(all_song[song_num][j] == C || all_song[song_num][j] == C1 || all_song[song_num][j] == C2 || all_song[song_num][j] == C3 ||all_song[song_num][j] == C4  )
		{
			LCD_Cursor(0x11); //17
			LCD_WriteData(' ');
			LCD_WriteData(' ');
			LCD_WriteData(' ');
			LCD_WriteData(' ');
			LCD_WriteData('C');
			LCD_WriteData(' ');
			LCD_WriteData(' ');
			LCD_WriteData(' ');
			LCD_WriteData(' ');
			LCD_WriteData(' ');
			LCD_WriteData(' ');
			LCD_WriteData(' ');
			LCD_WriteData(' ');
			LCD_WriteData(' ');
			LCD_WriteData(' ');
			LCD_WriteData(' ');
			
		}
		
		if(all_song[song_num][j] == D || all_song[song_num][j] == D1 || all_song[song_num][j] == D2 || all_song[song_num][j] == D3 ||all_song[song_num][j] == D4  )
		{
			LCD_Cursor(0x11); //17
			LCD_WriteData(' ');
			LCD_WriteData(' ');
			LCD_WriteData(' ');
			LCD_WriteData(' ');
			LCD_WriteData(' ');
			LCD_WriteData(' ');
			LCD_WriteData('D');
			LCD_WriteData(' ');
			LCD_WriteData(' ');
			LCD_WriteData(' ');
			LCD_WriteData(' ');
			LCD_WriteData(' ');
			LCD_WriteData(' ');
			LCD_WriteData(' ');
			LCD_WriteData(' ');
			LCD_WriteData(' ');
			
		}
		if(all_song[song_num][j] == E0 || all_song[song_num][j] == E1 || all_song[song_num][j] == E2 || all_song[song_num][j] == E3 ||all_song[song_num][j] == E4  )
		{
			LCD_Cursor(0x11); //17
			LCD_WriteData(' ');
			LCD_WriteData(' ');
			LCD_WriteData(' ');
			LCD_WriteData(' ');
			LCD_WriteData(' ');
			LCD_WriteData(' ');
			LCD_WriteData(' ');
			LCD_WriteData(' ');
			LCD_WriteData('E');
			LCD_WriteData(' ');
			LCD_WriteData(' ');
			LCD_WriteData(' ');
			LCD_WriteData(' ');
			LCD_WriteData(' ');
			LCD_WriteData(' ');
			LCD_WriteData(' ');
			
		}
		if(all_song[song_num][j] == F || all_song[song_num][j] == F1 || all_song[song_num][j] == F2 || all_song[song_num][j] == F3 ||all_song[song_num][j] == F4  )
		{
			LCD_Cursor(0x11); //17
			LCD_WriteData(' ');
			LCD_WriteData(' ');
			LCD_WriteData(' ');
			LCD_WriteData(' ');
			LCD_WriteData(' ');
			LCD_WriteData(' ');
			LCD_WriteData(' ');
			LCD_WriteData(' ');
			LCD_WriteData(' ');
			LCD_WriteData(' ');
			LCD_WriteData('F');
			LCD_WriteData(' ');
			LCD_WriteData(' ');
			LCD_WriteData(' ');
			LCD_WriteData(' ');
			LCD_WriteData(' ');
			
		}
		if(all_song[song_num][j] == G || all_song[song_num][j] == G1 || all_song[song_num][j] == G2 || all_song[song_num][j] == G3 ||all_song[song_num][j] == G4  )
		{
			LCD_Cursor(0x11); //17
			LCD_WriteData(' ');
			LCD_WriteData(' ');
			LCD_WriteData(' ');
			LCD_WriteData(' ');
			LCD_WriteData(' ');
			LCD_WriteData(' ');
			LCD_WriteData(' ');
			LCD_WriteData(' ');
			LCD_WriteData(' ');
			LCD_WriteData(' ');
			LCD_WriteData(' ');
			LCD_WriteData(' ');
			LCD_WriteData('G');
			LCD_WriteData(' ');
			LCD_WriteData(' ');
			LCD_WriteData(' ');
			
		}
}



int main(void)
{
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
	TimerSet(214);
	TimerOn();
	LCD_init();
	Menu_state = welcome;
	PWM_on();
	while (1)
	{
		Jukebox();
		while(!TimerFlag);
		TimerFlag = 0;
	}
}

