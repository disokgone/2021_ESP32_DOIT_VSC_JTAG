//#include <Adafruit_GFX.h>
//#include <Adafruit_GrayOLED.h>
//#include <Adafruit_SPITFT.h>
//#include <Adafruit_SPITFT_Macros.h>
//#include <gfxfont.h>
// 中文字取代 E:\UNO_D1R2\scripts\libraries\U8g2_for_Adafruit_GFX\src\u8g2_fonts.c 
// 底下在 line 78354 ~ 78807 的 chinese1 陣列資料 (包括大小)
// const uint8_t u8g2_font_unifont_t_chinese1[14522] U8G2_FONT_SECTION("u8g2_font_unifont_t_chinese1") = 
// ** Chinese1.map 已有的內碼才需新增進去 !
// cd C:\Users\JurngChenSu\Documents\Arduino\libraries\U8g2_for_TFT_eSPI-master\tools\font\bdfconv
// bdfconv.exe -v ../bdf/unifont.bdf -b 0 -f 1 -M ../build/chinese1.map -d ../bdf/7x13.bdf -n u8g2_font_unifont -o u8g2_font_unifont.c
// 打開 u8g2_font_unifont.c
// 全部  UTF-8 字體點陣圖在 ...\Arduino\libraries\U8g2_for_TFT_eSPI-master\tools/font/bdf/unifont.bdf
#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <U8g2_for_Adafruit_GFX.h>
#include <stdlib.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_GrayOLED.h>
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <Adafruit_ST77xx.h>
/* !! PIO12 -- PIO15 給 JTAG 使用 !!
#define SD_CS_PIN  15
#define SD_SS_MOSI_PIN  12
#define SD_MISO_PIN  13
#define SD_SCK_PIN  14 */

// #include <Adafruit_SSD1306.h>
// #include <U8g2lib.h>  有些與 <U8g2_for_Adafruit_GFX.h> 重複定義
#include <Fonts/FreeSerif12pt7b.h>
#include "SdFat.h"
#include "sdios.h"
#include "OV7670.h"

OV7670 *camera;   // 80x60 (QQQVGA). 160x120x2 frame (QQVGA), 320x240 (QVGA)
void i2c_scan();
void printDirectory (SdFile CFile, int numTabs);
void receive_Event(int howMany);  // int howMany
// 草稿碼->匯入程式庫->加入.ZIP程式庫
// Adafruit_SSD1306 display(/*MOSI*/ 23, /*CLK*/ 18, /*DC*/ 15, /*RESET*/ 4, /*CS*/ 2);
// 中文字碼在 E:\UNO_D1R2\scripts\libraries\U8g2_for_TFT_eSPI-master\src\u8g2_fonts.c (line 76546)
// U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

#define TFT_CS    32    // TFT CS pin is GPIO2  connected to arduino pin 2
#define TFT_DC    25    // TFT DC pin is GPIO15 connected to arduino pin 15
#define TFT_MOSI  26    // 23
#define TFT_SCLK  27    // 18
#define TFT_RESET 33    // 4
// Adafruit_ST7735
Adafruit_ST7735 tft1 = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RESET);
U8G2_FOR_ADAFRUIT_GFX u8g2_for_adafruit_gfx;
// const uint8_t SD_CS_PIN = 5;
#define USE_SDIO 0
const uint8_t SD_CHIP_SELECT = 5;
const int8_t DISABLE_CHIP_SELECT = -1;
uint16_t numberOfFiles = 0;
// Position of file's directory entry.
uint16_t dirIndex[50];
SdFile parIndex[50];

SdFat sd;
SdFile dirFile;
SdFile file;

// global for card size
uint32_t cardSize;

// global for card erase size
uint32_t eraseSize;
//  Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RESET);
// Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RESET);
const int SIOD = 21;  //SDA
const int SIOC = 22;  //SCL

const int VSYNC = 34;
const int HREF = 35;

const int XCLK = 32;
const int PCLK = 33;

const int D0 = 27;
const int D1 = 17;
const int D2 = 16;
const int D3 = 15;
const int D4 = 14;
const int D5 = 13;
const int D6 = 12;
const int D7 = 4;

// float p = 3.1415926;
int   count = 0;
int   i2c_Addr = 0, count_i2c_device;


