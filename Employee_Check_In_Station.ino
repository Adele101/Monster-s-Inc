// Library code:
#include "Wire.h"
#include "Adafruit_LiquidCrystal.h"
#include <Button.h>
#include <SoftwareSerial.h>
#include <Adafruit_NeoPixel.h>

​

// Declare buttons
Button backwards (2, INPUT_PULLUP);
Button forwards (5, INPUT_PULLUP);
Button enter (7, INPUT_PULLUP);
Button clr (3, INPUT_PULLUP);

​

// Connect via i2c, default address #0 (A0-A2 not jumpered)
Adafruit_LiquidCrystal lcd(0);

​

// Connect the NeoPixel strip
Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, PIN, NEO_GRB + NEO_KHZ800);
#define PIN 6
#define NUM_LEDS 60

​

// Initialize the XBee
SoftwareSerial xBee(8, 9); // (TX, RX) :
const int MAX_FIELDS = 4;
const String myNodeName = "EmployeeNames";
const byte TAB_CHAR = 0x09;
const byte NEWLINE_CHAR = 0x0A;

​

// A list of employee names from the movie
String employeeNames[] = {"< Sullivan     >", "< Randall      >", "< Ranft        >", "< Luckey       >", "< Rivera       >", "< Peterson     >", "< Jones       >", "< Sanderson    >", "< Plesuski     >", "< Schmidt      >",  "< Mike        >"};
int currIndex = -1;


void setup() {
  // Set up the LCD's first message
  lcd.begin(16, 2);
  lcd.print("Welcome to the");
  lcd.setCursor(0, 1);
  lcd.print("Scare Factory");
  Serial.begin(9600);

​

  // Begin the NeoPixel strip
  strip.begin();
  strip.show();
  Twinkle(0, 0, 255, 30, 100);
  colorWipe(strip.Color(0, 0, 255), 5); // Blue

  lcd.clear();
  lcd.print("< Select Name  >");
}


void loop() {
  int action = backwards.checkButtonAction();
  int action2 = forwards.checkButtonAction();
  int action3 = enter.checkButtonAction();
  int action4 = clr.checkButtonAction();

​

  // Iterate backwards if the back button is pressed
  if (action == Button::PRESSED) {
    if (currIndex == 0) {
      currIndex = 10;
      lcd.clear();
      lcd.print(employeeNames[currIndex]);
      colorSelect();
    } else {
      currIndex --;
      lcd.clear();
      lcd.print(employeeNames[currIndex]);
      colorSelect();
    }
  }

​

  // Iterate forwards if the forward button is pressed
  if (action2 == Button::PRESSED) {
    if (currIndex == 10) {
      currIndex = 0;
      lcd.clear();
      lcd.print(employeeNames[currIndex]);
      strip.show();
      colorSelect();
    } else {
      currIndex ++;
      lcd.clear();
      lcd.print(employeeNames[currIndex]);
      strip.show();
      colorSelect();
    }
  }

​

  // Sends the selected employee name to mission control
  if (action3 == Button::PRESSED) {
    String msg = myNodeName + "\t5555\t" + currIndex + "\n";
    xBee.begin(9600);
    xBee.print(msg);
    currIndex = -1;
    lcd.clear();
    lcd.setCursor(16, 0);
    lcd.print("Welcome back to");
    lcd.setCursor(16, 1);
    lcd.print("the scare floor");
    for (int positionCounter = 0; positionCounter < 16; positionCounter++) {
      // scroll one position left:
      lcd.scrollDisplayLeft();
      if (positionCounter % 5 == 0) {
        rainbowCycle(1);
      }
    }
  }

​

  // Back to "home" (Select Name)
  if (action4 == Button::PRESSED) {
    lcd.clear();
    lcd.print("< Select Name  >");
    currIndex = -1;
    Twinkle(0, 0, 255, 5, 100);
    colorWipe(strip.Color(0, 0, 255), 5); // Blue
  }

​

  // Receiving an XBee message
  String msg = checkMessageReceived();

​

  if (msg.length() > 0) {
    String msgFields[MAX_FIELDS];
    for (int i = 0; i < MAX_FIELDS; i++) {
      msgFields[i] = "";
    }

    int fieldsFound = 0;
    String buf = "";

    for (int i = 0; i < msg.length(); i++) {
      if (((msg.charAt(i) == TAB_CHAR) ||
           (msg.charAt(i) == NEWLINE_CHAR)) &&
          (fieldsFound < MAX_FIELDS)) {
        msgFields[fieldsFound] = buf;
        buf = "";
        fieldsFound++;
      }
      else {
        buf += msg.charAt(i);
      }
    }

​

    Serial.print("found fields = ");
    Serial.println(fieldsFound);
    for (int i = 0; i < fieldsFound; i++) {
      Serial.println(msgFields[i]);
    }

​

    int aNumber = msgFields[1].toInt();
    Serial.println(aNumber);

​

    switch (aNumber) {
      // All Clear
      case 9999:
        lcd.clear();
        lcd.print("All Clear");
        lcd.clear();
        Twinkle(0, 0, 255, 30, 100);
        colorWipe(strip.Color(0, 0, 255), 15); // Blue
        lcd.print("< Select Name  >");
        break;

      case 2319:
        // Contamination
        lcd.clear();
        lcd.print("CONTAMINATION");
        lcd.setCursor(0, 1);
        lcd.print("ALERT 2319");
        strip.show();
        Strobe(255, 0, 0, 30, 100, 1000);
        break;
    }

  }

}

