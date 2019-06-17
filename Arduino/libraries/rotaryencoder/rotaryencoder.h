// RotaryEncoder.h
// based on https://github.com/marcmerlin/IoTuz code - extracted and modified Encoder code

#ifndef _ROTARYENCODER_h
#define _ROTARYENCODER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

// Rotary Encocer
#define ENC_DEF_A_PIN 		15
#define ENC_DEF_B_PIN 		 2
#define ENC_DEF_BUT_PIN 	 0

typedef enum {
	BUT_DOWN = 0,
	BUT_PUSHED = 1,
	BUT_UP = 2,
	BUT_RELEASED = 3,
	BUT_DISABLED = 99,
} ButtonState;

class RotaryEncoder {
	
private:
	portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
	volatile int16_t 		encoder0Pos = 0;
	bool 	_circleValues 	= false;
	bool 	isEnabled 		= true;

	uint8_t pin_a   = ENC_DEF_A_PIN;
	uint8_t pin_b   = ENC_DEF_B_PIN;
	uint8_t pin_btn = ENC_DEF_BUT_PIN;

	int16_t val_min	= -1 << 15;
	int16_t val_max = 1 << 15;

	uint8_t old_AB;
	int16_t lastReadEncoder0Pos;
	bool 	previous_butt_state;

	int8_t enc_states[16] = {0,-1,1,0,1,0,0,-1,-1,0,0,1,0,1,-1,0};
	
	void(*ISR_callback)();	
	void(*ISR_callback_enc)();	
	
public: 
	RotaryEncoder(uint8_t pin_a = ENC_DEF_A_PIN, uint8_t pin_b = ENC_DEF_B_PIN, uint8_t pin_btn =ENC_DEF_BUT_PIN);
	void setBoundaries(int16_t minValue = -100, int16_t maxValue = 100, bool circleValues = false);
	void IRAM_ATTR readEncoder_ISR();
	void IRAM_ATTR enc_isr();
	void IRAM_ATTR btn_isr();
	
	void init(void (*ISR_callback)(void), void (*ISR_callback_enc)(void));
	void begin();
	void reset(int16_t newValue = 0);
	void enable();
	void disable();
	int16_t readEncoder();
	int16_t encoderChanged();
	ButtonState currentButtonState();
};
#endif

