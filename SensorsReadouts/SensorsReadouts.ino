/*
  AnalogReadSerial

  Reads an analog input on pin 0, prints the result to the Serial Monitor.
  Graphical representation is available using Serial Plotter (Tools > Serial Plotter menu).
  Attach the center pin of a potentiometer to pin A0, and the outside pins to +5V and ground.

  This example code is in the public domain.

  https://docs.arduino.cc/built-in-examples/basics/AnalogReadSerial/
*/
#include <Adafruit_MLX90614.h>


#include <Wire.h>
#include "RTClib.h"

#include "DHT.h"

#define DHTPIN 2 

#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
// Initialize DHT sensor.
// Note that older versions of this library took an optional third parameter to
// tweak the timings for faster processors.  This parameter is no longer needed
// as the current DHT reading algorithm adjusts itself to work on faster procs.
DHT dht(DHTPIN, DHTTYPE);
RTC_DS3231 rtc;
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

const float RG = 11874;
const float RW = 12048;
const float V0 = 5;

const float GA = 0.00510;
const float GB = -0.000409;
const float GC = 2.59e-6;

const float WA = 0.00563;
const float WB = -0.000503; //suf voor WB, 'Vo voor 112
const float WC = 3.00e-6;

float skytemp;

bool rtc_ok = false;

bool mlx_ok = false;


bool readDHT(float &h, float &t) {
  h = dht.readHumidity();
  t = dht.readTemperature();
  return !(isnan(h) || isnan(t));
}


void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);

  Wire.begin();   

 if (!rtc.begin()) {
  Serial.println(" RTC not responding!");
  rtc_ok = false;
  } else {
  rtc_ok = true;

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, setting the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
 }

  Serial.println(F("Start Measurements!"));

mlx_ok = mlx.begin();
if (!mlx_ok) {
  Serial.println(" MLX90614 not responding!");
}



  dht.begin();
}


// the loop routine runs over and over again forever:
void loop() {
  delay(5000);

  // read the input on analog pin 0:
  float Voltage_Green_Thermistor = analogRead(A0)*V0/1024;
  float Voltage_White_Thermistor = analogRead(A1)*V0/1024;
  float Resistance_Green_Thermistor = RG*(Voltage_Green_Thermistor/(V0-Voltage_Green_Thermistor));
  float Resistance_White_Thermistor = RW*(Voltage_White_Thermistor/(V0-Voltage_White_Thermistor));

  float Temperature_Green_Thermistor = pow(GA + GB*log(Resistance_Green_Thermistor) + GC*pow(log(Resistance_Green_Thermistor), 3), -1)-273.15;
  float Temperature_White_Thermistor = pow(WA + WB*log(Resistance_White_Thermistor) + WC*pow(log(Resistance_White_Thermistor),3),-1)-273.15;
  // print out the value you read:

  // Wait a few seconds between measurements.
  

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
float h, t;
bool dht_ok = readDHT(h, t);

if (!dht_ok) {
  Serial.println(F(" DHT sensor reageert niet!"));
}




if (rtc_ok) {
  DateTime now = rtc.now();

  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(' ');
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.println(now.second(), DEC);
} else {
  Serial.println("Time: RTC unavailable");
}

 
  Serial.print("Green Thermistor Temperature: ");
  Serial.print(Temperature_Green_Thermistor);
  Serial.println(" C");
  Serial.print("White Thermistor Temperature: ");
  Serial.print(Temperature_White_Thermistor);
  Serial.println(" C");
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.println(" %");
  Serial.print("Ambient Temperature: ");
  Serial.print(t);
  Serial.println(" C");
if (mlx_ok) {
  skytemp = mlx.readObjectTempC();
  Serial.print("Sky temperature: ");
  Serial.print(skytemp);
  Serial.println(" C");
} else {
  Serial.println("Sky temperature: ---");
}


  
}

