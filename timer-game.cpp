#include <LiquidCrystal.h>

// function prototypes
void drawGuy(int direction, int x, int alternateLegs);
void updateGuy();
int readButton();
void printTime(int time);
void updateTime();
void timesUp();
void stopTime();

/*
 * LCD RS pin to digital pin 12
 * LCD Enable pin to digital pin 11
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 * LCD R/W pin to ground
 * LCD VSS pin to ground
 * LCD VCC pin to 5V
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)
 */

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

const int joystickXPin = A0;
const int joystickYPin = A1;
const int joystickSWPin = 8;

int guyX = 0;  // guy's location
int alternateLegs = 0;
int delta = 1;
unsigned long prevTime = 0;
int countTime = 0; // count up from 0 to 1000, where 1000 is 10 seconds
unsigned long prevTimeGuy = 0; // another prevTime var for drawing the guy at a slower rate
int guyUpdateTime = 500; // time between updates

// make a custom characters:
// backslash escape doesn't work so make my own
byte backslash[8] = {0b00000, 
                     0b10000, 
                     0b01000, 
                     0b00100,
                     0b00010, 
                     0b00001, 
                     0b00000, 
                     0b00000
};

void setup() {
  // initialize LCD and set up the number of columns and rows:
  lcd.begin(16, 2);

  // create a new character
  lcd.createChar(0, backslash);

  // set the cursor to the top left
  lcd.setCursor(0, 0);

  pinMode(joystickSWPin, INPUT_PULLUP);

  Serial.begin(9600);
}

void loop() {
  lcd.clear();
  updateTime();
  printTime(countTime);
  updateGuy();
  drawGuy(guyDirection, guyX, alternateLegs);
  if (readButton() == LOW) {
    stopTime();
  }
  delay(10);
}

void drawGuy(int direction, int x, int alternateLegs) {
  if (alternateLegs == 0) {
    lcd.setCursor(x, 0);
    lcd.print("( ^_^)");
    lcd.setCursor(x, 1);
    lcd.print("/    ");
    lcd.write(byte(0));
  } else {
    lcd.setCursor(x, 0);
    lcd.print("( ^_^)");
    lcd.setCursor(x, 1);
    lcd.print(" ");
    lcd.write(byte(0));
    lcd.print("  /");
  }
}

void updateGuy() {
  unsigned long currTimeGuy = millis();
  if (currTimeGuy - prevTimeGuy < guyUpdateTime) {
    return;
  }
  prevTimeGuy = currTimeGuy;

  // update the position of the guy
  guyX = guyX + delta;

  if (guyX > 4) {     // right edge
    delta = -1;
  } else if (guyX < 1) {  // left edge
    delta = 1;
  }

  //alternate leg animation
  alternateLegs = !alternateLegs;
}

int readButton() { 
  int res = digitalRead(joystickSWPin);
  // if (res == LOW) Serial.println("button pressed");
  return res;   
}

void printTime(int time) {
  // time should range from 0000 to 1000
  if (time == 1500) {
    lcd.setCursor(11, 0);
    lcd.print("15.00");
  }
  if (time == 1000) {
    lcd.setCursor(11, 0);
    lcd.print("10.00");
  } else {
    if (time > 1000) {
      lcd.setCursor(11, 0);
    } else {
      lcd.setCursor(12, 0);
    }
    lcd.print((time / 100));
    lcd.print(".");
    lcd.print((time % 100));
  }
}

void updateTime() {
  unsigned long currTime = millis();
  // increment the game time
  if (currTime - prevTime >= 10) {
    countTime += 1;
    prevTime = currTime;
  }
  // check if the player won the game
  if (countTime == 1501) {
    timesUp();
  }
}

void timesUp() {
  lcd.setCursor(11, 1);
  lcd.print("OVER!");

  delay(3000);
  countTime = 0;
  return;
}

void stopTime() {
  lcd.setCursor(11, 1);
  if (countTime < 990) {
    lcd.print("EARLY");
  } else if (countTime >= 990 && countTime < 1000) {
    lcd.print("GOOD.");
  } else if (countTime == 1000) {
    lcd.print("BEST!");
  } else if (countTime > 1000 && countTime < 1010) {
    lcd.print("GOOD.");
  } else {
    lcd.print("LATE");
  }

  delay(3000);
  countTime = 0;
  return;
}





