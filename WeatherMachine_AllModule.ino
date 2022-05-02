// Donny Phan and Carson Yilidirim
// EE 4750 - 3
// 5/2/2022
// Weather Machine Program
enum { BT_RESET, BT_START, BT_STOP} CmdState = BT_RESET;

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

//Initializing Prototype 
void OledDisplay();
void Tempread();
void BTread();
float displayPrompt(float, float);

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
  oled.println("Hello, Please connect"); // set text
  oled.display();              // display on OLED
  Serial2.begin(115200, SERIAL_8N1, RX2, TX2);        //Initialize UART connection
  // put your setup code here, to run once:
  Serial.println("The device started, now you can pair it with bluetooth!");
  Serial.println(F("DHT11 TEST"));
  dht.begin();
}

void loop() {
  BTread();
  Tempread();
  OledDisplay();
}

void BTread(){          // Starting point / Reset of enum
  if(CmdState !=BT_RESET){
  return;
  }
  oled.clearDisplay();
  oled.setCursor(0,0);
  oled.setTextSize(1);
  oled.setTextColor(WHITE);
  oled.print("Waiting on command");
  oled.display();
  if(SerialBT.available() >1){
    BTreadChar = SerialBT.read();
    Serial.print(BTreadChar);
    if(BTreadChar == '1')
    {
      CmdState = BT_START;
    }
    if(BTreadChar == '0')
      {
        CmdState = BT_STOP;
      }
  }
}

//Collect temperature and humidity
void Tempread(){
  if(CmdState !=BT_START)
  {
    return;
  }  
    float h = dht.readHumidity();
    float t = dht.readTemperature(true);
  if(isnan(h) || isnan(t)){
      Serial.println(F("Failed to read from DHT sensor"));
      return;
  }
  // Displaying Data on OLED
    oled.clearDisplay(); // clear display
    oled.setTextSize(1);         // set text size
    oled.setTextColor(WHITE);    // set text color
    oled.setCursor(0,0);       // set position to display
    oled.print("Temperature: ");
    oled.print(t);
    oled.print(" C");
    oled.setCursor(0,10);
    oled.print("Humidity: ");
    oled.print(h);
    oled.print(" %");
    oled.display();
   
    // Displaying Data on Serial Monitor
    Serial.print("Humidity: ");
    Serial.print(h);
    Serial.print("% \n");
    Serial.print("Temperature: ");
    Serial.print(t);
    Serial.print("C \n");
    delay(1500);

    // Interrupt via Phone commands
    if(SerialBT.available() >1){
    BTreadChar = SerialBT.read();
    Serial.print(BTreadChar);
    if(BTreadChar == '0')
    {
      CmdState = BT_STOP;
      delay(500);
    }
    if(BTreadChar == '2')
      {
        CmdState = BT_RESET;
        delay(500);
      }
    if(BTreadChar == '3')
    {
      displayPrompt(h,t);
    }
   }
}

void OledDisplay(){
  if(CmdState != BT_STOP){
    return;
  }
  oled.setTextColor(WHITE);
  oled.setTextSize(1);
  oled.setCursor(0,28);
  oled.print("Paused...");
  oled.println("Waiting for command");
  oled.display();
  delay(1000);
  
  // Interrupt via Phone commands
  if(SerialBT.available() >1){
    BTreadChar = SerialBT.read();
    Serial.print(BTreadChar);
    if(BTreadChar == '1')
    {
      CmdState = BT_START;
      SerialBT.print("Collecting Data...");
      delay(1000);
    }
    if(BTreadChar == '2')
      {
        CmdState = BT_RESET;
        delay(1000);
      }
   }
}

//Prompting phone weather condition
float displayPrompt(float H, float T){
  if((T >=70.01) && (T<=74.00))
  {
    SerialBT.println("It's chilly!");
    delay(1000);
  }
  else if ((T>=74.01) || (H >=61.00))
  {
    SerialBT.println("It's hot...");
    delay(1000);
  }
  else
  {
    SerialBT.println("Are we up North???");
    delay(1000);
  }
  return 0;
}
