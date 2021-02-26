#include <DHT.h>
#include <DHT_U.h>

int PIN_SENSOR_TEMP_HUMITAT = 2; //Pin digital on hi ha connectat el sensor de temperatura i humitat DHT22

int temperatura;
int humitat;

DHT dht(PIN_SENSOR_TEMP_HUMITAT, DHT22); //Estem definint quin pin utilitzarem per llegir dades i model del sensor

void setup() {
  //Codi de configuració/inicialització que s'executarà en posar-se en marxa l'Arduino

  Serial.begin(9600); //Definim a quina velocitat volem utilitzar el port sèrie
  dht.begin(); //Inicialitzem el sensor

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

  //Esperem mig segon abans no tornem a fer una lectura dels sensors
  delay(1000);
  
}
