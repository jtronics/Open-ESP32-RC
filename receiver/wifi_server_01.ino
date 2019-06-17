#include <WiFi.h>
#include <WiFiUdp.h>

// COM21

#include "average_float.h"

#include <Ticker.h>
#include <time.h>
#include <ArduinoJson.h>
#include <ESP32_Servo.h>
#include <driver/adc.h>
#include <driver/gpio.h>


// ADC Auflösung 10Bit (4096 = 1,1V)
#define   ADC_MAX         4096
#define   LED_PIN         2
#define   TIME_MSEC_DTX   111
int       TIME_MSEC_LED  = 500;

/************************* Servo *********************************/
// Possible PWM GPIO pins on the ESP32: 0(used by on-board button),
// 2,4,5(used by on-board LED),12-19,21-23,25-27,32-33
Servo pwm0;
Servo pwm1;
Servo pwm2;
Servo pwm3;
Servo pwm4;
Servo pwm5;
Servo pwm6;
Servo pwm7;



/************************* Constants, Variables *********************************/
float val_volt = 0;

AverageFloat mAvgRaw0(2);
AverageFloat mAvgRaw1(2);
/*************************** Timer OS Code ************************************/
Ticker  mTickerDtx;
Ticker  mTickerLed;

float channel[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
/*************************** Wifi Variables ************************************/
const char* ssid  = "JBOOT123";
const char* pass  = "JBOOT123";

IPAddress ServerIP(192, 168, 4, 1);
IPAddress ClientIP(192, 168, 4, 2);

WiFiUDP udp;

boolean       connected = false;
unsigned int  localUdpPort = 2000;  // local port to listen on
char          incomingPacket[255];  // buffer for incoming packets

/*************************** Sketch Code ************************************/
void setup() {
  Serial.begin(115200);

  adc1_config_width(ADC_WIDTH_12Bit);
  adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_6db  );  // ADC_ATTEN_DB_6 = 2
  adc1_config_channel_atten(ADC1_CHANNEL_7, ADC_ATTEN_6db  );  
  
  // using SG90 servo min/max of 500us and 2400us
  // for MG995 large servo, use 1000us and 2000us,
  // 2,4,5(used by on-board LED),12-19,21-23,25-27,32-33
  pwm0.attach(12, 550, 2200);
  pwm2.attach(14, 550, 2200);
  pwm3.attach(27, 550, 2200);
  pwm4.attach(26, 500, 2400);
  pwm5.attach(25, 500, 2400);  
  pwm7.attach(33, 500, 2400); 

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, !digitalRead(LED_PIN));

  delay(10);
  Serial.println((String)"SERVER - BOOT");
  Serial.println((String)"SETUP ACCESS POINT");

  mTickerDtx.attach_ms(200, dtx_isr);
  mTickerLed.attach_ms(500, led_isr);

  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(ssid, pass);

  Serial.println((String)"SSID: " + ssid + "  PASS: " + pass);
  Serial.print("IP address: ");
  Serial.println(WiFi.softAPIP());

  //Start UDP
  Serial.println("Starting UDP");
  udp.begin(localUdpPort);

  Serial.print("Local port: ");
  Serial.println(localUdpPort);
}

void led_isr() {
  digitalWrite(LED_PIN, !digitalRead(LED_PIN));
}

float mapfloat(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void dtx_isr(void) {  
      mAvgRaw0.append(adc1_get_raw(ADC1_CHANNEL_6));
      mAvgRaw1.append(adc1_get_raw(ADC1_CHANNEL_7));
      
      double v0 = mapfloat(mAvgRaw0.avg(),120,2431,2.0,15.00);
      double v1 = mapfloat(mAvgRaw1.avg(),120,2431,2.0,15.00);


    //Serial.println("timer_snd_state");
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    root["adc0"] = mAvgRaw0.avg();
    root["v0"] = v0;
    root["v1"] = v1;   
    
    udp.beginPacket(ClientIP, localUdpPort);
    root.printTo(udp);
    udp.endPacket();
    
    Serial.print("UDP TX: "); root.printTo(Serial); Serial.println();
}

void drx(void) {
  int packetSize = udp.parsePacket();
  if (packetSize)  {

    // We've received a UDP packet
    //Serial.printf("Received %d bytes from %s, port %d\n", packetSize, udp.remoteIP().toString().c_str(), udp.remotePort());
    int len = udp.read(incomingPacket, 255);
    if (len > 0)   {
      incomingPacket[len] = 0;
    }
    Serial.printf("UDP RX: %s\n", incomingPacket);

    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(incomingPacket);
    if (!root.success()) {
      Serial.println("parseObject() failed");
      return;
    }
    channel[0] = root["ch"][0];
    channel[1] = root["ch"][1];
    channel[2] = root["ch"][2];
    channel[3] = root["ch"][3];
    channel[4] = root["ch"][4];
    channel[5] = root["ch"][5];
    channel[6] = root["ch"][6];
    channel[7] = root["ch"][7];

    //Serial.printf("map pwm: %i\n", val);
    pwm0.write(mapfloat(channel[0], -100.0, 100.0, 0, 180.0));
    pwm2.write(mapfloat(channel[1], -100.0, 100.0, 0, 180.0));
    pwm3.write(mapfloat(channel[2], -100.0, 100.0, 0, 180.0));
    pwm4.write(mapfloat(channel[3], -100.0, 100.0, 0, 180.0));
    pwm5.write(mapfloat(channel[4], -100.0, 100.0, 0, 180.0));
    pwm7.write(mapfloat(channel[5], -100.0, 100.0, 0, 180.0));
  }
}

void loop() {
  drx();
  delay(0);
}
