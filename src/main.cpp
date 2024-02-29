#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>

/////////////////////////////////////////////////////
// PINS SETUP
/////////////////////////////////////////////////////

// MOSI: 23
// MISO: 19
// SCK: 18

// SS (slave select)
const byte SS1_1 = 5;  // SS1_1
const byte SS1_2 = 2;  // SS1_1

// Demux RS (reset)
const byte RS_demux_A = 33;  // najmlodszy bit A
const byte RS_demux_B = 32;  // B
const byte RS_demux_C = 4;  // najstarszy bit C
const byte RS_demux_G = 16; // demux enable

/////////////////////////////////////////////////////
// GLOBAL VARIABLES 
/////////////////////////////////////////////////////
bool twoAntennaFlag = true;

/////////////////////////////////////////////////////
// GLOBAL INSTANCES 
/////////////////////////////////////////////////////
MFRC522 rfid(SS1_1, RS_demux_G); //MFRC522 instance

/////////////////////////////////////////////////////
// FUNCTION DECLARATION
/////////////////////////////////////////////////////
void dump_byte_array(byte * buffer, byte bufferSize);
void pinModeSetup();
void AntennaTest();



// Algorytm:
// 1. najpierw demuxy nieaktywne w setupie przez 50ms 
// 2. ustawiamy cala linie na aktywna demuxem od resetu i czekamy 50ms
// 3. ustawiamy demuxem jedno PCD i robimy inita PCD
// 4. sprawdzamy czy jest aktywna jakies PICC na PCD za pomoca dwoch metod PICC_IsNewCardPresent i PICC_ReadCardSerial
// 5. printujemy uid za pomoca funkcji dump_byte_array
// 6. konczymy komunikacje z PICC za pomoca metody

/////////////////////////////////////////////////////
// SETUP
/////////////////////////////////////////////////////

void setup() {
  Serial.begin(9600);           // Initialize serial communications with the PC
  SPI.begin();                  // Init SPI bus
  pinModeSetup();

  // RST demux select Y0
  digitalWrite(RS_demux_A, 0);
  digitalWrite(RS_demux_B, 0);
  digitalWrite(RS_demux_C, 0);
  delay(10);
}

/////////////////////////////////////////////////////
// LOOP
/////////////////////////////////////////////////////

void loop() {
  AntennaTest();
}

void AntennaTest() {
  // Init PCD
  
  if (twoAntennaFlag == true){
    rfid.PCD_ChangeChipSelectPin(SS1_1);
    twoAntennaFlag = !twoAntennaFlag;
    Serial.println("Antenna 1 INIT");
  }
  else{
    rfid.PCD_ChangeChipSelectPin(SS1_2);
    twoAntennaFlag = !twoAntennaFlag;
    Serial.println("Antenna 2 INIT");
  }
  
  rfid.PCD_Init();
  // Show version of PCD - MFRC522 Card Reader
  rfid.PCD_DumpVersionToSerial();  
  bool result = rfid.PCD_PerformSelfTest(); // perform the test
  Serial.println(F("-----------------------------"));
  Serial.print(F("Result: "));
  if (result)
    Serial.println(F("OK"));
  else
    Serial.println(F("DEFECT or UNKNOWN"));
  Serial.println();

  delay(500);

}


// Pin setup
void pinModeSetup() {
  pinMode(SS1_1, OUTPUT);
  pinMode(SS1_2, OUTPUT);
  pinMode(RS_demux_A, OUTPUT);
  pinMode(RS_demux_B, OUTPUT);
  pinMode(RS_demux_C, OUTPUT);
  pinMode(RS_demux_G, OUTPUT);

  digitalWrite(RS_demux_G, LOW);
  digitalWrite(SS1_1, HIGH);
  digitalWrite(SS1_2, HIGH);
}