​

// Checks for a message recievied by XBee
String checkMessageReceived () {

  static String msgBuffer = "";
  String returnMsg = "";

  if (xBee.available()) {

    byte ch = xBee.read();
    msgBuffer += char(ch);

    // Checks for the message terminator
    if (ch == NEWLINE_CHAR) {
      // If so, then return the completed message
      returnMsg = msgBuffer;
      // and clear out the buffer for the next message
      msgBuffer = "";
    }
    else {
      // The message isn't complete yet, so just return a null string to the caller
    }
  }

  else {
    // Nothing has been received, so
    // return a null string to the caller
  }

  return returnMsg;

}

​

// Clears the NeoPixel
void showStrip() {
#ifdef ADAFRUIT_NEOPIXEL_H
  strip.show();
#endif
#ifndef ADAFRUIT_NEOPIXEL_H
  FastLED.show();
#endif
}

​

// Sets the pixel color for the NeoPixel
void setPixel(int Pixel, byte red, byte green, byte blue) {
#ifdef ADAFRUIT_NEOPIXEL_H
  strip.setPixelColor(Pixel, strip.Color(red, green, blue));
#endif
#ifndef ADAFRUIT_NEOPIXEL_H
  leds[Pixel].r = red;
  leds[Pixel].g = green;
  leds[Pixel].b = blue;
#endif
}

​

void setAll(byte red, byte green, byte blue) {
  for (int i = 0; i < NUM_LEDS; i++ ) {
    setPixel(i, red, green, blue);
  }
  showStrip();
}

​

// NeoPixel cycles through the rainbow
void rainbowCycle(int num) {
  uint16_t i, j;

  for (j = 0; j < 256 * num; j++) {
    for (i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
  }
}

​

uint32_t Wheel(byte WheelPos) {
  if (WheelPos < 85) {
    return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
    WheelPos -= 170;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

​

// NeoPixel pattern for interating through the employee list
void colorSelect() {
  strip.show();
  colorWipe(strip.Color(0, 255, 0), 5); // Green
  colorWipe(strip.Color(0, 0, 255), 5); // Blue
}

void colorWipe(uint32_t c, uint8_t wait) {
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

​

// NeoPixel strobes red for a contamination
void Strobe(byte red, byte green, byte blue, int StrobeCount, int FlashDelay, int EndPause) {
  for (int j = 0; j < StrobeCount; j++) {
    setAll(red, green, blue);
    showStrip();
    delay(FlashDelay);
    setAll(0, 0, 0);
    showStrip();
    delay(FlashDelay);
  }
  delay(EndPause);
}

​

// NeoPixel twinkles as the station loads
void Twinkle(byte red, byte green, byte blue, int Count, int SpeedDelay) {
  setAll(0, 0, 0);

  for (int k = 0; k < Count; k++) {
    setPixel(random(NUM_LEDS), red, green, blue);
    showStrip();
    delay(SpeedDelay);
  }
  delay(SpeedDelay);
}