void setup() {
  csd_t csd;
  uint8_t eraseSingleBlock;
  // Chip is ESP32-D0WD (rev 1) 4MB Flash, 40Mzh Xtal,
  // 出現 Connecting .....____...__  時按下 EN , 再按下 BOOT, 就會開始燒錄
  pinMode(LED_BUILTIN, OUTPUT);
  // pinMode(23, OUTPUT);    // D23 = /CS for SD card
  Serial.begin(921600);
  Wire.begin(21, 22, 50000);   // join i2c bus as master, let I2C works (預設使用使用 PIO21=SDA, PIO22=SCLK) SCCB: max 400KHz
  // Wire.onReceive(receive_Event); // call receiveEvent when the master send any thing 
  tft1.initR(INITR_GREENTAB);   // initialize a ST7735S chip, black tab
  tft1.setRotation(3);  // 0=直 (4pin在上), 1=橫 (4pin在左), 3=橫 (4pin在右)
  tft1.fillScreen(ST77XX_RED);
  tft1.setTextColor(ST77XX_WHITE);   //設定文字顏色
  tft1.setFont(&FreeSerif12pt7b);  //字型
  tft1.setCursor(0, 0);            //設定文字游標點
  tft1.print("Thanks SJC !"); //顯示文字
  delay(1000);
//  testdrawtext("YELLOW !", ST77XX_YELLOW);
  tft1.fillScreen(ST77XX_BLUE);
  tft1.setTextWrap(true);
  tft1.setTextColor(ST77XX_YELLOW);   //設定文字顏色
  tft1.setCursor(0, 60);            //設定文字游標點
  tft1.print("start !");  //顯示文字
  delay(1000);
  // testdrawtext("Hi !", ST7735_WHITE);
  // tft.fillScreen(ST77XX_BLACK);
  Serial.begin(500000);   // 115200
  Serial.setDebugOutput(true);
  //digitalWrite(23, HIGH);
  Serial.println("-- START Main() --");
  Serial.print("SD FAT version = ");    Serial.println(SD_FAT_VERSION);
  // u8g2.begin();
  // u8g2.enableUTF8Print();  //啟用UTF8文字的功能 
  // display.begin(SSD1306_SWITCHCAPVCC);
  u8g2_for_adafruit_gfx.begin(tft1);  // connect u8g2 procedures to Adafruit GFX
  
  //判斷SD模組初始化是否成功
  // SPIClass spi = SPIClass(VSPI);
  // 請務必接這四條線 spi.begin(18 /* SCK */, 19 /* MISO */, 23 /* MOSI */, 5 /* SS */);
  // 這是成功的例子: SD FAT version = 10100   initialization done.   cardSize = 31116288
  // cardSize: 15193 MB bytes    flashEraseSize: 128 blocks   eraseSingleBlock: true
  if (!sd.cardBegin(SD_CHIP_SELECT, SD_SCK_MHZ(25))) { // 測試可以: 10,25 MHz, SPI_HALF_SPEED 也可以 (失敗: 30,40MHz)
       Serial.println("initialization failed!");     }  // return;
  else Serial.println("initialization done.");
  cardSize = sd.card()->cardSize();
  if (cardSize == 0) {
    Serial.println("cardSize failed");
    // return;
  }
  else {
    Serial.print("cardSize = ");    Serial.println(cardSize);
  }
  if (!sd.card()->readCSD(&csd)) {
    Serial.println("readCSD failed");
    return;
  }
  if (csd.v1.csd_ver == 0) {
    eraseSingleBlock = csd.v1.erase_blk_en;
    eraseSize = (csd.v1.sector_size_high << 1) | csd.v1.sector_size_low;
  } else if (csd.v2.csd_ver == 1) {
    eraseSingleBlock = csd.v2.erase_blk_en;
    eraseSize = (csd.v2.sector_size_high << 1) | csd.v2.sector_size_low;
    } else {
      Serial.println("csd version error.");
      return;
    }

  cardSize = sd.card()->cardSize();
  if (cardSize == 0) Serial.println("cardSize failed !");
  else {
    Serial.print("cardSize: ");
    Serial.print(cardSize >> 11, DEC); // 0.000512*
    Serial.println(" MB bytes");
    }

  eraseSize++;
  Serial.print("flashEraseSize: ");
  Serial.print(int(eraseSize), DEC);
  Serial.println(" blocks");

  Serial.print("eraseSingleBlock: ");
  if (eraseSingleBlock) {
    Serial.println("true");
  } else {
    Serial.println("false");
  }
  /*if (dirFile.open("/", O_READ)) 
  {
    printDirectory(dirFile, 0);
  } */
  /*i2c_scan();   // 實測記憶卡的 I2c 位址 = 61 (0x3D), 鏡頭的 I2C: 29 (變動!)
  Serial.print("I2C device count = ");
  Serial.println(count_i2c_device);
  Serial.print("I2C addr = ");
  Serial.println(i2c_Addr); */
  // camera = new OV7670(OV7670::Mode::QQVGA_RGB565, SIOD, SIOC, VSYNC, HREF, XCLK, PCLK, D0, D1, D2, D3, D4, D5, D6, D7);
  // E (3768) ledc: ledc_set_duty_with_hpoint(383): hpoint argument is invalid
}

