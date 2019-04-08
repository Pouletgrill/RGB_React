// HSV fade/bounce for Arduino - scruss.com - 2010/09/12
// Note that there's some legacy code left in here which seems to do nothing
// but should do no harm ...

// don't futz with these, illicit sums later
#define Indicator_LED 13
#define RED       9 // pin for red LED
#define GREEN    10 // pin for green - never explicitly referenced
#define BLUE     11 // pin for blue - never explicitly referenced
#define MIC       0//8 // Microphone
#define SIZE    255
#define DELAY    20
#define HUE_MAX  6.0
#define HUE_DELTA 0.01//0.01

/*TEST autogain code*/
const int sampleWindow = 50; // Sample window width in mS (50 mS = 20Hz)
unsigned int sample;
uint8_t volume = 0;
/*///////////////////*/

//long deltas[3] = { 5, 6, 7 };
long rgb[3];
long rgbval;
//float rand;
boolean flag = false;

int val;
// for reasons unknown, if value !=0, the LED doesn't light. Hmm ...
// and saturation seems to be inverted
float hue=0.0, last_hue=0.0, saturation=1, value=1;

/*
chosen LED SparkFun sku: COM-09264
 has Max Luminosity (RGB): (2800, 6500, 1200)mcd
 so we normalize them all to 1200 mcd -
 R  250/600  =  107/256
 G  250/950  =   67/256
 B  250/250  =  256/256
 */
long bright[3] = { 107, 67, 256};
//long bright[3] = { 256, 256, 256};

long k, temp_value;

void setup () {
pinMode(Indicator_LED, OUTPUT);
digitalWrite(Indicator_LED, LOW);

//Serial.begin(9600);/*Tempo*/
  
  randomSeed(analogRead(4));
  for (k=0; k<3; k++) {
    pinMode(RED + k, OUTPUT);
    rgb[k]=0;
    analogWrite(RED + k, rgb[k] * bright[k]/256);
  }
  //pinMode(MIC, INPUT);
}

void loop() {




/*TEST autogain code*/
unsigned long startMillis= millis();  // Start of sample window
   unsigned int peakToPeak = 0;   // peak-to-peak level
 
   unsigned int signalMax = 0;
   unsigned int signalMin = 1024;
 
   // collect data for 50 mS
   while (millis() - startMillis < sampleWindow)
   {
      sample = analogRead(MIC);
      if (sample < 1024)  // toss out spurious readings
      {
         if (sample > signalMax)
         {
            signalMax = sample;  // save just the max levels
         }
         else if (sample < signalMin)
         {
            signalMin = sample;  // save just the min levels
         }
      }
   }
   peakToPeak = signalMax - signalMin;  // max - min = peak-peak amplitude
   double volts = (peakToPeak * 5.0) / 1024;  // convert to volts
   bool pulse = (sample-signalMin) > (peakToPeak*0.50) && peakToPeak > 130;
   //Serial.println(peakToPeak);
   /*if(pulse){
    
   }*/

/*///////////////////*/


  
  //flag = false
  //val = digitalRead(MIC);//Board bleu
  //val = !digitalRead(MIC);//Board Rouge
  
  if ( /*volume > 0*/peakToPeak >= 260/*pulse*/)
  {
    /*if(flag)
    {
      digitalWrite(Indicator_LED, HIGH);
    }
    else
    {
      digitalWrite(Indicator_LED, LOW);
    }
    flag = !flag;*/

    do
    {
      hue = random(601)/100;
    }while (hue == last_hue);//break only when value is different
    last_hue = hue;
    
    //hue += HUE_DELTA;
    digitalWrite(Indicator_LED, HIGH);
    delay(50);
    digitalWrite(Indicator_LED, LOW);
  }
  /*hue += HUE_DELTA;
  if (hue > HUE_MAX) {
    hue=0.0;
  }*/
  rgbval=HSV_to_RGB(hue, saturation, value);
  rgb[0] = (rgbval & 0x00FF0000) >> 16; // there must be better ways
  rgb[1] = (rgbval & 0x0000FF00) >> 8;
  rgb[2] = rgbval & 0x000000FF;
  for (k=0; k<3; k++) { // for all three colours
    analogWrite(RED + k, rgb[k] * bright[k]/256);
  }



  /*   //If there is a decent change in volume since the last pass, average it into "avgBump"
  if (volume - last > 10) avgBump = (avgBump + (volume - last)) / 2.0;

  //If there is a notable change in volume, trigger a "bump"
  //  avgbump is lowered just a little for comparing to make the visual slightly more sensitive to a beat.
  bump = (volume - last > avgBump * .9);  

  //If a "bump" is triggered, average the time between bumps
  if (bump) {
    avgTime = (((millis() / 1000.0) - timeBump) + avgTime) / 2.0;
    timeBump = millis() / 1000.0;
  }*/


  
  //delay(DELAY);
}

long HSV_to_RGB( float h, float s, float v ) {
  /* modified from Alvy Ray Smith's site: http://www.alvyray.com/Papers/hsv2rgb.htm */
  // H is given on [0, 6]. S and V are given on [0, 1].
  // RGB is returned as a 24-bit long #rrggbb
  int i;
  float m, n, f;

  // not very elegant way of dealing with out of range: return black
  if ((s<0.0) || (s>1.0) || (v<0.0) || (v>1.0)) {
    return 0L;
  }

  if ((h < 0.0) || (h > 6.0)) {
    return long( v * 255 ) + long( v * 255 ) * 256 + long( v * 255 ) * 65536;
  }
  i = floor(h);
  f = h - i;
  if ( !(i&1) ) {
    f = 1 - f; // if i is even
  }
  m = v * (1 - s);
  n = v * (1 - s * f);
  switch (i) {
  case 6:
  case 0: 
    return long(v * 255 ) * 65536 + long( n * 255 ) * 256 + long( m * 255);
  case 1: 
    return long(n * 255 ) * 65536 + long( v * 255 ) * 256 + long( m * 255);
  case 2: 
    return long(m * 255 ) * 65536 + long( v * 255 ) * 256 + long( n * 255);
  case 3: 
    return long(m * 255 ) * 65536 + long( n * 255 ) * 256 + long( v * 255);
  case 4: 
    return long(n * 255 ) * 65536 + long( m * 255 ) * 256 + long( v * 255);
  case 5: 
    return long(v * 255 ) * 65536 + long( m * 255 ) * 256 + long( n * 255);
  }
} 
