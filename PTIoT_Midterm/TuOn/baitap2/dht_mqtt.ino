#include "DHTesp.h"
#include "WiFi.h"
#include "PubSubClient.h"

#define DHT_PIN 15
DHTesp cambienDHT;

const char *ssid = "Wokwi-GUEST";

WiFiClient espClient;
PubSubClient client(espClient);

const char *mqtt_broker = "broker.emqx.io";
const char *mqtt_id = "ed0df379-95c9-497b-9404-5d0e47b8fd91";
int Port  = 1883;

const char * MQTT_Topic = "FIT2008/info";

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

  client.setServer(mqtt_broker, Port);
}

void loop(){
  client.connect(mqtt_id);
  
  float do_am = cambienDHT.getHumidity();
  float nhietdo = cambienDHT.getTemperature();

  Serial.print("Do am: ");
  Serial.println(do_am);
  Serial.print("Nhiet do: ");
  Serial.println(nhietdo);

  String message = String(do_am) + "|" + String(nhietdo);
  client.publish(MQTT_Topic, message.c_str());

  delay(5000);
}