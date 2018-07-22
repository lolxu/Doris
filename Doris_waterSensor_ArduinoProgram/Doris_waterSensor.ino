#include <SoftwareSerial.h>
#include "U8glib.h"

U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_DEV_0|U8G_I2C_OPT_NO_ACK|U8G_I2C_OPT_FAST);

SoftwareSerial BT(10, 11);

byte sensorInterrupt = 0;
byte sensorPin       = 2;

float Q = 4.5;

volatile byte pulseCnt;

float flowRate;
unsigned int flowMilliLitres;
unsigned long totalMilliLitres, tempTot = -1;

unsigned long oldTime;

void setup(){
  BT.begin(9600);
  Serial.begin(9600);

  pinMode(sensorPin, INPUT);
  digitalWrite(sensorPin, HIGH);

  pulseCnt         = 0;
  flowRate         = 0.0;
  flowMilliLitres  = 0;
  totalMilliLitres = 0;
  oldTime          = 0;
  
  // assign default color value
  if ( u8g.getMode() == U8G_MODE_R3G3B2 ) {
    u8g.setColorIndex(255);     // white
  }
  else if ( u8g.getMode() == U8G_MODE_GRAY2BIT ) {
    u8g.setColorIndex(3);         // max intensity
  }
  else if ( u8g.getMode() == U8G_MODE_BW ) {
    u8g.setColorIndex(1);         // pixel on
  }
  else if ( u8g.getMode() == U8G_MODE_HICOLOR ) {
    u8g.setHiColorByRGB(255,255,255);
  }


  displayVolumeOfWater(0);

  attachInterrupt(sensorInterrupt, pulseCounter, FALLING);
}

void pulseCounter(){
  pulseCnt++;
}

void displayVolumeOfWater( int val){

  char buf[5];
  sprintf(buf, "%d", val);
  // picture loop
  u8g.firstPage();
  do {
     u8g.setFont(u8g_font_fub25);
     //u8g.setFont(u8g_font_osb21);
     u8g.drawStr( 0, 32, buf);

     u8g.setFont(u8g_font_fur14);
     u8g.drawStr( 0, 52, "volume ml");
  } while( u8g.nextPage() );

}

void loop(){
  BT.println(totalMilliLitres);
  //BT.println(" mL");
  delay(4000);
  
  if ((millis() - oldTime) > 1000){
    
    detachInterrupt(sensorInterrupt);

    flowRate = ((1000.0 / (millis() - oldTime)) * pulseCnt) / Q;

    oldTime = millis();

    flowMilliLitres = (flowRate / 60) * 1000;

    totalMilliLitres += flowMilliLitres;

    unsigned int frac;

    Serial.print("Flow rate: ");
    Serial.print(int(flowRate));
    Serial.println(";");

    frac = (flowRate - int(flowRate))*10;
    Serial.print(frac, DEC);
    Serial.println(" L/min");

    Serial.print("Current Liquid Floawing: ");
    Serial.print(flowMilliLitres);
    Serial.println(" ml/s");

//    Serial.println(pulseCnt);
//    Serial.println(flowRate);
    //Serial.print(totalMilliLitres);
    //Serial.println("mL");

    if (tempTot != totalMilliLitres){
      tempTot = totalMilliLitres;
      displayVolumeOfWater(totalMilliLitres);
    }
    
    pulseCnt = 0;

    attachInterrupt(sensorInterrupt, pulseCounter, FALLING);
  }
  
}

