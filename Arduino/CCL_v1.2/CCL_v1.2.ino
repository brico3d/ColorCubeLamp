#define VERSION  "\n\ ColorCubeLamp v1.0 - for ColorCubeLampApp\n\ brico3d.blogspot.com - brico3d@gmail.com"

//LED type: ws2812b 5050 RGB led strip
//Control LED pin 6
//Led number: 16
//BlueTooth module: pin#2=RX pin#3=TX
//Bluetooth board a 9600 bps

#include "SoftwareSerial.h"
#include <Adafruit_NeoPixel.h>
#include <FastLED.h>

//Led Strip configuration
#define PIN     6
#define NUMLED  16

//  Fire definitions
#define HEIGHT 4
#define WIDTH 4
#define FPS 16
#define FPS_DELAY 1000/FPS

int COOLING = 2;
int HOT = 105;
int MAXHOT = 120;

CRGB leds[NUMLED];
CRGBPalette16 gPal;

int horizontal[16] = {0,7,8,15,1,6,9,14,2,5,10,13,3,4,11,12};
int vertical[16] = {0,1,2,3,15,14,13,12,7,6,5,4,8,9,10,11};
int cmd[3] = {0, 0, 0};
bool estado = false;
int R = 255, G = 255, B = 255;
int BRIGHTNESS = 50, velocidad = 20;
int modo;

SoftwareSerial mySerial(2,3);                                       //BlueTooth configuration
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMLED, PIN, NEO_GRB);  //LED Configuration

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);
  Serial.println(VERSION);
  while(mySerial.available())  mySerial.read();         // empty RX buffer
  
  FastLED.addLeds<NEOPIXEL, PIN>(leds, NUMLED);
    
  // End of trinket special code
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

void loop() {
  bluetooth();
  
  if (modo=='i'){
    rainbow();
  }
  
  if (modo=='g'){
    rainbowVertical(); 
  }
  
  if (modo=='h'){
    rainbowHorizontal(); 
  }
  
  if (modo=='j'){
    gPal = HeatColors_p;
    HOT = 105;
    MAXHOT = 120;
    Fireplace(); 
  }
  
  if (modo=='k'){
    gPal = OceanColors_p;
    HOT = 125;
    MAXHOT = 130;
    Fireplace(); 
  }
}

void bluetooth(){
    if(mySerial.available()){
    delay(2);
    int i = 0;
    while(mySerial.available()){
      delay(2);
      cmd[i] = mySerial.read();
      if(i>3) break;
      i++;
    }
    
    if (cmd[0]=='m'){
      modo = cmd[1];
      velocidad = 20;
    }
    
    else if (cmd[0]=='e'){
      if (cmd[1]=='a')         estado=true;
      else if (cmd[1]=='b')    estado=false;
      modo = 0;
    }
    
    else if(cmd[0]=='l'){
      if(cmd[1]=='a'){
        if(BRIGHTNESS <= 80){
          BRIGHTNESS +=5;
        }
        if(BRIGHTNESS < 255 && BRIGHTNESS > 80){
          BRIGHTNESS +=10;
        }
      }
      else if(cmd[1]=='b'){
        if(BRIGHTNESS <= 80 && BRIGHTNESS >0){
          BRIGHTNESS -=5;
        }
        if(BRIGHTNESS > 80){
          BRIGHTNESS -=10;
        }
      }
    }
   
    else if(cmd[0]=='v'){
      if (cmd[1]=='a')         velocidad=10;
      else if (cmd[1]=='b')    velocidad=30;     
    }
    
    else{
      R= cmd[0];
      G= cmd[1];
      B= cmd[2];
      modo = 0;
    }
    Serial.print("Estado: "); Serial.print(estado);
    Serial.print("; Brillo: "); Serial.print(BRIGHTNESS);
    Serial.print("; Velocidad: "); Serial.print(velocidad);
    Serial.print("; Modo: "); Serial.print(modo);
    Serial.print("; Color: "); Serial.print(R);
    Serial.print(" "); Serial.print(G);
    Serial.print(" "); Serial.println(B);
    
    if (modo == 0){
      if (estado == true){
        for(int i = 0; i<NUMLED; i++){
          strip.setPixelColor(i, strip.Color(R,G,B));
          strip.setBrightness(BRIGHTNESS);
          strip.show();
        }
      }
      if (estado == false){
        for(int i = 0; i<NUMLED; i++){
          R=255;  G=255;  B=255;
          strip.setPixelColor(i, strip.Color(0,0,0));
          strip.show();
        }
      }
    } 
  }
}

