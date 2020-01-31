#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

volatile unsigned char TimerFlag = 0; // TimerISR() sets this to 1. C programmer should clear to 0.

// Internal variables for mapping AVR's ISR to our cleaner TimerISR model.
unsigned long _avr_timer_M = 1; // Start count from here, down to 0. Default 1 ms. 
unsigned long _avr_timer_cntcurr = 0; // Current internal count of 1ms ticks 

enum states {init, on1, on2, on3} state; 

void TimerOn(){
// AVR timer/counter controller register TCCR1
TCCR1B = 0x0B; // bit3 = 0: CTC mode (clear timer on compare)
	       // bit2bit1bit0=011: pre-scaler /64
	       // 00001011: 0x0B
	       // So, 8 Mhz clock or 8,000,000 /64 = 125,000 ticks/s
	       // Thus, TCNT1 register will count at 125,000 ticks/s

// AVR output compare register OCR1A.
OCR1A = 125; // Timer interrupt will be generated when TCNT1 == OCR1A
	     // We want a 1 ms tick. 0.001s * 125,0000 ticks/s = 125
	     // So when TCNT1 register equals 125,
	     // 1 ms has passed. Thus, we compare to 125.

// AVR timer interrupt mask register
TIMSK1 = 0x02; //bit1: OCIE1A -- enables compare match interrupt 

// Initialize avr counter
TCNT1 = 0;

_avr_timer_cntcurr = _avr_timer_M;
// TimerISR will be called every _avr_timer_cntcurr milliseconds 

// Enable  global interrupts 
SREG |= 0x80; // 0x80: 1000000
}

void TimerOff(){
	TCCR1B = 0x00; // bit3bit1bit0: timer off

}

void TimerISR(){
	TimerFlag = 1;
}

void TickSM(){
	switch (state) {
		case init:
			state = on1;
			break;
		case on1:
			state = on2;
			break;
		case on2:
			state = on3;
			break;
		case on3:
			state = on1;
			break;
		default:
			break;
	} //transitions
	switch (state) {
		case init:
			break;
		case on1:
			PORTB = 1;
			break;	
		case on2:
			PORTB = 2;
			break;
		case on3: 
			PORTB = 4;
			break;
		default:
			break;
	} //state logic 
}

// In our approach, the C programmer does not touch this ISR, but rather TimerISR()
ISR(TIMER1_COMPA_vect){
	// CPU automaticallt calls when TCNT1 == OCR1 (every 1 ms per TimerON settings)
	_avr_timer_cntcurr--; // Count down to 0 rather than up to TOP
	if(_avr_timer_cntcurr == 0){ //results in a more efficient compare
		TimerISR(); // Call the ISR that the user uses
		_avr_timer_cntcurr = _avr_timer_M;
	
	}
}
	// Set TimerISR() to tick every M ms
	void TimerSet(unsigned long M){
		_avr_timer_M = M;
		_avr_timer_cntcurr = _avr_timer_M;
	}

int main(){
	DDRB = 0xFF; // Set port B to output
	PORTB = 0x00; // Init port B to 0s
	TimerSet(100);
	TimerOn();
	state = init;
	//unsigned char tmpB = 0x00;
	while(1){
		// User code (i.e. synchSM calls)
		TickSM();
		while(!TimerFlag); // Wait 1 sec
		TimerFlag = 0;
		// Note: FOr the above a better style woulde use a synchSM with TickSM()
		// This example just illustrates the use of the ISR and flag
	
	}
	return 1;
}


