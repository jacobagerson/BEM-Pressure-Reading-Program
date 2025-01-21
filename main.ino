/*
Written by Jacob Gerson, Rebecca Brautigam 1/5/2025 for Mr. Douglass Lamm
BEM Materials
Pressure Reading/Speaker Arduino Script
*/

#include <DIYables_Keypad.h>  // DIYables_Keypad library
#include <Wire.h>
#include <LiquidCrystal_I2C.h> // Liquid Crystal Library
LiquidCrystal_I2C lcd(0x27, 20, 4);

#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"
#include "Arduino.h"
SoftwareSerial softwareSerial(/*rx =*/10, /*tx =*/11);

DFRobotDFPlayerMini MP3player;

//Pressure reading Initialization:
const int pressurePin1 = A0;  // Analog input pin for the first transducer
const int pressurePin2 = A1;  // Analog input pin for the second transducer
const float voltageReference = 5.0;  // Reference voltage of Arduino
const float minVoltage = 0.5;  // Minimum voltage corresponding to 0 psi
const float maxVoltage = 4.5;  // Maximum voltage corresponding to 300 psi
const int maxPressure = 300;  // Max pressure in psi

//Keypad Initialization:
const int ROW_NUM = 4;   
const int COLUMN_NUM = 3; 
char keys[ROW_NUM][COLUMN_NUM] = {
 { '1', '2', '3' },
 { '4', '5', '6' },
 { '7', '8', '9' },
 { '*', '0', '#' }
};


byte pin_rows[ROW_NUM] = { 9, 8, 7, 6 }; 
byte pin_column[COLUMN_NUM] = { 5, 4, 3 };
DIYables_Keypad keypad = DIYables_Keypad(makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM);


int curr_key = 0;
int count_arr[2] = {0, 0};


void setup() {
 Serial.begin(9600);
 softwareSerial.begin(9600);
 if (MP3player.begin(softwareSerial)) {
   Serial.println("MP3 Player ready");
 } else {
   Serial.println("MP3 Player NOT READY");
 }
 MP3player.volume(30);
 lcd.init();
 lcd.backlight();
 Serial.println("Start");
 delay(1000);
}


void loop() {
 int count = get_count();
 float pres_a, pres_b = get_pres();
 countdown(count, pres_a, pres_b);
}


void countdown(int count, float pres_a, float pres_b) {
 Serial.println("Starting countdown");
 int num;
 int called_num[18] = {18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1};
 int count_call = 16 - count;
 int count_call1 = count_call;
 int perm_count = count;
 int i = 0;
 while (i < 1000) {
   pres_a, pres_b = get_pres();
   float pres = (pres_a + pres_b) / 2;
   float perm_pause = (4308*(pow(pres, -1.73)) + 10.5*(pow(.9786, pres)) + .308) / 2;
   float spoken[18] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0.79-perm_pause, 3.78-perm_pause};
   char key = keypad.getKey();
   if (key) {
     if (key == '*') {
       break;
     }
   }
 if (count_call == 18) {
   count_call = count_call1;
 } else {
   count_call = count_call;
 }
 num = called_num[count_call];
 MP3player.play(num);
 lcd.clear();
 lcd.setCursor(0, 0);
 lcd.print("A: ");
 lcd.print(pres_a);
 lcd.print("B: ");
 lcd.print(pres_b);
 lcd.setCursor(0, 1);
 lcd.print("Countdown: ");
 lcd.print(num-2);
 lcd.setCursor(0, 3);
 lcd.print("Press * to restart");
 delay(1000*(perm_pause + spoken[count_call]));
 count_call++;
 i++;
 Serial.println(i);
 Serial.println(count_call);
 }
}

int get_count() {
 Serial.println("Getting Count");
 int count_arr[2] = {0, 0};
 int count;
 char key = keypad.getKey();
 lcd.clear();
 while (count_arr[1] == 0 && count_arr[0] == 0) {
   lcd.setCursor(0, 0);
   lcd.print("Press * to set count");
   key = keypad.getKey();
   if (key) {
     if (key == '*') {
       lcd.clear();
       while (key != '#') {
         key = keypad.getKey();
         if (key){
           if (key == '0') {
             curr_key = 0;
           } else if (key == '1') {
             curr_key = 1;
           } else if (key == '2') {
             curr_key = 2;
           } else if (key == '3') {
             curr_key = 3;
           } else if (key == '4') {
             curr_key = 4;
           } else if (key == '5') {
             curr_key = 5;
           } else if (key == '6') {
             curr_key = 6;
           } else if (key == '7') {
             curr_key = 7;
           } else if (key == '8') {
             curr_key = 8;
           } else if (key == '9') {
             curr_key = 9;
           } else {
             curr_key = 20;
           }
           if (curr_key < 10){
             if (count_arr[1] == 0) {
               count_arr[1] = curr_key;
             } else {
               count_arr[0] = count_arr[1];
               count_arr[1] = curr_key;
             }
           }
         } else {
         }
         count = (count_arr[0]*10) + count_arr[1];
         lcd.setCursor(0, 2);
         lcd.print("Your Count: ");
         lcd.setCursor(13, 2);
         lcd.print(count_arr[0]);
         lcd.setCursor(14, 2);
         lcd.print(count_arr[1]);
         lcd.setCursor(0, 0);
         lcd.print(" Press # to confirm ");
         delay(1000);
       }
     }
   }
 }
 Serial.println(count_arr[0]);
 Serial.println(count_arr[1]);
 if (count > 16) {
   count = 10;
 } else {
   count = count;
 }
 return count;
}

float get_pres() {
 Serial.println("Retreiving pressure");
 int sensorValue1 = analogRead(pressurePin1);
 int sensorValue2 = analogRead(pressurePin2);


 // Convert the analog reading (0-1023) to voltage for transducer 1
 float voltage1 = sensorValue1 * (voltageReference / 1023.0);
 if (voltage1 < minVoltage) voltage1 = minVoltage;  // Limit voltage to minimum
 if (voltage1 > maxVoltage) voltage1 = maxVoltage;  // Limit voltage to maximum


 // Convert the voltage to pressure for transducer 1
 float pressure1 = ((voltage1 - minVoltage) / (maxVoltage - minVoltage)) * maxPressure;


 // Convert the analog reading (0-1023) to voltage for transducer 2
 float voltage2 = sensorValue2 * (voltageReference / 1023.0);
 if (voltage2 < minVoltage) voltage2 = minVoltage;  // Limit voltage to minimum
 if (voltage2 > maxVoltage) voltage2 = maxVoltage;  // Limit voltage to maximum


 // Convert the voltage to pressure for transducer 2
 float pressure2 = ((voltage2 - minVoltage) / (maxVoltage - minVoltage)) * maxPressure;


 return pressure1, pressure2;
}
