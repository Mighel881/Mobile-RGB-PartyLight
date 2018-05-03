#include <FastLED.h>
#include <math.h>
#include <pixelmatrix.h>

#define LED_PIN     2
#define ROTA1_PIN   14
#define ROTA2_PIN   16
#define ROTAB_PIN   12
#define ROTB1_PIN   5
#define ROTB2_PIN   4
#define ROTBB_PIN   0
#define MAT_W       15
#define MAT_H       15
#define NUM_LEDS    225
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB


byte row_1[8]  = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00  };
byte row_2[8]  = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00  };
byte row_3[8]  = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00  };
byte row_4[8]  = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00  };
byte row_5[8]  = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00  };

byte row_6[8]  = { 0x7c, 0x38, 0x7c, 0x7c, 0x40, 0x40, 0x38, 0x44  };
byte row_7[8]  = { 0x44, 0x44, 0x10, 0x40, 0x40, 0x40, 0x44, 0x64  };
byte row_8[8]  = { 0x78, 0x44, 0x10, 0x78, 0x40, 0x40, 0x44, 0x54  };
byte row_9[8]  = { 0x44, 0x44, 0x10, 0x40, 0x40, 0x40, 0x44, 0x4c  };
byte row_10[8] = { 0x7c, 0x38, 0x10, 0x7c, 0x7c, 0x7c, 0x38, 0x44  };

byte row_11[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00  };
byte row_12[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00  };
byte row_13[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00  };
byte row_14[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00  };
byte row_15[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00  };


unsigned long   currentTime;
unsigned long   loopTime;

unsigned long   currentTime2;
unsigned long   loopTime2;

int             valueA              = 128;  // Helligkeit
int             buttonA             = 1;    // An / Aus
unsigned char   btnA;
unsigned char   btnA_prev           = 0;
unsigned char   rotA_1;
unsigned char   rotA_2;
unsigned char   rotA_1_prev         = 0;
unsigned int    rotA_resolution     = 20;
unsigned int    textRotation        = 90;
unsigned int    textRotationA_resolution     = 90;

int             valueB              = 6;    // Farbe / Frequenz / Speed
int             buttonB             = 6;    // Mode
unsigned char   btnB;
unsigned char   btnB_prev           = 0;
unsigned char   rotB_1;
unsigned char   rotB_2;
unsigned char   rotB_1_prev         = 0;
unsigned int    rotB_resolution     = 20;

int             rainbow_i           = 0;
int             rainbow_j           = 0;

int             brightness          = 128;  // gets up to 255
int             color_change_degree = 100;    // current Color / gets up to 255
int             color_change_offset = 50;   // to be addet on current CSV Value
int             color_change_delay  = 1000;  // gets up to 15000 in 500 steps
int             color_fade_delay    = 40;  // gets up to 15000 in 500 steps
int             color_flow_delay    = 20;  // gets up to 15000 in 500 steps
int             text_flow_delay     = 50;
float           strobe_factor       = 3.00;    // gets up to 10 in 0.5 steps
int             strobe_value        = 209;
int             rotation            = 0;
int             textPos             = -16;
int             textWait            = 0;
int             random_trigger      = random(10000, 1800000);
unsigned int    mode                = 5;    // 1: Color, 2: Strobe, 3: rainbowFade, 4: Color Change, 5: rainbowCycle

bool            textTrigger         = false; 

CRGB            persistent_color    = CRGB::White;
CHSV            change_color        = CHSV(color_change_degree, 255, 255);

CRGB leds[NUM_LEDS];

pixelmatrix pm (MAT_W, MAT_H, 0, 4);
pixelmap mat = pixelmap(MAT_W, MAT_H, 0, 1);

void setup() 
{
  Serial.begin (9600);
  Serial.println("start Setup");   
  //delay( 3000 ); // power-up safety delay
  pinMode(ROTA1_PIN, INPUT);
  pinMode(ROTA2_PIN, INPUT);
  pinMode(ROTAB_PIN, INPUT);
  pinMode(ROTB1_PIN, INPUT);
  pinMode(ROTB2_PIN, INPUT);
  pinMode(ROTBB_PIN, INPUT);
  currentTime = millis();
  loopTime = currentTime;
  currentTime2 = millis();
  loopTime2 = currentTime;
  // FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  // FastLED.setBrightness(brightness);
  pm.setBrightness(brightness);
  color(CRGB::White);
  // FastLED.show();
  Serial.println("finish Setup");  
}

