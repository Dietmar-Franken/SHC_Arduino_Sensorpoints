/**
 * Sensorpunkt
 * 
 * Benötigte Hardware:
 * - Arduino Uno, Mega, Due, Pro Mini oder Nano (für Pro Mini wird zum flashen noch ein FTDI/USB Adapter benötigt)
 * - Arduino Ethernet Shield
 *
 * @author Oliver Kleditzsch
 * @copyright Copyright (c) 2015, Oliver Kleditzsch
 * @license http://opensource.org/licenses/gpl-license.php GNU Public License
 * @since 1.0.0
 * @version 1.0.0
 */

#include <SPI.h>
#include <Ethernet.h> 
#include <DHT.h>         // https://github.com/markruys/arduino-DHT

//Allgemeine Einstellungen
#define PRINT_ON_SERIAL 1 //Werte auch auf der Seriellen Schnittstelle ausgebe

//Daten des Sensorpunktes
#define POINT_ID 1   //ID des Sensorpunktes (muss zwischen 1 und 999 sein)

//Sensorpunkt Spannung senden
#define SEND_POINT_CURRENT 1  //Sensorpunkt Spannung Senden
#define SEND_POINT_INDPUT A0  //Analogeingang für die Spannungsmessung

//Sensor Typen
//0 -> nicht Verwendet
//1 -> DS18x20
//2 -> DHT11/22
//3 -> BMP085/180
//4 -> Regensensor (Analogwert)
//5 -> Feuchtigkeitssensor (Analogwert)
//6 -> LDR (Analogwert)
#define SENSOR_1_TYPE 2 //Typ des ersten Sensors (0 wenn deaktiviert)
#define SENSOR_1_ID 1   //ID des ersten Sensors (muss Zwischen 1 und 998 sein und Systemweit eindeutig)
#define SENSOR_2_TYPE 0
#define SENSOR_2_ID 0
#define SENSOR_3_TYPE 0
#define SENSOR_3_ID 00
#define SENSOR_4_TYPE 0
#define SENSOR_4_ID 00
#define SENSOR_5_TYPE 0
#define SENSOR_5_ID 00
#define SENSOR_6_TYPE 0
#define SENSOR_6_ID 00

//Sonsorbelegungen
#define DHT_POWER_PIN 2 //Pin der den DHT ein/aus-schaltet
#define DHT_PIN 3       //Pin an dem der DHT Data Pin angeschlossen ist

//Ethernet Initalisieren
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
byte server[] = {192, 168, 115, 221};
EthernetClient client;
char host[] = "192.168.115.10";
char url[] = "/shc/index.php?app=shc&a&ajax=pushsensorvalues&"; //? oder & am Ende um dem Query String weiter zu fuehren

//DTH Initalisieren
// Connect pin 1 (on the left) of the sensor to +5V
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor
DHT dht;

void setup() {
  
  //Grundkonfiguration
  if(PRINT_ON_SERIAL) {
    
    Serial.begin(9600);
  }
  
  //DHT PIN
  pinMode(DHT_POWER_PIN, OUTPUT);
  
  //Bis zum ersten durchlauf einschalten
  digitalWrite(DHT_POWER_PIN, HIGH); 
  
  //Netzwerk initialisieren
  Ethernet.begin(mac);

  //DHT Setup
  delay(2500);
  dht.setup(DHT_PIN);
}

void loop() {
  
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Sensor 1 Daten senden /////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  if(SENSOR_1_TYPE != 0) {
    
    digitalWrite(DHT_POWER_PIN, HIGH);
    delay(2500);
  
    float temperature = dht.getTemperature();
    float humidity = dht.getHumidity();
    
    //Daten Senden
    if(client.connect(server, 80)) {
        
      if(PRINT_ON_SERIAL) {
        
        Serial.println("verbunden");
      }
      
      //Anfrage senden
      client.print("GET ");
      client.print(url);
      client.print("spid=");
      client.print(POINT_ID);
      client.print("&sid=");
      client.print(SENSOR_1_ID);
      client.print("&type=");
      client.print(SENSOR_1_TYPE);
      client.print("&v1=");
      client.print(temperature);
      client.print("&v2=");
      client.print(humidity);
      client.println(" HTTP/1.1");
      client.print("Host: ");
      client.println(host);
      client.println();
      client.println();
      client.stop();
        
    } else {
        
      if(PRINT_ON_SERIAL) {
        
        Serial.println("Verbindung Fehlgeschlagen"); 
      }
    }
    
    if(PRINT_ON_SERIAL) {
        
        Serial.print("Temperatur: ");
        Serial.print(temperature);
        Serial.println("C");
        Serial.print("Luftfeuchte: ");
        Serial.print(humidity);
        Serial.println("%");
        Serial.println("");
     }
    
    //Sensor ausschalten
    digitalWrite(DHT_POWER_PIN, LOW);
  }
  
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Sensor 2 Daten senden /////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Sensor 3 Daten senden /////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Sensor 4 Daten senden /////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Sensor 5 Daten senden /////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Sensor 6 Daten senden /////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Sensorpunkt Spannung senden ///////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

if(SEND_POINT_CURRENT) {
 
    //Betriebsspannung ermitteln
    float voltage = analogRead(SEND_POINT_INDPUT) * (14.5 / 1023.0);  
    
    //Daten Senden
    if(client.connect(server, 80)) {
        
      if(PRINT_ON_SERIAL) {
        
        Serial.println("verbunden");
        Serial.print("Voltage ");
        Serial.println(voltage);
      }
      
      //Anfrage senden
      client.print("GET ");
      client.print(url);
      client.print("spid=");
      client.print(POINT_ID);
      client.print("&type=");
      client.print(999);
      client.print("&v1=");
      client.print(voltage);;
      client.println(" HTTP/1.1");
      client.print("Host: ");
      client.println(host);
      client.println();
      client.println();
      client.stop();
        
    } else {
        
      if(PRINT_ON_SERIAL) {
        
        Serial.println("Verbindung Fehlgeschlagen"); 
      }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Verweilzeit bis zum nächsten Sendevorgang /////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  delay(30000);  
}