void rainbow() {
  uint16_t i, j;
  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.setBrightness(BRIGHTNESS);
    strip.show();
    delay(velocidad);
    bluetooth();
    if(modo==0 || modo =='g' || modo == 'h' || modo =='j'  || modo =='k') return;
  }
}

void rainbowHorizontal() {
  uint16_t i, j;

  for(j=256; j>0; j--) {
    for(int i=0; i<16; i+=4){
      for(int k=0; k<4 ;k++){   
        strip.setPixelColor(horizontal[i+k], Wheel(((i * 256 / (2*strip.numPixels()))+j) & 255));
      }
    }
    strip.setBrightness(BRIGHTNESS);  
    strip.show();
    delay(velocidad);
    bluetooth();
    if(modo==0  || modo =='i' || modo =='g' || modo =='j'  || modo =='k') return;
  }
}

void rainbowVertical() {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(int i=0; i<16; i+=4){
      for(int k=0; k<4 ;k++){   
        strip.setPixelColor(vertical[i+k], Wheel(((i * 256 / (2*strip.numPixels()))+j) & 255));
      }
    }  
    strip.setBrightness(BRIGHTNESS);
    strip.show();
    delay(velocidad);
    bluetooth();
    if(modo==0  || modo =='i' || modo =='h' || modo =='j'  || modo =='k') return;
  }
}

uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

void Fireplace () {
  static unsigned int spark[WIDTH]; // base heat
  CRGB stack[WIDTH][HEIGHT];        // stacks that are cooler
  
  // 1. Generate sparks to re-heat
  for( int i = 0; i < WIDTH; i++) {
    if (spark[i] < HOT ) {
      int base = HOT * 1.2;
      spark[i] = random16( base, MAXHOT );
    }
  }
  
  // 2. Cool all the sparks
  for( int i = 0; i < WIDTH; i++) {
    spark[i] = qsub8( spark[i], random8(0, COOLING) );
  }
  
  // 3. Build the stack
  /*    This works on the idea that pixels are "cooler"
        as they get further from the spark at the bottom */
  for( int i = 0; i < WIDTH; i++) {
    unsigned int heat = constrain(spark[i], HOT/2, MAXHOT);
    for( int j = HEIGHT-1; j >= 0; j--) {
      /* Calculate the color on the palette from how hot this
         pixel is */
      byte index = constrain(heat, 0, HOT);
      stack[i][j] = ColorFromPalette( gPal, index );
      
      /* The next higher pixel will be "cooler", so calculate
         the drop */
      unsigned int drop = random8(0,HOT);
      if (drop > heat) heat = 0; // avoid wrap-arounds from going "negative"
      else heat -= drop;
 
      heat = constrain(heat, 0, 120);
    }
  }
  
  // 4. map stacks to led array
  for( int i = 0; i < WIDTH; i++) {
    for( int j = 0; j < HEIGHT; j++) {
     leds[vertical[j]] = stack[3-i][3-j];
     leds[vertical[j+4]] = stack[3-i][3-j];
     leds[vertical[j+8]] = stack[3-i][3-j];
     leds[vertical[j+12]] = stack[3-i][3-j];
    }
  }
  
  FastLED.show();
  FastLED.delay(FPS_DELAY); //
  bluetooth();
  if(modo==0  || modo =='i' || modo =='h' || modo =='g') return;
}