void color(CRGB color)
{
  pm.fill(color);
//  if(color == CHSV(0, 255, 255)){
//    color = CRGB::White;
//  }
//  for(int i = 0;i < NUM_LEDS;i++){
//    leds[i] = color;
//  }
}

void rainbowCycle() 
{
  EVERY_N_MILLIS_I(thistimer1, color_flow_delay){
    rainbow_j++;
    rainbow_j = doOverflow(rainbow_j, 0, 255);
//    for(rainbow_i=0; rainbow_i< NUM_LEDS; rainbow_i++) {
//      leds[rainbow_i] = CHSV((rainbow_i * 256 / NUM_LEDS)+rainbow_j, 255, 255);
//    }
//    pm.setStrip(leds);
//    FastLED.show();
    for(int h = 0;h<MAT_H;h++) {
      for(int w = 0;w<MAT_W;w++) {
        if(rotation % 90 != 0) { // 45deg
          pm(w,h) = CHSV(((h*w) * 256 / NUM_LEDS)+rainbow_j, 255, 255);
        }else{
          pm(w,h) = CHSV(((h*MAT_W+w) * 256 / NUM_LEDS)+rainbow_j, 255, 255);
        }
      }
    }
    if(textTrigger) {
      textWait++;
      if(textWait >= 4){
        textPos++; 
        textWait = 0; // do this to scroll
      }
      for(int b = 0;b<8;b++) {
        if(textRotation == 90) {
          drawBitmap90(b, b*6-textPos, CRGB::Black);
        }else{
          drawBitmap(b, b*6-textPos, CRGB::Black);
        }
      }
      if(textPos > 48) {
        textPos = -16;// do this when reset
        textTrigger = false;
      }
    }
    pm.show();
  }
  thistimer1.setPeriod(color_flow_delay);
}

void rainbowFade() 
{
  EVERY_N_MILLIS_I(thistimer2, color_fade_delay){
    rainbow_i++;
    rainbow_i = doOverflow(rainbow_i, 0, 255);
    color(CHSV(rainbow_i, 255, 255));
    // FastLED.show();
  }
  thistimer2.setPeriod(color_fade_delay);
}

void strobe() 
{
  EVERY_N_MILLIS_I(thistimer3, strobe_value){
    color(CHSV(color_change_degree, 255, 255));
    // FastLED.show();
    delay(10);
    color(CHSV(255, 255, 0));
    // FastLED.show();
  }
  thistimer3.setPeriod(strobe_value);
}

void colorChange() 
{
  EVERY_N_MILLIS_I(thistimer4, color_change_delay){
    color_change_degree += color_change_offset;
    color_change_degree = doOverflow(color_change_degree, 0, 255);
    color(CHSV(color_change_degree, 255, 255));
    // FastLED.show();
  }
  thistimer4.setPeriod(color_change_delay);
}

void displayBotellon(CRGB mat_color)
{
  EVERY_N_MILLIS_I(thistimer5, text_flow_delay){
    
    textPos++;
    color(CRGB::Black);
    for(int b = 0;b<8;b++) {
      if(textRotation == 90) {
        drawBitmap90(b, b*6-textPos, mat_color);
      }else{
        drawBitmap(b, b*6-textPos, mat_color);
      }
    }
    pm.show();
    if(textPos > 48) textPos = -16;
    
  }
  thistimer5.setPeriod(text_flow_delay);
}

void drawBitmap(int pos, int offset, CRGB color)
{
  for(int w = offset;w<offset+5;w++) {
    int invert = map(w-offset, 0, 8, 8, 0);
    if(bitRead(row_1[pos], invert-2) == 1) pm(w,0) = color;
    if(bitRead(row_2[pos], invert-2) == 1) pm(w,1) = color;
    if(bitRead(row_3[pos], invert-2) == 1) pm(w,2) = color;
    if(bitRead(row_4[pos], invert-2) == 1) pm(w,3) = color;
    if(bitRead(row_5[pos], invert-2) == 1) pm(w,4) = color;
    if(bitRead(row_6[pos], invert-2) == 1) pm(w,5) = color;
    if(bitRead(row_7[pos], invert-2) == 1) pm(w,6) = color;
    if(bitRead(row_8[pos], invert-2) == 1) pm(w,7) = color;
    if(bitRead(row_9[pos], invert-2) == 1) pm(w,8) = color;
    if(bitRead(row_10[pos], invert-2) == 1) pm(w,9) = color;
    if(bitRead(row_11[pos], invert-2) == 1) pm(w,10) = color;
    if(bitRead(row_12[pos], invert-2) == 1) pm(w,11) = color;
    if(bitRead(row_13[pos], invert-2) == 1) pm(w,12) = color;
    if(bitRead(row_14[pos], invert-2) == 1) pm(w,13) = color;
    if(bitRead(row_15[pos], invert-2) == 1) pm(w,14) = color;
  }
}

