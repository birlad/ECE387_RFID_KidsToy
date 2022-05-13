/*
  Hardware:
  1. RFID Reader
  2. Arduino Uno R3
  3. MIFARE Classic White Cards (10)
  4. I2C LCD
*/

/*
  How to play the game:
  1. Use randomSeed() and random() to create an array of 10 random integers between 0-99
  2. Tap each card to the RFID Reader to assign it a number from the array
      - Assign a number from the array, making sure there are no duplicates
      - Show confirmation of number being assigned to the card on the LCD (don't display the number)
  3. Use randomSeed() and random() to pick a random index and the target value from the array
  4. Lay out the cards in a scattered formation
  5. Players take turns guessing which card contains the target value
      Pick up a card and hold it near the scanner
      LCD will show confirmation of card scanning, the number assigned to the card, and wrong/right
      Use randomSeed() and random() to pick a new index and target value from the array (could be the same as before)
  6. If the right card is scanned, confirmation will be shown on the LCD and the game will end.
*/

// I2C LCD:
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 4); // set the LCD address to 0x27 for a 16 chars and 2 line display

// MFRC522:
#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN         9
#define SS_PIN          10

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance
MFRC522::MIFARE_Key key;

int count = 0; // tracking the number of cards that have been written to
boolean scanned = false; // to check if the first card has been scanned or not
boolean filled[] = {false, false, false, false, false, false, false, false, false, false}; // to track what cards have data stored already
boolean reading = false; // to read card values
boolean setReading = false; // to initiate card reading

// Initializing array of 10 numbers to store card values:
//       Index: 0,  1,  2,  3,  4,  5,  6,  7,  8,  9
int nums[] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};

// Initializing index of target value for players to guess (from nums[]):
int target = 0;

char val[3];
int scanVal = 0; // storing value from 

// SETUP
void setup() {
  // DEBUGGING
  //Serial.begin(9600); // Uncomment this line for debugging

  // LCD Setup:
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);

  // MFRC522 Setup:
  SPI.begin();        // Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522 reader

  // Filling nums with 10 random integers from 0-99 with no duplicates:
  randArray(nums);

  // Setting target to a random value from nums
  randomSeed(analogRead(2));
  int r = random(0, 10);
  target = nums[r];
  Serial.print("Target: ");
  Serial.println(target); 

  /*
  // DEBUGGING - Printing values in nums
  Serial.println("Nums:");
  for (int i = 0; i < 10; i++) {
    Serial.println(nums[i]);
  }
  */

} // end setup()

void loop() {
  
  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if ( ! mfrc522.PICC_IsNewCardPresent() && !scanned) {
    return;
  } else {
    scanned = true;
  }

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial() && !scanned) {
    return;
  } else {
    scanned = true;
  }

  // Writing values from nums[] to each card that is scanned
  if (count < 10) {

    // Code modified from: https://www.instructables.com/RFID-RC522-Data-Reader-Multiple-Cards-With-Arduino/

    // Turning the card's UID into a String
    String content = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
      content.concat(String(mfrc522.uid.uidByte[i], HEX));
    }
    content.toUpperCase();

    // Card 1
    if (content.substring(1) == "F3 1F F6 00" && !filled[0]) {
      writeValue(mfrc522.uid, 0);
      mfrc522.PCD_Init();
    }

    // Card 2
    if (content.substring(1) == "A3 46 06 01" && !filled[1]) {
      writeValue(mfrc522.uid, 1);
      mfrc522.PCD_Init();
    }

    // Card 3
    if (content.substring(1) == "93 36 3B 01" && !filled[2]) {
      writeValue(mfrc522.uid, 2);
      mfrc522.PCD_Init();
    }

    // Card 4
    if (content.substring(1) == "D3 D1 2C 01" && !filled[3]) {
      writeValue(mfrc522.uid, 3);
      mfrc522.PCD_Init();
      //count++;
    }

    // Card 5
    if (content.substring(1) == "33 BD 3B 01" && !filled[4]) {
      writeValue(mfrc522.uid, 4);
      mfrc522.PCD_Init();
    }

    // Card 6
    if (content.substring(1) == "53 65 3B 01" && !filled[5]) {
      writeValue(mfrc522.uid, 5);
      mfrc522.PCD_Init();
    }

    // Card 7
    if (content.substring(1) == "33 75 9B 13" && !filled[6]) {
      writeValue(mfrc522.uid, 6);
      mfrc522.PCD_Init();
      //count++;
    }

    // Card 8
    if (content.substring(1) == "43 26 96 02" && !filled[7]) {
      writeValue(mfrc522.uid, 7);
      mfrc522.PCD_Init();
    }

    // Card 9
    if (content.substring(1) == "E3 4C 04 01" && !filled[8]) {
      writeValue(mfrc522.uid, 8);
      mfrc522.PCD_Init();
    }

    // Card 10
    if (content.substring(1) == "A3 4D 05 01" && !filled[9]) {
      writeValue(mfrc522.uid, 9);
      mfrc522.PCD_Init();
    }
  
  } // end writing to cards procedure - if count < 10

  // Reading scanned card values
  mfrc522.PCD_Init();
  
  if (count >= 10 && !setReading) {
    if ( ! mfrc522.PICC_IsNewCardPresent()) {
      reading = false;
    } else {
      reading = true;
      setReading = true;
    }

    // Select one of the cards
    if ( ! mfrc522.PICC_ReadCardSerial()) {
      reading = false;
    } else {
      reading = true;
      setReading = true;
    }
  }

  int cardVal = 0;
  while (reading) {

    if ( ! mfrc522.PICC_IsNewCardPresent()) {
      reading = false;
    } else {
      reading = true;
    }

    if ( ! mfrc522.PICC_ReadCardSerial()) {
      reading = false;
    } else {
      reading = true;
    }

    // Reading and printing card values
    Serial.println("\n Reading");
    cardVal = readValue(mfrc522.uid);
    if (scanVal == cardVal) {
      reading = true;
      mfrc522.PCD_Init();
      if (cardVal == 48 || scanVal == 48) {
        lcd.clear();
      } else {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(cardVal);
        lcd.setCursor(0, 1);
        lcd.print("Target: ");
        lcd.print(target);
      }
    }

    // If scanned card value = target value: Game won!
    if (cardVal == target) {
      lcd.setCursor(0, 1);
      lcd.print("Game won!");
      exit(0);
    }
    
    Serial.println(cardVal);
    
  } // end while reading == true

} // end void loop() method

