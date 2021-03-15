#include <SPI.h> // Per utilitzar la comunicació sèrie SPI
#include <RH_NRF24.h> //Utilitzem la llibreria RadioHead (http://www.airspayce.com/mikem/arduino/RadioHead/)

//Creem una variable que representarà l'antena per radiofreqüència NRF24
RH_NRF24 nrf24;

//Definim les variables on guarderem les lectures del sensor
float temperatura;
float humitat;
float pressio;

void setup() {
  
  //Definim la velicitat de dades serie
  Serial.begin(9600);
  Serial.println("S'ha inicialitzat la velocitat serie.");

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

  Serial.println("Inicialització del receptor finalitzada.");
  
}

void loop() {

  //Entrarà en aquesta condició IF, sempre i quan, s'hagin rebut dades provinents de l'emissor
  if (nrf24.available()) {

    uint8_t buf[RH_NRF24_MAX_MESSAGE_LEN]; //Array per emmagatzemar la informació rebuda (tamany màxim permès per a la llibreria que és de 32 bytes)
    uint8_t len = sizeof(buf); //Per saber la longitud de la cadena de text rebuda

    //Una vegada el buffer està ple amb la informació i que la integritat de les dades és correcta entrarem en aquesta condició IF
    if (nrf24.recv(buf, &len)) {

      //Mostrem pel monitor sèrie la informació que s'ha rebut
      Serial.print("S'ha rebut de l'emissor: ");
      Serial.print((char*)buf);
      Serial.println("");

      //Un cop rebuda la informació, enviem ara nosaltres (des del receptor cap a l'emissor) un missatge/dada
      uint8_t data[] = "Hola amic emissor!";
      nrf24.send(data, sizeof(data));
      nrf24.waitPacketSent(); //Espera fins que la informació ha sigut enviada (ja que depèn de la longitud de dades a enviar i la velocitat de transmissió)
      Serial.println("S'ha enviat un missatge a l'emissor.");
      
    } else {

      Serial.println("Ha fallat la recepció de dades.");
      
    }
    
  }
  
}