void drawBitmap90(int pos, int offset, CRGB color)
{
  for(int h = offset;h<offset+5;h++) {
    int invert = map(h-offset, 0, 8, 8, 0);
    if(bitRead(row_1[pos], invert-2) == 1) pm(0, h) = color;
    if(bitRead(row_2[pos], invert-2) == 1) pm(1, h) = color;
    if(bitRead(row_3[pos], invert-2) == 1) pm(2, h) = color;
    if(bitRead(row_4[pos], invert-2) == 1) pm(3, h) = color;
    if(bitRead(row_5[pos], invert-2) == 1) pm(4, h) = color;
    if(bitRead(row_6[pos], invert-2) == 1) pm(5, h) = color;
    if(bitRead(row_7[pos], invert-2) == 1) pm(6, h) = color;
    if(bitRead(row_8[pos], invert-2) == 1) pm(7, h) = color;
    if(bitRead(row_9[pos], invert-2) == 1) pm(8, h) = color;
    if(bitRead(row_10[pos], invert-2) == 1) pm(9, h) = color;
    if(bitRead(row_11[pos], invert-2) == 1) pm(10, h) = color;
    if(bitRead(row_12[pos], invert-2) == 1) pm(11, h) = color;
    if(bitRead(row_13[pos], invert-2) == 1) pm(12, h) = color;
    if(bitRead(row_14[pos], invert-2) == 1) pm(13, h) = color;
    if(bitRead(row_15[pos], invert-2) == 1) pm(14, h) = color;
  }
}

void readRot() 
{
  currentTime = millis();
  if(currentTime >= (loopTime + 5)) {  // 5ms since last check of encoder = 200Hz 
    
    loopTime = currentTime;           // Updates loopTime
     
    rotA_1_prev = rotA_1;               // Store value of A for next time 
    rotA_1 = digitalRead(ROTA1_PIN);    // Read encoder pins
    rotA_2 = digitalRead(ROTA2_PIN);   
    
    rotB_1_prev = rotB_1;               // Store value of A for next time 
    rotB_1 = digitalRead(ROTB1_PIN);    // Read encoder pins
    rotB_2 = digitalRead(ROTB2_PIN);  
    
    if((!rotA_1) && (rotA_1_prev)){
      // A has gone from high to low 
      if(rotA_2) {
        valueA+=rotA_resolution;          
        textRotation+=textRotationA_resolution;
      } else {
        valueA-=rotA_resolution; 
        textRotation-=textRotationA_resolution;              
      }   
      valueA = preventOverflow(valueA, 0, 255);
      textRotation = preventOverflow(textRotation, 0, 90);
    }
    
    if((!rotB_1) && (rotB_1_prev)){
      // B has gone from high to low 
      if(rotB_2) {
        switch(mode){
          case 1:
            color_change_degree += color_change_offset;
            color_change_degree = doOverflow(color_change_degree, 0, 255);
          break;
          case 2:
            strobe_factor += 0.5;
            strobe_factor = preventOverflow(strobe_factor, 3, 10);
            strobe_value = round(pow(strobe_factor, 3.32));
          break;
          case 3:
            color_fade_delay += 50;
            color_fade_delay = preventOverflow(color_fade_delay, 50, 500);
          break;
          case 4:
            color_change_delay += 500;
            color_change_delay = preventOverflow(color_change_delay, 500, 10000);
          break;
          case 5:
            color_flow_delay += 10;
            color_flow_delay = preventOverflow(color_flow_delay, 10, 100);
            rotation = doOverflow(rotation + 45, 0, 360);
            pm.setRotation(rotation);
          break;
          case 6:
            text_flow_delay += 10;
            text_flow_delay = preventOverflow(text_flow_delay, 10, 100);
          break;
          default:
            valueB += rotB_resolution;
            valueB = preventOverflow(valueB, 1, 255);
          break;
        }
      } else {
        switch(mode){
          case 1:
            color_change_degree -= color_change_offset;
            color_change_degree = doOverflow(color_change_degree, 0, 255);
          break;
          case 2:
            strobe_factor -= 0.5;
            strobe_factor = preventOverflow(strobe_factor, 3, 10);
            strobe_value = round(pow(strobe_factor, 3.32));
          break;
          case 3:
            color_fade_delay -= 50;
            color_fade_delay = preventOverflow(color_fade_delay, 50, 500);
          break;
          case 4:
            color_change_delay -= 500;
            color_change_delay = preventOverflow(color_change_delay, 500, 10000);
          break;
          case 5:
            color_flow_delay -= 10;
            color_flow_delay = preventOverflow(color_flow_delay, 10, 100);
            rotation = doOverflow(rotation - 45, 0, 360);
            pm.setRotation(rotation);
          break;
          case 6:
            text_flow_delay -= 10;
            text_flow_delay = preventOverflow(text_flow_delay, 10, 100);
          break;
          default:
            valueB -= rotB_resolution;
            valueB = preventOverflow(valueB, 1, 255);
          break;
        }
      }
    }
  }
}

