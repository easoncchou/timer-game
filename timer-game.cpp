#include <LiquidCrystal.h>

// function prototypes
void drawGuy(int direction, int x, int alternateLegs);
void updateGuy();
int readButton();
void printTime(int time);
void timesUp(bool win);

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
int guyDirection = 0;
int prevX = 0;
int alternateLegs = 0;
int delta = 1;
int prevTime = 0;
int countTime = 0; // count up from 0 to 1000, where 1000 is 10 seconds
int prevTimeGuy = 0; // another prevTime var for drawing the guy at a slower rate
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
                     0b00000};

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
  int currTime = millis();
  if (currTime - prevTime >= 10) {
    countTime += 1;
    prevTime = currTime;
  }
  if (countTime == 1001) {
    timesUp(!readButton());
    countTime = 0;
  }

  updateGuy();
  lcd.clear();
  printTime(countTime);
  drawGuy(guyDirection, guyX, alternateLegs);
  readButton();
  delay(10);
}

void drawGuy(int direction, int x, int alternateLegs) {
  // direction=0 is right, =1 is left
  if (direction == 0) {
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
  } else {
    if (alternateLegs == 0) {
      lcd.setCursor(x, 0);
      lcd.print("(^_^ )");
      lcd.setCursor(x, 1);
      lcd.print("/    ");
      lcd.write(byte(0));
    } else {
      lcd.setCursor(x, 0);
      lcd.print("(^_^ )");
      lcd.setCursor(x, 1);
      lcd.print(" ");
      lcd.write(byte(0));
      lcd.print("  /");
    }
  }
}

void updateGuy() {
  int currTimeGuy = millis();
  if (currTimeGuy - prevTimeGuy < guyUpdateTime) {
    return;
  }
  prevTimeGuy = currTimeGuy;

  // update the position of the guy
  prevX = guyX;
  guyX = guyX + delta;

  if (guyX > 4) {     // right edge
    delta = -1;
  } else if (guyX < 1) {  // left edge
    delta = 1;
  }

  // if the guy has moved, alternate leg animation
  if (prevX != guyX) {
    alternateLegs = !alternateLegs;
  }

  // update the direction of the guy for drawing
  if (delta < 0) {
    guyDirection = 1; // face left
  } else if (delta > 0) {
    guyDirection = 0; // face right
  }
}

// TODO: check against cheating by detecting rising edge in readButton
int readButton() { 
  int res = digitalRead(joystickSWPin);
  // if (res == LOW) Serial.println("button pressed");
  return res;   
}

void printTime(int time) {
  // time should range from 0000 to 1000
  if (time == 1000) {
    lcd.setCursor(11, 0);
    lcd.print("10.00");
  } else {
    lcd.setCursor(12, 0);
    lcd.print((time / 100));
    lcd.print(".");
    lcd.print((time % 100));
  }
}

void timesUp(bool win) {
  lcd.setCursor(11, 1);
  
  if (win) {
    lcd.print("WIN!!");
  } else {
    lcd.print("MISS!");
  }
  
  delay(3000);
  do {
  
  } while (readButton() == HIGH);

  return;
}

