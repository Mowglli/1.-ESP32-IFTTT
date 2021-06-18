// - DHT Sensor Library: https://github.com/adafruit/DHT-sensor-library
// - Adafruit Unified Sensor Lib: https://github.com/adafruit/Adafruit_Sensor
#include <arduino.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <WiFi.h>
#include <HTTPClient.h>

//DHT11 sensor
#define DHTPIN 23                 // Digital pin forbundet til  DHT11 sensor.. 
#define DHTTYPE    DHT11          // I biblioteket DHT skal vi definere type
DHT_Unified dht(DHTPIN, DHTTYPE);
uint32_t delayMS;

//Soil sensor
#define sensorPin 36
int soil_sensor, output_value;
float output_value_pct;

//Wifi info
#define WIFI_NETWORK "Åhusene 11 - 4.3 - gæst"
#define WIFI_PASSWORD "40147281"
#define WIFI_TIMEOUT_MS 20000 //20 ms

//IFFT setup
String key = "cgCCkRGzGfCspgVFdYtWe7"; //nøgle
String event_name= "soil_moisture_email";


void connectToWiFi(){
  Serial.print("Connecting to Wifi..");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_NETWORK, WIFI_PASSWORD);

  unsigned long startAttempTime = millis();

  while(WiFi.status() !=WL_CONNECTED && millis()- startAttempTime < WIFI_TIMEOUT_MS){
    Serial.print(".");
    delay(100);
  }
  if(WiFi.status() !=WL_CONNECTED){
    Serial.println("Failed!");
    //En genstart kan sættes her
  }
  else{
    Serial.println(" ");
    Serial.print("Connected to Wifi with IP: ");
    Serial.println(WiFi.localIP());
  }
}

void IFFT_notifikation(float value1,int value2,float value3){
HTTPClient http;
http.begin("https://maker.ifttt.com/trigger/"+event_name+"/with/key/"+key+"?value1="+value1+"&value2="+value2+"&value3="+value3+"");
http.GET();
http.end();
Serial.print("Notifikation sendt!");
}

void sensors(){
  float temp;
  int hum;

   sensors_event_t event;
  //print adc udlæsning:
   Serial.print(F("ADC-aflæsning(0 - 4095)"));
   Serial.println(analogRead(sensorPin));

    //Udregn til procent 0% er tør, 100% er vådt
    soil_sensor = analogRead(sensorPin);
    //output_value  =  (soil_sensor / 4095.00);
    output_value_pct =  (100 - ( (soil_sensor/4095.00) * 100 ) );
    Serial.print(F("Jorfugtighed: "));
    Serial.print(output_value_pct);
    Serial.println(F("%"));

  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    Serial.println(F("Fejl ved aflæsning af temperatur!"));
  }
  else {
    Serial.print(F("Temperatur: "));
    Serial.print(event.temperature);
    Serial.println(F("°C"));
  }
  temp = event.temperature;
 
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println(F("Fejl ved aflæsning af Fugtighed!"));
  }
  else {
    Serial.print(F("Fugtighed: "));
    Serial.print(event.relative_humidity);
    Serial.println(F("%"));
  }
    hum = event.relative_humidity;
  Serial.println("");

    if(output_value_pct <= 30 ) // hvis under 30 pct, sendes der besked til telefon
    {
      IFFT_notifikation(output_value_pct, hum, temp);
    }
    else
    {}

}

void setup() {
  //Initialiser serial monitor
  Serial.begin(9600);
  
  //Initialiser Wifi
  connectToWiFi();
 
  // Initialiser DHT11 sensor.
  dht.begin();
  Serial.println(F("DHT11 - Temperatur og fugtighedsmåler"));
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  dht.humidity().getSensor(&sensor);

  //Initialiser Soilsensor
  pinMode(sensorPin, INPUT);

  // Sæt tidsforsinkelse
  delayMS = sensor.min_delay / 100;
}


void loop() {
  // Tidsforsinkelse mellem læsning.
  delay(delayMS);
  sensors();
}