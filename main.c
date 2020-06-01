/*
 * main.c
 *
 *  Created on: 26 kwi 2020
 *      Author: osiek
 */
#define F_CPU 1000000L
#include "global.h"
#include <util/delay.h>
#include <avr/io.h>
#include <avr/signal.h>
#include <stdlib.h>

#define PWM_A (1<<PB1)
#define PWM_B (1<<PB2)

void ADC_init(int kanal)
{
ADMUX&=0xFC; //wyzerowanie mux0 i mux1
ADMUX|=_BV(REFS0);
if(kanal==0){} //kanał 0
else if(kanal==1)
ADMUX|=_BV(MUX0); //kanał 1
else if(kanal==2)
	ADMUX|=_BV(MUX1); //kanał2
ADCSRA |=_BV(ADEN); // zezwolenie na konwersje
ADCSRA |=_BV(ADPS0);
ADCSRA |=_BV(ADPS1);
ADCSRA |=_BV(ADPS2);

}
void silniki_init()
{
	 TCCR1A |= (1<<WGM10);
	 TCCR1B |= (1<<WGM12);
	 TCCR1A |= (1<<COM1A1)|(1<<COM1B1);
	 TCCR1B |= (1<<CS10)|(1<<CS11);
	 OCR1A=0;
	 OCR1B=0;
}
void jazda_przod()
{
sbi(PORTD, PD2);
PORTD|=0x01; //ustawia na 1 SIL1 0000|0001
PORTD&=0xFD; //ustawia na 0 SIL1 1111|1101
PORTD&=0x7F; //ustawia na 0 SIL2 0111|1111
PORTB|=0x01; // ustawia na 1 SIL2 0000|0001
OCR1A=100;
OCR1B=100;
}
void jazda_tyl()
{
	cbi(PORTD, PD2);
		PORTD&=0xFE; //ustawia na 0 SIL1 1111|1110
		PORTD|=0x02; //ustawia na 1 SIL1 0000|0010
		PORTD|=0x80; //ustawia na 1 SIL2 1000|0000
		PORTB&=0xFE; //ustawia na 0 SIL2 1111|1110
		OCR1A=50;
		OCR1B=50;
}
void jazda_kolko()
{
	cbi(PORTD, PD2);
			PORTD&=0xFE; //ustawia na 0 SIL1 1111|1110
			PORTD|=0x02; //ustawia na 1 SIL1 0000|0010
			PORTD&=0x7F; //ustawia na 0 SIL2 0111|1111
			PORTB&=0xFE; //ustawia na 0 SIL2 1111|1110
			OCR1A=0;     //SIL2 Lewy
			OCR1B=50;   //sil1 Prawy

}
void jazda_lewo()
{
	cbi(PORTD, PD2);
PORTD|=0x01; //ustawia na 1 SIL1 0000|0001
PORTD&=0xFD; //ustawia na 0 SIL1 1111|1101
PORTD&=0x7F; //ustawia na 0 SIL2 0111|1111
PORTB&=0xFE; //ustawia na 0 SIL2 1111|1110
OCR1A=0;     //SIL2 Lewy
OCR1B=50;   //sil1 Prawy
}
void jazda_prawo()
{
	cbi(PORTD, PD2);
PORTD&=0xFE; //ustawia na 0 SIL1 1111|1110
PORTD&=0xFD; //ustawia na 0 SIL1 1111|1101
PORTD&=0x7F; //ustawia na 0 SIL2 0111|1111
PORTB|=0x01; // ustawia na 1 SIL2 0000|0001
OCR1A=50;     //SIL2 Lewy
OCR1B=0;   //sil1 Prawy
}
void stop()
{
	PORTD&=0xFE; //ustawia na 0 SIL1 1111|1110
	PORTD&=0xFD; //ustawia na 0 SIL1 1111|1101
	PORTD&=0x7F; //ustawia na 0 SIL2 0111|1111
	PORTB&=0xFE; //ustawia na 0 SIL2 1111|1110
	OCR1A=0;     //SIL2 Lewy
	OCR1B=0;   //sil1 Prawy
}

float pomiary(int wybor)
{
	float pomiar;
	ADC_init(wybor);
	ADCSRA |=_BV(ADSC);
	        while(ADCSRA & _BV(ADSC)){}
	        pomiar=(float)(ADCL | (ADCH<<8))/1024*5;
	        return pomiar;
}


int main() {
 DDRD=0xBF;
 DDRC=0x00;
 DDRB=0xFF;
silniki_init();
int lewo=0;

    while (1) {
    do{
    	cbi(PORTD, PD2);
    	_delay_ms(300);
    	sbi(PORTD, PD2);
    	_delay_ms(300);
    	stop();
    	}
    	while(PIND & 0x40 ); //guzik

while(PIND & 0x40 ){
	if(pomiary(0)>2.4){  //jezeli czujnik prawy
	jazda_tyl();
	_delay_ms(500);
	jazda_lewo();
	_delay_ms(500);
		lewo=1;
	} else if(pomiary(1)>2.4){
	jazda_tyl();
	_delay_ms(500);
	jazda_prawo();
	_delay_ms(500);
	lewo=0;	    	
	}
	else if(pomiary(2)>0.8){
	jazda_przod();
	_delay_ms(1000);
	}else if(lewo=1){
	jazda_lewo();
	_delay_ms(500);;}
	else jazda_kolko();
    	}
    }
}

