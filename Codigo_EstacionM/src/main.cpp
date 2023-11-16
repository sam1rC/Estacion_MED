#include <Arduino.h>
#include <TinyGPS++.h>
#include <Wire.h>
#include "ClosedCube_HDC1080.h"
#include <AP3216_WE.h>
#include <WiFi.h>

//Datos de la conexión Wifi
const char* ssid = "dlink";
const char* password = "";

//poner la direccion IP del servidor​
const char* server = "10.38.32.137";
//Instancia del cliente Wifi
WiFiClient client;

AP3216_WE myAP3216 = AP3216_WE();



TinyGPSPlus gps;

static void bundling(float Humidity, float Temperature, float Latitude, float Longitude, float Lux);
static void bundlingHumidity(float Humidity);
static void bundlingTemperature(float Temperature);
static void bundlingSolarRadiation(float Lux);
static void postData(String data);

//Instancia del sensor 
ClosedCube_HDC1080 sensor;
//Incializacion de variables de temperatura, humedad y contador
double temperature;
double humidity;


static void smartDelay(unsigned long ms);

void setup() {
  Serial.begin(115200);
  Serial1.begin(9600,SERIAL_8N1,34,12);
  WiFi.begin(ssid, password);
  //para cargar el estado de la conexión
  while (WiFi.status() != WL_CONNECTED){
    delay(500);

    Serial.print(".");
  }
  //incializamos las variables en 0
  temperature = 0;
  humidity = 0;
  Wire.begin(4,0);
  myAP3216.init();
  myAP3216.setLuxRange(RANGE_20661);
  myAP3216.setMode(AP3216_ALS_PS_ONCE);
  sensor.begin(0x40);

}

void loop() {
  float als = myAP3216.getAmbientLight();
  unsigned int prox = myAP3216.getProximity();
  Serial.print("Lux: "); Serial.print(als);
  Serial.print("  Proximity: "); Serial.println(prox);
  myAP3216.setMode(AP3216_ALS_PS_ONCE);

  Serial.print("Temperatura:");
  Serial.println(sensor.readTemperature(), 5);
  Serial.print("Humedad:");
  Serial.println(sensor.readHumidity(), 5);
  Serial.print("Latitud:");
  Serial.println(gps.location.lat(), 5);
  Serial.print("Longitud:");
  Serial.println(gps.location.lng(), 4);
  Serial.print("Satelite:");
  Serial.println(gps.satellites.value());
  Serial.print("Altitud:");
  Serial.println(gps.altitude.feet() / 3.2808);
  Serial.print("Hora: ");
  Serial.print(gps.time.hour());
  Serial.print(":");
  Serial.print(gps.time.minute());
  Serial.print(":");
  Serial.println(gps.time.second()); 

  //bundling(sensor.readHumidity(),sensor.readTemperature(),gps.location.lat(),gps.location.lng(), myAP3216.getAmbientLight());
  //Mandar humedad
  bundlingHumidity(sensor.readHumidity());
  //Mandar temperatura
  bundlingTemperature(sensor.readTemperature());
  //Mandar radiación solar
  bundlingSolarRadiation(myAP3216.getAmbientLight());
  
  smartDelay(10000);
  if (millis() > 5000 && gps.charsProcessed() < 10 )
    Serial.println(F("No GPS data received: check wiring"));
}

static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (Serial1.available())
      gps.encode(Serial1.read());
  } while (millis() - start < ms);
}

static void bundling(float Humidity, float Temperature, float Latitude, float Longitude, float Lux){
    //Creación de la estructura del JSON que se envia al servidor 
  String PostData = String("{ \"temperature\": { \"value\":"+ String(Temperature,7)+ ", \"type\": \"Float\" }, \"humidity\": { \"value\":"+ String(Humidity,7)+ ", \"type\": \"Float\" }, \"latitude\": { \"value\":"+ String(Latitude,7)+ ", \"type\": \"Float\" }, \"longitude\": { \"value\":"+ String(Longitude,7)+ ", \"type\": \"Float\" }, \"solar_radiation\": { \"value\":"+ String(Lux,7)+ ", \"type\": \"Float\" } }");
  Serial.println(PostData);
  
  // conexión al servidor por el puerto 80
  if ( client.connect(server,1030))
  {
    Serial.println("conectado");
    client.print("PATCH /v2/entities/Estacion_UPB_Med/attrs HTTP/1.1\n");
    // poner la direccion IP del servidor ​
    client.print("Host: 10.38.32.137 \n");
    client.println("User-Agent: Arduino/1.0");
    client.println("Connection: close");
    client.println("Content-Type: application/json");
    client.print("Content-Length: ");
    client.println(PostData.length());
    client.println();
    client.println(PostData);

  } else {

    Serial.println("error de conexion");

  }

}

static void bundlingHumidity(float Humidity){
  if(Humidity >=60 && Humidity<=80){
    String PostData = String("{ \"humidity\": { \"value\":"+ String(Humidity,7)+ ", \"type\": \"Float\" }}");
    Serial.println(PostData);
    postData(PostData);
    
  }
}

static void bundlingTemperature(float Temperature){
  if(Temperature >= 10 && Temperature <= 50){
    String PostData = String("{ \"temperature\": { \"value\":"+ String(Temperature,7)+ ", \"type\": \"Float\" }}");
    Serial.println(PostData);
    postData(PostData); 
  }
}

static void bundlingSolarRadiation(float Lux){
  float solar_radiation = Lux * 0.0083;
  if(solar_radiation > 0 && solar_radiation<1000){
    String PostData = String("{ \"solar_radiation\": { \"value\":"+ String(solar_radiation,7)+ ", \"type\": \"Float\" }}");
    Serial.println(PostData);
    postData(PostData);
    
  }
}


static void postData(String data){
  // conexión al servidor por el puerto 80
    if ( client.connect(server,1031))
    {
      Serial.println("conectado");
      client.print("PATCH /v2/entities/Estacion_UPB_Med/attrs HTTP/1.1\n");
      // poner la direccion IP del servidor ​
      client.print("Host: 10.38.32.137 \n");
      client.println("User-Agent: Arduino/1.0");
      client.println("Connection: close");
      client.println("Content-Type: application/json");
      client.print("Content-Length: ");
      client.println(data.length());
      client.println();
      client.println(data);

    } else {

      Serial.println("error de conexion");

    }
}


