// Per utilitzar la comunicaci serie I2C
#include <Wire.h>

//Llibreria pel sensor BME280
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h> //Llibreria pel sensor BME280

//Creem una variable que contindr l'objecte del tipus Adafruit_BME280
Adafruit_BME280 bme;

//Definim les variables on guarderem les lectures del sensor
float temperatura;
float humitat;
float pressio;

void setup() {
  
  //Definim la velicitat de dades serie
  Serial.begin(9600);
  Serial.println("S'ha inicialitzar la velocitat serie.");
  
  //Verifiquem que es pot inicialitzar la comunicacio amb el sensor connectat
  if (!bme.begin()) {
    Serial.println("No s'ha trobat el sensor BME280!");
    while(1); //Bucle infinit perqu no arribi a saltar a la funcio loop
  }
  
}

void loop() {
  
  //Obtenim la temperatura
  temperatura = bme.readTemperature();
  humitat = bme.readHumidity();
  pressio = bme.readPressure()/100; //La funcio retorna el valor en unitat de Pascals, dividim per 100 per obteni hPa (hectopascals)

  //Mostrem els valors llegits al monitor serie
  Serial.print("Temperatura: ");
  Serial.print(temperatura);
  Serial.print(" C ");

  Serial.print("Humitat: ");
  Serial.print(humitat);
  Serial.print(" % ");

  Serial.print("Pressió: ");
  Serial.print(pressio);
  Serial.println(" hPa");

  //Esperarem 5 segons abans de tornar a llegir valors del sensor
  delay(5000);
  
}
