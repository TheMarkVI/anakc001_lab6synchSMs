/*	Author: Artip Nakchinda
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab 6  Exercise 2
 *	Exercise Description: 
 *      - Create a light game
 *      - LEDs PB0-2 light in sequence for 300ms each
 *      - if button press -> current LED stays lit
 *      - press button again -> restart game
 * 
 *  Video link: https://emailucr-my.sharepoint.com/:v:/g/personal/artip_nakchinda_email_ucr_edu/Ee3fszSAICxNqChB7xwN-KgBKeNW2aV_NBQ0s4zNH5nWyQ?e=bU2bgr
 * 
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

volatile unsigned char TimerFlag = 0;

//Internal variables for mapping AVR's to the cleaner TimerISR model
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

void TimerOff() { TCCR1B = 0x00; }

void TimerISR() { TimerFlag = 1; }

ISR(TIMER1_COMPA_vect) { 
    _avr_timer_cntcurr--;

    if(_avr_timer_cntcurr == 0) {
        TimerISR();
        _avr_timer_cntcurr = _avr_timer_M;
    }
}

void TimerSet(unsigned long M) {
    _avr_timer_M = M;
    _avr_timer_cntcurr = _avr_timer_M;
}


enum States{start, init, led1, led2, led3, buttonPress, buttonRel} state;

unsigned char tempA = 0x00;
unsigned char i;

void Tick() {
    tempA = ~PINA;

    switch(state) { // transitions
        case start:
            state = init;
            break;
        case init:
            if((tempA & 0x01) == 0x01)
                state = init;
            else
                state = led1;
            break;
        case led1:
            if((tempA & 0x01) == 0x01)
                state = buttonPress;
            else
                state = led2;
            break;
        case led2:
            if((tempA & 0x01) == 0x01)
                state = buttonPress;
            else
                state = led3;
            break;
        case led3:
            if((tempA & 0x01) == 0x01)
                state = buttonPress;
            else
                state = led1;
            break;
        case buttonPress:
            if((tempA & 0x01) == 0x01)
                state = buttonPress;
            else
                state = buttonRel;
            break;
        case buttonRel:
            if((tempA & 0x01) == 0x01)
                state = init;
            else
                state = buttonRel;
            break;
        default:
            break;
    }
    switch(state) { // state actions
        case start:
        case init:
            PORTB = 0x00;
            break;
        case led1:
            PORTB = 0x01;
            break;
        case led2:
            PORTB = 0x02;
            break;
        case led3:
            PORTB = 0x04;
            break;
        case buttonPress:
        case buttonRel:
            PORTB = PORTB;
            break;
        default:
            break;
    }
}

int main(void) {
    /* Insert DDR and PORT initializations */
    DDRA = 0x00; PORTA = 0xFF; // input
    DDRB = 0xFF; PORTB = 0x00; // output

    TimerSet(300);
    TimerOn();
    
    

    /* Insert your solution below */
    while (1) {
        Tick();
        while(!TimerFlag) {}
        TimerFlag = 0;
    }
    return 1;
}
