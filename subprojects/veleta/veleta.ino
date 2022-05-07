//Llibreries per utilitzar la pantalla OLED
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Per utilitzar la comunicaci serie I2C
#include <Wire.h>

//CONSTANTS
const int analogPin = A0;     //Pin analògic A0 on llegirem els valors de la veleta
#define I2C_OLED        0x3C  //Direcció del canal de comunicació del protocol de comunicació I2C on hi haurà connectat la pantalla OLED de 128x64 píxels
#define OLED_AMPLE      128   //Número de píxels d'amplada que tindrà la pantalla OLED
#define OLED_ALT        64    //Número de píxels d'altura que tindrà la pantalla OLED
#define OLED_RESET      4     //Valor que ha de rebre per defecte un dels seus pins en determinats models de pantalla OLED d'Adafruit (en el nostre cas no l'utilitzarem)

//VARIABLES
Adafruit_SSD1306 oled(OLED_AMPLE, OLED_ALT, &Wire, OLED_RESET); //Creem un objecte que representarà la pantalla OLED

void setup() {

  //Definim la velicitat de dades sèrie
  Serial.begin(9600);
  Serial.println("S'ha inicialitzar la velocitat serie.");

  //Inicialitzem la comunicació pel bus I2C
  //Wire.begin();
  
  //Inicialitzem la pantalla OLED
  if (!oled.begin(SSD1306_SWITCHCAPVCC, I2C_OLED)){
    Serial.println("No s'ha trobat la pantalla OLED!");
    while (true); //Bucle infinit perquè no arribi a saltar a la funcio loop
  }

}

void loop() {

  //Llegim el pin analògic on hi ha connectada la veleta
  int valorA0 = analogRead(analogPin);

  //Convertim a vols (V) el valor analògic del pin 0
  float voltatge = valorA0 * (5.0/1023.0);
 
  //Mostrem el voltatge en el monitor sèrie
  Serial.println(voltatge);

  //Mostrem la informació per la pantalla OLED
  oledPrint(voltatge, obtenirDireccioVeleta(voltatge));

  //Esperem un segon abans de tornar a llegir el valor del PIN
  delay(1000);

}

//Funció per obtenir la orientació/punt cardinal
char* obtenirDireccioVeleta(float voltatge) {

    if (voltatge > 0 && voltatge < 0.625) {
      //Nord (N)
      return "N";
    } else if (voltatge >= 0.625 && voltatge < 1.250) {
      //Nord Est (NE)
      return "NE";
    } else if (voltatge >= 1.250 && voltatge < 1.875) {
      //Est (E)
      return "E";
    } else if (voltatge >= 1.875 && voltatge < 2.500) {
      //Sud Est (SE)
      return "SE";
    } else if (voltatge >= 2.500 && voltatge < 3.125) {
      //Sud (S)
      return "S";
    } else if (voltatge >= 3.125 && voltatge < 3.750) {
      //Sud Oest (SO)
      return "SO";
    } else if (voltatge >= 3.750 && voltatge < 4.375) {
      //Oest (SO)
      return "O";
    } else if (voltatge >= 4.375 && voltatge < 5.000) {
      //Nord Oest (NO)
      return "NO";
    } else {
      return "?";
    }
    
}

//Enviem valors a la pantalla OLED
void oledPrint(float voltatge, char *orientacio) {

  Serial.println("Anem a escriure a la pantalla OLED.");
  oled.clearDisplay();
  oled.setTextColor(WHITE);
  oled.setCursor(0, 0);
  oled.setTextSize(3); //1 (6 x 8 px), 2 (12 x 16 px), 3 (18 x 24 px)
  //oled.print("Meteo Bonmati");
  oled.print(voltatge);
  oled.println(" V");
  oled.print(orientacio);
  Serial.println("Anem a escriure a la pantalla OLED.");
  oled.display();
  Serial.println("S'ha escrit a la pantalla OLED.");

}
