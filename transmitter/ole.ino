

// Range Battery  sdf
#define BATT_VOLT_FULL  4.2
#define BATT_VOLT_EMPT  3.0

// Range Wifi
#define WIFI_RSSI_FULL  58
#define WIFI_RSSI_EMPT  88

#define ARRAY_SIZE(x)  (sizeof(x) / sizeof(*x))

/*************************** Oled Variables ************************************/
#define OLED_SDA    21  // pin 14
#define OLED_SDC    22  // pin 12
#define OLED_ADDR   0x3C
SSD1306Wire   mOled(OLED_ADDR, OLED_SDA, OLED_SDC);    // For I2C

/*************************** source ************************************/

char s[10];

void ole_init() {
  mOled.init();
  mOled.displayOn();
  mOled.setContrast(88);
  mOled.flipScreenVertically();
  mOled.setFont(ArialMT_Plain_10);
  mOled.setTextAlignment(TEXT_ALIGN_LEFT);
  mOled.clear();
  mOled.display();
  mOled.flipScreenVertically();
}

void ole_isr(void) {
  ole_state = true;
}


void draw_serv(float val, int xs, int ys, int w, int h, bool frame) {

  int val_min = -100;
  int val_max = +100;

  int m       = w / 2;
  int pix_val = 0;

  if (frame)
    mOled.drawRect(xs, ys, w, h);

  //mOled.drawLine(xs + m, ys + 1, xs + m, ys + h - 2);
  mOled.setPixel(xs + m, ys + 1);
  mOled.setPixel(xs + m, ys + h - 2);
  if (val >= 0)  {
    pix_val = mapfloat(val, 0, val_max, 0, m - 1);
    mOled.fillRect(xs + m , ys + 2, pix_val, h - 4);
  }
  else  {
    pix_val = mapfloat(-val, 0, val_max, 0, m - 1);
    mOled.fillRect(xs + m - pix_val + 1, ys + 2, pix_val, h - 4);
  }
}

void draw_batt(float val, int xs, int ys, int w, int h) {
  mOled.fillRect(xs + w - 2, ys + 2, 2, h - 4);
  draw_barg(val, xs, ys, w - 2, h, true, 0, 100);
}

void draw_wifi(float val, int xs, int ys, int w, int h) {
  mOled.fillRect(xs + w - 2, ys + 2, 2, h - 4);
  draw_barg(val, xs, ys, w - 2, h, true, 0, 100);
}

void draw_barg(float val, int xs, int ys, int w, int h, bool frame, float val_min, float val_max) {
  if (frame)  mOled.drawRect(xs, ys, w, h);

  int pix_val = mapfloat(val, val_min, val_max, 0, w - 4);
  mOled.fillRect(xs + 2, ys + 2, pix_val, h - 4);
}


typedef void (*funcptr_t) (void);

struct menu_t;

typedef struct menu_item_t {
  const char       text[10];
  menu_t          *menu_target;
  int8_t           valu_target;
  funcptr_t        func_target;
} menu_item_t;

typedef struct menu_t {
  const char       text[10];          // text
  int8_t          idx_selected;      // idx_selected
  int8_t          itemcount;         // itemcount
  menu_item_t     *item;              // item[]
  menu_t          *menu_parent;       // menu_parent
  funcptr_t        func_draw;         // func_draw
  funcptr_t        func_encoder;      // func_encoder
} menu_t;


extern  menu_t menuMain;
extern  menu_t menuSub0;

menu_item_t menuWifiItems[] = {
  { "Select" , NULL , -1 , NULL},
  { "Scan" , NULL , -1 , NULL},  
  { "exit"  , NULL , -1    , fct_exit}
};

menu_t menuWifi = {
  "Wifi",                       // text
  0,                            // idx_selected
  ARRAY_SIZE( menuWifiItems ),  // itemcount
  menuWifiItems,                // items[]
  NULL,                         // menu_parent
  draw_menu_list,               // func_draw
  fct_itema                     // func_encoder
};

menu_item_t menuSaveItems[] = {
  { "Save"    , NULL , -1 , fct_save},
  { "Load"    , NULL , -1 , fct_load},  
  { "Reset"   , NULL , -1 , fct_reset},  
  { "exit"    , NULL , -1 , fct_exit}
};

menu_t menuSave = {
  "Save",                       // text
  0,                            // idx_selected
  ARRAY_SIZE( menuSaveItems ),  // itemcount
  menuSaveItems,                // items[]
  NULL,                         // menu_parent
  draw_menu_list,               // func_draw
  fct_itema                     // func_encoder
};

menu_item_t menuAlarItems[] = {
  { "LowBatt"  , NULL , -1 , NULL},
  { "Distance" , NULL , -1 , NULL},  
  { "Wifi"     , NULL , -1 , NULL},    
  { "exit"     , NULL , -1 , fct_exit}
};

