#include <Servo.h>

//#include <MQ2.h>

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#include <DS1302.h>

#include <IRremote.h>

#include "DHT.h"
 // CHANGE PINS
DS1302 rtc(5, 36, 37); //RST,DAT,CLK Pins of the DS1302 Module 
LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display

void temperatureAndHumidityMeasure();
void smokeDetecting();


#define delayTime 10
int refreshTimer = 0;

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~/
//IRremote Declaration
#define IRpin 46
IRrecv IR(IRpin);
decode_results cmd;
String myCom;

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~/
// LIVINGROOM PINS DECLARATION 
#define livigRoomLightOne 2      // LEDS MUST BE ON PWM PORTS!!!!!!!!!
#define livigRoomLightTwo 3      //2,3,4 at mega
#define livigRoomLightThree 4
#define potentiometerPin A0
int livingRoomLightVolume = 0;
int potetiometerValue = 0;
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~/

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~/
//  OUTDOOR LIGHTING SYSTEM DECLARATION
#define sunLightControlPin A1
#define frontLedLight 24
#define sideLedOneLight 22
#define sideLedTwoLight 23
int   sunLightReadValue = 0;
float sunLightValue = 0;
int   sunLightCatchValue = 35;
int   sunLightTimerCounter = 0;
int   sunLightCounter = 0;
int   sunLightTotalValue = 0;
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~/

// /~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~/
// TEMPERATURE AND HUMIDITY SYSTEM DECLARATION
#define DHTPin 38 //Digital Pin Connected to the DHT Sensor
#define DHTTYPE DHT11 // DEFINE SENSOR TYPE
int isSleepPressed = LOW;
DHT dht(DHTPin, DHTTYPE);
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~/

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~/
// CLAP TURN ON/OFF LIGHT
#define storageRoomLight 26
#define soundSensor 25
int storageRoomLightStatus = false;
int autoTurnOffLightTimer = 0;
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~/

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~/
//     GATE      SYSTEM      DECLARATION  
#define gateLightControlPin A2
#define servoPin 28
int gateLightValue;
int gateSensorOpenValue = 3000;
int gateSensorCloseValue = 1000;
int gateUpTimer = 0;
int gateDownTimer = 0;
int gateIsUp = LOW;
int buttonGatePressed = false;
Servo servo;
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~/

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~/
// GAMINGROOM RGB LIGHS DECLARATION
#define redRGBLightOnePin 8
#define greenRGBLightOnePin 10
#define blueRGBLightOnePin 9


//RGB DEFAULT VALUES
int gamingRGBLightsButtonPressed = false;
int redRGBLightOneBright = 0;
int blueRGBLightOneBright = 0;
int greenRGBLightOneBright = 0;
float dFact = 1;
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~/

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~/
//hallway light
int hallwayLightOne = 31; //31
int hallwayLightTwo = 32; //32
int hallwayLightSwitchButtonPin = 34;
int hallwayLightState = LOW;
int hallwayLightSwitchButtonNewState;
int hallwayLightSwitchButtonPrevState = LOW;
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~/

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~/
//WATER LEVEL SYSTEM DECLARATION
#define waterSensorPin A3
float waterSensorValue;
int waterSensorTime = 0;
int waterSensorValueSum = 0;
int waterSensorCounter = 0;
int isSModePressed = LOW; 
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~/


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~/
#define smokeDetectorPin A4
int smokeSensorValue = 0;
int smokeDetectionLed = 40;
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~/

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
//  mq2.begin();
  servo.attach(28);
  servo.write(118);
  pinMode(hallwayLightOne, OUTPUT);
  pinMode(hallwayLightTwo, OUTPUT);
  pinMode(hallwayLightSwitchButtonPin, INPUT);
  pinMode(livigRoomLightOne, OUTPUT);
  pinMode(livigRoomLightTwo, OUTPUT);
  pinMode(livigRoomLightThree, OUTPUT);
  pinMode(potentiometerPin, INPUT);
  pinMode(gateLightControlPin, INPUT);
  pinMode(storageRoomLight, OUTPUT);
  pinMode(soundSensor, INPUT);
  pinMode(servoPin, OUTPUT);
  pinMode(sunLightControlPin, INPUT);
  pinMode(frontLedLight, OUTPUT);
  pinMode(sideLedOneLight, OUTPUT);
  pinMode(sideLedTwoLight, OUTPUT);
  pinMode(redRGBLightOnePin, OUTPUT);
  pinMode(blueRGBLightOnePin, OUTPUT);
  pinMode(greenRGBLightOnePin, OUTPUT);
  pinMode(waterSensorPin, INPUT);
  
  pinMode(smokeDetectorPin, INPUT);
  pinMode(smokeDetectionLed,OUTPUT);
  IR.enableIRIn();
  dht.begin();
  lcd.init();       // initialize the lcd 
  lcd.backlight();
  // Print a message to the LCD.
  lcd.setCursor(3, 0);
  lcd.print("Welcome To");
  lcd.setCursor(4, 1);
  lcd.print("My Home!");
  
  rtc.halt(false);
  rtc.writeProtect(false);
  rtc.setDOW(SATURDAY);      //name of the day of the week
  rtc.setTime(12, 15, 00);  //Hour, Min, Sec 
  rtc.setDate(2, 6, 2022); //Day, Month, Year
  
  delay(2000);
}

