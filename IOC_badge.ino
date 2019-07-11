#include <nfc.h>
#include <Wire.h>



/**
  @file    nfc_mifare_mf1s50_reader.ino
  @author  www.elechouse.com
  @brief   example of reading mf1s50 card for NFC_MODULE
  
    For this demo, waiting for a MF1S50 card or tag, after reading a card/tag UID,
    then try to read the block 4/5/6/7 ..
  
  @section  HISTORY
  
  V1.0 initial version
  
    Copyright (c) 2012 www.elechouse.com  All right reserved.
*/

/** include library */
#include <EEPROM.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1:
#define LED_PIN    6

// How many NeoPixels are attached to the Arduino?
#define LED_COUNT 7

// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
// Argument 1 = Number of pixels in NeoPixel strip
// Argument 2 = Arduino pin number (most are valid)
// Argument 3 = Pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)


// setup() function -- runs once at startup --------------------------------

char card_id[] = "";
String id = "";
int bob = 0;
String current_id = "";
int address = 0;
bool change = false;
bool first_run = true;

u8 hextab2[17]="0123456789ABCDEF";
#include "nfc.h"

/** define a nfc class */
NFC_Module nfc;

void clearEEPROM();
void writeEEPROM();
void boot();

void clearEEPROM()
{
  for (int i = 0 ; i < EEPROM.length() ; i++) {
    if(EEPROM.read(i) != 0)                     //skip already "empty" addresses
    {
      EEPROM.write(i, 0);                       //write 0 to address i
    }
  }
  Serial.println("EEPROM erased");
  address = 0;                                  //reset address counter
}

void boot()
{
  current_id = "6854";
  if (EEPROM.read(address) == 0)
  {
    colorWipe(strip.Color(0, 0, 0), 50);
    current_id = "5351";
    first_run = false;   
  }
  if (EEPROM.read(address) == 1)
  {
    colorWipe(strip.Color(255, 0, 0), 50);
    current_id = "5651";
    first_run = false;   
  }

  if (EEPROM.read(address) == 2)
  {
    colorWipe(strip.Color(0, 255, 0), 50);
    current_id = "6849";
    first_run = false;   
  }

  if (EEPROM.read(address) == 3)
  {
    colorWipe(strip.Color(0, 0, 255), 50);
    current_id = "5251";
    first_run = false;   
  }

  if (EEPROM.read(address) == 4)
  {
    colorWipe(strip.Color(255, 0, 255), 50);
    current_id = "5265";
    first_run = false;   
  }

  if (EEPROM.read(address) == 5)
  {
    colorWipe(strip.Color(255, 255, 0), 50);
    current_id = "4965";
    first_run = false;   
  }

  if (EEPROM.read(address) == 6)
  {
    colorWipe(strip.Color(0, 255, 255), 50);
    current_id = "5268";
    first_run = false;   
  }

  if (EEPROM.read(address) == 7)
  {
    colorWipe(strip.Color(255, 255, 255), 50);
    current_id = "6966";
    first_run = false;   
  }

  if (EEPROM.read(address) == 8)
  {
    theaterChaseRainbow(1);
    current_id == "6854";
    first_run = false;   
  }
}

void setup(void)
{
  
  #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
  #endif
  // END of Trinket-specific code.

  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(40); // Set BRIGHTNESS to about 1/5 (max = 255)

  Serial.begin(9600);
  nfc.begin();
  Serial.println("MF1S50 Reader Demo From Elechouse!");
  
  uint32_t versiondata = nfc.get_version();
  if (! versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1); // halt
  }
  
  // Got ok data, print it out!
  Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX); 
  Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC); 
  Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);
  
  /** Set normal mode, and disable SAM */
  nfc.SAMConfiguration();
}

void theaterChaseRainbow(int wait) {
  int firstPixelHue = 0;     // First pixel starts at red (hue 0)
  for(int a=0; a<30; a++) {  // Repeat 30 times...
    for(int b=0; b<3; b++) { //  'b' counts from 0 to 2...
      strip.clear();         //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in increments of 3...
      for(int c=b; c<strip.numPixels(); c += 3) {
        // hue of pixel 'c' is offset by an amount to make one full
        // revolution of the color wheel (range 65536) along the length
        // of the strip (strip.numPixels() steps):
        int      hue   = firstPixelHue + c * 65536L / strip.numPixels();
        uint32_t color = strip.gamma32(strip.ColorHSV(hue)); // hue -> RGB
        strip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
      }
      strip.show();                // Update strip with new contents
      delay(wait);                 // Pause for a moment
      firstPixelHue += 65536 / 90; // One cycle of color wheel over 90 frames
    }
  }
}

