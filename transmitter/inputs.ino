
#define ROTARY_ENCODER_A_PIN 19
#define ROTARY_ENCODER_B_PIN 18
#define ROTARY_ENCODER_BUTTON_PIN 15

Encoder rotaryEncoder = Encoder(ROTARY_ENCODER_A_PIN, ROTARY_ENCODER_B_PIN, ROTARY_ENCODER_BUTTON_PIN);

void inp_init() {
  rotaryEncoder.init([] {rotaryEncoder.isrBtn();});
  rotaryEncoder.setCallbackBtnLong(inp_isrBtnLong);
  rotaryEncoder.setCallbackBtnShort(inp_isrBtnShort);
  rotaryEncoder.setCallbackEnc(inp_isrEnc);
}

void inp_isr() {
  rotaryEncoder.isrEnc();
}


