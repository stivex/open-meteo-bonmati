#include "DCF77.h"
//#include "Time.h"
#include "TimeLib.h"


#define DCF_PIN 2 // Connexió al pin per a on rebrem les dades T del mòdul DCF77
#define DCF_INTERRUPT 0  // Interrupt number associated with pin
#define POWER 7 //Power on/off = pin P1

time_t time;
DCF77 DCF = DCF77(DCF_PIN,DCF_INTERRUPT);

void setup() {
  
  Serial.begin(9600); //Inicialitzem la comunicació sèrie
  
  pinMode(POWER, OUTPUT); //Establim que el pin digital POWER serà de sortida
  digitalWrite(POWER, LOW); //Indiquem que el valor serà baix = 0 (cosa que indica que el mòdul DCF77 estarà actiu, amb HIGH estari aturat)
  
  DCF.Start();
  Serial.println("Esperant la data/hora provinent del DCF77... ");
  Serial.println("Això pot portar almenys fins a 2 minuts fins la primera actualització de l'hora.");
  
}

void loop() {
  
  delay(1000); //Esperem un segon abans d'intentar obtenir novament la data/hora
  
  time_t DCFtime = DCF.getTime(); //Verifiquem si s'ha pogut obenir/disponible una nova data/hora del mòdul DCF77
  if (DCFtime!=0) {
    Serial.println("La data/hora ha estat actualitzada!");
    setTime(DCFtime);
  }

  //Mostrem el valor actual pel monitor sèrie
  digitalClockDisplay();
    
}

//Mètode que mostra la data/hora actual pel monitor sèrie
void digitalClockDisplay(){
  
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.print(" ");
  Serial.print(day());
  Serial.print(" ");
  Serial.print(month());
  Serial.print(" ");
  Serial.print(year());
  Serial.println();
  
}

//Mètode que ens ajuda a mostrar correctament l'hora del rellotge: pinta els zeros davant del número/a l'esquerra
void printDigits(int digits){
  
  Serial.print(":");
  
  if(digits < 10) {
    Serial.print('0');
  }
  
  Serial.print(digits);
  
  
}
