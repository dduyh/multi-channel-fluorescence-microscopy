#include <Adafruit_DotStar.h>
#include <SPI.h>

#define SERIAL_BAUD_RATE 9600


#define DATAPIN    4   //data signal input pin
#define CLOCKPIN   5   //clock signal input pin
#define NUMPIXELS 255  // Number of LEDs in strip

#define Delta_X    10      // mm

#define blue_pin   8    //blue LED PIN

float      NAobj = 0.25;   //numerical aperture
float      d = 150;          //Distance from the sample to Adafruit DotStar LEDs

boolean Beginflag = true;
boolean flag = true;
char terminator = '/';
String mode;
uint32_t color = 0xFF0000;

//declare a DotStar object
Adafruit_DotStar strip(NUMPIXELS, DATAPIN, CLOCKPIN, DOTSTAR_BRG);

void setup() {
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000L)
  clock_prescale_set(clock_div_1); // Enable 16 MHz on Trinket
#endif

  // Initialize serial interface
  Serial.begin(SERIAL_BAUD_RATE);
  pinMode(blue_pin, OUTPUT);

  Serial.println("<Arduino is ready>");
  Serial.println("-----------------------------------");
  Serial.println("Command List:");
  Serial.println("-----------------------------------");
  Serial.println("COMMAND: Fluorescence excitation");
  Serial.println("SYNTAX:  fe/color_on/off/");
  Serial.println("EXAMPLE: fe/blue_on/");
  Serial.println("-----------------------------------");
  Serial.println("COMMAND: Bright Field");
  Serial.println("SYNTAX:  bf/(r,g,b)/");
  Serial.println("EXAMPLE: bf/(000,064,254)/");
  Serial.println("-----------------------------------");
  Serial.println("COMMAND: Dark Field");
  Serial.println("SYNTAX:  df/(r,g,b)/");
  Serial.println("EXAMPLE: df/(000,064,254)/");
  Serial.println("-----------------------------------");
  Serial.println("COMMAND: Phase Contrast");
  Serial.println("SYNTAX:  pc/(r,g,b)/field_direction(bt/bb/bl/br/dt/db/dl/dr)");
  Serial.println("EXAMPLE: pc/(000,064,254)/bl/");
  Serial.println("-----------------------------------");
  Serial.println("COMMAND: Multiple Points");
  Serial.println("SYNTAX:  mp/number(1~255)/(r,g,b)num1/(r,g,b)num2/.../");
  Serial.println("EXAMPLE: mp/3/(000,064,254)254/.../");
  Serial.println("-----------------------------------");
  Serial.println("COMMAND: Set Parameters");
  Serial.println("SYNTAX:  set/dist/num/");
  Serial.println("         set/na/num/");
  Serial.println("EXAMPLE: set/dist/90/");
  Serial.println("         set/na/0.5/");
  Serial.println("-----------------------------------");
  Serial.println("COMMAND: Turn Off All");
  Serial.println("SYNTAX:  x/");
  Serial.println("-----------------------------------");
  Serial.println("-----------------------------------");

  strip.begin(); // Initialize pins for output
  strip.show();  // Turn all LEDs off ASAP
}

void loop() {
  if (Serial.available())  {
    if (Beginflag) {
      mode = Serial.readStringUntil(terminator);  //set mode
      Serial.print("MODE: ");
      Serial.println(mode);
      Beginflag = false;         //maintain mode until next command
    }
    if (flag) {
      char val = mode.charAt(0);  // get the first character of mode
      switch (val) {
        case 'f':  //fe
          Fluorescence_excitation();  // Fluorescence excitation function
          break;

        case 'b':  //bf
          bright_field();  // bright field function
          break;

        case 'd':  //df
          dark_field();    // dark field function
          break;

        case 'p':  //pc
          phase_contrast();  // phase contrast function
          break;

        case 'm':  //mp
          multiple_points();   //multiple points function
          break;

        case 's':  //set
          set();  // set parameters function
          break;

        case 'x':  //x
          strip.clear();
          strip.show();
          Serial.println("Turn Off All");
          Serial.println("-----------------------------------");
          break;

        default:
          Serial.println("Wrong Command.");
          break;
      }
      flag = false;
    }
    if (Serial.available())  {
      char tem = Serial.read();   //gets one byte from serial buffer
      if (tem == '\n') {
        Beginflag = true;
        flag = true;           //maintain lights mode until next command( avoid flashing)
      }
    }
  }
}

