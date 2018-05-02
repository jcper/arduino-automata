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
#include <EEPROM.h>
#include "DHT.h" 
#define DHTPIN 2 
#define DHTTYPE DHT11 
DHT dht(DHTPIN, DHTTYPE); 
// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[6];
// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:

char server[50];    // name address for Google (using DNS)

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
String ID;//direccion del dispositivo
// variables will change:
int buttonState = 0;         // variable for reading the pushbutton status
int estado= 0;
int comunicacion=0;
int salida;//variable rele
unsigned long lastConnectionTime = 0;// last time you connected to the server, in milliseconds
long tiempo_polling;
unsigned long postingInterval;  // delay between updates, in milliseconds
// 
String mensaje;
bool recibido;
int contador;
long tiempoUltimaLectura=0;
String urlserver;
float h,t;
bool comando;//si recibe 'r' (rele) o 'a' (alarma)
struct MyStruct{
  long tiempo;
  byte mac[6];
  char server[50];
};

void setup() {
    pinMode(ledPin, OUTPUT);
    pinMode(buttonPin, INPUT);
    urlserver="";
    int eeAddress=0;
    salida=0;
    envio=String();
    mensaje=String();
    dht.begin(); //Se inicia el sensor
    contador=0;
    recibido=true;
    comando=false;
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
   MyStruct customVar; //Variable to store custom object read from EEPROM.
  EEPROM.get(eeAddress, customVar);

  Serial.println("Read custom object from EEPROM: ");
  Serial.println(customVar.tiempo);
  Serial.println(customVar.server);
  tiempo_polling=customVar.tiempo;
  postingInterval= tiempo_polling * 1000L;
  server[50]=customVar.server;
  for(int i=0; i<50; i++){
   server[i]=customVar.server[i];
  };
   for(byte j=0; j<6; j++){
   mac[j]=customVar.mac[j];
  Serial.print(mac[j],HEX);
  };
  char str[32] = "";
  array_to_string(mac, 6, str);
  Serial.println(str);
  ID=str;

  
  Serial.println(ID);
  Serial.println(server);
  Serial.println(tiempo_polling);
  
  
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
          if(contador==23 && c=='r' && comando==false){
             comando=true;
               Serial.print(c);
          }
          if(contador==27 && c=='0' && comando==true){
             digitalWrite(ledPin,LOW);
              Serial.print(c);
             salida=0;
          }
           if(contador==27 && c=='1' && comando==true){
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

void array_to_string(byte array[], unsigned int len, char buffer[])
{
    for (unsigned int i = 0; i < len; i++)
    {
        byte nib1 = (array[i] >> 4) & 0x0F;
        byte nib2 = (array[i] >> 0) & 0x0F;
        buffer[i*2+0] = nib1  < 0xA ? '0' + nib1  : 'A' + nib1  - 0xA;
        buffer[i*2+1] = nib2  < 0xA ? '0' + nib2  : 'A' + nib2  - 0xA;
    }
    buffer[len*2] = '\0';
}
