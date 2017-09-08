/*AnneCamille*/
#include "DHT.h"
#include <SPI.h>
//#include "RF24.h"
#define DHTPIN 2    // what digital pin we're connected to 2
#define DHTTYPE DHT11   // DHT 11
#include "LowPower.h"
#include <Battery.h> 
#include<RF24.h>
#include <printf.h>

Battery battery(5600, 5600, A0, 3); // battery level en mA

RF24 radio(9,10); // ce csn pins

unsigned long count = 0;
int sensor1 = 2; // 2
String hum;
String temp;
String lum;
String out;
const int pinLight = A1; // 5
const int sensepin = A0; // connected to the resistor to monitor the battery


DHT dht(DHTPIN, DHTTYPE);


void setup() {
  while (!Serial);
  Serial.begin(9600);
  
  radio.begin();
  dht.begin();
  radio.setPALevel(RF24_PA_MAX);
  radio.setChannel(0x60);
  //listen to a command from the raspberry
  const uint64_t pipe = (0xE8E8F0F0E1LL);
  radio.openReadingPipe(1, pipe);
  radio.openWritingPipe(0xF0F0F0F0E1LL);
  radio.enableDynamicPayloads();
  radio.powerUp();
  //radio.setAutoAck(true);
  pinMode(sensor1, INPUT);
  pinMode(2, OUTPUT);
  float ratio = ( + 10) / 10; // values of the resistor of the tension divider bridge
  // 5V board, change to 3300 for a 3.3V board
  battery.begin(5000, ratio);
 
}



void Temps(){

int i;
for ( i=0; i<30 ; i++) { // during 4 minutes arduino sleeping
  
        i++;
        //radio.Sleep();
        LowPower.powerDown(SLEEP_8S,ADC_OFF, BOD_OFF); // decrease the energy delivering by the arduino

      }
}

void loop(void) {
       

       
       //send command to the raspberry pi
       
       radio.startListening();
       Serial.println("Starting loop. Radio on");
       char receivedMessage[32] = {0};
       Serial.println(receivedMessage);
       //if (radio.available()){
         //Serial.println("radio available");
        
      if (radio.available()){
          radio.read(&receivedMessage,sizeof(receivedMessage));
          Serial.println(receivedMessage);
          Serial.println("Turning off the radio.");
          radio.stopListening();
          String stringMessage(receivedMessage);
          Serial.println(receivedMessage);
          
          if(stringMessage == "GETDATA"){

            /*How the data is send to the raspberry*/

            // Reading temperature or humidity takes about 250 milliseconds!
          // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
             float h = dht.readHumidity();
      // Read temperature as Celsius (the default)
             float t = dht.readTemperature();

             //reading the luminosity

            int sensorValue = analogRead(pinLight);

            char outBuffer[32]="";

            // changing the float and the int to string

            temp += t;
            hum += h;
            lum += sensorValue;
            out = temp + ";" + hum + ";" + lum; 
            Serial.println(out);
      
          out.toCharArray(outBuffer, 32); // convert string in char
          radio.write(&outBuffer, sizeof(outBuffer));
          Serial.println("Message send");

           //Clearfunction();
          out.remove(0); // remove from the inex 0 to the end of the string
          temp.remove(0);
          hum.remove(0);
          lum.remove(0);

          

       
        Temps();
            

          Serial.print("Battery voltage is ");
          Serial.print(battery.voltage());
          Serial.print(" (");
          Serial.print(battery.level());
          Serial.println(")");

        //batt += battery.voltage;

       //Serial.print("\n Humidity: ");
        //Serial.print(h);
      //Serial.print(" %\t");
      //Serial.print("\n Temperature: ");
      //Serial.print(t);
      //Serial.print(" *C ");
      //Serial.print("\n Luminosity indication : ");
      //Serial.print(sensorValue

       
        }

      }
       /*else{
        Serial.println("radio not available");
       }*/
  
}

