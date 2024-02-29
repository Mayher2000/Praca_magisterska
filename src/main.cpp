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
byte activeAntenna = 1;
/////////////////////////////////////////////////////
// GLOBAL INSTANCES 
/////////////////////////////////////////////////////
MFRC522 rfid(SS1_1, RS_demux_G); //MFRC522 instance

/////////////////////////////////////////////////////
// FUNCTION DECLARATION
/////////////////////////////////////////////////////
void dump_byte_array(byte * buffer, byte bufferSize);
void pinModeSetup();
void AntennaTestInit();
void AntennaTestRead();



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
  AntennaTestRead();
}

void AntennaTestRead() {
  // State machine for antennas switching
  switch (activeAntenna){
  case 1:
    AntennaSelect(SS1_1, 0, 0, 0, 1);
    break;
  case 2:
    AntennaSelect(SS1_2, 0, 0, 0, 1);
    break;
  case 3:
    AntennaSelect(SS1_1, 1, 0, 0, 1);
    break;
  case 4:
    AntennaSelect(SS1_2, 1, 0, 0, 1);
    break;
  default:
    Serial.println("Invalid antenna number");
    break;
  }
  // Initing the Antenna 
  rfid.PCD_Init();

  // Looking for a card
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    Serial.print("|\t");
    rfid.PICC_DumpDetailsToSerial(&(rfid.uid)); //dump some details about the card
    Serial.print("\t|");
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();  
  }
  else Serial.println("|\tNONE\t|");
  if (activeAntenna % 2 == 1) Serial.println();
  delay(100);
}

void AntennaSelect(byte SS, byte RS_A, byte RS_B, byte RS_C, int selectedAntenna) {
    // set SS for Antenna 1
    rfid.PCD_ChangeChipSelectPin(SS);
    // set RST for Antenna 1
    digitalWrite(RS_demux_A, RS_A);
    digitalWrite(RS_demux_B, RS_B);
    digitalWrite(RS_demux_C, RS_C);
    // debug info
    Serial.print("Ant");
    Serial.print(selectedAntenna);
    Serial.print(" :");
    // change the state for next iteration
    if (activeAntenna == 4) activeAntenna = 1;
    else activeAntenna = selectedAntenna + 1;
}
void AntennaTestInit() {
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