menu_t menuAlar = {
  "Alarm",                       // text
  0,                            // idx_selected
  ARRAY_SIZE( menuAlarItems ),  // itemcount
  menuAlarItems,                // items[]
  NULL,                         // menu_parent
  draw_menu_list,               // func_draw
  fct_itema                     // func_encoder
};

menu_item_t menuViewItems[] = {
  { "View" , NULL , -1 , NULL}
};

menu_t menuView = {
  "View",                       // text
  0,                            // idx_selected
  ARRAY_SIZE( menuViewItems ),  // itemcount
  menuViewItems,                // items[]
  NULL,                         // menu_parent
  draw_menu_view,               // func_draw
  fct_itema                     // func_encoder
};

menu_item_t menuEditItems[] = {
  { "value" , NULL , VALUE , NULL},
  { "raw"   , NULL , RAW   , NULL},  
  { "inver" , NULL , INVER , fct_inver},
  { "inmin" , NULL , INMIN , fct_inmxx},
  { "inmax" , NULL , INMAX , fct_inmxx},
  { "oumin" , NULL , OUMIN , fct_oumxx},
  { "oumax" , NULL , OUMAX , fct_oumxx},
  { "exit"  , NULL , -1    , fct_exit}
};

menu_t menuEdit = {
  "Edit",                       // text
  0,                            // idx_selected
  ARRAY_SIZE( menuEditItems ),  // itemcount
  menuEditItems,                // items[]
  NULL,                         // menu_parent
  draw_menu_edit,               // func_draw
  fct_itema                     // func_encoder
};

menu_item_t menuChanItems[] = {
  { "ch0"   , &menuEdit , CH00 , NULL},
  { "ch1"   , &menuEdit , CH01 , NULL},
  { "ch2"   , &menuEdit , CH02 , NULL},
  { "ch3"   , &menuEdit , CH03 , NULL},
  { "ch4"   , &menuEdit , CH04 , NULL},
  { "ch5"   , &menuEdit , CH05 , NULL},
  { "ch6"   , &menuEdit , CH06 , NULL},
  { "ch7"   , &menuEdit , CH07 , NULL},
  { "exit"  , NULL      , -1   , fct_exit}
};

menu_t menuChan = {
  "channel",                    // text
  0,                            // idx_selected
  ARRAY_SIZE( menuChanItems ),  // itemcount
  menuChanItems,                // items[]
  NULL,                         // menu_parent
  draw_menu_chan,               // func_draw
  fct_itema                     // func_encoder
};

menu_item_t menuSystItems[] = {
  { "Con Rssi"   , &menuEdit , RSCO , NULL},
  { "Con Wifi"   , &menuEdit , WICO , NULL},
  { "Con Volt"   , &menuEdit , VOCO , NULL},
  { "Con Batt"   , &menuEdit , BACO , NULL},
  { "Toy Volt"   , &menuEdit , VOTO , NULL},
  { "Toy Batt"   , &menuEdit , BATO , NULL},
  { "Distance"   , &menuEdit , DIST , NULL},
  { "exit"       , NULL      , -1   , fct_exit}
};  

menu_t menuSyst = {
  "System",                     //
  0,                            // idx_selected
  ARRAY_SIZE( menuSystItems ),
  menuSystItems,
  NULL,
  draw_menu_list,
  fct_itema
};

menu_item_t menuMainItems[] = {
  { "View"   , &menuView, -1 , NULL },  
  { "Channel", &menuChan, -1 , NULL },
  { "System" , &menuSyst, -1 , NULL },
  { "Alarm"  , &menuAlar, -1 , NULL },
  { "Wifi"   , &menuWifi, -1 , NULL },
  { "Save"   , &menuSave, -1 , NULL }  
};

menu_t menuMain = {
  "main",
  0,                            // idx_selected
  ARRAY_SIZE( menuMainItems ),
  menuMainItems,
  NULL,
  draw_menu_list,
  fct_itema
};

//und zuguter letzt haben wir noch den Pointer auf das zunächst oberste Menü:
menu_t *menu_act = &menuMain;
bool    edit_act = false;


void fct_save(void) {
  EEPROM.put(0, uint8_t(253));
  EEPROM.put(8, cfg);
  EEPROM.commit();   
  Serial.println("config saved");    
  
}

void fct_load(void) {
  EEPROM.get(8, cfg);
  Serial.println("config loaded");  
  uint8_t eep = 128;
  EEPROM.get(0, eep);
  Serial.print("eep: ");  Serial.println(eep);   
}

void fct_reset(void) {
  Serial.println("reset");    
  ESP.restart();
}