void colorWipe(uint32_t color, int wait) {
  for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(wait);                           //  Pause for a moment
  }
}
void loop(void)
{
  if (first_run == true)boot();

  if (current_id == "5351" && change == true)
  {
    EEPROM.write(address, 0);
    Serial.print("OFF");
    colorWipe(strip.Color(0, 0, 0), 50);
    change = false;
  }
 
  if (current_id == "5651" && change == true)
  {
    EEPROM.write(address, 1);
    Serial.print("RED");
    colorWipe(strip.Color(255, 0, 0), 50);
    change = false;
  }

  if (current_id == "6849" && change == true)
  {
    EEPROM.write(address, 2);
    Serial.print("GREEN");
    colorWipe(strip.Color(0, 255, 0), 50);
    change = false;
  }

  if (current_id == "5251" && change == true)
  {
    EEPROM.write(address, 3);
    Serial.print("BLUE");
    colorWipe(strip.Color(0, 0, 255), 50); 
    change = false;
  }

  if (current_id == "5265" && change == true)
  {
    EEPROM.write(address, 4);
    Serial.print("PINK");
    colorWipe(strip.Color(255, 0, 255), 50);
    change = false;
  }

  if (current_id == "4965" && change == true)
  {
    EEPROM.write(address, 5);
    Serial.print("YELLOW");
    colorWipe(strip.Color(255, 255, 0), 50); 
    change = false;
  }

  if (current_id == "5268" && change == true)
  {
    EEPROM.write(address, 6);
    Serial.print("AQUA");
    colorWipe(strip.Color(0, 255, 255), 50);
    change = false;
  }

  if (current_id == "6966" && change == true)
  {
    EEPROM.write(address, 7);
    Serial.print("WHITE");
    colorWipe(strip.Color(255, 255, 255), 50); // Red;
    change = false;
  }

  if (current_id == "6854")
  {
    EEPROM.write(address, 8);
    Serial.print("RAINBOWS!");
    theaterChaseRainbow(1);
  }

//  if (current_id == "6553")colorWipe(strip.Color(0, 255, 0), 50); // Green
//  if (current_id == "4950")colorWipe(strip.Color(0, 0, 255), 50); // Blue
//  if (current_id == "4948")colorWipe(strip.Color(255, 0, 255), 50); // Pink
//  if (current_id == "4957")colorWipe(strip.Color(255, 255, 0), 50); // Yellow
//  if (current_id == "6950")colorWipe(strip.Color(0, 255, 255), 50); // Aqua
//  if (current_id == "4951")colorWipe(strip.Color(255, 255, 255), 50); // White
//  if (current_id == "5048")theaterChaseRainbow(50);
  u8 buf[32],sta;
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
  
  
  /** Polling the mifar card, buf[0] is the length of the UID */
  sta = nfc.InListPassiveTarget(buf);
  
  /** check state and UID length */
  if(sta && buf[0] == 4){
    /** the card may be Mifare Classic card, try to read the block */  
    Serial.print("UUID length:");
    Serial.print(buf[0], DEC);
    Serial.println();
    Serial.print("UUID:");
    nfc.puthex(buf+1, buf[0]);
    Serial.write('*');
    u32 i;
    
    for(i=0; i<buf[0]; i++)
    {
        Serial.write(hextab2[(buf[i]>>4)&0x0F]);
        id += (hextab2[(buf[i]>>4)&0x0F]);
        Serial.write(hextab2[buf[i]&0x0F]);
        id += (hextab2[buf[i]&0x0F]);
        Serial.write(' ');
        current_id = id;
        change = true;
        id = "";
    }
    Serial.println();
    Serial.println("Card ID:");
    Serial.println(current_id);
    Serial.write('*');
    Serial.println();
    /** factory default KeyA: 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF */
    u8 key[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    u8 blocknum = 4;
    /** Authentication blok 4 */
    sta = nfc.MifareAuthentication(0, blocknum, buf+1, buf[0], key);
    if(sta){
      /** save read block data */
      u8 block[16];
      
      Serial.println("Authentication success.");
      Serial.println(bob);
      Serial.println(block[16]);
      
      // uncomment following lines for writing data to blok 4
/*      
      strcpy((char*)block, "Elechoues - NFC");
      sta = nfc.MifareWriteBlock(blocknum, block);
      if(sta){
        Serial.println("Write block successfully:");
      }
*/  

      /** read block 4 */
      sta = nfc.MifareReadBlock(blocknum, block);
      if(sta){
        Serial.println("Read block successfully:");
        
        
        nfc.puthex(block, 16);
        Serial.println();
      }
      
      /** read block 5 */
      sta = nfc.MifareReadBlock(blocknum+1, block);
      if(sta){
        Serial.println("Read block successfully:");
        
        nfc.puthex(block, 16);
        Serial.println();
      }
      
      /** read block 6 */
      sta = nfc.MifareReadBlock(blocknum+2, block);
      if(sta){
        Serial.println("Read block successfully:");
        
        nfc.puthex(block, 16);
        Serial.println();
      }
      
      /** read block 7 */
      sta = nfc.MifareReadBlock(blocknum+3, block);
      if(sta){
        Serial.println("Read block successfully:");
        
        nfc.puthex(block, 16);
        Serial.println();
      }
    }  
  }
}
