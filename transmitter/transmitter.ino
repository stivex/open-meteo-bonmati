#include <SPI.h> // Per utilitzar la comunicació sèrie SPI
#include <RH_NRF24.h> //Utilitzem la llibreria RadioHead (http://www.airspayce.com/mikem/arduino/RadioHead/)

// Per utilitzar la comunicaci serie I2C
#include <Wire.h>

//Llibreria pel sensor BME280
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h> //Llibreria pel sensor BME280

//CONSTANTS
#define PIN_LED         13    //Número de pin on hi ha el LED
#define PIN_SENSOR      3     //Número de pin on hi ha connectat el sensor/mòdul de camp magnètic (ky-003)
#define RADI            12    //Ràdi (en cm) de la circunferència que generen el sistema de caçoletes (part externa  caçoleta al centre)
#define TIMEOUT_DETECT  5000  //Milisegons màxim que estarem disposats a esperar perquè detectem la presència de l'imant
#define TIMEOUT_IMANT   10000 //Milisegons màxim que estarem disposats a esperar perquè l'imant surti del davant del sensor
#define TIMEOUT_VOLTA   10000 //Milisegons màxim que estarem disposats a esperar perquè faci una volta completa
#define I2C_BME         0x76  //Direcció del canal de comunicació del protocol de comunicació I2C on hi haurà connectat el sensor de temperatura+humitat+pressio

//VARIABLES
Adafruit_BME280 bme; //Contindrà l'objecte del tipus Adafruit_BME280 (sensor de temperatura+humitat+pressio)
RH_NRF24 nrf24; //Representarà l'antena per radiofreqüència NRF24
float longitud_circunferencia; //Emmagatzema la logitud de la circunferència que generen les caçoletes de l'anemòmetre

float temperatura; //On guarderem la lectura de temperatura del sensor
float humitat; //On guarderem la lectura de temperatura del sensor
float pressio; //On guarderem la lectura de temperatura del sensor
float velocitat; //On guardarem la lectura de la velocitat del vent

void setup() {
  
  //Definim la velicitat de dades sèrie
  Serial.begin(9600);
  Serial.println("S'ha inicialitzar la velocitat serie.");

  //Inicialitzem la comunicació pel bus I2C
  Wire.begin();

  //Indiquem que el pin del led és output i estarà apagat
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW);

  //Indiquem quin és el pin del sensor de l'anemòmetre i que és input
  pinMode(PIN_SENSOR, INPUT);

  //Calculem la longitud de la circunferència de l'anemòmetre
  longitud_circunferencia = 2 * PI * RADI;

  //Convertim centímetres a metres (dividint per cent)
  longitud_circunferencia = longitud_circunferencia / 100.00;

  //Verfiquem que s'ha pogut inicialitzar l'antena de radiofreqüència NRF24
  if (!nrf24.init()) {
    Serial.println("Ha fallat la inicialització del NRF24.");
  }

  //Definim per quin canal es comunicarà l'emissor i receptor NRF24 (com si es tractés de sintonitzar una freqüència d'un canal de TV o ràdio)
  if (!nrf24.setChannel(75)) {
    Serial.println("Ha fallat intentant establir el canal pel que es vol dur la comunicació per radiofreqüència.");
  }

  //De quina longitud màxima (de bytes/caràcters) podrem enviar a cada missatge que transmeti el mòdul nrf24
  Serial.print("Longitud màxima d'enviament/recepció de cada missatge acceptada per el mòdul nrf24 és de: ");
  Serial.print(RH_NRF24_MAX_MESSAGE_LEN);
  Serial.println(" bytes/caràcters.");

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
  
  //Indiquem el canal de comunicació del sensor de temperatura+humitat+pressio i verifiquem que es pot inicialitzar la comunicacio
  if (!bme.begin(I2C_BME)) {
    Serial.println("No s'ha trobat el sensor BME280!");
    while(1); //Bucle infinit perquè no arribi a saltar a la funcio loop
  }

  Serial.println("Inicialització de l'emissor finalitzada.");
  
}

