/*
 * Auteur : G.Menez
 */

#undef TLS_USE
#define MQTT_CRED

#include <stdio.h>
#include <time.h>

// Arduino JSON
#include "ArduinoJson.h"

// SPIFFS
#include <SPIFFS.h>
// OTA
#include <ArduinoOTA.h>
#include "ota.h"
// Capteurs
#include "OneWire.h"
#include "DallasTemperature.h"
// Wifi (TLS) https://github.com/espressif/arduino-esp32/tree/master/libraries/WiFiClientSecure
#include <WiFi.h>
#include <WiFiClientSecure.h>

#include "classic_setup.h"
// MQTT https://pubsubclient.knolleary.net/
#include <PubSubClient.h>

#include "HTTPClient.h"
HTTPClient http;


/*===== ESP GPIO configuration ==============*/
/* ---- LED         ----*/
const int LEDpin = 19; // LED will use GPIO pin 19
/* ---- Light       ----*/
const int LightPin = A5; // Read analog input on ADC1_CHANNEL_5 (GPIO 33)
/* ---- Temperature ----*/
OneWire oneWire(23); // Pour utiliser une entite oneWire sur le port 23
DallasTemperature TempSensor(&oneWire) ; // Cette entite est utilisee par le capteur de temperature

String whoami; // Identification de CET ESP au sein de la flotte

/*===== JSON =================================*/
//StaticJsonBuffer<200> jsonBuffer;

/*===== WIFI =================================*/

#ifdef TLS_USE
WiFiClientSecure secureClient;     // Avec TLS !!!
#else
WiFiClient espClient;                // Use Wifi as link layer
#endif

/*===== MQTT broker/server and TOPICS ========*/
//String MQTT_SERVER = "192.168.197.132";
String MQTT_SERVER = "test.mosquitto.org";

#ifdef TLS_USE
int MQTT_PORT =  8883; // for TLS cf https://test.mosquitto.org/
#else
int MQTT_PORT = 1883;
#endif

//==== MQTT Credentials =========
#ifdef MQTT_CRED
char *mqtt_id     = "deathstar";
char *mqtt_login  = "darkvador";
char *mqtt_passwd = "6poD2R2";
#else
char *mqtt_id     = "deathstar";
char *mqtt_login  = NULL;
char *mqtt_passwd = NULL;
#endif

//==== MQTT TOPICS ==============
#define TOPIC_TEMP "sensors/temp"
#define TOPIC_LED "sensors/led"
#define TOPIC_LIGHT "sensors/light"
#define TOPIC_DATA "sensors/data"
#define TOPIC_VALIDATION "test123/Remi"

#ifdef TLS_USE
PubSubClient client(secureClient); // MQTT client
#else
PubSubClient client(espClient);      // The ESP is a MQTT Client
#endif

void mqtt_pubcallback(char* topic, byte* message, unsigned int length) {
  /* 
   *  MQTT Callback ... if a message is published on this topic.
   */
  
  // Byte list to String ... plus facile a traiter ensuite !
  // Mais sans doute pas optimal en performance => heap ?
  String messageTemp ;
  for(int i = 0 ; i < length ; i++) {
    messageTemp += (char) message[i];
  }
  
  Serial.print("Message : ");
  Serial.println(messageTemp);
  Serial.print("arrived on topic : ");
  Serial.println(topic) ;
 
  // Analyse du message et Action 
  if(String (topic) == TOPIC_LED) {
     // Par exemple : Changes the LED output state according to the message   
    Serial.print("Action : Changing output to ");
    if(messageTemp == "on") {
      Serial.println("on");
      set_pin(LEDpin,HIGH);
     
    } else if (messageTemp == "off") {
      Serial.println("off");
      set_pin(LEDpin,LOW);
    }
  }
}

void setup_mqtt_client() {
  /*
    Setup the MQTT client
  */

  // set server
  client.setServer(MQTT_SERVER.c_str(), MQTT_PORT);
  // set callback when publishes arrive for the subscribed topic
  client.setCallback(mqtt_pubcallback);
}

void mqtt_connect() {
  /*
    Connection to a MQTT broker
  */

#ifdef TLS_USE
  // For TLS
  const char* cacrt = readFileFromSPIFFS("/ca.crt").c_str();
  secureClient.setCACert(cacrt);
  const char* clcrt = readFileFromSPIFFS("/client.crt").c_str();
  secureClient.setCertificate(clcrt);
  const char* clkey = readFileFromSPIFFS("/client.key").c_str();
  secureClient.setPrivateKey(clkey);
#endif
  
  while (!client.connected()) { // Loop until we're reconnected
    Serial.print("Attempting MQTT connection...");

    // Attempt to connect => https://pubsubclient.knolleary.net/api
    if (client.connect(mqtt_id,      /* Client Id when connecting to the server */
                       mqtt_login,   /* With credential */
                       mqtt_passwd)) {
      Serial.println("connected");
    }
    else {
      Serial.print("failed, rc=");
      Serial.print(client.state());

      Serial.println(" try again in 5 seconds");
      delay(5000); // Wait 5 seconds before retrying
    }
  }
}

