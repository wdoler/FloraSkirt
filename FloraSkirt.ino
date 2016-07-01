#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>
#include <Adafruit_NeoPixel.h>

#define PIN 6
#define NUM_PIXEL 19


//define modes
#define compass 1
#define twinkle 2


// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_PIXEL, PIN, NEO_GRB + NEO_KHZ800);

//Prototypes
void LED_modes(int local_mode);
uint32_t Wheel(byte WheelPos);
void theaterChaseRainbow(uint8_t wait) ;
void theaterChase(uint32_t c, uint8_t wait);
void rainbowCycle(uint8_t wait);
void rainbow(uint8_t wait);
void rainbow_nodelay(uint8_t wait, uint8_t current_time);
void colorWipe(uint32_t c, uint8_t wait);
void setAllColor(uint32_t c);


/* Assign a unique ID to this sensor at the same time */
Adafruit_LSM303_Accel_Unified AccelAndMag = Adafruit_LSM303_Accel_Unified(54321);

void displaySensorDetails(void)
{
  sensor_t sensor;
  AccelAndMag.getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" m/s^2");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" m/s^2");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" m/s^2");  
  Serial.println("------------------------------------");
  Serial.println("");
  delay(500);
}

void setup(void) 
{
  Serial.begin(9600);
  Serial.println("Accelerometer Test"); Serial.println("");
  
  strip.begin();
  // Update the strip, to start they are all 'off'
  strip.show();
  /* Initialise the sensor */
  if(!AccelAndMag.begin())
  {
    /* There was a problem detecting the ADXL345 ... check your connections */
    Serial.println("Ooops, no LSM303 detected ... Check your wiring!");
    while(1);
  }
  
  /* Display some basic information on this sensor */
  displaySensorDetails();
}

int AccelMillis = 0;
int MagMillis = 0;
int ReadSensorMillis = 0;
sensors_event_t event; 
int mode = compass;

void loop(void) 
{
  /* Get a new sensor event */ 
  int currentmillis = millis();
  if(currentmillis>=ReadSensorMillis+200){
    ReadSensorMillis = currentmillis;
    AccelAndMag.getEvent(&event);

    //define movement and change the mode of the lights
    if (event.acceleration.z>200){
      //on jump
      //;
    // mode = twinkle;
    }
    else{
      mode = compass;
    }
  }

  //print statements acceleration
  if(currentmillis>=AccelMillis+200){
    AccelMillis = currentmillis;
    /* Display the results (acceleration is measured in m/s^2) */
    Serial.print("X: "); Serial.print(event.acceleration.x); Serial.print("  ");
    Serial.print("Y: "); Serial.print(event.acceleration.y); Serial.print("  ");
    Serial.print("Z: "); Serial.print(event.acceleration.z); Serial.print("  ");Serial.println("m/s^2 ");
  }

  //print statements acceleration
  if(currentmillis>=MagMillis+500){
    MagMillis = currentmillis;
      /* Display the results (magnetic vector values are in micro-Tesla (uT)) */
    //
    Serial.print("X: "); Serial.print(event.magnetic.x); Serial.print("  ");
    Serial.print("Y: "); Serial.print(event.magnetic.y); Serial.print("  ");
    Serial.print("Z: "); Serial.print(event.magnetic.z); Serial.print("  ");Serial.println("uT");
  }

  //Built in mapping function
  //map(value, fromLow, fromHigh, toLow, toHigh)

  //random(min, max)
  
  //I would probably add a function below to handle a case statement for color "modes"
  //   rainbows, color whipes, twinkles
  mode = LED_modes(mode);
}

int LED_modes(int local_mode){
  switch (local_mode){
    case compass:
      setAllColor(strip.Color(event.magnetic.x,event.magnetic.y, event.magnetic.z));
      break;
    case twinkle:
      for(int x =0; x<255;x++){
        Twinkle(200);
      }
      local_mode = compass;
      break;
    default:
      setAllColor(strip.Color(255,0, 0));
      break;
  }
  return local_mode;
  
}

//maybe make the time delay the delta of acceleration 
void Twinkle(uint32_t max_wait){
  uint32_t brightness = random(0, 255);
  uint32_t pixel = random(0, strip.numPixels());
  strip.setPixelColor(pixel, strip.Color(brightness,brightness,brightness));
  strip.show();
  delay(random(10,max_wait));
  strip.setPixelColor(pixel, strip.Color(0,0,0));
  strip.show();
}


void setAllColor(uint32_t c){
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
  }
  strip.show();
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}


void rainbow_nodelay(uint8_t wait, uint8_t current_time) {
  static uint16_t local_time,i,j;

  //dont update unless its time to
  if(current_time>local_time+wait){
    local_time=current_time;

    //for each color
    if(j<256){
      //for each pixel
      if(i<strip.numPixels()) {
        //set the color
        strip.setPixelColor(i, Wheel((i+j) & 255));
        i++; //next pixel
      }
      else{
        i=0; //go to beginning pixel
      }
      j++; //inc on color wheel
      strip.show();
    }
    else{
      j=0; //reset to beginning of color wheel
    }
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
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

