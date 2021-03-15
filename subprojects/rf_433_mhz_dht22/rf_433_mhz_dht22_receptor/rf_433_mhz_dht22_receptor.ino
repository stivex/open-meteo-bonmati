//Libreries pel receptor RF433 Mhz
#include <RH_ASK.h> //Llibreria de RadioHead
#include <SPI.h> //No utilitzarem la comunicació per SPI però la llibreria de RadioHead la requereix com a dependència

RH_ASK rf_driver; //Objecte de tipus de modulació ASK que utilitzarem

void setup() {
  //Codi de configuració/inicialització que s'executarà en posar-se en marxa l'Arduino

  Serial.begin(9600); //Definim a quina velocitat volem utilitzar el port sèrie

  //Inicialitzem la llibreria de comunicació (si no indiquem el contrari, el pin per defecte serà el digital 11)
  if (rf_driver.init()) {
    Serial.println("S'ha inicialitzat!");
  } else {
    Serial.println("Ha fallat la inicialització!");    
  }
  
  
}

void loop() {
  //Codi que s'executerà repetidament un cop s'hagi inicialitzat l'Arduino

  uint8_t buf[5]; //Array on guardarem el valor rebut, on indicarem tantes posicions com caracters de longitud tindrà el text que sabem que rebrem
  uint8_t buflen = sizeof(buf); //Guardarem la longitud del missatge/text rebut

  //Verifiquem si s'ha recepcionat un missatge per part de l'emissor
  if (rf_driver.recv(buf,&buflen)) {


    //Serial.println((char*)buf);

    char *val;
    val = strtok(buf, ";");

    if(val) {
      Serial.print("Temperatura: ");
      Serial.print(val);
    }

    val = strtok(NULL, ";");

    if(val) {
      Serial.print(" Humitat: ");
      Serial.println(val);
    }


  }
  
}
