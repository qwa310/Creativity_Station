#include <DHTesp.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "FirebaseESP8266.h" 
#include <ESP8266WiFi.h>
#define RELAY 13
#define TEMP 0
#define FIREBASE_HOST "newoven-b4420.firebaseio.com" 
#define FIREBASE_AUTH "FOsWDtHHfoDsVaZRksdlHnLta7JWpmLWmbf06RwA"
#define WIFI_SSID "qwa310" 
#define WIFI_PASSWORD "310310310" 
DHTesp dht;
LiquidCrystal_I2C lcd(0x27,20,4);
FirebaseData firebaseData;
FirebaseJson json;

void setup() {
  ESP.wdtDisable();
  dht.setup(0,DHTesp::DHT22);
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
  //Set the size of WiFi rx/tx buffers in the case where we want to work with large data.
  firebaseData.setBSSLBufferSize(1024, 1024);
  //Set the size of HTTP response buffers in the case where we want to work with large data.
  firebaseData.setResponseSize(1024);
  //Set database read timeout to 1 minute (max 15 minutes)
  Firebase.setReadTimeout(firebaseData, 1000 * 60);
  //tiny, small, medium, large and unlimited.
  //Size and its write timeout e.g. tiny (1s), small (10s), medium (30s) and large (60s).
  Firebase.setwriteSizeLimit(firebaseData, "tiny");
  lcd.begin();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Injeolmi Controller");
  lcd.setCursor(0,1);
  lcd.print("Temperature : ");
  lcd.setCursor(0,2);
  lcd.print("Humidity : ");
  lcd.setCursor(0,3);
  lcd.print("Status : ");
  pinMode(TEMP,INPUT_PULLUP);
  pinMode(RELAY, OUTPUT);
  delay(4000);
  ESP.wdtDisable();
}

void loop() {
  ESP.wdtDisable();
  delay(4000);
  int target;
  String val;
  float t=dht.getTemperature();
  float h=dht.getHumidity();
  Serial.print("humidity:");
  Serial.println(h); 
  Serial.print("temperature:");
  Serial.println(t);
  // Clear the screen
  lcd.setCursor(14,1);
  lcd.print(t);
  lcd.print("C");
  lcd.setCursor(11,2);
  lcd.print(h);
  lcd.print("%");
  if (isnan(t) || isnan(h)){
    delay(2000);
  }
  else{
    ESP.wdtDisable();
    Firebase.setFloat(firebaseData,"Humidity", h);   
    delay(4000);
    Firebase.setFloat(firebaseData,"Temperature", t); 
    yield();
    if (Firebase.getString(firebaseData, "value")){
      yield();
      ESP.wdtDisable();
      val = firebaseData.stringData();
      target=val.toInt();
      Serial.println(target);
      yield();
      delay(4000);
    }
    yield();
    delay(4000);
 
    if(t < target){
      ESP.wdtDisable();
      lcd.setCursor(9,3);
      lcd.print("Good!     ");
      digitalWrite(RELAY,HIGH);
      Firebase.setString(firebaseData,"STATUS", "GOOD");
      yield();
      delay(4000);
    }
    else{
      ESP.wdtDisable();
      lcd.setCursor(9,3);
      lcd.print("Not Good!!");
      digitalWrite(RELAY,LOW);
      Firebase.setString(firebaseData,"STATUS", "NOTGOOD");
      yield();
      delay(4000);
    }
  }
  yield();
  delay(4000);
}
