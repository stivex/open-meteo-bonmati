#include <SPI.h> // Per utilitzar la comunicació sèrie SPI
#include <RH_NRF24.h> //Utilitzem la llibreria RadioHead (http://www.airspayce.com/mikem/arduino/RadioHead/)
#include <Wire.h> //Llibreria per gestiona l'interfíce de comunicació I2C
#include <LCD.h> //Per controlar la pantalla LCD (https://github.com/fmalpartida/New-LiquidCrystal)
#include <LiquidCrystal_I2C.h> //Necessària per enviar les dades a la pantalla LCD per comunicació I2C

//Creem una variable que representarà l'antena per radiofreqüència NRF24
RH_NRF24 nrf24;

//Creem un objecte per la pantalla LCD
LiquidCrystal_I2C lcd (0x27, 2, 1, 0, 4, 5, 6, 7); //Número de pins de la pantalla LCD (DIR - Direccó I2C, E, RW, RS, D4, D5, D6, D7)

//Definim les variables on guarderem les lectures del sensor
String temperatura;
String humitat;
String pressio;
String vent_velocitat;

void setup() {
  
  //Definim la velicitat de dades serie
  Serial.begin(9600);
  Serial.println("S'ha inicialitzat la velocitat serie.");

  //Inicialitzem la configuració de la pantalla LCD
  lcd.setBacklightPin(3, POSITIVE); //Indiquem el número de port paral·lel que s'utilitzarà per indicar si volem que la pantalla LCD estigui retro-il·luminada
  lcd.setBacklight(HIGH); //Li indiquem que la pantalla LCD estigui retro-il·luminada
  lcd.begin(16, 2); //Dues files de text de 16 caràcters
  lcd.clear(); //Que comenci sense mostrar d'entrada cap data a la pantalla

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

    uint8_t buf[RH_NRF24_MAX_MESSAGE_LEN]; //Array per emmagatzemar la informació rebuda (tamany màxim permès per a la llibreria que és de 32 bytes dels quals 28 bytes són pel missatge)
    uint8_t len = sizeof(buf); //Per saber la longitud de la cadena de text rebuda

    //Una vegada el buffer està ple amb la informació i que la integritat de les dades és correcta entrarem en aquesta condició IF
    if (nrf24.recv(buf, &len)) {

      //Mostrem pel monitor sèrie la informació que s'ha rebut
      Serial.print("S'ha rebut de l'emissor: ");
      Serial.print((char*)buf);
      Serial.println("");

      //Mostrem per la pantalla LCD la informació que s'ha rebut
      String strData = (char*)buf;
      printLCD(strData);

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

void printLCD(String strData) {

      Serial.println("El valor rebut dins de la funció és: " + strData);

      char *s; //És un punter auxiliar necessàri per poder recorrer tota la cadena
      s = strtok(strData.c_str(), ";"); //Obtenim el primer valor fins a trobar el primer punt i coma

      int i = 0;
      while (s != NULL) {
        
        Serial.println(s);

        if (i == 0) {
            //Temperatura
            temperatura = "";
            temperatura.concat(s);
            temperatura.concat("C");
            Serial.println("Temperatura: " + temperatura);
        } else if (i == 1) {
            //Humitat
            humitat = "";
            humitat.concat(s);
            humitat.concat("%");
            Serial.println("Humitat: " + humitat);
        } else if (i == 2) {
            //Pressió atmosfèrica
            pressio = "";
            pressio.concat(s);
            pressio.concat("hPa");
            Serial.println("Pressió: " + pressio);
        } else if (i == 3) {
            //Velocitat del vent
            vent_velocitat = "";
            vent_velocitat.concat(s);
            //vent_velocitat.concat("km/h");
            Serial.println("Velocitat vent: " + vent_velocitat);
        } else {
          Serial.println("Valor rebut no previst a la posició " + i);
        }

        s = strtok(NULL, ";");
        i=i+1;
      }

      //Un cop tractades les dades rebudes, ho enviem a la pantalla LCD

      //Primera fila
      String filaLCD1 = "";
      filaLCD1.concat(temperatura);
      filaLCD1.concat(" ");
      filaLCD1.concat(humitat);
      lcd.setCursor(0, 0);
      lcd.print(filaLCD1);

      //Segona fila
      String filaLCD2 = "";
      filaLCD2.concat(pressio);
      filaLCD2.concat(" ");
      filaLCD2.concat(vent_velocitat);
      lcd.setCursor(0, 1);
      lcd.print(filaLCD2);
  
}
