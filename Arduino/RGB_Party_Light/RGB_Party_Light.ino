#include <FastLED.h>
#include <math.h>

#define LED_PIN     D4
#define ROTA1_PIN   14
#define ROTA2_PIN   16
#define ROTAB_PIN   12
#define ROTB1_PIN   5
#define ROTB2_PIN   4
#define ROTBB_PIN   0
#define NUM_LEDS    255
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB

unsigned long   currentTime;
unsigned long   loopTime;

unsigned long   currentTime2;
unsigned long   loopTime2;

int             valueA              = 128;  // Helligkeit
int             buttonA             = 0;    // An / Aus
unsigned char   btnA;
unsigned char   btnA_prev           = 0;
unsigned char   rotA_1;
unsigned char   rotA_2;
unsigned char   rotA_1_prev         = 0;
unsigned int    rotA_resolution     = 20;

int             valueB;                     // Farbe / Frequenz / Speed
int             buttonB             = 0;    // Mode
unsigned char   btnB;
unsigned char   btnB_prev           = 0;
unsigned char   rotB_1;
unsigned char   rotB_2;
unsigned char   rotB_1_prev         = 0;
unsigned int    rotB_resolution     = 20;

int             rainbow_i           = 0;
int             rainbow_j           = 0;

int             brightness          = 128;  // gets up to 255
int             color_change_degree = 0;    // current Color / gets up to 255
int             color_change_offset = 50;   // to be addet on current CSV Value
int             color_change_delay  = 1000;  // gets up to 15000 in 500 steps
int             color_fade_delay    = 40;  // gets up to 15000 in 500 steps
int             color_flow_delay    = 20;  // gets up to 15000 in 500 steps
float           strobe_factor       = 3.00;    // gets up to 10 in 0.5 steps
int             strobe_value        = 209;
unsigned int    mode                = 5;    // 1: Color, 2: Strobe, 3: rainbowFade, 4: Color Change, 5: rainbowCycle
CRGB            persistent_color    = CRGB::White;
CHSV            change_color        = CHSV(color_change_degree, 255, 255);

CRGB leds[NUM_LEDS];

void setup() 
{
  Serial.begin (115200);
  Serial.println("start Setup");   
  delay( 3000 ); // power-up safety delay
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
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(  brightness );
  color(CRGB::White);
  FastLED.show();
  Serial.println("finish Setup");  
}

void color(CRGB color)
{
  if(color == CHSV(0, 255, 255)){
    color = CRGB::White;
  }
  for(int i = 0;i < NUM_LEDS;i++){
    leds[i] = color;
  }
}

void rainbowCycle() 
{
  EVERY_N_MILLIS_I(thistimer1, color_flow_delay){
    rainbow_j++;
    rainbow_j = doOverflow(rainbow_j, 0, 255);
    for(rainbow_i=0; rainbow_i< NUM_LEDS; rainbow_i++) {
      leds[rainbow_i] = CHSV((rainbow_i * 256 / NUM_LEDS)+rainbow_j, 255, 255);
    }
    FastLED.show();
  }
  thistimer1.setPeriod(color_flow_delay);
}

void rainbowFade() 
{
  EVERY_N_MILLIS_I(thistimer2, color_fade_delay){
    rainbow_i++;
    rainbow_i = doOverflow(rainbow_i, 0, 255);
    color(CHSV(rainbow_i, 255, 255));
    FastLED.show();
  }
  thistimer2.setPeriod(color_fade_delay);
}

void strobe() 
{
  EVERY_N_MILLIS_I(thistimer3, strobe_value){
    color(CHSV(color_change_degree, 255, 255));
    FastLED.show();
    delay(10);
    color(CHSV(255, 255, 0));
    FastLED.show();
  }
  thistimer3.setPeriod(strobe_value);
}

void colorChange() 
{
  EVERY_N_MILLIS_I(thistimer4, color_change_delay){
    color_change_degree += color_change_offset;
    color_change_degree = doOverflow(color_change_degree, 0, 255);
    color(CHSV(color_change_degree, 255, 255));
    FastLED.show();
  }
  thistimer4.setPeriod(color_change_delay);
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
      } else {
        valueA-=rotA_resolution;               
      }   
      valueA = preventOverflow(valueA, 0, 255);
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
    }
    if((!btnB) && (btnB_prev)){
      buttonB++;
      buttonB = doOverflow(buttonB, 0, 5);
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
    return value - maximum;
  } else if(value < minimum) {
    return value + maximum;
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
      default:
        color(CRGB::White);
      break;
    }
    FastLED.setBrightness(  brightness );
    FastLED.show();
  } else {
    color(CRGB::White);
    color_change_degree = 0;
    color(CHSV(0, 0, 0));
    FastLED.show();
  }
  
  //FastLED.delay(1000); //ToDo: implement SleepMode, wakeup on button interrupt
  //delay(1000);
  //Serial.println("loop");
}
