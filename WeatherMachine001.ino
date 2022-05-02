// Donny Phan and Carson Yilidirim
// EE 4750 - 3
// 5/2/2022
// Weather Machine Program
#include "BluetoothSerial.h"  //Library for BT funciton of ESP32
#include "DHT.h"              // Library for DHT11
#include <Wire.h>             //OLED Library for SDA & SDL
#include <Adafruit_GFX.h>     
#include <Adafruit_SSD1306.h> // OLED library
#define SCREEN_WIDTH 128 // OLED width,  in pixels
#define SCREEN_HEIGHT 64 // OLED height, in pixels

// create an OLED display object connected to I2C
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#define DHTPIN 15             //GPIO 15 
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;           // Initializing Object
#define RX2 16
#define TX2 17

char readChar;
char BTreadChar;
char DHTcollect(char);


void setup() {
  SerialBT.begin("ESP32test"); //Bluetooth device name
  Serial.begin(115200);         // Enabling Serial monitor
  
  if(!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)){
    Serial.println(F("failed to start SSD1306 OLED"));
    while(1);
  }
  delay(2000);
  oled.clearDisplay(); // clear display
  oled.setTextSize(1);         // set text size
  oled.setTextColor(WHITE);    // set text color
  oled.setCursor(0, 10);       // set position to display
  oled.println("esp32io.com"); // set text
  oled.display();              // display on OLED
  Serial2.begin(115200, SERIAL_8N1, RX2, TX2);        //Initialize UART connection
  // put your setup code here, to run once:
  Serial.println("The device started, now you can pair it with bluetooth!");
  Serial.println(F("DHT11 TEST"));
  dht.begin();
}

void loop() {
 
  if(SerialBT.available() >1){
    BTreadChar = SerialBT.read();
    Serial.print(BTreadChar);
    DHTcollect(BTreadChar);
  }
}


char DHTcollect(char val){

  if(val == '1')      // Collect Temp/Humid
  {
    float h = dht.readHumidity();
    float t = dht.readTemperature(true);
  if(isnan(h) || isnan(t)){
      Serial.println(F("Failed to read from DHT sensor"));
      return 0;
  }
    Serial.print("Humidity: ");
    Serial.print(h);
    Serial.print("% \n");
    Serial.print("Temperature: ");
    Serial.print(t);
    Serial.print("C \n");
    delay(2001);
  }
   else
   return 0;
  
 return 0;
  }