void loop() {
  // put your main code here, to run repeatedly:
  
  //Smoke Detecting
  smokeDetecting();  
  
  if (waterSensorTime == 10000) {
    waterSensorTime = 0;
    waterSensorValueSum += analogRead(waterSensorPin);
    //      Serial.println(analogRead(waterSensorPin));
    waterSensorCounter++;
    if (waterSensorCounter == 5) {
      waterSensorValue = (waterSensorValueSum / 5);
//      Serial.println(waterSensorValue);
      waterSensorValueSum = 0;
      waterSensorCounter = 0;
    }
    //      waterSensorValue = 0;
  }
  waterSensorTime += delayTime;

  //  HALLWAY LIGHT CONTROL
  hallwayLightSwitchButtonNewState = digitalRead(hallwayLightSwitchButtonPin);
  if (hallwayLightSwitchButtonNewState == HIGH && hallwayLightSwitchButtonPrevState == LOW) {
    if (hallwayLightState == LOW) {
      digitalWrite(hallwayLightOne, HIGH);
      digitalWrite(hallwayLightTwo, HIGH);
      hallwayLightState = HIGH;
    } else {
      digitalWrite(hallwayLightOne, LOW);
      digitalWrite(hallwayLightTwo, LOW);
      hallwayLightState = LOW;
    }
  }
  hallwayLightSwitchButtonPrevState = hallwayLightSwitchButtonNewState;

  potetiometerValue = analogRead(potentiometerPin);
  livingRoomLightVolume = map(potetiometerValue, 0, 1023, 0, 255);
  analogWrite(livigRoomLightOne, livingRoomLightVolume);
  analogWrite(livigRoomLightTwo, livingRoomLightVolume);
  analogWrite(livigRoomLightThree, livingRoomLightVolume);
  

  //  OUTDOOR LIGHTING SYSTEM
  if (sunLightTimerCounter == 5000) {
    sunLightTimerCounter = 0;
    sunLightReadValue = analogRead(sunLightControlPin);
//    Serial.println(sunLightReadValue);
    sunLightTotalValue += sunLightReadValue;
    sunLightCounter++;
    if (sunLightCounter == 5) {
      sunLightCounter = 0;
      sunLightValue = (sunLightTotalValue / 5);
//      Serial.print(sunLightValue);
      sunLightTotalValue = 0;
    }
  }
  sunLightTimerCounter += delayTime;

  if (sunLightValue < sunLightCatchValue) {
    digitalWrite(frontLedLight, HIGH);
    digitalWrite(sideLedOneLight, HIGH);
    digitalWrite(sideLedTwoLight, HIGH);
  } else {
    digitalWrite(frontLedLight, LOW);
    digitalWrite(sideLedOneLight, LOW);
    digitalWrite(sideLedTwoLight, LOW);
  }

  // Storage ROOM CLAP LIGHT ///////
  int SensorData = digitalRead(soundSensor);
  if (SensorData == 1) {
    if (storageRoomLightStatus == false) {
      storageRoomLightStatus = true;
      digitalWrite(storageRoomLight, HIGH);
    }
  }
  if (storageRoomLightStatus == true) {
    autoTurnOffLightTimer += delayTime;
    //    Serial.println(autoTurnOffLight);
    if (autoTurnOffLightTimer == 10000) {
      autoTurnOffLightTimer = 0;
      SensorData = 0;
      storageRoomLightStatus = false;
      digitalWrite(storageRoomLight, LOW);
    }
  } else {
    storageRoomLightStatus = false;
    digitalWrite(storageRoomLight, LOW);
  }

  if (IR.decode( & cmd)) {
    Serial.println(cmd.value, HEX);
    // TURN ON/OFF BUTTON ON TV REC
    if (cmd.value == 0x155EC89F || cmd.value == 0x4FB4AB5) {
      buttonGatePressed = !buttonGatePressed;
      if (!buttonGatePressed) {
        gateDownTimer = 0;
        gateUpTimer = 0;
      }
    }
    //  EXIT
    if (cmd.value == 0x2427CC83 || cmd.value == 0x4FB50AF) {
      lcd.clear();
      lcd.setCursor(3, 0);
      lcd.print("Welcome To");
      lcd.setCursor(4, 1);
      lcd.print("My Home!");
      isSModePressed = LOW;
    }
    
    // SMODE press button handler
    if (cmd.value == 0xF9EFD0DF) {
      isSModePressed = !isSModePressed;
      isSleepPressed = false;
    }

    //    MUTE button 
    if (cmd.value == 0x4FBCA35 || cmd.value == 0xB2F22C63) {
      // ADD Tur ON/OFF KIDS RGB LIGHT  
      gamingRGBLightsButtonPressed = !gamingRGBLightsButtonPressed;
      if (gamingRGBLightsButtonPressed) {
        redRGBLightOneBright = 255;
        blueRGBLightOneBright = 255;
        greenRGBLightOneBright = 255;
      }else{
        redRGBLightOneBright = 0;
        blueRGBLightOneBright = 0;
        greenRGBLightOneBright = 0;
      }
    }
    
    //   ADD MULTIPLE COLORS ON KIDS ROOM
    //  one
    if (cmd.value == 0xF3DDC8C3 || cmd.value == 0x4FB00FF) {
      redRGBLightOneBright = 0;
      greenRGBLightOneBright = 0;
      blueRGBLightOneBright = 255;
    }
    //    two
    if (cmd.value == 0x3EE5EF3F || cmd.value == 0x4FB08F7) {
      redRGBLightOneBright = 255;
      greenRGBLightOneBright = 0;
      blueRGBLightOneBright = 0;
    }
    //   Three 
    if (cmd.value == 0x3EE5EF3F || cmd.value == 0x4FB8877 || cmd.value == 0x98934743) {
      redRGBLightOneBright = 0;
      greenRGBLightOneBright = 255;
      blueRGBLightOneBright = 0;
    }

    //  Four
    if (cmd.value == 0x3EE5EF3F) {
      redRGBLightOneBright = 255;
      greenRGBLightOneBright = 0;
      blueRGBLightOneBright = 255;
    }

    if (cmd.value == 0xCF34F03) {
      redRGBLightOneBright = 255;
      greenRGBLightOneBright = 255;
      blueRGBLightOneBright = 0;
    }

    //  PMODE 
    if (cmd.value == 0xCED71B3F) {
      isSModePressed = false;
      isSleepPressed = false;
      if (waterSensorValue < 600) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Water Level Is");
        lcd.setCursor(1, 1);
        lcd.print("Extremely Low!");
      }
      if (waterSensorValue > 600 && waterSensorValue < 650) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Water Level Is");
        lcd.setCursor(1, 1);
        lcd.print("Medium");
      }
      if (waterSensorValue > 651) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Water Level Is");
        lcd.setCursor(1, 1);
        lcd.print("High");
      }
    }
