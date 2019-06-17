
#include "encoder.h"
const uint8_t PRESSED = 1;
const uint16_t shortPress = 50;
const uint16_t longPress = 1000;

Encoder::Encoder(uint8_t pin_a, uint8_t pin_b, uint8_t pin_btn){
	this->pin_a 	= pin_a;
	this->pin_b 	= pin_b;
	this->pin_btn 	= pin_btn;	
}

void IRAM_ATTR Encoder::isrBtn(){	

	btn_curr = !digitalRead(this->pin_btn);       // then toggle debounced state
	//Serial.printf("btn_curr: %d \n", btn_curr);
	
    if (btn_curr != btn_last) {
		
		if (btn_curr == PRESSED) {
			btn_ms = millis();			
		}
        else {
            uint32_t currMs = millis();
			uint16_t  delta = currMs - btn_ms ;
			//Serial.printf("%d curr:  %d, delta: %d , %d   , %d\n",btn_last, btn_curr, delta, btn_ms, currMs);
            
			if      (delta > longPress)		_callbackBtnLong();									
            else if (delta > shortPress) 	_callbackBtnShort();				            			
        }				
		btn_last = btn_curr;        
    } 
}

void Encoder::isrEnc(){		
	int8_t new_, diff;
	new_ = 0;
	if( !digitalRead(this->pin_a) ) new_ = 3;
	if( !digitalRead(this->pin_b) ) new_ ^= 1; // convert gray to binary
	
	diff = enc_last - new_; // difference last - new
	if( diff & 1 )
	{ 
		enc_last = new_; // store new as next last
		enc_delta += (diff & 2) - 1; // bit 1 = direction (+/-)
		//Serial.printf("enc_delta: %d\n", enc_delta);
		if((enc_delta>> 2) != 0)
		{
			_callbackEnc();	
		}
	}
	
}

void Encoder::init(void (*callbackBtn)(void)){
	
	if (this->pin_btn >= 0) {
		pinMode(this->pin_btn, INPUT_PULLUP);	
		attachInterrupt(digitalPinToInterrupt(this->pin_btn), 	callbackBtn, CHANGE);	
	}
	pinMode(this->pin_a, INPUT_PULLUP);	
	pinMode(this->pin_b, INPUT_PULLUP);	
	
	
	//attachInterrupt(digitalPinToInterrupt(this->pin_a), 	callbackEnc, CHANGE);	
	//attachInterrupt(digitalPinToInterrupt(this->pin_b), 	callbackEnc, CHANGE);	

	// encoder
	// init the pins extern
	int8_t _new;
	
	_new = 0;
	if( !digitalRead(this->pin_a) )
	_new = 3;
	if( !digitalRead(this->pin_b) )
	_new ^= 1; // convert gray to binary
	enc_last = _new; // power on state
	enc_delta = 0;
}

void Encoder::setCallbackEnc(void (*callback)(void)){
	_callbackEnc = callback;
}

void Encoder::setCallbackBtnLong(void (*callback)(void)){
	_callbackBtnLong = callback;
}

void Encoder::setCallbackBtnShort(void (*callback)(void)){
	
	_callbackBtnShort = callback;
}

int8_t Encoder::readEnc1( void )         // read single step encoders
{
  int8_t val;
  val = enc_delta;
  enc_delta = 0;
  return val;                   // counts since last call
}


int8_t Encoder::readEnc2( void )         // read two step encoders
{
  int8_t val;
  val = enc_delta;
  enc_delta = val & 1;
  return val >> 1;
}


int8_t Encoder::readEnc4( void )         // read four step encoders
{
  int8_t val;
  val = enc_delta;
  enc_delta = val & 3;
  return val >> 2;
}
