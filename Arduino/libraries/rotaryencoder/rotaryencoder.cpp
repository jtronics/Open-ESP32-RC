// based on https://github.com/marcmerlin/IoTuz code - extracted and modified Encoder code
// 
// 

#include "rotaryencoder.h"


RotaryEncoder::RotaryEncoder(uint8_t pin_a, uint8_t pin_b, uint8_t pin_btn){
	this->old_AB 	= 0;	
	this->pin_a 	= pin_a;
	this->pin_b 	= pin_b;
	this->pin_btn 	= pin_btn;
	this->lastReadEncoder0Pos = 0;
	this->previous_butt_state = 0;
	
	
}

void IRAM_ATTR RotaryEncoder::readEncoder_ISR(){	
	Serial.println("interrupt!");
	portENTER_CRITICAL_ISR(&(this->mux));
	/*
	if (this->isEnabled) {
		this->old_AB <<= 2;                   //remember previous state

		//Serial.print("OldAB= ");
		//Serial.println(old_AB, BIN);

		int8_t ENC_PORT = ((digitalRead(this->pin_b)) ? (1 << 1) : 0) | ((digitalRead(this->pin_a)) ? (1 << 0) : 0);
		
		//Serial.print("ENC_PORT= ");
		//Serial.println(ENC_PORT, BIN);
		
		this->old_AB |= ( ENC_PORT & 0x03 );  //add current state

		//Serial.print("NewAB= ");
		//Serial.println(old_AB, BIN);		

		//Serial.print("old_AB & 0x0f= ");
		//Serial.println(( old_AB & 0x0f ), BIN);

		this->encoder0Pos += ( this->enc_states[( this->old_AB & 0x0f )]);	

		if (this->encoder0Pos > this->val_max)
			this->encoder0Pos = this->_circleValues ? this->val_min : this->val_max;
		if (this->encoder0Pos < this->val_min)
			this->encoder0Pos = this->_circleValues ? this->val_max : this->val_min;		

		//Serial.print("encoder0Pos= ");
		//Serial.println(this->encoder0Pos);	
		//Serial.println("---------------");
	}
	*/	
	portEXIT_CRITICAL_ISR(&mux);
}


void IRAM_ATTR RotaryEncoder::btn_isr(){	
	Serial.println("interrupt btn_ISR!");
	portENTER_CRITICAL_ISR(&(this->mux));
	portEXIT_CRITICAL_ISR(&mux);
}

void IRAM_ATTR RotaryEncoder::enc_isr(){	
	Serial.println("interrupt enc_ISR!");
	portENTER_CRITICAL_ISR(&(this->mux));
	portEXIT_CRITICAL_ISR(&mux);
}

void RotaryEncoder::init(void (*ISR_callback)(void), void (*ISR_callback_enc)(void)){
	
	if (this->pin_btn >= 0) 
	{
		//pinMode(this->pin_btn, INPUT);	
		pinMode(this->pin_btn, INPUT_PULLUP);	
		attachInterrupt(digitalPinToInterrupt(this->pin_btn), 	ISR_callback, CHANGE);	
	}

	pinMode(this->pin_a, INPUT_PULLUP);	
	pinMode(this->pin_b, INPUT_PULLUP);	

	//pinMode(this->pin_a, INPUT);
	//pinMode(this->pin_b, INPUT);
	
	//attachInterrupt(digitalPinToInterrupt(this->pin_a), 	f_enc, FALLING);	
	//attachInterrupt(digitalPinToInterrupt(this->pin_b), 	f_enc, FALLING);	
	
	attachInterrupt(digitalPinToInterrupt(this->pin_a), ISR_callback_enc, CHANGE);
	attachInterrupt(digitalPinToInterrupt(this->pin_b), ISR_callback_enc, CHANGE);
}

void RotaryEncoder::setBoundaries(int16_t min, int16_t max, bool circleValues){
	this->val_min = min * 2;
	this->val_max = max * 2;
	this->_circleValues = circleValues;
}

int16_t RotaryEncoder::readEncoder(){
	return (this->encoder0Pos / 2);
}

int16_t RotaryEncoder::encoderChanged() {
	int16_t _encoder0Pos = readEncoder();
	
	int16_t encoder0Diff = _encoder0Pos - this->lastReadEncoder0Pos;

	this->lastReadEncoder0Pos = _encoder0Pos;
	return encoder0Diff;
}

void RotaryEncoder::begin(){
	




}

ButtonState RotaryEncoder::currentButtonState(){
	if (!this->isEnabled) {
		return BUT_DISABLED;
	}

	uint8_t butt_state = !digitalRead(this->pin_btn);


	if (butt_state && !this->previous_butt_state)
	{
		this->previous_butt_state = true;
		//Serial.println("Button Pushed");
		return BUT_PUSHED;
	}
	if (!butt_state && this->previous_butt_state)
	{
		this->previous_butt_state = false;
		//Serial.println("Button Released");
		return BUT_RELEASED; 
	}
	return (butt_state ? BUT_DOWN : BUT_UP);
}

void RotaryEncoder::reset(int16_t newValue_) {
	newValue_ = newValue_ * 2;
	this->encoder0Pos = newValue_;
	if (this->encoder0Pos > this->val_max) this->encoder0Pos = this->_circleValues ? this->val_min : this->val_max;
	if (this->encoder0Pos < this->val_min) this->encoder0Pos = this->_circleValues ? this->val_max : this->val_min;	
}

void RotaryEncoder::enable() {
	this->isEnabled = true;
}

void RotaryEncoder::disable() {
	this->isEnabled = false;
}