void fct_exit(void) {
  if (menu_act->menu_parent != NULL) 
  menu_act = menu_act->menu_parent;
}
void fct_itema(void) {
  int val = rotaryEncoder.readEnc4();
  if (val < 0)  {
    menu_act->idx_selected--;
    if (menu_act->idx_selected < 0) menu_act->idx_selected = menu_act->itemcount - 1;
  }
  if (val > 0)  {
    menu_act->idx_selected++;
    if (menu_act->idx_selected > menu_act->itemcount - 1) menu_act->idx_selected = 0;
  }
}
void fct_inver(void) {
  int val = rotaryEncoder.readEnc4();

  int r = menu_act->menu_parent->item[menu_act->menu_parent->idx_selected].valu_target;
  int c = menu_act->item[menu_act->idx_selected].valu_target;
  
  cfg.eep[r][c] = -cfg.eep[r][c];
}
void fct_inmxx(void) {
  int val = rotaryEncoder.readEnc4();
  
  int r = menu_act->menu_parent->item[menu_act->menu_parent->idx_selected].valu_target;
  int c = menu_act->item[menu_act->idx_selected].valu_target;
  
  float var = cfg.eep[r][c];
  var += val * 1.0;
  if (var <        0)   var =       0;
  if (var >  ADC_Max)   var = ADC_Max;
  cfg.eep[r][c] = var;
}
void fct_oumxx(void) {
  int val = rotaryEncoder.readEnc4();

  int r = menu_act->menu_parent->item[menu_act->menu_parent->idx_selected].valu_target;
  int c = menu_act->item[menu_act->idx_selected].valu_target;
  
  float var = cfg.eep[r][c];
  var += val * 0.5;
  if (var < -100)   var = -100;
  if (var >  100)   var = 100;
  cfg.eep[r][c] = var;
}

void draw_header() {
  mOled.setColor(INVERSE);
  mOled.setFont(ArialMT_Plain_10);
  mOled.setTextAlignment(TEXT_ALIGN_CENTER);
  if(menu_act->menu_parent != NULL)
  mOled.drawString(64, 0, menu_act->menu_parent->item[menu_act->menu_parent->idx_selected].text);//menu_act->text);

  mOled.setTextAlignment(TEXT_ALIGN_LEFT);
  dtostrf(cfg.eep[WICO][VALUE], 3, 0, s); mOled.drawString(0, 0, s);
  //draw_wifi(cfg.eep[WICO][VALUE], 0, 0, 20, 10);
  mOled.setTextAlignment(TEXT_ALIGN_RIGHT);

  //dtostrf(cfg.eep[VOCO][VALUE], 3, 0, s); mOled.drawString(64, 0, s);
  dtostrf(cfg.eep[BACO][VALUE], 3, 0, s); mOled.drawString(106, 0, s);
   
  draw_batt(cfg.eep[BACO][VALUE], 108, 1, 20, 9);
  mOled.drawHorizontalLine(0, 11, 128);
}

void draw_menu_list(void) {
  mOled.setColor(INVERSE);//WHITE
  mOled.setFont(ArialMT_Plain_10);
  mOled.setTextAlignment(TEXT_ALIGN_LEFT);

  uint8_t ende  = 5;
  uint8_t line  = 12;
  uint8_t idx   = 0;

  if (menu_act->itemcount < 5)
    ende = menu_act->itemcount;

  if (menu_act->idx_selected > 4)
    idx = menu_act->idx_selected - 4;

  for ( uint8_t i = 0; i < ende; i++ ) {
    if (idx == menu_act->idx_selected)
      mOled.drawRect(0, line + 1, 128, 11);

    // Name
    mOled.setTextAlignment(TEXT_ALIGN_LEFT);
    mOled.drawString(3, line, menu_act->item[idx].text);

    if (menu_act->item[idx].valu_target != -1) {
      mOled.setTextAlignment(TEXT_ALIGN_RIGHT);
      dtostrf(cfg.eep[menu_act->item[idx].valu_target][VALUE], 5, 2, s);
      mOled.drawString( 100, line, s);
    }

    line += 10;
    idx  +=  1;
  }
}

void draw_menu_edit(void) {
  mOled.setColor(INVERSE);//WHITE
  mOled.setFont(ArialMT_Plain_10);
  mOled.setTextAlignment(TEXT_ALIGN_LEFT);

  uint8_t ende  = 5;
  uint8_t line  = 12;
  uint8_t idx   = 0;

  if (menu_act->itemcount < 5)
    ende = menu_act->itemcount;

  if (menu_act->idx_selected > 4)
    idx = menu_act->idx_selected - 4;

  int r = menu_act->menu_parent->item[menu_act->menu_parent->idx_selected].valu_target;
    
  for ( uint8_t i = 0; i < ende; i++ ) {
    mOled.setTextAlignment(TEXT_ALIGN_LEFT);
    if (idx == menu_act->idx_selected)
      if (edit_act) mOled.fillRect(0, line + 1, 128, 11);
      else          mOled.drawRect(0, line + 1, 128, 11);
    mOled.drawString(3, line, menu_act->item[idx].text);
    mOled.setTextAlignment(TEXT_ALIGN_RIGHT);
     if (menu_act->item[idx].valu_target != -1) {
    dtostrf(cfg.eep[r][menu_act->item[idx].valu_target], 5, 2, s);
    mOled.drawString( 80, line, s);
     }
    
    line += 10;
    idx  +=  1;
  }
}

