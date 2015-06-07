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

#include <UIPEthernet.h> // https://github.com/ntruchsess/arduino_uip
#include <OneWire.h>
#include <DallasTemperature.h> // https://github.com/milesburton/Arduino-Temperature-Control-Library
#include <PString.h>

//Allgemeine Einstellungen
#define PRINT_ON_SERIAL 1 //Werte auch auf der Seriellen Schnittstelle ausgebe

//Daten des Sensorpunktes
#define POINT_ID 5   //ID des Sensorpunktes (muss zwischen 1 und 999 sein)

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
#define SENSOR_1_TYPE 1 //Typ des ersten Sensors (0 wenn deaktiviert)
#define SENSOR_1_ID 0   //ID des ersten Sensors (muss Zwischen 1 und 998 sein und Systemweit eindeutig)
#define SENSOR_2_TYPE 1
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
#define ONE_WIRE_BUS 8          //Daten Pin des DS18x20
#define TEMPERATURE_PRECISION 9

//DS18x20 Initialisieren
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress sensor1, sensor2;

//Ethernet Initalisieren
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
byte server[] = {192, 168, 115, 10};
EthernetClient client;
char host[] = "192.168.115.10";
char url[] = "/shc/index.php?app=shc&a&ajax=pushsensorvalues&"; //? oder & am Ende um dem Query String weiter zu fuehren

void setup() {
  
  //Grundkonfiguration
  if(PRINT_ON_SERIAL) {
    
    Serial.begin(9600);
  } 
  
  //Netzwerk initialisieren
  Ethernet.begin(mac);

  //DS18x20 starten
  sensors.begin();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Hilfsfunktionen ///////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//gibt den Wert des Sensors zurueck
float printTemperature(DeviceAddress deviceAddress) {
  
  //Wert schreiben
  float tempC = sensors.getTempC(deviceAddress);
  return tempC;
}

void loop() {
  
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Sensor 1 Daten senden /////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  if(SENSOR_1_TYPE != 0) {
    
    //DS18x20 lesen
    sensors.requestTemperatures();
    
    for(int n = 0; n < sensors.getDeviceCount(); n++) {
    if(sensors.getAddress(sensor1, n)) {
      
      //Puffer
      char buffer1[20];
      PString address(buffer1, sizeof(buffer1), "");
      
      //Adresse schreiben
      address.print(sensor1[0], HEX);
      address.print("-");
      for (uint8_t i = 1; i < 8; i++) {
        
        // zero pad the address if necessary
        if (sensor1[i] < 16) address.print("0");
          address.print(String(sensor1[i], HEX));
      }
      
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
          client.print(address);
          client.print("&type=");
          client.print(SENSOR_1_TYPE);
          client.print("&v1=");
          client.print(printTemperature(sensor1));
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
        
          Serial.print("Sensor: ");
          Serial.println(address);
          Serial.print("Temperatur: ");
          Serial.print(printTemperature(sensor1));
          Serial.println("C");
          Serial.println("");
       }
    }
  }
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
    
    if(PRINT_ON_SERIAL) {
          
        Serial.print("Voltage ");
        Serial.println(voltage);
        Serial.println("");
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Verweilzeit bis zum nächsten Sendevorgang /////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  delay(30000);  
}