/* Fluorescence excitation function */
void Fluorescence_excitation() {
  // read and print light color and status
  String Status = Serial.readStringUntil(terminator);
  Serial.print("STATUS: ");
  Serial.println(Status);
  Serial.println("-----------------------------------");

  if (Status == "blue_on")
  {
    digitalWrite(blue_pin, 0);
  }
  if (Status == "blue_off")
  {
    digitalWrite(blue_pin, 1);
  }

}

/* bright field function */
void bright_field() {

  // calculate optical parameters
  float theta = asin(NAobj);
  double x = d * tan(theta);
  int radius = int(x / Delta_X) + 1;      //bright field diameter

  // read and print light color
  String Col = Serial.readStringUntil(terminator);
  int red  = (Col.substring(1, 4)).toInt();
  int green  = (Col.substring(5, 8)).toInt();
  int blue  = (Col.substring(9, 12)).toInt();
  Serial.print("RED: ");
  Serial.print(red);
  Serial.print(" GREEN: ");
  Serial.print(green);
  Serial.print(" BLUE: ");
  Serial.println(blue);
  Serial.println("-----------------------------------");

  if (red == 0 && green == 0 && blue == 0) {
    strip.clear();
    strip.show();
    Serial.println("Turn Off All");
  }
  else {
    strip.clear();
    strip.show();

    int first = 0;       //first led index
    int count = 0;       //number of lights turned on
    switch (radius) {
      case 1:
        first = 254;
        count = 1;
        break;
      case 2:
        first = 248;
        count = 7;
        break;
      case 3:
        first = 236;
        count = 19;
        break;
      case 4:
        first = 216;
        count = 39;
        break;
      case 5:
        first = 192;
        count = 63;
        break;
      case 6:
        first = 164;
        count = 91;
        break;
      case 7:
        first = 132;
        count = 123;
        break;
      case 8:
        first = 92;
        count = 163;
        break;
      case 9:
        first = 48;
        count = 207;
        break;
      case 10:
        first = 0;
        count = 255;
        break;
      default:
        first = 0;
        count = 255;
        Serial.println("Num is Out of Range");
        break;
    }
    color = strip.Color(green, red, blue);
    strip.fill(color, first, count);
    strip.show();                     // Refresh strip
  }
}

/* dark field function */
void dark_field() {

  // calculate optical parameters
  float theta = asin(NAobj);
  double x = d * tan(theta);
  int radius = int(x / Delta_X) + 1;     //bright field diameter

  // read and print light color
  String Col = Serial.readStringUntil(terminator);
  int red  = (Col.substring(1, 4)).toInt();
  int green  = (Col.substring(5, 8)).toInt();
  int blue  = (Col.substring(9, 12)).toInt();
  Serial.print("RED: ");
  Serial.print(red);
  Serial.print(" GREEN: ");
  Serial.print(green);
  Serial.print(" BLUE: ");
  Serial.println(blue);
  Serial.println("-----------------------------------");

  if (red == 0 && green == 0 && blue == 0) {
    strip.clear();
    strip.show();
    Serial.println("Turn Off All");
  }
  else {
    strip.clear();
    strip.show();

    int first = 0;   //first led index
    int count = 0;   //number of lights turned on
    switch (radius) {
      case 1:
        first = 0;
        count = 254;
        break;
      case 2:
        first = 0;
        count = 248;
        break;
      case 3:
        first = 0;
        count = 236;
        break;
      case 4:
        first = 0;
        count = 216;
        break;
      case 5:
        first = 0;
        count = 192;
        break;
      case 6:
        first = 0;
        count = 164;
        break;
      case 7:
        first = 0;
        count = 132;
        break;
      case 8:
        first = 0;
        count = 92;
        break;
      case 9:
        first = 0;
        count = 48;
        break;
      case 10:
        first = 0;
        count = 1;
        break;
      default:
        first = 0;
        count = 1;
        Serial.println("Num is Out of Range");
        break;
    }
    color = strip.Color(green, red, blue);
    strip.fill(color, first, count);
    strip.show();                     // Refresh strip
  }
}