/**
 * Takes an array and populates it with non-repeating random ints from 0-99 for 15 elements.
 */
void randArray(int *nums) {
  randomSeed(analogRead(3));

  for (int i = 0; i < 10; i++) {
    boolean duplicate = false;
    int r = random(0, 100);
    while (r == 48) {
      r = random(0, 100);
    }
    for (int j = 0; j < i; j++) {
      if (nums[j] == r) {
        duplicate = true;
        i--;
        break;
      }
    }
    if (!duplicate) {
      nums[i] = r;
    }
  } // end for loop to populate nums

} // end randArray method

/**
 * Writes data from the nums array to the card with the specified UID.
 */
void writeValue(MFRC522::Uid, int n) {

  // Prepare the key (used both as key A and as key B) for reading/writing data
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

  byte sector         = 0;
  byte blockAddr      = 1;
  byte dataBlock[]    = {
      (byte)nums[n], 0, 0, 0,
      0, 0, 0, 0,
      0, 0, 0, 0,
      0, 0, 0, 0
  };

  byte trailerBlock   = 2;
  MFRC522::StatusCode status;
  byte buffer[18];
  byte size = sizeof(buffer);

  // Authenticate using key A
  status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("PCD_Authenticate() failed: "));
    //return;
  }

  // Write data to the block
  status = (MFRC522::StatusCode) mfrc522.MIFARE_Write(blockAddr, dataBlock, 16);
  if (status != MFRC522::STATUS_OK) {
      Serial.print(F("MIFARE_Write() failed: "));
  }
  Serial.println();

  // Read data from the block
  status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(blockAddr, buffer, &size);
  if (status != MFRC522::STATUS_OK) {
    Serial.println("Not ok");
  } else {
    Serial.println("OK!");
    count++;
    filled[n] = true; // this only writes data once to the card - not over and over again, reducing power consumption
  }

  Serial.println(F("Current data in sector:"));
  mfrc522.PICC_DumpMifareClassicSectorToSerial(&(mfrc522.uid), &key, sector);
  mfrc522.MIFARE_Read(blockAddr, buffer, &size); // data in blockAddr of card gets written to buffer
  Serial.println();

  // Printing card value to LCD:
  // Code modified from: https://forum.arduino.cc/t/is-it-possible-to-convert-a-byte-to-char/373366/12
  char dest[3];
  memset(dest, 0, sizeof(dest));
  sprintf(&dest[0], "%02d", (int)buffer[0]);
  lcd.setCursor(0, 0);
  lcd.print("Count: ");
  lcd.print(count);
  Serial.print("Dest: ");
  Serial.print(dest);
  Serial.println();
  
} // end writeValue method

int readValue(MFRC522::Uid) {

  MFRC522::StatusCode status;
  byte sector         = 0;
  byte blockAddr      = 1;
  byte trailerBlock   = 2;
  byte buffer[18];
  byte size = sizeof(buffer);

  // Authenticate using key A
  Serial.println(F("Authenticating using key A..."));
  status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("PCD_Authenticate() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    //return;
  }

  // Show the whole sector as it currently is
  Serial.println(F("Current data in sector:"));
  mfrc522.PICC_DumpMifareClassicSectorToSerial(&(mfrc522.uid), &key, sector);
  Serial.println();

  // Read data from the block //
  Serial.print(F("Reading data from block ")); Serial.print(blockAddr);
  Serial.println(F(" ..."));
  status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(blockAddr, buffer, &size);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Read() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
  }
  Serial.print(F("Data in block ")); Serial.print(blockAddr); Serial.println(F(":"));
  Serial.println();

  reading = false; // when to read card

  // Store card value in a string
  memset(val, 0, sizeof(val));
  sprintf(&val[0], "%02d", (int)buffer[0]);
  Serial.print(val);
  Serial.println();

  // Convert string to int
  scanVal = atoi(val);
  return scanVal;
  
} // end readValue method
