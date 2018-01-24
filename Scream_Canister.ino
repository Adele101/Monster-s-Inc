// Library code:
#include "Wire.h"
#include "Adafruit_LiquidCrystal.h"
#include <Button.h>
#include <SoftwareSerial.h>

​

//Declare button

Button butt (11, INPUT_PULLUP);

​

// Connect via i2c, default address #0 (A0-A2 not jumpered)
Adafruit_LiquidCrystal lcd(0);

​

// Define hardware connections for sound sensor
#define PIN_GATE_IN 2
#define IRQ_GATE_IN  0
#define PIN_LED_OUT 13
#define PIN_ANALOG_IN A0
boolean soundOn = false;
boolean doneListening = false;

​

// Initialize the XBee
SoftwareSerial xBee(8, 9); // (TX, RX) :
const int MAX_FIELDS = 4;
const String myNodeName = "ScreamCanister";
const byte TAB_CHAR = 0x09;
const byte NEWLINE_CHAR = 0x0A;

​

// Character for the LCD
byte entire[8] = {
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111
};

​

void setup() {
  xBee.begin(9600);
  Serial.begin(9600);
  lcd.begin(16, 2);

​

  // Set up the LCD
  lcd.createChar(0, entire);

​

  // Set up the sound sensor
  pinMode(PIN_GATE_IN, INPUT);
  attachInterrupt(IRQ_GATE_IN, soundISR, CHANGE);

​

  Serial.println("Initialized");
}


void loop() {
  int action = butt.checkButtonAction();

​

  // Initialize sound sensor value
  int value;
  value = analogRead(PIN_ANALOG_IN);

​

  // Receiving an XBee message
  String msg = checkMessageReceived();

  if (msg.length() > 0) {
    String msgFields[MAX_FIELDS];


    for (int i = 0; i < MAX_FIELDS; i++) {
      msgFields[i] = "";
    }

​

    int fieldsFound = 0;
    String buf = "";

​

    for (int i = 0; i < msg.length(); i++) {
      if (((msg.charAt(i) == TAB_CHAR) ||
           (msg.charAt(i) == NEWLINE_CHAR)) &&
          (fieldsFound < MAX_FIELDS)) {
        msgFields[fieldsFound] = buf;
        buf = "";
        fieldsFound++;
      } else {
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

    switch (aNumber) {
      case 2100:
        // Begin sound sensor
        lcd.clear();
        Serial.println("begin sensing sound");
        soundOn = true;
        break;
    }

​

    if (soundOn == true) {
      Serial.println("Sensing now");


      if (value <= 30) {
        Serial.println("1");
        xBee.print(myNodeName + "\t1234\t1\n");
        fillUp(0);
        soundOn = false;
      } else if ((value > 30) && ( value <= 50)) {
        Serial.println("2");
        xBee.print(myNodeName + "\t1234\t2\n");
        fillUp(0);
        fillUp(4);
        soundOn = false;
      } else if ((value > 50) && (value <= 80)) {
        Serial.println("3");
        xBee.print(myNodeName + "\t1234\t3\n");
        fillUp(0);
        fillUp(4);
        fillUp(8);
        soundOn = false;
      } else if (value > 80) {
        Serial.println("4");
        xBee.print(myNodeName + "\t1234\t4\n");
        fillUp(0);
        fillUp(4);
        fillUp(8);
        fillUp(12);
        soundOn = false;
      }
    }
  }

​

  if ((action == Button::CLICKED)) {
    lcd.clear();
    xBee.print(myNodeName + "\t9999\n"); // reset the system
    Serial.println("Cleared");
  }
}

​

String checkMessageReceived () {
  static String msgBuffer = ""; 
  String returnMsg = "";       

​

  if (xBee.available()) {
    byte ch = xBee.read();
    msgBuffer += char(ch);

​

    // Checks for the message terminator
    if (ch == NEWLINE_CHAR) {
      // If so, then return the completed message
      returnMsg = msgBuffer;
      // and clear out the buffer for the next message
      msgBuffer = "";
    }
    else {
      // the message isn't complete yet, so just return a null string to the caller
    }
  }

  else {
    // nothing has been received, so
    // return a null string to the caller
  }

  return returnMsg;

}

​

// For the sound sensor
void soundISR() {
  int pin_val;
  pin_val = digitalRead(PIN_GATE_IN);
}

​

// Fills up the LCD row by row
void fillUp(int startingRow) {
  for (int x = startingRow; x < startingRow + 4; x++) {
    lcd.setCursor(x, 0);
    lcd.write(0);
    lcd.setCursor(x, 1);
    lcd.write(0);
    lcd.setCursor(x, 0);
    lcd.write(0);
    lcd.setCursor(x, 1);
    lcd.write(0);
    lcd.setCursor(x, 0);
    lcd.write(0);
    lcd.setCursor(x, 1);
    lcd.write(0);
    lcd.setCursor(x, 0);
    lcd.write(0);
    lcd.setCursor(x, 1);
    lcd.write(0);
  }
};