//    isSleepPressed
    if(cmd.value == 0xE0C073FF || cmd.value == 0x4FBF807){
      isSleepPressed = !isSleepPressed;
      isSModePressed = LOW;
    }

    if (cmd.value == 0xDF6E70A3 || cmd.value == 0x4FB906F) {
      dFact = dFact * .75;
    }

    if (cmd.value == 0xBBF4F9F || cmd.value == 0x4FB40BF) {
      dFact = dFact * 1.3;
      if (dFact > 1) {
        dFact = 1;
      }
    }
    IR.resume();
    analogWrite(redRGBLightOnePin, redRGBLightOneBright * dFact);
    analogWrite(greenRGBLightOnePin, greenRGBLightOneBright * dFact);
    analogWrite(blueRGBLightOnePin, blueRGBLightOneBright * dFact);
  }

  // SMODE display message per loop
  if (isSModePressed) {
    refreshTimer += delayTime;
    if(refreshTimer == 1000){
      refreshTimer = 0;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Time:");
      lcd.setCursor(5, 0);
      lcd.print(rtc.getTimeStr());
      lcd.setCursor(0, 1);
      lcd.print("Date:");
      lcd.setCursor(5, 1);
      lcd.print(rtc.getDateStr(FORMAT_SHORT, FORMAT_LITTLEENDIAN, '/'));
      }
  }