void readBtn()
{
  currentTime2 = millis();
  if(currentTime2 >= (loopTime2 + 50)) {
    btnA   = digitalRead(ROTAB_PIN);
    btnB   = digitalRead(ROTBB_PIN);
    
    if((!btnA) && (btnA_prev)){
      buttonA = !buttonA;
      Serial.println("A Button Down");
      color(CRGB::White);
      color_change_degree = 0;
    }
    if((!btnB) && (btnB_prev)){
      buttonB++;
      buttonB = doOverflow(buttonB, 0, 6);
      Serial.println("B Button Down");
    }
    
    btnA_prev = btnA;
    btnB_prev = btnB;
    
    loopTime2 = currentTime2;
  }
}

float preventOverflow(float value, float minimum, float maximum) 
{ 
  if(value < minimum) {
    return minimum;
  } else if(value > maximum) {
    return maximum;
  } else {
    return value;
  }
}

float doOverflow(float value, float minimum, float maximum) 
{ 
  if(value > maximum) {
    return value - (maximum+1);
  } else if(value < minimum) {
    return value + (maximum+1);
  } else {
    return value;
  }
}

void loop() 
{
  readRot();
  readBtn();
  EVERY_N_MILLISECONDS( 150 ) { 
    Serial.print("A: ");
    Serial.print(valueA);
    Serial.print(" | ");
    Serial.print("B: ");
    Serial.print(valueB);
    Serial.print(" | ");
    Serial.print("btn A: ");
    Serial.print(buttonA);
    Serial.print(" | ");
    Serial.print("btn B: ");
    Serial.print(buttonB);
    Serial.print(" | ");
    Serial.print("Collor Deg: ");
    Serial.print(color_change_degree);
    Serial.print(" | ");
    Serial.print("Collor Delay: ");
    Serial.print(color_change_delay);
    Serial.print(" | ");
    Serial.print("Collor Flow: ");
    Serial.print(color_flow_delay);
    Serial.print(" | ");
    Serial.print("Strobe Fac: ");
    Serial.print(strobe_factor);
    Serial.print(" | ");
    Serial.print("Strobe value: ");
    Serial.print(strobe_value);
    Serial.println();
  }
  
  brightness = valueA;
  mode = buttonB;
  if(buttonA){
    switch(mode){
      case 1:
        color(CHSV(color_change_degree, 255, 255));
      break;
      case 2:
        strobe();
      break;
      case 3:
        rainbowFade();
      break;
      case 4:
        colorChange();
      break;
      case 5:
        rainbowCycle();
      break;
      case 6:
        displayBotellon(CHSV(color_change_degree, 255, 255));
      break;
      default:
        color(CRGB::White);
        //displayBotellon();
      break;
    }
    // FastLED.setBrightness(  brightness );
    pm.setBrightness(brightness);
    // FastLED.show();
  } else {
    pm.clear();
    // FastLED.show();
  }
  
  //// FastLED.delay(1000); //ToDo: implement SleepMode, wakeup on button interrupt
  //delay(1000);
  //Serial.println("loop");
  
  EVERY_N_MILLIS_I(thistimer7, random_trigger){
      textTrigger = true;
      random_trigger = random(10000, 1800000);
  }
  thistimer7.setPeriod(random_trigger);
}
