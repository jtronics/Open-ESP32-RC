
#include <WiFi.h>
#include <WiFiUDP.h>
#include <Ticker.h>
#include <time.h>
#include <Wire.h>
#include <ArduinoJson.h>
#include <driver/adc.h>

#include <driver/adc.h>
#include <driver/gpio.h>
#include <esp_err.h>
#include <driver/dac.h>
#include <EEPROM.h>
    
#include "encoder.h"
#include "SSD1306Wire.h"
#include "OLEDDisplayUi.h"

#define   LED_PIN         2
#define   ADC_Max 4095 
#define   V_REF 1100  // ADC reference voltage

// Output ADC2 reference voltage to gpio 25 or 26 or 27.
/*
GPIO36 ADC0
GPIO37
GPIO38
GPIO39
GPIO32
GPIO33
GPIO34
GPIO35
*/

/************************* Constants, Variables *********************************/
bool ole_state = true;

enum PARAM {
  VALUE = 0,
  RAW,
  INVER,
  INMIN,
  INMID,  
  INMAX,
  OUMIN,
  OUMID,  
  OUMAX 
};

enum DATA {
  CH00 = 0,
  CH01,
  CH02,
  CH03,
  CH04,
  CH05 = 5,
  CH06,
  CH07,
  CH08,
  // sonstiges  
  RSCO,
  WICO = 10,    
  VOCO,
  BACO,
  RSTO,
  WITO,  
  VOTO = 15,
  BATO,
  DIST,    
  DUM0,    
  DUM1,    
  DUM2 = 20
};

#define CFG_MAX_AMP 8

typedef struct chann_t {   
  float val;
  float raw;
  float inv;
  int   siz;
  float mpi[CFG_MAX_AMP];
  float mpo[CFG_MAX_AMP];
}chann_t;

typedef struct model_t {   
  char     nam[8];
  chann_t  cha[8];
  chann_t  tel[8];  
}bcfg_t;

typedef struct sys_t {   
  model_t mod[8];
  chann_t  sy[8];  
}sys_t;


typedef struct cfg_t {   
  float eep[21][9];
}cfg_t;



cfg_t cfg {   
  {
  { 0,0,1,  0,0,ADC_Max,-100,0, 100},
  { 1,0,1,  0,0,ADC_Max,-100,0, 100},  
  { 2,0,1,  0,0,ADC_Max,-100,0, 100},   
  { 3,0,1,  0,0,ADC_Max,-100,0, 100}, 
  { 4,0,1,  0,0,ADC_Max,-100,0, 100}, 
  { 5,0,1,  0,0,ADC_Max,-100,0, 100},
  { 6,0,1,  0,0,ADC_Max,-100,0, 100}, 
  { 7,0,1,  0,0,ADC_Max,-100,0, 100},            
  { 8,0,1,  0,0,ADC_Max,-100,0, 100},
  // sonstiges
  { 9,0,1,  0,0,    100,   0,0, 100},    // RSCO 
  {10,0,1, 90,0,     50,   0,0, 100},    // WICO
  {11,0,1,  0,0,ADC_Max,   0,0,4.95},    // VOCO
  {12,0,1,3.2,0,   4.25,   0,0, 100},    // BACO 
  {13,0,1,  0,0,ADC_Max,-100,0, 100},             
  {14,0,1,  0,0,ADC_Max,-100,0, 100},             
  {15,0,1,  0,0,ADC_Max,-100,0, 100},             
  {16,0,1,  0,0,ADC_Max,-100,0, 100},             
  {17,0,1,  0,0,ADC_Max,-100,0, 100},             
  {18,0,1,  0,0,ADC_Max,-100,0, 100},             
  {19,0,1,  0,0,ADC_Max,-100,0, 100},             
  {20,0,1,  0,0,ADC_Max,-100,0, 100}
  }
};


//model_t *model_act = &menuMain;
/*************************** Timer OS Code ************************************/
Ticker  mTickerDtx;
Ticker  mTickerLed;
Ticker  mTickerOle;
Ticker  mTickerInp;
/*************************** Sketch Code ************************************/
void setup() {
  Serial.begin(115200);
  Serial.println("SETUP Client");

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, !digitalRead(LED_PIN));

  
  adc1_config_width(ADC_WIDTH_12Bit);
  adc1_config_channel_atten(ADC1_CHANNEL_3, ADC_ATTEN_0db);  
  adc1_config_channel_atten(ADC1_CHANNEL_7, ADC_ATTEN_0db);
  adc1_config_channel_atten(ADC1_CHANNEL_5, ADC_ATTEN_0db);  
  adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_0db);
  
  adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_0db);
  //adc1_config_channel_atten(ADC1_CHANNEL_1, ADC_ATTEN_0db);  
  

  

 // adc2_vref_to_gpio(GPIO_NUM_25);
    dac_output_enable(DAC_CHANNEL_1);
    dac_output_voltage(DAC_CHANNEL_1, 1100.0*255.0/3300.0);
//adc2_vref_to_gpio(GPIO_NUM_26);
//      adc2_vref_to_gpio(GPIO_NUM_27);

  Serial.println("- oled");
  ole_init();

  Serial.println("- wifi");
  wif_init();
  inp_init();

  mTickerInp.attach_ms(  2, inp_isr);
  mTickerDtx.attach_ms(100, wif_dtx_isr);
  mTickerLed.attach_ms(500, led_isr);
  mTickerOle.attach_ms(200, ole_isr);

  if (!EEPROM.begin(EEPROM.length()-1))
    {
      Serial.println("failed to initialise EEPROM"); 
      Serial.println("Restarting...");
      delay(1000);
      ESP.restart();
  }


  uint8_t eep = 128;
  eep = EEPROM.read(0);
  Serial.print("eep: ");  Serial.println(eep);     
  if(eep == uint8_t(253)){
      EEPROM.get(8, cfg);  
  }
}

void led_isr() {
  digitalWrite(LED_PIN, !digitalRead(LED_PIN));
}

void loop() {
  wif_run();

  if (ole_state) {
    ole_state = false;
    ole_run();
  }
  delay(0);
}


