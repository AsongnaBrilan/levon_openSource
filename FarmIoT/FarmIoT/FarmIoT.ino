//------style guard ----

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

//set the lcd number of columns and rows
int lcdColumns = 16;
int lcdRows = 2;

// soil nutrients
#define RE 33
#define DE 32

#define RXD2 16
#define TXD2 17
 
const byte ec[] = {0x01, 0x03, 0x00, 0x15, 0x00, 0x01, 0x95, 0xCE};
const byte salinity[] = {0x01, 0x03, 0x00, 0x14, 0x00, 0x01, 0xC4, 0x0E};
byte values[8];
 

// set LCD address, number of columns and rows
// if you don't know your display address, run an I2C scanner sketch
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);

#ifdef __cplusplus

extern "C" {

#endif

uint8_t temprature_sens_read();

#ifdef __cplusplus

}

#endif

uint8_t temprature_sens_read();

// ------header files----

#include <WiFi.h>

#include "DHT.h"

#include "ThingSpeak.h"

//-----netwrok credentials

char* ssid = "levon"; //enter SSID

char* passphrase = "levonubuntu2.."; // enter the password

WiFiServer server(80);

WiFiClient client;

//-----ThingSpeak channel details

unsigned long myChannelNumber = 3;

const char * myWriteAPIKey = "G2OHCSOWH6SR4S3E";

//----- Timer variables

unsigned long lastTime = 0;

unsigned long timerDelay = 500;

//----DHT declarations

#define DHTPIN 4 // Digital pin connected to the DHT sensor

#define DHTTYPE DHT11 // DHT 11

// Initializing the DHT11 sensor.

DHT dht(DHTPIN, DHTTYPE);

// soil moisture sensor
int _moisture, sensor_analog;
const int sensor_pin = 34;  /* Soil moisture sensor O/P pin */


void setup() {
  // initialize LCD
  lcd.init();
  // turn on LCD backlight
  lcd.backlight();

  lcd.setCursor(0, 0);

  Serial.begin(115200); //Initialize serial

  //Serial.print("Connecting to ");
  lcd.print("Connecting to ");

  //Serial.println(ssid);

  WiFi.begin(ssid, passphrase);

  while (WiFi.status() != WL_CONNECTED) {

    delay(500);

    //Serial.print(".");

  }

  // Print local IP address and start web server
  lcd.clear();

  //Serial.println("");

  //Serial.println("WiFi connected.");
  lcd.print("WiFi connected!");
  lcd.clear();
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  lcd.setCursor(0, 1);
  lcd.print(WiFi.localIP());
  delay(1000);

  server.begin();

  //----nitialize dht11

  dht.begin();

  ThingSpeak.begin(client); // Initialize ThingSpeak

  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
  Serial.println("Serial Txd is on pin: "+String(TX));
  Serial.println("Serial Rxd is on pin: "+String(RX));
  
  //mod.begin(9600);
  pinMode(RE, OUTPUT);
  pinMode(DE, OUTPUT);

}

void loop() {
  lcd.setCursor(0, 0);
  sensor_analog = analogRead(sensor_pin);
  _moisture = ( 100 - ( (sensor_analog / 4095.00) * 100 ) );
  
  //Serial.print("Moisture = ");
  //Serial.print(_moisture);  /* Print Temperature on the serial window */
  //Serial.println("%");
  
  if ((millis() - lastTime) > timerDelay)

  {

    delay(2500);

    // Reading temperature or humidity takes about 250 milliseconds!

    float h = dht.readHumidity();

    // Read temperature as Celsius (the default)

    float t = dht.readTemperature();

    float f = dht.readTemperature(true);

    if (isnan(h) || isnan(t) || isnan(f)) {

      Serial.println(F("Failed to read from DHT sensor!"));

      return;

    }

    Serial.print("Temperature (ºC): ");
    lcd.print("Temp(c):");
   
    
    Serial.print(t);
    lcd.setCursor(9, 0);
    lcd.print(t);
    lcd.setCursor(14, 0);
    lcd.print("C");
    
    lcd.setCursor(0, 1);
    Serial.println("ºC");

    Serial.print("Humidity");
    lcd.print("Hum:");

    Serial.println(h);
    lcd.setCursor(4, 1);
    lcd.print(h, 1);

    lcd.setCursor(8, 1);
    lcd.print(" S:");
    lcd.setCursor(10, 1);
    lcd.print(_moisture);
    lcd.setCursor(14, 1);
    lcd.print("%");
    //lcd.clear();
    ThingSpeak.setField(4, h);

    ThingSpeak.setField(1, t);

    ThingSpeak.setField(3, _moisture);
    //ThingSpeak.setField(4, _moisture);

    // Write to ThingSpeak. There are up to 8 fields in a channel, allowing you to store up to 8 different

    // pieces of information in a channel. Here, we write to field 1.

    int x = ThingSpeak.writeFields(myChannelNumber,

                                   myWriteAPIKey);

    if (x == 200) {

      Serial.println("Channel update successful.");

    }

    else {

      Serial.println("Problem updating channel. HTTP error code " + String(x));

    }

    lastTime = millis();

  }

}
