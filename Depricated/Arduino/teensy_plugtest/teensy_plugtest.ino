int ledDelay;
int ledPin = 13; //LED conectado ao pino digital 13
int sparkPin = 4; // for yuntest
//int sparkPin = 9;
int coilDwell = 2;
//unsigned long changeTime;
//int potPin = 14; // for yuntest
//int potPin = A0;
void setup (){ 
pinMode (ledPin, OUTPUT);
pinMode (sparkPin, OUTPUT);
//Serial.begin(9600);
} 
void loop (){
digitalWrite (ledPin, HIGH);// define o pino do LED em nivel lógico alto(Deve acender o LED) 
digitalWrite (sparkPin, HIGH);
delay (250); // espera por 4 milissegundos,  coil dwell time) 

digitalWrite (ledPin, LOW); // define o pino do LED em nivel logico baixo(Deve apagar o LED) 
digitalWrite (sparkPin, LOW);
delay (5000); // espera  
}
