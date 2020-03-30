
/*
 

 */

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>


// Declaracion de variables
#define RST_PIN  5   //Pin 5 para el reset del RC522
#define SS_PIN  53   //Pin 53 para el SS (SDA) del RC522
#define LED_A 9 //Definimos el pin LED azul
#define LED_R 10 //Definimos el pin LED rojo
#define BUZZER 8 
#define SERVO 11

#define ACCESS_DELAY 3000
#define DENIED_DELAY 1000

String baseDatos[] = {"C7 52 6B 63","A9 A0 8B C1"};
int bdSize = sizeof(baseDatos)/sizeof(String);

// Crear nuestros OBJETOS
MFRC522 mfrc522(SS_PIN, RST_PIN); //Creamos el objeto para el RC522
LiquidCrystal_I2C lcd(0x27, 16, 2); //Creamos el objeto para la LCD
Servo servo; // Creamos un objeto para el servo

void setup() {

  Serial.begin(9600); //Iniciamos la comunicación  serial
  
  SPI.begin();        //Iniciamos el Bus SPI
  mfrc522.PCD_Init(); // Iniciamos  el MFRC522

  servo.attach(SERVO); // Vinculamos al servo con el pin ~11 
  servo.write(0); // Colocamos el servo en la posicion inicial
  
  lcd.init(); //Iniciamos el lcd
  lcd.backlight(); //Encender la luz del lcd

  pinMode(LED_A, OUTPUT);
  pinMode(LED_R, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  // Mostramos un mensaje de
  String msg_Inicio = "Security System";
  Serial.println(msg_Inicio);
  lcd.print(msg_Inicio);
}

void loop()
{
  
  loopLCD();

  // Revisamos si hay nuevas tarjetas  presentes  
  if ( mfrc522.PICC_IsNewCardPresent())
  {  
    
    if ( mfrc522.PICC_ReadCardSerial()){
      
      lcd.clear();
      lcd.print("READING CARD...");
      delay(1000);

      String uid = obtenerUID(); // Obtenemos el UID de la tarjeta
      
      if(verificarTarjeta(uid))
        accesoPermitido();
      else
        accesoDenegado(); 
    }
  }     
}

// Mensaje que se presenta cada vez que entra al loop
void loopLCD(){
  lcd.clear();
  lcd.print("RFID Lock");
  delay(300);
}

// Enciende el led azul e imprime un mensaje en el lcd
void accesoPermitido(){
  lcd.clear();
  lcd.print("ACCESS ALLOWED");
  doubleBeep();
  digitalWrite(LED_A,HIGH);
  servo.write(90);
  delay(ACCESS_DELAY);
  servo.write(0);
  digitalWrite(LED_A,LOW);
}

void doubleBeep(){
  int count = 2;
  while(count > 0){
    digitalWrite(BUZZER,HIGH);
    digitalWrite(LED_A,HIGH);
    delay(300);
    digitalWrite(BUZZER,LOW);
    digitalWrite(LED_A,LOW);
    delay(100);
    count--;
  }
}

// Enciende el led rojo y el buzzer e imprime un mensaje en el lcd
void accesoDenegado(){
  lcd.clear();
  lcd.print("ACCESS DENIED");
  digitalWrite(LED_R,HIGH);
  digitalWrite(BUZZER,HIGH);
  delay(DENIED_DELAY);
  digitalWrite(LED_R,LOW);
  digitalWrite(BUZZER,LOW);
}

// Devuelve el UID de la tarjeta
String obtenerUID(){
  String uid = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
    uid.concat(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    uid.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  mfrc522.PICC_HaltA();
  uid.toUpperCase();
  return uid;
}

// Verifica si la tarjeta esta en la base de datos
bool verificarTarjeta(String tarUid){
  
  for(int i = 0; i < bdSize; i++){
    if(tarUid.substring(1) == baseDatos[i])
      return true;
  }
  return false;
}
