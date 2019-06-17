
#ifndef _ENCODER_h
#define _ENCODER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#define REPEAT_START    70								// after 500ms
#define REPEAT_NEXT     20								// every 200ms
	

class Encoder {
	
private:
	uint8_t pin_a   = 15;
	uint8_t pin_b   = 2;
	uint8_t pin_btn = 0;
	
	volatile uint8_t btn_curr = 0;                             // debounced and inverted key state: bit = 1: key pressed
	volatile uint8_t btn_last = 0;                             // key press detect
	volatile uint32_t btn_ms  = 0;                               // key long press and repeat
		
	uint8_t enc_val 	= 50;
	uint8_t enc_delta	= 0; // -128 ... 127
	uint8_t enc_last	= 0;

	void (*_callbackBtnShort)(void);
	void (*_callbackBtnLong)(void);
	void (*_callbackEnc)(void);	
	
public: 
	Encoder(uint8_t pin_a = 15, uint8_t pin_b = 2, uint8_t pin_btn =0);
	
	void isrBtn(void);
	void isrEnc(void);	
	void init(void (*callbackBtn)(void));
	
	void setCallbackBtnLong(void (*callback)(void));
	void setCallbackBtnShort(void (*callback)(void));
	void setCallbackEnc(void (*callback)(void));
	
	int8_t readEnc1();
	int8_t readEnc2();
	int8_t readEnc4();
};
#endif