void mqtt_subscribe(char *topic) {
  /*
    Subscribe to a topic
  */
  if (!client.connected())
    mqtt_connect();
  
  client.subscribe(topic);
}


/*============= ACCESSEURS ====================*/

float get_temperature() {
  float temperature;
  TempSensor.requestTemperaturesByIndex(0);
  delay (750);
  temperature = TempSensor.getTempCByIndex(0);
  return temperature;
}

float get_light(){
  return analogRead(LightPin);
}

void set_pin(int pin, int val){
 digitalWrite(pin, val) ;
}

int get_pin(int pin){
  return digitalRead(pin);
}

int compteur = 0;

String get_utilisateur() {
  if(compteur == 0) {
    compteur = 1;
    return "dr008319";
  } else {
    compteur = 0;
    return "he608678";
  }
}

String getJSONTemp(){
  StaticJsonDocument<1000> jsondoc;
  jsondoc["who"] = getMacAdresse();
  jsondoc["value"] = get_temperature();
  String data = "";
  serializeJson(jsondoc, data);
  return data;
 }

String getJSONLight(){
  StaticJsonDocument<1000> jsondoc;
  jsondoc["who"] = getMacAdresse();
  jsondoc["value"] = get_light();
  String data = "";
  serializeJson(jsondoc, data);
  return data;
 }

String getJSONString(){
  /*
   * put all relevant data from esp in a "json formatted" String
   */
  StaticJsonDocument<1000> jsondoc;
  jsondoc["status"]["temperature"] = get_temperature();
  jsondoc["status"]["light"] = get_light();
  //jsondoc["status"]["ledCooler"] = led01;
  //jsondoc["status"]["ledHeater"] = led02;
  jsondoc["status"]["running"] = 1;

  jsondoc["info"]["loc"]["lat"] = 18.364548;
  jsondoc["info"]["loc"]["lgn"] = -70.222671;
  jsondoc["info"]["user"] = get_utilisateur();
  jsondoc["info"]["uptime"] = time(0);
  //jsondoc["info"]["ssid"] = getSSID();
  jsondoc["info"]["ident"] = getMacAdresse();
  //jsondoc["info"]["ip"] = getIP();

  //jsondoc["reporthost"]["target_ip"] = target_ip;
  //jsondoc["reporthost"]["target_port"] = target_port;
  //jsondoc["reporthost"]["sp"] = target_sp;
  
  //jsondoc["regul"]["threshold"] = DAY_LIGHT;
  //jsondoc["regul"]["sbn"] = TEMP_NIGHT_LOW;
  //jsondoc["regul"]["shn"] = TEMP_NIGHT_HIGH;
  //jsondoc["regul"]["sbj"] = TEMP_DAY_LOW;
  //jsondoc["regul"]["shj"] = TEMP_DAY_HIGH;
  
  String data = "";
  serializeJson(jsondoc, data);
  return data;
}
/*=============== SETUP =====================*/

void setup () {

  Serial.begin(9600);
  while (!Serial); // wait for a serial connection. Needed for native USB port only

  // Connexion Wifi
  connect_wifi();
  print_network_status();

  //Choix d'une identification pour cet ESP
  whoami =  String(WiFi.macAddress());

  // Initialize the LED
  setup_led(LEDpin, OUTPUT, LOW);

  // Init temperature sensor
  TempSensor.begin();

  // Initialize SPIFFS
  SPIFFS.begin(true);

  // MQTT broker connection
  setup_mqtt_client();
  mqtt_connect();

}

/*================= LOOP ======================*/

void loop () {
  static uint32_t tick = 0;
  char common[1000];
  char temp[100];
  char light[100];
  int32_t period = 6 * 1000l; // Publication period
  String payload;

  int httpResponseCode = 1;
  
  if ( millis() - tick < period)
  {
    goto END;
  }

  //Serial.println("End of stand by period");
  tick = millis();
  
  payload = getJSONString();
  /*
  http.begin("https://ps4iot.herokuapp.com");
  http.addHeader("Content-Type", "application/json");
  
  httpResponseCode = http.POST(payload);
  http.end();

  Serial.print("HTTP Response code: ");
  Serial.println(httpResponseCode);
  */
  

  payload.toCharArray(common, (payload.length() + 1));
  /*
  payload = getJSONTemp();
  payload.toCharArray(temp, (payload.length() + 1));

  payload = getJSONLight();
  payload.toCharArray(light, (payload.length() + 1));
*/
  client.publish(TOPIC_VALIDATION,common);
  //client.publish(TOPIC_TEMP,temp);
  //client.publish(TOPIC_LIGHT,light);
  
  
END :
  // Process MQTT ... obligatoire une fois par loop()
  client.loop(); 
}