/*single half-ring function for phase contrast function*/
void single_half_ring(int num, int red,  int green, int blue, char t) {
  int first = 0;      //first led index
  int count = 0;      //number of lights turned on
  color = strip.Color(green, red, blue);
  switch (t) {
    case 't':
      switch (num) {  //ring number
        case 1:
          first = 254;
          count = 1;
          break;
        case 2:
          first = 248;
          count = 4;
          break;
        case 3:
          first = 236;
          count = 7;
          break;
        case 4:
          first = 216;
          count = 11;
          break;
        case 5:
          first = 192;
          count = 13;
          break;
        case 6:
          first = 164;
          count = 15;
          break;
        case 7:
          first = 132;
          count = 17;
          break;
        case 8:
          first = 92;
          count = 21;
          break;
        case 9:
          first = 48;
          count = 23;
          break;
        case 10:
          first = 0;
          count = 25;
          break;
        default:
          Serial.println("Num is Out of Range");
          break;
      }
      strip.fill(color, first, count);
      strip.show();                     // Refresh strip
      break;
    case 'b':
      switch (num) {
        case 1:
          first = 254;
          count = 1;
          break;
        case 2:
          strip.setPixelColor(248, color);
          first = 251;
          count = 3;
          break;
        case 3:
          strip.setPixelColor(236, color);
          first = 242;
          count = 6;
          break;
        case 4:
          strip.setPixelColor(216, color);
          first = 226;
          count = 10;
          break;
        case 5:
          strip.setPixelColor(192, color);
          first = 204;
          count = 12;
          break;
        case 6:
          strip.setPixelColor(164, color);
          first = 178;
          count = 14;
          break;
        case 7:
          strip.setPixelColor(132, color);
          first = 148;
          count = 16;
          break;
        case 8:
          strip.setPixelColor(92, color);
          first = 112;
          count = 20;
          break;
        case 9:
          strip.setPixelColor(48, color);
          first = 70;
          count = 22;
          break;
        case 10:
          strip.setPixelColor(0, color);
          first = 24;
          count = 24;
          break;
        default:
          Serial.println("Num is Out of Range");
          break;
      }
      strip.fill(color, first, count);
      strip.show();                     // Refresh strip
      break;
    case 'l':
      switch (num) {
        case 1:
          first = 254;
          count = 1;
          strip.fill(color, first, count);
          break;
        case 2:
          first = 248;
          count = 2;
          strip.fill(color, first, count);
          first = 253;
          count = 1;
          strip.fill(color, first, count);
          break;
        case 3:
          first = 236;
          count = 4;
          strip.fill(color, first, count);
          first = 245;
          count = 3;
          strip.fill(color, first, count);
          break;
        case 4:
          first = 216;
          count = 6;
          strip.fill(color, first, count);
          first = 231;
          count = 5;
          strip.fill(color, first, count);
          break;
        case 5:
          first = 192;
          count = 7;
          strip.fill(color, first, count);
          first = 210;
          count = 6;
          strip.fill(color, first, count);
          break;
        case 6:
          first = 164;
          count = 8;
          strip.fill(color, first, count);
          first = 185;
          count = 7;
          strip.fill(color, first, count);
          break;
        case 7:
          first = 132;
          count = 9;
          strip.fill(color, first, count);
          first = 156;
          count = 8;
          strip.fill(color, first, count);
          break;
        case 8:
          first = 92;
          count = 11;
          strip.fill(color, first, count);
          first = 122;
          count = 10;
          strip.fill(color, first, count);
          break;
        case 9:
          first = 48;
          count = 12;
          strip.fill(color, first, count);
          first = 81;
          count = 11;
          strip.fill(color, first, count);
          break;
        case 10:
          first = 0;
          count = 13;
          strip.fill(color, first, count);
          first = 36;
          count = 12;
          strip.fill(color, first, count);
          break;
        default:
          Serial.println("Num is Out of Range");
          break;
      }
      strip.show();                     // Refresh strip
      break;
    case 'r':
      switch (num) {
        case 1:
          first = 254;
          count = 1;
          break;
        case 2:
          first = 250;
          count = 3;
          break;
        case 3:
          first = 239;
          count = 7;
          break;
        case 4:
          first = 221;
          count = 11;
          break;
        case 5:
          first = 198;
          count = 13;
          break;
        case 6:
          first = 171;
          count = 15;
          break;
        case 7:
          first = 140;
          count = 17;
          break;
        case 8:
          first = 102;
          count = 21;
          break;
        case 9:
          first = 59;
          count = 23;
          break;
        case 10:
          first = 12;
          count = 25;
          break;
        default:
          Serial.println("Num is Out of Range");
          break;
      }
      strip.fill(color, first, count);
      strip.show();                     // Refresh strip
      break;
  }
}

