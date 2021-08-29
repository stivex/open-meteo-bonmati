//CONSTANTS
#define PIN_LED         13    //Número de pin on hi ha el LED
#define PIN_SENSOR      3     //Número de pin on hi ha connectat el sensor/mòdul de camp magnètic (ky-003)
#define RADI            14    //Ràdi (en cm) de la circunferència que generen el sistema de caçoletes (part externa  caçoleta al centre)
#define TIMEOUT_DETECT  5000 //Milisegons màxim que estarem disposats a esperar perquè detectem la presència de l'imant
#define TIMEOUT_IMANT   10000 //Milisegons màxim que estarem disposats a esperar perquè l'imant surti del davant del sensor
#define TIMEOUT_VOLTA   10000 //Milisegons màxim que estarem disposats a esperar perquè faci una volta completa

//VARIABLES
float longitud_circunferencia; //Emmagatzema la logitud de la circunferència que generen les caçoletes

void setup()
{
  //Definim la velocitat de dades sèrie
  Serial.begin(9600);

  //Indiquem que el pin del led és output i estarà apagat
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW);
  
  //Indiquem que el pin del sensor és input
  pinMode(PIN_SENSOR, INPUT);

  //Calculem la longitud de la circunferència
  longitud_circunferencia = 2 * PI * RADI;

  //Convertim centímetres a metres (dividint per cent)
  longitud_circunferencia = longitud_circunferencia / 100.00;
  
}

void loop()
{

  //Consultem la velocitat del vent a l'anemòmetre
  //Serial.print("Velocitat del vent: ");
  //Serial.print(getSpeedWind(), 2);
  //Serial.println(" m/s");

  getSpeedWind();

  delay(1000);

}

//Funció que llegeix l'estat del sensor (si ha al davant del sensor un imant retorna LOW, en cas contrari HIGH)
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
        Serial.println("No s'ha aconseguit detectar.");
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
      Serial.println("L'imant està davant del sensor.");
      return 0;
    }
    
    estat_sensor = read_sensor();
    
  } while (estat_sensor == LOW);
  

  //Esperarem fins que el sensor torni a detectar el camp magnètic (senyal que s'ha fet una volta completa)
  do {

    //També, si triga exageradament molt a tornar a passar pel davant (potser no ha pogut fer tota la volta completa) cancel·larem la mesura fins la propera ocasió
    //Considerem que la velocitat actual és zero
    if ((temps_inici + TIMEOUT_VOLTA) < millis()) {
      Serial.println("S'ha superat el temps màxim per donar una volta.");
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

  return velocitat;
  
}