//  Sleep button on remote -- Print values at LCD 
  if (isSleepPressed) {
    refreshTimer += delayTime;
    if(refreshTimer == 1000){
      refreshTimer = 0;
      temperatureAndHumidityMeasure();
    }
  }

  gateLightValue = analogRead(gateLightControlPin);
  //  Serial.println(gateLightValue);
  if (buttonGatePressed) {
    gateDownTimer = 0;
    //    Serial.println("Gate is Opening");
    //    Serial.println(gateUpTimer);
    gateUpTimer += delayTime;
    if (gateUpTimer == gateSensorOpenValue) {
      gateUpTimer = 0;
      servo.write(18);
      gateIsUp = HIGH;
      gateDownTimer = 0;
    }
  }
  if (gateLightValue > 100 && gateIsUp == HIGH) {
    gateUpTimer = 0;
    buttonGatePressed = false;
    gateDownTimer += delayTime;
    if (gateDownTimer == gateSensorCloseValue) {
      gateDownTimer = 0;
      servo.write(118);
      gateIsUp = LOW;
    }
  }

  if (gateDownTimer != gateSensorCloseValue && gateLightValue < 100 && gateIsUp == HIGH) {
    gateDownTimer = 0;
    Serial.println("Sensor Detecting Car On The Way ");
  }
  
  delay(delayTime);
} // END OF LOOP 

void temperatureAndHumidityMeasure() {
  // Reading humidity from sensor
  float humidity = dht.readHumidity();
  // Reading temperature from sensor(Celsius)
  float celsius = dht.readTemperature();

    // Check if any reads failed and exit early (to try again).
    if (isnan(humidity) || isnan(celsius)) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Failed to read");
      lcd.setCursor(0,1);
      lcd.print("from DHT sensor!");
//      Serial.println(F("Failed to read from DHT sensor!"));
      return;
    }
  // Compute heat index in Celsius (isFahreheit = false)
  float heatIndexInCelsius = dht.computeHeatIndex(celsius, humidity, false);
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Hum:");
  lcd.setCursor(4,0);
  lcd.print(humidity);
  lcd.setCursor(0, 1);
  lcd.print("Temp: ");
  lcd.setCursor(6,1);
  lcd.print(celsius);
}

void smokeDetecting(){
  smokeSensorValue = analogRead(smokeDetectorPin);
  if(smokeSensorValue > 500){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("SMOKE DETECTED");
    lcd.setCursor(0,1);
    lcd.print("Check kitchen");
    digitalWrite(smokeDetectionLed, HIGH);
    }else{
       digitalWrite(smokeDetectionLed, LOW);
     }
  }
  
