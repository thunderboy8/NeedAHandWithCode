#include <WiFi.h>
#include <Wire.h>
#include <ESP32Servo.h>
#include <IRremote.hpp> // include the library

#define IR_RECEIVER_PIN 19 // The ESP32 pin GPIO19 connected to IR controller

enum Mode
{
  MODE_1, // Keypad
  MODE_2,  // Pot
  MODE_DEMO_1,
  MODE_DEMO_2
};
Mode currentMode = MODE_2;

const int TOTAL_SERVOS = 5;

int fingerSelected = 5;

// Servo position between 0 and 180 (0 is straight up)
int targetFingerPositions[] = {0, 0, 0, 0, 0};
int currentFingerPositions[] = {0, 0, 0, 0, 0};

// Input pins
const int FINGER_POT_PINS[] = {33, 34, 35, 36, 39};

// Output pins
const int FINGER_SERVO_PINS[] = {15, 25, 26, 27, 32};

Servo myservos[TOTAL_SERVOS]; // create servo object to control a servo

int val; // variable to read the value from the analog pin

int lastButtonPressTime = 0;

void moveFingerTarget(int fingerToMove, int amount)
{
  if (fingerToMove == 5)
  {
    for (int i = 0; i < 5; i++)
    {
      moveFingerTarget(i, amount);
    }
  }
  else
  {
    targetFingerPositions[fingerToMove] += amount;

    if (targetFingerPositions[fingerToMove] > 180)
    {
      targetFingerPositions[fingerToMove] = 180;
    }
    if (targetFingerPositions[fingerToMove] < 0)
    {
      targetFingerPositions[fingerToMove] = 0;
    }
  }
}

void moveFingerAbsolute(int fingerToMove, int position)
{
  if (fingerToMove == 5)
  {
    for (int i = 0; i < 5; i++)
    {
      targetFingerPositions[i] = position;
    }
  }
  else
  {
    targetFingerPositions[fingerToMove] = position;
  }
}

void demoMode1() {
  
  int speed = 1;

  float t = ((float)millis() / 1000.0) * speed;

  int vals[TOTAL_SERVOS];
  for (int i = 0; i < TOTAL_SERVOS; i++)
  {
    moveFingerAbsolute(i, ((sin(t + i * 0.3) + 1) / 2) * 180);
  }
  //int val2 = ((sin(t) + 1) / 2) * 180;

  //val = analogRead(FINGER_POT_PINS[0]); // reads the value of the potentiometer (value between 0 and 1023)

  // val = 0;
  // val = map(val, 0, 4095, 0, 180);     // scale it to use it with the servo (value between 0 and 180)

}


void demoMode2() {
  
  int speed = 1;

  float t = ((float)millis() / 1000.0) * speed;

  int vals[TOTAL_SERVOS];
  for (int i = 0; i < TOTAL_SERVOS; i++)
  {
    moveFingerAbsolute(i, ((sin(t) + 1) / 2) * 180);
  }
  

}

