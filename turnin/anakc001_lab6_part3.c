/*	Author: Artip Nakchinda
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab 6  Exercise 3
 *	Exercise Description: 
 *      - Two buttons
 *      - Output at PORTB initally 7
 *      - PA0 increments PORTB (stops at 9)
 *      - Press PA1 decrements PORTB (stop at 0)
 * 
 *  Video link: https://emailucr-my.sharepoint.com/:v:/g/personal/artip_nakchinda_email_ucr_edu/Eeh9bC6sDcNIkzCy1PjU2AsBT6Oc03P3tKIGLe28uWeYhQ?e=lHOIjn
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


enum States{start, init, wait, increment, decrement, reset} state;

unsigned char tempA = 0x00;
unsigned char counter = 0;
unsigned char i;

void Tick() {
    tempA = ~PINA;

    switch(state) { // transitions
        case start:
            state = init;
            break;
        case init:
            state = wait;
            break;
        case wait:
            if((tempA & 0x03) == 0x00)
                state = wait;
            else if((tempA & 0x03) == 0x01)
                state = increment;
            else if((tempA & 0x03) == 0x02)
                state = decrement;
            else if((tempA & 0x03) == 0x03)
                state = reset;
            break;
        case increment:
            state = wait;
            break;
        case decrement:
            state = wait;
            break;
        case reset:
            state = wait;
            break;
        default:
            break;
    }
    switch(state) { // state actions
        case start:
        case init:
            counter = 7;
            break;
        case wait: 
            counter = counter;
            break;
        case increment:
            if(counter != 9)
                counter += 1;
            break;
        case decrement:
            if(counter != 0)
                counter -= 1;
            break;
        case reset:
            counter = 0;
            break;
        default:
            break;
    }
}

int main(void) {
    /* Insert DDR and PORT initializations */
    DDRA = 0x00; PORTA = 0xFF; // input
    DDRB = 0xFF; PORTB = 0x00; // output

    TimerSet(100);
    TimerOn();

    /* Insert your solution below */
    while (1) {
        Tick();
        PORTB = counter;
        // PORTB = counter;
        while(!TimerFlag) {}
        TimerFlag = 0;
    }
    return 1;
}
