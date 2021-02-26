//Llibreries pel sensor DHT22
#include <DHT.h>
#include <DHT_U.h>

//Libreries per l'emissor RF433 Mhz
#include <RH_ASK.h> //Llibreria de RadioHead
#include <SPI.h> //No utilitzarem la comunicació per SPI però la llibreria de RadioHead la requereix com a dependència

int PIN_SENSOR_TEMP_HUMITAT = 2; //Pin digital on hi ha connectat el sensor de temperatura i humitat DHT22

RH_ASK rf_driver; //Objecte de tipus de modulació ASK que utilitzarem

int temperatura;
int humitat;

DHT dht(PIN_SENSOR_TEMP_HUMITAT, DHT22); //Estem definint quin pin utilitzarem per llegir dades i model del sensor

void setup() {
  //Codi de configuració/inicialització que s'executarà en posar-se en marxa l'Arduino

  Serial.begin(9600); //Definim a quina velocitat volem utilitzar el port sèrie
  dht.begin(); //Inicialitzem el sensor
  rf_driver.init(); //Inicialitzem la llibreria de comunicació (si no indiquem el contrari, el pin per defecte serà el digital 12)

}

void loop() {
  //Codi que s'executerà repetidament un cop s'hagi inicialitzat l'Arduino

  //Llegim el valor dels sensors
  temperatura = dht.readTemperature(); //Llegim del sensor, la temperatura amb la funció de la llibreria
  humitat = dht.readHumidity(); //Legim del sensor, la humitat amb la funció de la llibreria

  //Escribim en el monitor sèrie els valor acabats de llegir
  Serial.print("Temperatura: ");
  Serial.print(temperatura);
  Serial.print(" Humitat: ");
  Serial.println(humitat);

  //Enviem la informació que acabem de mostrar pel monitor sèrie, ara per ràdiofreqüencia
  char msg[6];
  sprintf(msg, "%02d%c%02d\0",temperatura, ';', humitat);
  
  rf_driver.send((uint8_t *)msg, strlen(msg)); //Li indiquem el valor/dada a enviar
  rf_driver.waitPacketSent(); //Funció que esperarà que s'hagi completat l'enviament per seguir amb l'execució del codi

  //Esperem mig segon abans no tornem a fer una lectura dels sensors i tornem a enviar la informació per l'emissor
  delay(1000);
  
}