void readIR()
{
  if (IrReceiver.decode())
  {
    /*  v
     * Print a short summary of received data
     */
    // IrReceiver.printIRResultShort(&Serial);
    // IrReceiver.printIRSendUsage(&Serial);
    if (IrReceiver.decodedIRData.protocol == UNKNOWN)
    {
      Serial.println(F("Received noise or an unknown (or not yet enabled) protocol"));
      // We have an unknown protocol here, print more info
      IrReceiver.printIRResultRawFormatted(&Serial, true);
    }
    // Serial.println();

    /*
     * !!!Important!!! Enable receiving of the next value,
     * since receiving has stopped after the end of the current received data packet.
     */
    IrReceiver.resume(); // Enable receiving of the next value

    // Limit number of button presses 
    if (millis() - lastButtonPressTime > 100)
    {
      lastButtonPressTime = millis();

      /*
       * Finally, check the received data and perform actions according to the received command
       */

      // Check mode
      switch (IrReceiver.decodedIRData.command)
      {
      case 0x42:
        Serial.println("STAR");
        currentMode = MODE_1;
        break;
      case 0x4a:
        Serial.println("HASH");
        currentMode = MODE_2;
        break;
      case 0x5a:
          Serial.println("9");
          currentMode = MODE_DEMO_1;
          break;
        case 0x52:
          Serial.println("0");
          currentMode = MODE_DEMO_2;
          break;
       case 0x8:
          Serial.println("7");
          currentMode = MODE_1;
          // Rock on
          moveFingerAbsolute(0, 0);
          moveFingerAbsolute(1, 0);
          moveFingerAbsolute(2, 180);
          moveFingerAbsolute(3, 180);
          moveFingerAbsolute(4, 0);
          break;
        case 0x1c:
          Serial.println("8");
          currentMode = MODE_1;
          // Point
          moveFingerAbsolute(0, 0);
          moveFingerAbsolute(1, 0);
          moveFingerAbsolute(2, 180);
          moveFingerAbsolute(3, 180);
          moveFingerAbsolute(4, 180);
          break;


        case 0x40:
          Serial.println("OK");
          currentMode = MODE_1;
          // OK ;)
          moveFingerAbsolute(0, 180);
          moveFingerAbsolute(1, 180);
          moveFingerAbsolute(2, 0);
          moveFingerAbsolute(3, 0);
          moveFingerAbsolute(4, 0);
          break;
      default:
        IrReceiver.printIRResultShort(&Serial);
      }

      if (currentMode == MODE_1)
      {
        switch (IrReceiver.decodedIRData.command)
        {
        case 0x46:
          Serial.println("UP");
          moveFingerTarget(fingerSelected, -30);
          break;
        case 0x44:
          Serial.println("LEFT");
          moveFingerAbsolute(fingerSelected, 0);
          break;
        case 0x43:
          Serial.println("RIGHT");
          moveFingerAbsolute(fingerSelected, 180);
          break;
        case 0x15:
          Serial.println("DOWN");
          moveFingerTarget(fingerSelected, 30);
          break;
        case 0x16:
          Serial.println("1");
          fingerSelected = 0;
          break;
        case 0x19:
          Serial.println("2");
          fingerSelected = 1;
          break;
        case 0xd:
          Serial.println("3");
          fingerSelected = 2;
          break;
        case 0xc:
          Serial.println("4");
          fingerSelected = 3;
          break;
        case 0x18:
          Serial.println("5");
          fingerSelected = 4;
          break;
        case 0x5e:
          Serial.println("6");
          fingerSelected = 5;
          break;
       
        }
      }

      // Serial.println();
    }

    // if (IrReceiver.decodedIRData.command == 0x46) {
    //     // do s
    //     switch(IrReceiver.decodedIRData.commandomething) {
    //       case 0x46:

    //     }
    // } else if (IrReceiver.decodedIRData.command == 0x11) {
    //     // do something else
    // }
  }
}

void servoExecute()
{
  const int step = 4;
  for (int i = 0; i < TOTAL_SERVOS; i++)
  {
    if (currentFingerPositions[i] < targetFingerPositions[i])
    {
      currentFingerPositions[i] += step;
      if (currentFingerPositions[i] > targetFingerPositions[i])
      {
        currentFingerPositions[i] = targetFingerPositions[i];
      }
    }

    if (currentFingerPositions[i] > targetFingerPositions[i])
    {
      currentFingerPositions[i] -= step;
      if (currentFingerPositions[i] < targetFingerPositions[i])
      {
        currentFingerPositions[i] = targetFingerPositions[i];
      }
    }

    myservos[i].write(currentFingerPositions[i]);
  }
}

void setup()
{
  Serial.begin(115200);

  // Just to know which program is running on my Arduino
  Serial.println(F("START " __FILE__ " from " __DATE__ "\r\nUsing library version " VERSION_IRREMOTE));

  // Start the receiver and if not 3. parameter specified, take LED_BUILTIN pin from the internal boards definition as default feedback LED
  IrReceiver.begin(IR_RECEIVER_PIN, ENABLE_LED_FEEDBACK);

  Serial.print(F("Ready to receive IR signals of protocols: "));
  printActiveIRProtocols(&Serial);
  Serial.println();

  for (int i = 0; i < TOTAL_SERVOS; i++)
  {
    myservos[i].attach(FINGER_SERVO_PINS[i]);
  }

  lastButtonPressTime = millis();

  // resetActualFingerPositions();
}

void loop()
{
  readIR();

  if (currentMode == MODE_2)
  {
    // Pot mode
    for (int i = 0; i < TOTAL_SERVOS; i++)
    {
      int potVal = analogRead(FINGER_POT_PINS[i]);
      int mappedVal = map(potVal, 0, 4095, 0, 180); // scale it to use it with the servo (value between 0 and 180)
      Serial.print(mappedVal);
      Serial.print(", ");
      moveFingerAbsolute(i, mappedVal);
    }
    Serial.println();
  }
  else if(currentMode == MODE_DEMO_1) {
    demoMode1();
  }
  else if(currentMode == MODE_DEMO_2) {
    demoMode2();
  }
  // Move the servos
  servoExecute();

  delay(50);
}
