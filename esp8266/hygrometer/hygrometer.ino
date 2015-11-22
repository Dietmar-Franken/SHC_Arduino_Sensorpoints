/**
 * Sensorpunkt
 * 
 * Benötigte Hardware:
 * - Arduino Uno, Mega, Due, Pro Mini oder Nano (für Pro Mini wird zum flashen noch ein FTDI/USB Adapter benötigt)
 * - ESP8266 WLAN Chip
 *
 * @author Oliver Kleditzsch
 * @copyright Copyright (c) 2015, Oliver Kleditzsch
 * @license http://opensource.org/licenses/gpl-license.php GNU Public License
 * @since 1.0.0
 * @version 1.0.0
 */

#include "ESP8266.h"        //https://github.com/itead/ITEADLIB_Arduino_WeeESP8266
#include <SoftwareSerial.h>

//WLan Verbindungsdaten
#define SSID                "SSID des WLAN" //SSID
#define PASSWORD            "WLAN Passwort" //Passwort
#define HOST_NAME           "192.168.178.10" //IP des SHC Masters
#define HOST_PORT           (80)
#define USER_SEL_VERSION    VERSION_22

//Allgemeine Einstellungen
#define PRINT_ON_SERIAL 1 //Werte auch auf der Seriellen Schnittstelle ausgebe

//Daten des Sensorpunktes
#define POINT_ID 5   //ID des Sensorpunktes (muss zwischen 1 und 999 sein)

//Sensorpunkt Spannung senden
#define SEND_POINT_CURRENT 1  //Sensorpunkt Spannung Senden
#define SEND_POINT_INDPUT A0  //Analogeingang für die Spannungsmessung

//Verbindungs LED
#define CONNECTED_LED 5       //Leuchtet solange die WLAN Verbindung aufgebaut ist

//Sensor Typen
//0 -> nicht Verwendet
//1 -> DS18x20
//2 -> DHT11/22
//3 -> BMP085/180
//4 -> Regensensor (Analogwert)
//5 -> Feuchtigkeitssensor (Analogwert)
//6 -> LDR (Analogwert)
#define SENSOR_1_TYPE 5 //Typ des ersten Sensors (0 wenn deaktiviert)
#define SENSOR_1_ID 5   //ID des ersten Sensors (muss Zwischen 1 und 998 sein und Systemweit eindeutig)
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

//Konfiguration Hygrometer
#define HYGROMETER_POWER_PIN 2  //Pin zum anschalten des Hygrometers
#define HYGROMETER_INPUT     A1 //Spanungseingang des Hygrometers
#define HYGROMETER_INVERT    1  //Wert Invertiert

//WLan Initalisieren
SoftwareSerial mySerial(9, 8); /* RX:D9, TX:D8 */
ESP8266 wifi(mySerial, 9600);  //bei manchen auch 115200
char host[] = "192.168.178.10"; //IP des SHC Masters
char url[] = "/shc/index.php?app=shc&a&ajax=pushsensorvalues&"; //? oder & am Ende um dem Query String weiter zu fuehren

void setup(void) {
    
  //Grundkonfiguration
  if(PRINT_ON_SERIAL) {
    
    Serial.begin(9600);
    Serial.println("WLan verbinden start");
  }
  
  //Verbindungs LED
  if(CONNECTED_LED > -1) {
     
     pinMode(CONNECTED_LED, OUTPUT); 
  }
    
  //Firmware Version
  if(PRINT_ON_SERIAL) {
    
    Serial.print("FW Version: ");
    Serial.println(wifi.getVersion().c_str());
  }
    
  //WLan Modus setzen
  if (wifi.setOprToStation()) {
      
    if(PRINT_ON_SERIAL) {
      
      Serial.println("Client OK");
    }
  } else {
      
    if(PRINT_ON_SERIAL) {
      
      Serial.println("Client Error");
      while(1);
    }
  }

  //mit AccessPoint verbinden
  if (wifi.joinAP(SSID, PASSWORD)) {
      
    if(PRINT_ON_SERIAL) {
      
      Serial.println("erfolgreich mit dem AccessPoint verbunden");
      Serial.print("IP: ");      
      Serial.println(wifi.getLocalIP().c_str());
    }
  } else {
      
    if(PRINT_ON_SERIAL) {
      
      Serial.println("verbindung zum AccessPoint fehlgeschlagen");
      while(1);
    }
  }
    
  if (wifi.disableMUX()) {
    
    if(PRINT_ON_SERIAL) {
      
      Serial.println("Single Modus OK");
    }
  } else {
    
    if(PRINT_ON_SERIAL) {
      
      Serial.println("Single Modus Error");
      while(1);
    }
  }
    
  if(PRINT_ON_SERIAL) {
    
    Serial.println("Verbindung erfolgreich hergestellt");
    Serial.println("");
  }
  
  if(CONNECTED_LED > -1) {
     
     digitalWrite(CONNECTED_LED, HIGH); 
  }
  
  //Ausgang initalisieren
  pinMode(HYGROMETER_POWER_PIN, OUTPUT);
}

