


// Range Battery
#define BATT_VOLT_FULL  4.2
#define BATT_VOLT_EMPT  3.0

// Range Wifi
#define WIFI_RSSI_FULL  58
#define WIFI_RSSI_EMPT  88


/*************************** Adc  Variables ************************************/
// Possible ADC pins on the ESP32: 0,2,4,12-15,32-39; 34-39 are recommended for analog input
// This is the default ADC max value on the ESP32 (12 bit ADC width);
// this width can be set (in low-level oode) from 9-12 bits, for a
// a range of max values of 512-4096


/*************************** Wifi Variables ************************************/
const char* ssid = "JBOOT123";
const char* pass = "JBOOT123";

IPAddress ServerIP(192, 168, 4, 1);
IPAddress ClientIP(192, 168, 4, 2);

WiFiUDP udp;

unsigned int  localUdpPort = 2000;  // local port to listen on
char          incomingPacket[255];  // buffer for incoming packets
boolean connected = false;

void WiFiEvent(WiFiEvent_t event) {
  Serial.printf("[WiFi-event] event: %d: ", event);

  switch (event) {
    case SYSTEM_EVENT_STA_GOT_IP:
      Serial.println("");
      Serial.print("Connected to ");
      Serial.print(ssid);
      Serial.print(" IP address: ");
      Serial.println(WiFi.localIP());

      //Start UDP
      Serial.println("Starting UDP");
      //udp.begin(localUdpPort);
      Serial.print("Local port: ");
      Serial.println(localUdpPort);
      udp.begin(WiFi.localIP(), localUdpPort);
      connected = true;
      mTickerLed.detach();
      mTickerLed.attach_ms(500, led_isr);
      break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
      Serial.println("WiFi lost connection");
      connected = false;
      mTickerLed.detach();
      mTickerLed.attach_ms(50, led_isr);
      WiFi.begin(ssid, pass);
      break;
  }
}

