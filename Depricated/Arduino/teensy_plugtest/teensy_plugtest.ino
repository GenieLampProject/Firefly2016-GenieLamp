int ledDelay;
int ledPin = 13; //LED conectado ao pino digital 13
int sparkPin = 15; // for yuntest
//int sparkPin = 9;
int coilDwell = 2;
unsigned long changeTime;
int potPin = 14; // for yuntest
//int potPin = A0;
void setup (){ 
pinMode (ledPin, OUTPUT);
pinMode (sparkPin, OUTPUT);
changeTime = millis();
Serial.begin(9600);
} 
void loop (){
ledDelay = analogRead(potPin); // lê o valor do potenciômetro
coilDwell = analogRead(potPin);
Serial.println(ledDelay);

digitalWrite (ledPin, HIGH);// define o pino do LED em nivel lógico alto(Deve acender o LED) 
digitalWrite (sparkPin, HIGH);
delay (coilDwell); // espera por 4 milissegundos,  coil dwell time) 

digitalWrite (ledPin, LOW); // define o pino do LED em nivel logico baixo(Deve apagar o LED) 
digitalWrite (sparkPin, LOW);
delay (ledDelay); // espera  
}
