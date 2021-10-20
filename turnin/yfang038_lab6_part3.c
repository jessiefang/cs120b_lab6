/*	Author: Yunjie Fang
 *  Partner(s) Name: N/A
 *	Lab Section:021
 *	Assignment: Lab #6  Exercise #3
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo link:
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

enum States {Start, Init, Increment, Decrement, Reset, On_Increment, On_Decrement} state;
unsigned char cnt = 0x07;
volatile unsigned char TimerFlag = 0;
unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;
void TimerOn() {
	TCCR1B = 0x0B;
	OCR1A = 125;
	TIMSK1 = 0x02;
	TCNT1 = 0;
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

void TimerSet (unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

void Tick() {
 unsigned char tmpA = ~PINA & 0x03;
 switch(state){
		case Start:
			state = Init;
			break;
		case Init:
			if (tmpA == 0x03){
				state = Reset;
			}
			else if(tmpA == 0x01){
				state = On_Increment;
			}
			else if(tmpA == 0x02){
				state = On_Decrement;
			}
			break;
		case Increment:
			if(tmpA == 0x01){
				state = Increment;
			}
			else if (tmpA == 0x00){
				state = Init;
			}else if(PINA == 0x02){
				state = On_Decrement;
			}
			else if (tmpA == 0x03){
				PORTC = 0x00;
				state = Reset;
			}
                        break;
		case On_Increment:
			state = Increment;
			break; 
		case Decrement:
                        if(tmpA == 0x02){
                                state = Decrement;
                        }else if (tmpA == 0x00){
                                state = Init;
                        }else if (tmpA == 0x01){
				state = On_Increment;
			}
			else if (tmpA == 0x03){
                                state = Reset;
                        }
                        break;
		case On_Decrement:
                        state = Decrement;
	                break;
		case Reset:
			if(PINA == 0x03){
				state = Reset;
			}else if(PINA == 0x00){
				state = Init;
			}
			else{
				state = Init;
			}
			break;
		 default:
			state = Start;
			break;
	}
	switch(state){
		case Start:
			PORTB = 0x07;
			break;
		case Init:
			break;
		case Increment:
			break;
		case On_Increment:
			if(cnt < 0x09){
				cnt++;
			}
			break;
		case Decrement:
			break;
		case On_Decrement:
			if(cnt > 0x00){
				cnt--;
			}
			break;
		case Reset:
			PORTB = 0x00;
			cnt = 0;
			break;
		default:
			PORTB = 0x07;
			break;
	}
}


int main(void) {
    /* Insert DDR and PORT initializations */
    DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0xFF; PORTB = 0x00;
    state = Start;
    TimerSet(100);
    TimerOn();
    /* Insert your solution below */
    while (1) {
    	Tick();
	while(!TimerFlag);
	TimerFlag = 0;
    	PORTB = cnt;
    }
    return 1;
}
