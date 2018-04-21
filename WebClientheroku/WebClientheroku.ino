/*
  Web client

 This sketch connects to a website (http://www.google.com)
 using an Arduino Wiznet Ethernet shield.

 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13

 created 18 Dec 2009
 by David A. Mellis
 modified 9 Apr 2012
 by Tom Igoe, based on work by Adrian McEwen
 Cliente para heruku basico sistema de alarma cada 10 segundos.
 */

#include <SPI.h>
#include <Ethernet.h>
#include <string.h>
#include "DHT.h" 
#define DHTPIN 2 
#define DHTTYPE DHT11 
DHT dht(DHTPIN, DHTTYPE); 
// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:

char server[] = "protected-anchorage-54868.herokuapp.com";    // name address for Google (using DNS)

// Set the static IP address to use if the DHCP fails to assign
IPAddress ip(192, 168, 0, 214);

// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
EthernetClient client;
const int buttonPin = 4;     // the number of the pushbutton pin
const int ledPin =  7;      // the number of the LED pin
const int S0=5;
String cabeceraGet="GET /test?";
String id="mac=";
String estat="&estado=";
String entrada="&entrada=";
String rele="&salida=";
String temperatura="&temp=";
String humedad="&hum=";
String final=" HTTP/1.1"; 
String envio;
String ID="DE:AD:BE:EF:FE:ED";//direccion del dispositivo
// variables will change:
int buttonState = 0;         // variable for reading the pushbutton status
int estado= 0;
int comunicacion=0;
int salida=0;//variable rele
unsigned long lastConnectionTime = 0;             // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 10L * 1000L; // delay between updates, in milliseconds
// 
String mensaje;
bool recibido;
int contador;
long tiempoUltimaLectura=0;
float h,t;
bool comando;//si recibe 'r' (rele) o 'a' (alarma)


void setup() {
    pinMode(ledPin, OUTPUT);
    pinMode(buttonPin, INPUT);
    envio=String();
    mensaje=String();
    dht.begin(); //Se inicia el sensor
    contador=0;
    recibido=true;
    comando=false;
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
   delay(1000);
  // start the Ethernet connection:
   if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, ip);
  }
    // print the Ethernet board/shield's IP address:
  Serial.print("My IP address: ");
  Serial.println(Ethernet.localIP());
   
 }

void loop() {
  // read the state of the pushbutton value:
  buttonState = digitalRead(buttonPin);

  if(millis()-tiempoUltimaLectura>2000){    
   h = dht.readHumidity(); //se lee la humedad
   t= dht.readTemperature(); // se lee la temperatura
  tiempoUltimaLectura=millis(); //actualizamos el tiempo de la última lectura
  }
  envio=cabeceraGet+id+ID+estat+comunicacion+entrada+estado+rele+salida+temperatura+t+humedad+h+final;
  
  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (buttonState == HIGH) {
        estado=1;
       Serial.println(" Alarma enviada desde arduino  " + ID);
        delay(500);
        
        } 
  // if there are incoming bytes available
  // from the server, read them and print them:
  
  if (client.available()) {
     char c = client.read();//Leer 1 carácter
     if(contador<40){
      if(c=='[' && recibido){
          recibido=false;
          contador ++;
        }
        if(contador>0){
          Serial.print(c);
          if(contador==28 && c=='r' && comando==false){
             comando=true;
          }
          if(contador==32 && c=='0' && comando==true){
             digitalWrite(ledPin,LOW);
             salida=0;
          }
           if(contador==32 && c=='1' && comando==true){
             digitalWrite(ledPin,HIGH);
             salida=1;
          }
          contador ++;
       }
     }
  }
   
  // if ten seconds have passed since your last connection,
  // then connect again and send data:
 
  if (millis() - lastConnectionTime > postingInterval) {
    httpRequest();
    }
  }


// this method makes a HTTP connection to the server:
void httpRequest() {
  // close any connection before send a new request.
  // This will free the socket on the WiFi shield
  client.stop();
   recibido=true;
   contador=0;
   comando=false;
  // if there's a successful connection:
  if (client.connect(server, 80)) {
    //Serial.println("connecting...");
    // send the HTTP GET request:
     
     comunicacion=1; //Ya hay comunicacion
   
    client.println(envio);
    client.println("Host: protected-anchorage-54868.herokuapp.com");
    client.println("Connection: close");
    client.println();
    estado=0;
    // note the time that the connection was made:
    lastConnectionTime = millis();
  } else {
    // if you couldn't make a connection:
    Serial.println("connection failed");
  }
}

