#include "MFRC522.h"
#define RST_PIN D4 // RST-PIN for RC522 - RFID - SPI - Modul GPIO15 
#define SS_PIN  D3  // SDA-PIN for RC522 - RFID - SPI - Modul GPIO2
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <FS.h>
MFRC522 mfrc522(SS_PIN, RST_PIN); 
ESP8266WebServer server(80);

//  1 TX
//  3 RX


void setup() {
//    Serial.begin(115200);    // Initialize serial communications
    web();
    pinMode(1, OUTPUT);    // TX
    pinMode(3, OUTPUT);    // RX
    SPI.begin();           // Init SPI bus
    mfrc522.PCD_Init();    // Init MFRC522   
}


void loop() { 
    checkRfid();
    server.handleClient();
//  delay(100);
}