void loop() {

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Sende LED an //////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

if(CONNECTED_LED > -1) {
             
  digitalWrite(CONNECTED_LED, HIGH); 
}
 
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Sensor 1 Daten senden /////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  if(SENSOR_1_TYPE != 0) {
     
      digitalWrite(HYGROMETER_POWER_PIN, HIGH);
      delay(1000);
      
      int value = analogRead(HYGROMETER_INPUT);

      //Wert invertieren
      if(HYGROMETER_INVERT) {
       
         value = 1023 - value; 
      }

    //Daten Senden
    if(wifi.createTCP(HOST_NAME, HOST_PORT)) {
        
      if(PRINT_ON_SERIAL) {
        
        Serial.println("verbunden");
      }
      
      char buffer1[512];
      PString request1(buffer1, sizeof(buffer1), "");
      
      //Anfrage senden
      request1.print("GET ");
      request1.print(url);
      request1.print("spid=");
      request1.print(POINT_ID);
      request1.print("&sid=");
      request1.print(SENSOR_2_ID);
      request1.print("&type=");
      request1.print(SENSOR_2_TYPE);
      request1.print("&v1=");
      request1.print(value);
      request1.println(" HTTP/1.1");
      request1.print("Host: ");
      request1.println(host);
      request1.println();
      request1.println();
      
      //Daten senden
      const char *req1 = request1;
      wifi.send((const uint8_t*)req1, strlen(req1));
      
      //TCP verbindung trennen
      wifi.releaseTCP();
        
    } else {
        
      if(PRINT_ON_SERIAL) {
        
        Serial.println("Verbindung Fehlgeschlagen");
      }
    }
    
      if(PRINT_ON_SERIAL) {
          
          Serial.print("Feuchtigkeit: ");
          Serial.print(value);
          Serial.println("");
       }
      
      //Sensor ausschalten
      digitalWrite(HYGROMETER_POWER_PIN, LOW);
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
    if(wifi.createTCP(HOST_NAME, HOST_PORT)) {
        
      if(PRINT_ON_SERIAL) {
        
        Serial.println("verbunden");
      }
      
      if(CONNECTED_LED > -1) {
             
        digitalWrite(CONNECTED_LED, HIGH); 
      }
      
      char buffer7[512];
      PString request7(buffer7, sizeof(buffer7), "");
      
      //Anfrage senden
      request7.print("GET ");
      request7.print(url);
      request7.print("spid=");
      request7.print(POINT_ID);
      request7.print("&type=");
      request7.print(999);
      request7.print("&v1=");
      request7.print(voltage);;
      request7.println(" HTTP/1.1");
      request7.print("Host: ");
      request7.println(host);
      request7.println();
      request7.println();
      
      const char *req7 = request7;
      wifi.send((const uint8_t*)req7, strlen(req7));
      
      //TCP verbindung trennen
      wifi.releaseTCP();
        
    } else {
        
      if(PRINT_ON_SERIAL) {
        
        Serial.println("Verbindung Fehlgeschlagen"); 
      }
     
    }
    
    if(PRINT_ON_SERIAL) {
        
        Serial.print("Voltage ");
        Serial.print(voltage);
        Serial.println("V");
        Serial.println("");
      }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Sende LED aus /////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

if(CONNECTED_LED > -1) {
             
  digitalWrite(CONNECTED_LED, LOW); 
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Verweilzeit bis zum nächsten Sendevorgang /////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  delay(60000);  
}
