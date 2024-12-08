#include "DHTesp.h"
#include "WiFi.h"

#define DHT_PIN 15
DHTesp cambienDHT;

const char *ssid = "Wokwi-GUEST";

void WIFIConn(){
  Serial.println("Ket noi toi:" + String(ssid));
  WiFi.begin(ssid, "");
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("Da ket noi voi " + String(ssid));
  Serial.print(", IP: ");
  Serial.println(WiFi.localIP());
}

void setup(){
  Serial.begin(115200);
  cambienDHT.setup(DHT_PIN, DHTesp::DHT22);
  
  WIFIConn();
}

void loop(){
    float do_am = cambienDHT.getHumidity();
    float nhietdo = cambienDHT.getTemperature();

    Serial.print("Do am: ");
    Serial.println(do_am);
    Serial.print("Nhiet do: ");
    Serial.println(nhietdo);

    delay(5000);
}