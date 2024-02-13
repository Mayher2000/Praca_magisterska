#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>

/////////////////////////////////////////////////////
// PINS SETUP
/////////////////////////////////////////////////////

// MOSI: 23
// MISO: 19
// SCK: 18

// TODO pozmieniac piny dla demuxow
// Demux SS (slave select)
const int SS_demux_A = 22;  // najmlodszy bit A
const int SS_demux_B = 21;  // B
const int SS_demux_C = 17;  // najstarszy bit C
const int SS_demux_G = 5;  // najstarszy bit C

// Demux RS (reset)
const int RS_demux_A = 33;  // najmlodszy bit A
const int RS_demux_B = 32;  // B
const int RS_demux_C = 4;  // najstarszy bit C
const int RS_demux_G = 16; // demux enable


/////////////////////////////////////////////////////
// FUNCTION DECLARATION
/////////////////////////////////////////////////////
void dump_byte_array(byte * buffer, byte bufferSize);
void pinModeSetup();
void PCD_PrintUID();
void demuxChange();
void oneAntennaTest();

// Create an MFRC522 instance :
MFRC522 rfid(13, 14);

// Algorytm:
// 1. najpierw demuxy nieaktywne w setupie przez 50ms 
// 2. ustawiamy cala linie na aktywna demuxem od resetu i czekamy 50ms
// 3. ustawiamy demuxem jedno PCD i robimy inita PCD
// 4. sprawdzamy czy jest aktywna jakies PICC na PCD za pomoca dwoch metod PICC_IsNewCardPresent i PICC_ReadCardSerial
// 5. printujemy uid za pomoca funkcji dump_byte_array
// 6. konczymy komunikacje z PICC za pomoca metody


void setup() {
  Serial.begin(9600);           // Initialize serial communications with the PC
  SPI.begin();                  // Init SPI bus
  pinModeSetup();
  delay(500);

}

void loop() {
  oneAntennaTest();
}

void oneAntennaTest() {
  // demux activation
  digitalWrite(RS_demux_G, HIGH);
  digitalWrite(SS_demux_G, HIGH);
  // demux select Y0
  digitalWrite(RS_demux_A, 0);
  digitalWrite(RS_demux_B, 0);
  digitalWrite(RS_demux_C, 0);
  digitalWrite(SS_demux_A, 0);
  digitalWrite(SS_demux_B, 0);
  digitalWrite(SS_demux_C, 0);
  delay(500);
  // Init PCD
  rfid.PCD_Init();
  // Show version of PCD - MFRC522 Card Reader
  rfid.PCD_DumpVersionToSerial();  
  // demux deactivation
  digitalWrite(RS_demux_G, LOW);
  digitalWrite(SS_demux_G, LOW);
  delay(500);

}
void demuxChange() {
  // Aktywuj demultipleksery
  // Przełącz demultiplekser RS dla różnych kombinacji pinów A, B i C
  // TODO narazie sa ustawione zera pod mniejsza siatke testowa 2 RS zamiast 8
  for (int RS_C = 0; RS_C < 1; RS_C++) {
    for (int RS_B = 0; RS_B < 1; RS_B++) {
      for (int RS_A = 0; RS_A < 2; RS_A++) {
        // Ustaw stany pinów A, B i C
        digitalWrite(RS_demux_A, RS_A);
        digitalWrite(RS_demux_B, 0);
        digitalWrite(RS_demux_C, 0);
        delay(1000);
        // Przełącz demultiplekser SS dla różnych kombinacji pinów A, B i C
        // TODO narazie sa ustawione zera pod mniejsza siatke testowa 2 SS zamiast 8
        for (int SS_C = 0; SS_C < 1; SS_C++) {
          for (int SS_B = 0; SS_B < 1; SS_B++) {
            for (int SS_A = 0; SS_A < 2; SS_A++) {
              // Ustaw stany pinów A, B i C
              digitalWrite(SS_demux_A, SS_A);
              digitalWrite(SS_demux_B, 0);
              digitalWrite(SS_demux_C, 0);
              Serial.print("RS_A: ");
              Serial.print(RS_A);
              Serial.print(" SS_A: ");
              Serial.println(SS_A);
              // Wywołaj funkcje odpowiedzialną za setup PCD i printowanie UID
              PCD_PrintUID();
              Serial.println();
              delay(1000);
            }
          }
        }
      }
    }
  }
}

void PCD_PrintUID() {
  rfid.PCD_Init();
  delay(50);
  Serial.print(F("Reader: "));
  rfid.PCD_DumpVersionToSerial();
  delay(50);
  for (int i = 0; i < 10; i++) {
    if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {

      // Show some details of the PICC (that is: the tag/card)
      Serial.print(F("|\t"));
      dump_byte_array(rfid.uid.uidByte, rfid.uid.size);
      Serial.print("\t| ");
      // Halt PICC
      rfid.PICC_HaltA();
      // Stop encryption on PCD
      rfid.PCD_StopCrypto1();
    } 
    else {
      Serial.print("|\t");
      Serial.print("NULL\t");
      Serial.println("\t| ");
    }
  }
}
// Helper routine to dump a byte array as hex values to Serial.
void dump_byte_array(byte * buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

// Pin setup
void pinModeSetup() {
  pinMode(SS_demux_A, OUTPUT);
  pinMode(SS_demux_B, OUTPUT);
  pinMode(SS_demux_C, OUTPUT);
  pinMode(SS_demux_G, OUTPUT);
  pinMode(RS_demux_A, OUTPUT);
  pinMode(RS_demux_B, OUTPUT);
  pinMode(RS_demux_C, OUTPUT);
  pinMode(RS_demux_G, OUTPUT);

  digitalWrite(RS_demux_G, LOW);
  digitalWrite(SS_demux_G, LOW);

}