void loop() {

  //Consultem la velocitat del vent a l'anemòmetre
  velocitat = getSpeedWind();
  
  //Obtenim la temperatura, humitat i pressió
  temperatura = bme.readTemperature();
  humitat = bme.readHumidity();
  pressio = bme.readPressure()/100; //La funcio retorna el valor en unitat de Pascals, dividim per 100 per obteni hPa (hectopascals)

  //Mostrem els valors llegits al monitor serie
  String BME280Temperatura = "";
  BME280Temperatura.concat(temperatura);
  //BME280Temperatura.concat("C");

  String BME280Humitat = "";
  BME280Humitat.concat(humitat);
  //BME280Humitat.concat("%");

  String BME280Pressio = "";
  BME280Pressio.concat(pressio);
  //BME280Pressio.concat("hPa");

  String VentVelocitat = "";
  VentVelocitat.concat(velocitat);
  //VentVelocitat.concat("km/h");

  //Màxim 28 bytes/caràcters per missatge (la constant RH_NRF24_MAX_MESSAGE_LEN ens dona aquesta informació)
  //Si volem enviar més de 28 caràcters, caldrà enviar un segon missatge
  String resultat = BME280Temperatura + ";" + BME280Humitat + ";" + BME280Pressio + ";" + VentVelocitat;
  Serial.println(resultat);

  //Un cop hem llegit la informació dels sensors, enviarem aquesta informació via radiofreqüencia cap al receptor
  uint8_t resultat_buffer[resultat.length() + 1];
  resultat.getBytes(resultat_buffer, resultat.length() + 1);
  
  nrf24.send(resultat_buffer, sizeof(resultat_buffer));
  nrf24.waitPacketSent();

  //Un cop l'emissor ha enviat la informació cap el receptor, esperem algun tipus de missatge de confirmació que ens envia el receptor cap a l'emissor
  //(No és estrictament necessàri, només per veure que l'enviament d'informació pot ser bi-direccional)

  //Entrarà en aquesta condició IF, sempre i quan, s'hagin rebut dades provinents del receptor
  if (nrf24.available()) {

    uint8_t buf[RH_NRF24_MAX_MESSAGE_LEN]; //Array per emmagatzemar la informació rebuda (tamany màxim permès per a la llibreria que és de 32 bytes dels quals 28 bytes són pel missatge)
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

//Funció que llegeix l'estat del sensor (si hi ha al davant del sensor un imant retorna LOW, en cas contrari HIGH)
int read_sensor() {
  return digitalRead(PIN_SENSOR);
}

//Funció que retorna la velocitat del vent en metres/segon
float getSpeedWind() {

  int estat_sensor = HIGH; //En aquest estat, es considera que l'imant no està davant del sensor
  long temps_cerca = millis(); //Ens quedem amb el temps actual (en milisegons, des que s'ha alimentat l'Arduino)

  do {

      //Esperarem un temps fins detectar que l'imant acaba de passar per davant del sensor
      //Considerem que la velocitat actual és zero i s'ha superat l'espera màxima
      if ((temps_cerca + TIMEOUT_DETECT) < millis()) {
        Serial.println("No s'ha aconseguit detectar l'imant, l'anemòmetre no està girant.");
        return 0;
      }

      estat_sensor = read_sensor();
      
  } while (estat_sensor == HIGH);
  
  //Quan un cap magnètic és detectat pel sensor (l'imant acaba de passar per davant del sensor)...

  //Ens quedem amb el temps actual (en milisegons, des que s'ha alimentat l'Arduino)
  long temps_inici = millis();

  //Esperem fins que el sensor ja no detecti el camp magnètic (per evitar la situació que l'imant quedi parat davant del sensor)
  do {

    if ((temps_inici + TIMEOUT_IMANT) < millis()) {
      //Hem superat el temps màxim d'espera, considerem que l'imant s'ha quedat aturant davant del sensor
      //Considerem que la velocitat actual és zero
      Serial.println("L'imant de l'anemòmetre està aturat davant del sensor.");
      return 0;
    }
    
    estat_sensor = read_sensor();
    
  } while (estat_sensor == LOW);
  

  //Esperarem fins que el sensor torni a detectar el camp magnètic (senyal que s'ha fet una volta completa)
  do {

    //També, si triga exageradament molt a tornar a passar pel davant (potser no ha pogut fer tota la volta completa) cancel·larem la mesura fins la propera ocasió
    //Considerem que la velocitat actual és zero
    if ((temps_inici + TIMEOUT_VOLTA) < millis()) {
      Serial.println("S'ha superat el temps màxim per donar una volta completa de l'anemòmetre.");
      return 0;
    }
    
    estat_sensor = read_sensor();
    
  }while (estat_sensor == HIGH);

  //Calulem el temps que ha trigat a fer una volta sencera
  long temps_fi = millis();
  long temps_volta = temps_fi - temps_inici;
  float temps = temps_volta / 1000.00; //Passem milisegons a segons (dividim per mil)
  
  //Calculem la velocitat (velocitat = longitud / temps)
  float velocitat = longitud_circunferencia / temps;

  Serial.println("======================");
  Serial.print("Longitud: ");
  Serial.println(longitud_circunferencia);
  Serial.print("Temps: ");
  Serial.println(temps);
  Serial.print("Velocitat: ");
  Serial.print(velocitat);
  Serial.print(" m/s - ");
  Serial.print(velocitat * 3.6); //El 3.6 és el resultat de dividir 3600 segons / 1000 metres
  Serial.println(" km/h");
  Serial.println("======================");

  return (velocitat * 3.6);
  
}
