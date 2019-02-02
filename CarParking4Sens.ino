#include <LiquidCrystal.h>
#include <EEPROM.h>
#include <Ultrasonic.h>

#define RELAY 13
#define LCD_CONTRAST 9
#define SW1 8
#define SW2 10

LiquidCrystal lcd(7, 6, 5, 4, 3, 2);
Ultrasonic frontSense(A0, A1);
Ultrasonic rearSense(A2, A3);
Ultrasonic leftSense(A4, A5);
Ultrasonic rightSense(11, 12);

byte distance[4];
byte pos;

void setup() {
  pinMode(RELAY, OUTPUT);
  pinMode(LCD_CONTRAST, OUTPUT);
  pinMode(SW1, INPUT_PULLUP);
  pinMode(SW2, INPUT_PULLUP);
  
  analogWrite(LCD_CONTRAST, 130); 
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.print("Connecting..");

  for(pos = 0; pos < 4; pos++)
    distance[pos] = EEPROM.read(pos);
}

void updateDisplay(){
  if(pos < 4){
    lcd.clear();
    switch(pos){
      case 0:
        lcd.print("Front Offset");
        break;
      case 1:
        lcd.print("Rear Offset");
        break;
      case 2:
        lcd.print("Left Offset");
        break;
      case 3:
        lcd.print("Right Offset");
        break;
    }
    lcd.setCursor(0, 1);
    lcd.print(distance[pos]);
  }  
}

void loop() {
byte front, rear, left, right;
  
  if(pos == 4){
    //do jobs
    lcd.clear();
    front = frontSense.Ranging(CM);
    rear = rearSense.Ranging(CM);
    left = leftSense.Ranging(CM);
    right = rightSense.Ranging(CM);
    lcd.setCursor(0, 0);lcd.print(front);
    lcd.setCursor(8, 0);lcd.print(rear);
    lcd.setCursor(0, 1);lcd.print(left);
    lcd.setCursor(8, 1);lcd.print(right);

    if(front >= distance[0] &&
       rear >= distance[1] &&
       left >= distance[2] &&
       right >= distance[3]){
      digitalWrite(RELAY, HIGH);
    }else{
      digitalWrite(RELAY, LOW);
    }
    
    delay(500);
  }
  
  if(!digitalRead(SW1)){
    pos++;
    if(pos > 4)
      pos = 0;
    while(!digitalRead(SW1)){
      delay(1);
    }
    updateDisplay();
  }
  
  if(!digitalRead(SW2) && pos < 4){
    distance[pos]++;
    if(distance[pos] > 25)
      distance[pos] = 0;
      
    while(!digitalRead(SW2)){
      delay(1);
    }

    EEPROM.write(pos, distance[pos]);

    updateDisplay();
  }
}