void loop() {
  char buf[32];

//  Serial.println("--OK 6--");
  tft1.fillScreen(ST77XX_BLACK);
  // display.clearDisplay();                               // clear the graphcis buffer  
  tft1.setCursor(10, 30);            //設定文字游標點
  tft1.setTextColor(ST77XX_WHITE);   //設定文字顏色
//  tft1.setFont(&FreeSerif12pt7b);  //字型
  sprintf(buf, "SD: %d Mb", cardSize >> 11);
  tft1.print(buf);  //顯示文字
  Serial.println(buf);

  u8g2_for_adafruit_gfx.setFontMode(1);                 // use u8g2 transparent mode (this is default)
  u8g2_for_adafruit_gfx.setFontDirection(0);            // left to right (this is default)
  u8g2_for_adafruit_gfx.setFont(u8g2_font_unifont_t_chinese1);  // select u8g2 font from here: https://github.com/olikraus/u8g2/wiki/fntlistall
  u8g2_for_adafruit_gfx.setCursor(20,100);                // start writing at this position
  u8g2_for_adafruit_gfx.setForegroundColor(0x3E0);  // 暗綠色
  // sprintf(buf, "記憶卡: %d Mb", cardSize >> 11);
  u8g2_for_adafruit_gfx.print(F(buf)); // 溫度 (xx記憶容量)
  u8g2_for_adafruit_gfx.setCursor(20,80);                // start writing at this position
  u8g2_for_adafruit_gfx.setForegroundColor(0x1F);   // 紅色
  u8g2_for_adafruit_gfx.print(F(" 濕度確定離開>"));   // UTF-8 string with Chinese chars 濕度確定離開
  u8g2_for_adafruit_gfx.setCursor(20,120);              // start writing at this position
  u8g2_for_adafruit_gfx.setForegroundColor(0xFFE0); // 亮水色
  u8g2_for_adafruit_gfx.print(F(" 放棄離開好?"));   // UTF-8 string with Chinese chars 放棄離開好ible
/*  u8g2.setFont(u8g2_font_unifont_t_chinese1); //使用我們做好的字型
  u8g2.firstPage();
  do {
    u8g2.setCursor(0, 0);
    u8g2.print("溫濕度確定放棄離開好");
    u8g2.setCursor(0, 50);
    u8g2.print("濕度 溫度,放棄離開!");   
    u8g2.setCursor(0, 100);
    u8g2.print("濕度 80%");  */
//  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
//  tft1.fillScreen(ST77XX_BLUE);   
  tft1.setCursor(10, 50);          //設定文字游標點
  tft1.setTextColor(ST77XX_RED);   //設定文字顏色
  i2c_Addr = 0;     i2c_scan();
  Serial.print("i2c_Addr = ");  Serial.println(i2c_Addr);
  sprintf(buf, "I2c: %d", i2c_Addr);
  tft1.print(buf);
  /*tft1.print("count = (");  //顯示文字
  tft1.print(count);        
  tft1.print(")");*/
//  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  // Serial.print("** count = ");    Serial.println(count);    count ++;
//  testdrawtext("Hi here !", ST77XX_YELLOW);
  delay(750);           // wait for a second
//  } while ( u8g2.nextPage() );
  count ++;
}

void testdrawtext(const char *text, uint16_t color) {
  tft1.setCursor(0, 0);
  tft1.setTextColor(color);
  tft1.setTextWrap(true);
  tft1.print(text);
}

    void printDirectory (SdFile CFile, int numTabs)
    {
      SdFile file;
      while (file.openNext(&CFile, O_READ))
      {
      if (file.isHidden()||false)
        {
          //file hidden, skip
        }
        else
        {
          for (uint8_t i = 0; i < numTabs; i++) 
          {
            //create tabs for spacing
            Serial.print('\t');
          }
          if (file.isSubDir())
          {
            SdFile SubDirFile;
            printDirectory(SubDirFile, numTabs+1);
          }
          else 
          {
            // Save dirIndex of file in directory.
            dirIndex[numberOfFiles] = file.dirIndex();
            parIndex[numberOfFiles] = CFile;
            // Print the file number and name.
            Serial.print(numberOfFiles);
            Serial.write(' ');
            file.printName(&Serial);
            Serial.println();
            numberOfFiles++;
          }
        }
      file.close();
      }
    }

void i2c_scan() {
  byte error, address;
  int nDevices;

  Serial.println("I2C Scanning...");
  nDevices = 0;      Wire.begin(21, 22);  // 10M -- 24M clock needed !
  for(address = 1; address < 127; address++ ) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address<16) {
        Serial.print("0");
      }
      i2c_Addr = address;
      Serial.println(address,HEX);
      nDevices++;
    }
    else if (error==4) {
      Serial.print("Unknow error at address 0x");
      if (address<16) {
        Serial.print("0");
      }
      Serial.println(address,HEX);
    }    
  }
  if (nDevices == 0) {
    Serial.println("No I2C devices found\n");
  }
  else {
    Serial.println("done\n");
  }
  count_i2c_device = nDevices;
}

void receive_Event(int howMany) { 
// char c;

   while (Wire.available() > 1) { // loop through all but the last {
      char c = Wire.read(); // receive byte as a character
      Serial.print(c); // print the character
   }
}
