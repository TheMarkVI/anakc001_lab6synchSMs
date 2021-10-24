/*	Author: Artip Nakchinda
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab 6  Exercise 1
 *	Exercise Description: 
 *      - Create a synchSM to blink 3 LEDs connected to PB0-2
 * 
 *  Video link: https://emailucr-my.sharepoint.com/:v:/g/personal/artip_nakchinda_email_ucr_edu/EXOMfBfUIEZDjm_qHzmSydMBfSijJj3jq8LRNDlI2fgPKA?e=dpSsko
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


enum States{start, led1, led2, led3} state;

void Tick() {
    switch(state) { // transitions
        case start:
            state = led1;
            break;
        case led1:
            state = led2;
            break;
        case led2:
            state = led3;
            break;
        case led3:
            state = led1;
            break;
        default:
            break;
    }
    switch(state) { // state actions
        case start:
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
        default:
            break;
    }
}

int main(void) {
    /* Insert DDR and PORT initializations */
    DDRB = 0xFF; PORTB = 0x00;

    TimerSet(1000);
    TimerOn();
    
    /* Insert your solution below */
    while (1) {
        // Tick();
        Tick();
        while(!TimerFlag) {}
        TimerFlag = 0;
    }
    return 1;
}