void wif_dtx_isr(void) {
  /**/
  cfg.eep[CH00][RAW] = adc1_get_raw(ADC1_CHANNEL_3);    cfg.eep[CH00][VALUE]    = cfg.eep[CH00][INVER] * mapfloat(cfg.eep[CH00][RAW],   cfg.eep[CH00][INMIN], cfg.eep[CH00][INMAX], cfg.eep[CH00][OUMIN], cfg.eep[CH00][OUMAX]);
  cfg.eep[CH01][RAW] = adc1_get_raw(ADC1_CHANNEL_7);    cfg.eep[CH01][VALUE]    = cfg.eep[CH01][INVER] * mapfloat(cfg.eep[CH01][RAW],   cfg.eep[CH01][INMIN], cfg.eep[CH01][INMAX], cfg.eep[CH01][OUMIN], cfg.eep[CH01][OUMAX]);
  cfg.eep[CH02][RAW] = adc1_get_raw(ADC1_CHANNEL_5);    cfg.eep[CH02][VALUE]    = cfg.eep[CH02][INVER] * mapfloat(cfg.eep[CH02][RAW],   cfg.eep[CH02][INMIN], cfg.eep[CH02][INMAX], cfg.eep[CH02][OUMIN], cfg.eep[CH02][OUMAX]);
  cfg.eep[CH03][RAW] = adc1_get_raw(ADC1_CHANNEL_6);    cfg.eep[CH03][VALUE]    = cfg.eep[CH03][INVER] * mapfloat(cfg.eep[CH03][RAW],   cfg.eep[CH03][INMIN], cfg.eep[CH03][INMAX], cfg.eep[CH03][OUMIN], cfg.eep[CH03][OUMAX]);
              //cfg.eep[CH04][RAW] = analogRead(14);    cfg.eep[CH04][VALUE]    = cfg.eep[CH04][INVER] * mapfloat(cfg.eep[CH04][RAW],   cfg.eep[CH04][INMIN], cfg.eep[CH04][INMAX], cfg.eep[CH04][OUMIN], cfg.eep[CH04][OUMAX]);
              //cfg.eep[CH05][RAW] = analogRead(14);    cfg.eep[CH05][VALUE]    = cfg.eep[CH05][INVER] * mapfloat(cfg.eep[CH05][RAW],   cfg.eep[CH05][INMIN], cfg.eep[CH05][INMAX], cfg.eep[CH05][OUMIN], cfg.eep[CH05][OUMAX]);
              //cfg.eep[CH06][RAW] = analogRead(14);    cfg.eep[CH06][VALUE]    = cfg.eep[CH06][INVER] * mapfloat(cfg.eep[CH06][RAW],   cfg.eep[CH06][INMIN], cfg.eep[CH06][INMAX], cfg.eep[CH06][OUMIN], cfg.eep[CH06][OUMAX]);
              //cfg.eep[CH07][RAW] = analogRead(14);    cfg.eep[CH07][VALUE]    = cfg.eep[CH07][INVER] * mapfloat(cfg.eep[CH07][RAW],   cfg.eep[CH07][INMIN], cfg.eep[CH07][INMAX], cfg.eep[CH07][OUMIN], cfg.eep[CH07][OUMAX]);
              //cfg.eep[CH08][RAW] = analogRead(14);    cfg.eep[CH08][VALUE]    = cfg.eep[CH08][INVER] * mapfloat(cfg.eep[CH08][RAW],   cfg.eep[CH08][INMIN], cfg.eep[CH08][INMAX], cfg.eep[CH08][OUMIN], cfg.eep[CH08][OUMAX]);
  // Battery
  cfg.eep[VOCO][RAW] = adc1_get_raw(ADC1_CHANNEL_0);    cfg.eep[VOCO][VALUE]    = cfg.eep[VOCO][INVER] * mapfloat(cfg.eep[VOCO][RAW],   cfg.eep[VOCO][INMIN], cfg.eep[VOCO][INMAX], cfg.eep[VOCO][OUMIN], cfg.eep[VOCO][OUMAX]);
  cfg.eep[BACO][RAW] = cfg.eep[VOCO][VALUE];  cfg.eep[BACO][VALUE]    = cfg.eep[BACO][INVER] * mapfloat(cfg.eep[BACO][RAW],   cfg.eep[BACO][INMIN], cfg.eep[BACO][INMAX], cfg.eep[BACO][OUMIN], cfg.eep[BACO][OUMAX]);
  // Wifi
  cfg.eep[RSCO][RAW] = WiFi.RSSI();       cfg.eep[RSCO][VALUE]    =                             cfg.eep[RSCO][RAW];
  cfg.eep[WICO][RAW] = cfg.eep[RSCO][VALUE];  cfg.eep[WICO][VALUE]    = cfg.eep[WICO][INVER] * mapfloat(cfg.eep[WICO][RAW],   cfg.eep[WICO][INMIN], cfg.eep[WICO][INMAX], cfg.eep[WICO][OUMIN], cfg.eep[WICO][OUMAX]);
  
  if (connected)
  {
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    //root["rssi"] = val_rssi;
    //root["volt"] = val_volt;
    JsonArray& data = root.createNestedArray("ch");
    data.add(cfg.eep[CH00][VALUE]);
    data.add(cfg.eep[CH01][VALUE]);
    data.add(cfg.eep[CH02][VALUE]);
    data.add(cfg.eep[CH03][VALUE]);
    data.add(cfg.eep[CH04][VALUE]);
    data.add(cfg.eep[CH05][VALUE]);
    data.add(cfg.eep[CH06][VALUE]);
    data.add(cfg.eep[CH07][VALUE]);
    //data.add(cfg.eep[CH08][VALUE]);
    udp.beginPacket(ServerIP, localUdpPort);
    root.printTo(udp);
    udp.endPacket();

    //Serial.print("UDP TX: ");  root.printTo(Serial); Serial.println();
  }
}

void wif_drx(void) {
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

  // Battery
  cfg.eep[VOTO][RAW] = root["volt"];      cfg.eep[VOTO][VALUE]    = cfg.eep[VOTO][INVER] * mapfloat(cfg.eep[VOTO][RAW],   cfg.eep[VOTO][INMIN], cfg.eep[VOTO][INMAX], cfg.eep[VOTO][OUMIN], cfg.eep[VOTO][OUMAX]);
  cfg.eep[BATO][RAW] = cfg.eep[VOTO][VALUE];  cfg.eep[BATO][VALUE]    = cfg.eep[BATO][INVER] * mapfloat(cfg.eep[BATO][RAW],   cfg.eep[BATO][INMIN], cfg.eep[BATO][INMAX], cfg.eep[BATO][OUMIN], cfg.eep[BATO][OUMAX]);
  // Wifi
  //cfg.eep[RSTO][RAW] = root["rssi"];      cfg.eep[RSTO][VALUE]    =                             cfg.eep[RSTO][RAW];
  //cfg.eep[WITO][RAW] = cfg.eep[RSTO][VALUE];  cfg.eep[WITO][VALUE]    = cfg.eep[WITO][INVER] * mapfloat(cfg.eep[WITO][RAW],   cfg.eep[WITO][INMIN], cfg.eep[WITO][INMAX], cfg.eep[WITO][OUMIN], cfg.eep[WITO][OUMAX]);
  }
}

void wif_init() {
  // delete old config
  WiFi.disconnect(true);
  //register event handler
  WiFi.onEvent(WiFiEvent);
  //Initiate connection
  WiFi.begin(ssid, pass);
  Serial.println((String)"- SSID: " + ssid + "  PASS: " + pass);
}

void wif_run() {
  wif_drx();
}

void wifi_scan(void) {
  
}

void wifi_connect(void) {
  
}






