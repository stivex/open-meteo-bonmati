#include <SPI.h> // Per utilitzar la comunicació sèrie SPI
#include <RH_NRF24.h> //Utilitzem la llibreria RadioHead (http://www.airspayce.com/mikem/arduino/RadioHead/)

// Per utilitzar la comunicaci serie I2C
#include <Wire.h>

//Llibreria pel sensor BME280
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h> //Llibreria pel sensor BME280

//Creem una variable que contindrà l'objecte del tipus Adafruit_BME280
Adafruit_BME280 bme;

//Creem una variable que representarà l'antena per radiofreqüència NRF24
RH_NRF24 nrf24;

//Definim les variables on guarderem les lectures del sensor
float temperatura;
float humitat;
float pressio;

void setup() {
  
  //Definim la velicitat de dades serie
  Serial.begin(9600);
  Serial.println("S'ha inicialitzar la velocitat serie.");

  //Verfiquem que s'ha pogut inicialitzar l'antena de radiofreqüència NRF24
  if (!nrf24.init()) {
    Serial.println("Ha fallat la inicialització del NRF24.");
  }

  //Definim per quin canal es comunicarà l'emissor i receptor NRF24 (com si es tractés de sinatotzar una freüència d'un canal de TV o ràdio)
  if (!nrf24.setChannel(75)) {
    Serial.println("Ha fallat intentant establir el canal pel que es vol dur la comunicació per radiofreqüència.");
  }

  //Establim la velocitat de transferència de dades i la potència d'emissió
  /* ### Possibles velocitats de transferència (bits/segon) ###
   * - DataRate250kbps
   * - DataRate1Mbps
   * - DataRate2Mbps
   * 
   * ### Possibles nivells de potència d'emissió ###
   * - TransmitPowerm18dBm    -18 dBm (0,0158 mW)
   * - TransmitPowerm12dBm    -12 dBm (0,0613 mW)
   * - TransmitPowerm6dBm      -6 dBm (0,251 mW)
   * - TransmitPower0dBm        0 dBm (1 mW)
   */
  if (!nrf24.setRF(RH_NRF24::DataRate250kbps, RH_NRF24::TransmitPower0dBm)) {
    Serial.println("Ha fallat intentant establir la velocitat de transferència i la potència d'emissió.");
  }
  
  //Verifiquem que es pot inicialitzar la comunicacio amb el sensor connectat
  if (!bme.begin()) {
    Serial.println("No s'ha trobat el sensor BME280!");
    while(1); //Bucle infinit perqu no arribi a saltar a la funcio loop
  }

  Serial.println("Inicialització de l'emissor finalitzada.");
  
}

void loop() {
  
  //Obtenim la temperatura, humitat i pressió
  temperatura = bme.readTemperature();
  humitat = bme.readHumidity();
  pressio = bme.readPressure()/100; //La funcio retorna el valor en unitat de Pascals, dividim per 100 per obteni hPa (hectopascals)

  //Mostrem els valors llegits al monitor serie
  String BME280Temperatura = "";
  BME280Temperatura.concat(temperatura);
  BME280Temperatura.concat("C");

  String BME280Humitat = "";
  BME280Humitat.concat(humitat);
  BME280Humitat.concat("%");

  String BME280Pressio = "";
  BME280Pressio.concat(pressio);
  BME280Pressio.concat("hPa");

  //Màxim 27 caràcters (cal mirar el perquè hi ha aquesta limitació per cada enviament)
  String BME280Resultat = "[" + BME280Temperatura + " " + BME280Humitat + " " + BME280Pressio + "]";
  Serial.println(BME280Resultat);

  //Un cop hem llegit la informació dels sensors, enviarem aquesta informació via radiofreqüencia cap al receptor
  uint8_t BME280ResultatBuffer[BME280Resultat.length() + 1];
  BME280Resultat.getBytes(BME280ResultatBuffer, BME280Resultat.length() + 1);
  
  nrf24.send(BME280ResultatBuffer, sizeof(BME280ResultatBuffer));
  nrf24.waitPacketSent();

  //Un cop l'emissor ha enviat la informació cap el receptor, esperem algun tipus de missatge de confirmació que ens envia el receptor cap a l'emissor
  //(No és estrictament necessàri, només per veure que l'enviament d'informació pot ser bi-direccional)

  //Entrarà en aquesta condició IF, sempre i quan, s'hagin rebut dades provinents del receptor
  if (nrf24.available()) {

    uint8_t buf[RH_NRF24_MAX_MESSAGE_LEN]; //Array per emmagatzemar la informació rebuda (tamany màxim permès per a la llibreria que és de 32 bytes)
    uint8_t len = sizeof(buf); //Per saber la longitud de la cadena de text rebuda

    //Una vegada el buffer està ple amb la informació i que la integritat de les dades és correcta entrarem en aquesta condició IF
    if (nrf24.recv(buf, &len)) {
  
      //Mostrem pel monitor sèrie la informació que s'ha rebut
      Serial.print("S'ha rebut del receptor: ");
      Serial.print((char*)buf);
      Serial.println("");
      
    } else {
  
      Serial.println("Ha fallat la recepció de dades.");
      
    }
  }

  //Esperarem 2 segons abans de tornar a llegir valors del sensor i enviar les dades
  delay(2000);
  
}