/* phase contrast function */
void phase_contrast() {
  strip.clear();
  strip.show();

  // calculate optical parameters
  float theta = asin(NAobj);
  double x = d * tan(theta);
  int radius = int(x / Delta_X) + 1;    //bright field diameter

  // read and print light color
  String Col = Serial.readStringUntil(terminator);
  int red  = (Col.substring(1, 4)).toInt();
  int green  = (Col.substring(5, 8)).toInt();
  int blue  = (Col.substring(9, 12)).toInt();
  Serial.print("RED: ");
  Serial.print(red);
  Serial.print(" GREEN: ");
  Serial.print(green);
  Serial.print(" BLUE: ");
  Serial.println(blue);

  // read and print phase direction
  String Dir = Serial.readStringUntil(terminator);
  Serial.print("DIRECTION: ");
  Serial.println(Dir);
  Serial.println("-----------------------------------");

  int val = 0;
  if (Dir == "bt") val = 1;         //bright field top
  else if (Dir == "bb") val = 2;    //bright field bottom
  else if (Dir == "bl") val = 3;    //bright field left
  else if (Dir == "br") val = 4;    //bright field right
  else if (Dir == "dt") val = 5;    //dark field top
  else if (Dir == "db") val = 6;    //dark field bottom
  else if (Dir == "dl") val = 7;    //dark field left
  else if (Dir == "dr") val = 8;    //dark field right
  else Serial.println("Direction is Wrong");

  int k;
  switch (val) {
    case 1:
      for (k = 1 ; k <= radius; k++ ) {
        single_half_ring(k, red, green, blue, 't');
      }
      strip.show();                     // Refresh strip
      break;
    case 2:
      for (k = 1 ; k <= radius; k++ ) {
        single_half_ring(k, red, green, blue, 'b');
      }
      strip.show();                     // Refresh strip
      break;
    case 3:
      for (k = 1 ; k <= radius; k++ ) {
        single_half_ring(k, red, green, blue, 'l');
      }
      strip.show();                     // Refresh strip
      break;
    case 4:
      for (k = 1 ; k <= radius; k++ ) {
        single_half_ring(k, red, green, blue, 'r');
      }
      strip.show();                     // Refresh strip
      break;
    case 5:
      for (k = radius + 1 ; k <= 10; k++ ) {
        single_half_ring(k, red, green, blue, 't');
      }
      strip.show();                     // Refresh strip
      break;
    case 6:
      for (k = radius + 1 ; k <= 10; k++ ) {
        single_half_ring(k, red, green, blue, 'b');
      }
      break;
    case 7:
      for (k = radius + 1 ; k <= 10; k++ ) {
        single_half_ring(k, red, green, blue, 'l');
      }
      break;
    case 8:
      for (k = radius + 1 ; k <= 10; k++ ) {
        single_half_ring(k, red, green, blue, 'r');
      }
      break;
  }
}

/* multiple points function */
void multiple_points() {
  strip.clear();
  strip.show();

  // read and print number of points
  int num = Serial.readStringUntil(terminator).toInt();
  Serial.print("Points_Number: ");
  Serial.println(num);

  /* turn on the points */
  for (int i = 1; i <= num; i++) {

    // read and print light color and index
    String string = Serial.readStringUntil(terminator);
    int red  = (string.substring(1, 4)).toInt();
    int green  = (string.substring(5, 8)).toInt();
    int blue  = (string.substring(9, 12)).toInt();
    int index  = (string.substring(13)).toInt();
    Serial.print("Index: ");
    Serial.print(index);
    Serial.print(" RED: ");
    Serial.print(red);
    Serial.print(" GREEN: ");
    Serial.print(green);
    Serial.print(" BLUE: ");
    Serial.println(blue);

    color = strip.Color(green, red, blue);
    strip.setPixelColor(index, color);    //turn on one point
  }
  strip.show();
  Serial.println("-----------------------------------");
}

/* set parameters function */
void set() {
  String string = Serial.readStringUntil(terminator);

  if (string == "na" ) {                           // change numerical aperture
    String number = Serial.readStringUntil(terminator);
    float para = number.toFloat();
    NAobj = para;
    Serial.print("new NAobj: ");
    Serial.println(NAobj);
    Serial.println("-----------------------------------");
  }
  else if (string == "dist") {                     //change distance from the sample to Adafruit DotStar LEDs
    String number = Serial.readStringUntil(terminator);
    int para = number.toInt();
    d = para;
    Serial.print("new distance: ");
    Serial.println(d);
    Serial.println("-----------------------------------");
  }
  else {
    Serial.println("No command");
  }

}