void draw_menu_chan() {
  mOled.setColor(INVERSE);
  mOled.setFont(ArialMT_Plain_10);

  uint8_t ende  = 5;
  uint8_t line  = 12;
  uint8_t idx   = 0;


  if (menu_act->itemcount < 5)
    ende = menu_act->itemcount;

  if (menu_act->idx_selected > 4)
    idx = menu_act->idx_selected - 4;

  for ( uint8_t i = 0; i < ende; i++ ) {
    if (idx == menu_act->idx_selected)
      mOled.drawRect(0, line + 1, 128, 10);

    mOled.setTextAlignment(TEXT_ALIGN_LEFT);
    mOled.drawString( 1, line, menu_act->item[idx].text);

     if (menu_act->item[idx].valu_target != -1) {
    mOled.setTextAlignment(TEXT_ALIGN_RIGHT);
    dtostrf  (cfg.eep[idx][VALUE], 5, 1, s); mOled.drawString( 53, line, s);
    draw_serv(cfg.eep[idx][VALUE], 55, line + 2, 60, 8, true);
    dtostrf  (cfg.eep[idx][INVER], 2, 0, s); mOled.drawString( 128, line, s);
     }

    line += 10;
    idx  +=  1;
  }
  mOled.setTextAlignment(TEXT_ALIGN_LEFT);
}

void draw_menu_view() {
  mOled.setColor(INVERSE);
  mOled.setFont(ArialMT_Plain_10);

  uint8_t ende  = 5;
  uint8_t line  = 12;
  uint8_t idx   = 0;


  if (menu_act->itemcount < 5)
    ende = menu_act->itemcount;

  if (menu_act->idx_selected > 4)
    idx = menu_act->idx_selected - 4;

  for ( uint8_t i = 0; i < ende; i++ ) {
    if (idx == menu_act->idx_selected)
      mOled.drawRect(0, line + 1, 128, 10);

    mOled.setTextAlignment(TEXT_ALIGN_LEFT);
    mOled.drawString( 1, line, menu_act->item[idx].text);

     if (menu_act->item[idx].valu_target != -1) {
    mOled.setTextAlignment(TEXT_ALIGN_RIGHT);
    dtostrf  (cfg.eep[idx][VALUE], 5, 1, s); mOled.drawString( 53, line, s);
    draw_serv(cfg.eep[idx][VALUE], 55, line + 2, 60, 8, true);
    dtostrf  (cfg.eep[idx][INVER], 2, 0, s); mOled.drawString( 128, line, s);
     }

    line += 10;
    idx  +=  1;
  }
  mOled.setTextAlignment(TEXT_ALIGN_LEFT);
}


void ole_run(void) {
  mOled.clear();
  draw_header();
  menu_act->func_draw();
  mOled.display();
}

void inp_isrBtnLong() {
if ( edit_act == false && menu_act->menu_parent != NULL) 
    menu_act = &menuMain;// menu_act->menu_parent;
}

void inp_isrBtnShort() {
  // next sub menu ?
  if ( menu_act->item[menu_act->idx_selected].menu_target != NULL ) {
    menu_t *menu_tmp = menu_act;
    menu_act = menu_act->item[menu_act->idx_selected].menu_target;      // Ja: führe sie aus
    menu_act->menu_parent = menu_tmp;
    menu_act->idx_selected = 0;  
    //Serial.println("run next menue");
  }
  // editable ?
  else if (menu_act->item[menu_act->idx_selected].valu_target != -1 
        && menu_act->item[menu_act->idx_selected].func_target != NULL){
    edit_act = edit_act ? false : true;
    //Serial.println("run edit");
  }
  // check run func --> exit
  else if(menu_act->item[menu_act->idx_selected].func_target != NULL 
       && menu_act->item[menu_act->idx_selected].valu_target == -1){
    menu_act->item[menu_act->idx_selected].func_target();
    //Serial.println("run funct");
  }
  
  //Serial.printf("BtnShort: %d : %d\n", menu_act->menu_parent->idx_selected, menu_act->idx_selected);  
}

void inp_isrEnc() {
  if (edit_act && menu_act->item[menu_act->idx_selected].func_target!= NULL)   
    menu_act->item[menu_act->idx_selected].func_target();
  else if(menu_act->func_encoder != NULL)            
    menu_act->func_encoder();
}




