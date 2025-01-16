// C++ code
#include <LiquidCrystal_I2C.h>
#include <LiquidCrystal.h>
#include <Keypad.h>
#include <Wire.h>
#include <TimeLib.h>
#include <TimeAlarms.h>
#include <DHT.h>
#include "pitches.h"

#define KEY_ROW 4
#define KEY_COL 4
#define LCD_COL 16
#define LCD_ROW 2
#define ADR_LCD 0x27
#define BUZZ_PIN 13
#define TRIG_PIN 11
#define ECHO_PIN 12
#define DHT_PIN 10
#define DHT_TYPE DHT11

DHT dht(DHT_PIN, DHT_TYPE);
LiquidCrystal_I2C lcd(ADR_LCD,LCD_COL,LCD_ROW);

char keys[KEY_ROW][KEY_COL] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte ROW_PINS[KEY_ROW]={9, 8, 7, 6}; //キーパッドの列ピン
byte COL_PINS[KEY_COL]={5, 4, 3, 2}; //キーパッドの行ピン
Keypad keypad=Keypad(makeKeymap(keys), ROW_PINS, COL_PINS, KEY_ROW, KEY_COL); //キーパッドの行列とピンの対応
char key;

int melody[] = {
  NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
};

int durations[] = {
  4, 8, 8, 4, 4, 4, 4, 4
};

float distance() {
  //超音波センサの距離の計測
  float distance;
  digitalWrite(TRIG_PIN,LOW);
	delayMicroseconds(2);
	digitalWrite(TRIG_PIN,HIGH);
	delayMicroseconds(10);
	digitalWrite(TRIG_PIN,LOW);	
	distance=pulseIn(ECHO_PIN,HIGH);	
	distance=distance/58.0;
  return distance;
}

void temp_and_humid() {
  lcd.setCursor(0,1);
  lcd.print(dht.readTemperature());
  lcd.setCursor(5,1);
  lcd.print("C");
  lcd.setCursor(7,1);
  lcd.print(dht.readHumidity());
  lcd.setCursor(13,1);
  lcd.print("%");
}

void show_time() {
  time_t t = 1736774888;
  setTime(t);
  lcd.setCursor(0,0);
  lcd.print(year(t));
  lcd.setCursor(5,0);
  lcd.print(month(t));
  lcd.setCursor(7,0);
  lcd.print("/");
  lcd.setCursor(8,0);
  lcd.print(day(t));
  lcd.setCursor(11,0);
  lcd.print(hour(t));
  lcd.setCursor(13,0);
  lcd.print(":");
  lcd.setCursor(14,0);
  lcd.print(minute(t));
}

void play_music() {
  int length = sizeof(durations)/sizeof(durations[0]);
  while (true) {
    for (int note = 0; note < length; note++) {
    int duration = 1000 / durations[note];
    tone(BUZZ_PIN, melody[note], duration);
    int pauseBetweenNotes = duration * 1.30;
    delay(pauseBetweenNotes);
    }
  }
}

void set_timer() {
  key = keypad.getKey();
  int seconds = int(key);
  Alarm.timerOnce(seconds, play_music);
}

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  //show_time(get_time());
  //ピンモード設定
  pinMode(BUZZ_PIN, OUTPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(DHT_PIN, OUTPUT);

}

void loop() {
  //時間と温湿度の表示
  temp_and_humid();
  show_time();
  key = keypad.getKey();
  if (key == "*") {
    Serial.println(key);
    lcd.setCursor(15, 1);
    lcd.print(key);
    set_timer();
  }

  //タイマー停止
  if (distance() <= 50 || key == "#") {
    noTone(BUZZ_PIN);
    lcd.clear();
  } 

  delay(5000);
}
