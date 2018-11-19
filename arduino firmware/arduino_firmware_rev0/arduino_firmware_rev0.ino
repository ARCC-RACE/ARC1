#include <Servo.h>

Servo steering, throttle; //servo objects on pins 3, 5

//Values to store the packet data in
int steeringVal = 1500;
int throttleVal = 1500;
int steeringTrim = -60; //For adjusting steering


//For serial event
const int packetLength = 5;         //Packet structure = |PEC|steering MSB|steering LSB|throttle MSB|throttle LSB|
byte packet[5] = {0,0,0,0,0};       // a byte array to hold incoming data (length = 4)
boolean packetComplete = false;     // whether the string is complete

//For watchdog timer
unsigned long now = 0;
unsigned long lastPacket = 0;  //Two 16 bit ints, steering then thrptle 
boolean wdt_isTripped = false; //so  the timer is not tripped continuously 

//For pin 13 LED in order to see packet loss
const int led = 13;

void activeBrake(){
   throttle.writeMicroseconds(1700);
   //Brake starting now
   for(int i = 1600; i > 1100; --i){
      throttle.writeMicroseconds(1100);
      delay(1);
    }
   delay(100);
  //Stop breaking and go to neutral  
  throttle.writeMicroseconds(1500);
  delay(10);
  }

//Function for converting thruster input (-1000 to 1000) to microseconds (1100 to 1900)
int calculateHardwareValues(int input){ 
  input = constrain(input, -1000, 1000);
  return map(input, -1000, 1000, 1100, 1900);
  }

/*------------------------------Start of runtime program------------------------------*/
void setup() {
  // initialize serial to 115200 baud (SERIAL_8E1) 8 bit, even parity, 1 stop bit
  Serial.begin(115200, SERIAL_8E1);

  pinMode(led, OUTPUT);

  //Setup Servo pins
  steering.attach(3);
  throttle.attach(5);

  //Set hawrdware to neutral
  steering.writeMicroseconds(1500);
  throttle.writeMicroseconds(1500);

  delay(1000); //delay to make sure hardware get setup properly
  Serial.println("Hardware controller initialized");

  lastPacket = millis(); //start watchdog timer for the first packet
}

void loop() {

  now = millis();//get current time to  ensure connection to main contorller 

  if(wdt_isTripped || now - lastPacket > 500){ //If the contorller hasn't recived a new packet in half a second (short circuit limits calcs)
    
     steeringVal = 1500;
     throttleVal = 1500;
    
    if(wdt_isTripped == false){
     Serial.println("Drive controller watchdog tripped!");
     }
     wdt_isTripped = true;
   }
    
  // When a new packet arrives indicated by a newline '\n' char:
  if (packetComplete) {
      //If packet is valid

      steeringVal = calculateHardwareValues((packet[3]<<8)|packet[2]);
      throttleVal = calculateHardwareValues((packet[1]<<8)|packet[0]);

//      Serial.println(packet, BIN);      //DEBUG
//
//      Serial.println(steeringVal, BIN); //DEBUG
//      Serial.println(steeringVal);      //DEBUG
//      Serial.println(throttleVal, BIN); //DEBUG
//      Serial.println(throttleVal);      //DEBUG
      
      lastPacket = millis(); //Pet the watchdog timer
      wdt_isTripped = false;
    
      // clear the packet:
      for(int i = 0; i < packetLength; i++){
        packet[i] = 0;
        }
      packetComplete = false;
  }

  //update thrusters to new values
  if(throttleVal < 1300 || throttleVal > 1700){ //Temporary safety system to prevent over throttle
      activeBrake();
    } else {
      throttle.writeMicroseconds(throttleVal);
    }
  steering.writeMicroseconds(constrain((steeringVal + steeringTrim), 1100, 1900));
}


void serialEvent() {
  
  byte pecVal = 0; //for Packet Error Checking

  for(int i = 0; i < packetLength; i++){
    packet[i] = static_cast<byte>(Serial.read()); //Never ever use readBytes()
    if(i < 4){
      pecVal = pecVal^packet[i]; //XOR with incomming byte
      }
    delayMicroseconds(1000); //Wait to finish adding the incomming byte to the buffer before reading it
    }

   //if packet is good based on PEC byte
   if(pecVal == packet[4])
   {
      packetComplete = true;
      digitalWrite(led, HIGH);
   }
   else
   {
      digitalWrite(led, LOW);
      while(Serial.available()){Serial.read();} //Clear the Serial input buffer
      // clear the packet:
      for(int i = 0; i < packetLength; i++){
        packet[i] = 0;
        }     
   }

  //DEBUGGING INFO
  /*Serial.write(packet[0]);
  Serial.write(packet[1]);
  Serial.write(packet[2]);
  Serial.write(packet[3]);
  Serial.flush();*/
}
