#include <Servo.h>

Servo steering, throttle; //servo objects on pins 3, 5

//Values to store the packet data in
int steeringVal = 0;
int throttleVal = 0;


//For serial event
String packet = "";         // a String to hold incoming data
boolean packetComplete = false;  // whether the string is complete

//For watchdog timer
unsigned long now = 0;
unsigned long lastPacket = 0;
boolean wdt_isTripped = false; //so  the timer is not tripped continuously 

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
  Serial.begin(115200);
  // reserve 100 bytes for the packet:
  packet.reserve(100);

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
    
    if(packet.length() == 5){ //check to make sure new packet is valid
      //If packet is valid
      steeringVal = 1500;
      throttleVal = 1500;
     }
    
    if(wdt_isTripped == false){
     Serial.println("Thruster controller watchdog tripped!");
     }
     wdt_isTripped = true;
   }
    
  // When a new packet arrives indicated by a newline '\n' char:
  if (packetComplete) {
    if(packet.length() == 5){ //check to make sure new packet is valid
      //If packet is valid

      steeringVal = (packet[0] << 8)|packet[1];
      throttleVal = (packet[2] << 8)|packet[3];

      Serial.println(packet[1], BIN);

      Serial.println(steeringVal, BIN); //DEBUG
      Serial.println(steeringVal);
      Serial.println(throttleVal, BIN); //DEBUG
      Serial.println(throttleVal);
      
      lastPacket = millis(); //Pet the watchdog timer
      wdt_isTripped = false;
      
    } else { //Packet invalid
      Serial.println("Hardware controller packet invalid!"); //Packet is invlaid
    }
    
    // clear the packet:
    packet = "";
    packetComplete = false;
  }

  //update thrusters to new values
  if(throttleVal < 1400 || throttleVal > 1600){ //Temporary safety system to prevent over throttle
      activeBrake();
    } 
  else {
      throttle.writeMicroseconds(throttleVal);
    }
  
  steering.writeMicroseconds(steeringVal);

  delay(1);
}


void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the packet:
    packet += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') {
      packetComplete = true;
    }
  }
}
