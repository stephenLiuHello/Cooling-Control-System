/*Control System Summative The Cooling System
 * 
 * Name: Stephen Liu
 * Date: January 13, 2022
 */

//Variable declarations and class importations
#include "dht.h"  //Imports the DHT class
#define dht_apin A1  // Analog Pin sensor is connected to Arduino
dht DHT;  //Creates a DHT object
int hum;  //Stores the humidity
int temp;  //Stores the temperature

const int toggle=13;  //The pin for the toggle switch
int toggleVal;  //Stores the state of the toggle switch

const int dataButton=12;  //The pin for push button
int buttonVal;  //The state of the button 
int dataState=0;  //Determines if either temperature or humidity will be displayed
boolean hasChange=false;  //Checks if the button already been pressed

// Motor A connections
const int enA=11;
const int in1=3;
const int in2=2;

const int led=A2;  //The pin to control the warning LED 

// include the library code:
#include <LiquidCrystal.h>

// Creates an LCD object. Parameters: (rs, enable, d4, d5, d6, d7)
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

//Declares the variables for the timer that controls how often the LCD's data is updated
unsigned long startMillis;
unsigned long currentMillis;
const unsigned long period=5000;

void setup(){
  pinMode(toggle, INPUT);
  pinMode(dataButton, INPUT_PULLUP);

  // Set all the motor control pins to outputs
  pinMode(enA, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);

  // Turn off motors - Initial state
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);

  pinMode(led, OUTPUT);

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);

  // Clears the LCD screen
  lcd.clear();

  //Initial start time (5000 milliseconds was deducted to allow for the program to display right away)
  startMillis=millis()-5000;
}

void loop(){
  //Changes the display data from humidity to temperature or vice versa
  buttonVal=digitalRead(dataButton);
  if(buttonVal==0 && !hasChange){
    if(dataState==0){
      dataState=1;
    }
    else{
      dataState=0;
    }
    hasChange=true;
  }
  delay(100);
  
  currentMillis=millis();  //Gets the current "time" (The number of milliseconds since the program started)
  DHT.read11(dht_apin);
  hum=(int) DHT.humidity;
  temp=(int) DHT.temperature;
  //If the period had elapsed, new updated data is displayed to the LCD
  if(currentMillis-startMillis>=period){
    lcd.clear();
    if(dataState==0){
      lcd.print("Humidity: ");
      lcd.setCursor(0, 1);
      lcd.print(hum);
      lcd.print("% ");    
    }
    else{
      lcd.print("Temperature: ");
      lcd.setCursor(0, 1);
      lcd.print(temp); 
      lcd.print("C ");
    }
    startMillis=currentMillis;
    hasChange=false;
  }

  //Turns on the warning LED when the humidity is higher than 60% or if the temperature exceeds 40C
  if(hum>60 || temp>40){
    digitalWrite(led, 1);
  }

  toggleVal=digitalRead(toggle);
  //Only when the toggle switch is switch on will the fan operate
  if(toggleVal==1){
    //Maps the humidity and temperature to the output range of the DC Motor
    hum=map(hum, 0, 100, 100, 255);
    if(temp<20){
      temp=0;
    }
    else if(temp>60){
      temp=255;
    }
    else{
      temp=map(temp, 20, 60, 100, 255);
    }
    
    // Set motor speed to average of humidity and temperature measure
    analogWrite(enA, (hum+temp)/2);
  
    // Turn on motor A
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
  }
  else{
    // Turn off motor A
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
  }
}
