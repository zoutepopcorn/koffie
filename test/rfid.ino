/*
RST     = GPIO15     D8
SDA(SS) = GPIO2      D4
MOSI    = GPIO13     D7
MISO    = GPIO12     D6
SCK     = GPIO14     D5
GND     = GND
3.3V    = 3.3V
*/
 
void checkRfid() {
     if ( ! mfrc522.PICC_IsNewCardPresent()) {
        delay(50);
        return;
      }
      // Select one of the cards
      if ( ! mfrc522.PICC_ReadCardSerial()) {
        delay(50);
        return;
      }
      // Show some details of the PICC (that is: the tag/card)
//      Serial.print(F("Card UID:"));
      dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
      mario();
//      Serial.println();
 }

void dump_byte_array(byte *buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
//        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
//        Serial.print(buffer[i], HEX);
    }
}
