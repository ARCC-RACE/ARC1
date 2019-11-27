#include <Servo.h>

#define ch1_pin 0
#define ch2_pin 1
#define ch3_pin 6
#define ch4_pin 7
#define ch5_pin 8
#define ch6_pin 9

#define steering_pin 4
#define throttle_pin 5

#define led 13

Servo steering, throttle;

//Values to store the packet data in
int steeringVal = 1500;
int throttleVal = 1500;

//For serial event
const int packetLength = 5;         //Packet structure = |PEC|steering MSB|steering LSB|throttle MSB|throttle LSB|
byte packet[5] = {0,0,0,0,0};       // a byte array to hold incoming data (length = 4)
boolean packetComplete = false;     // whether the string is complete

//For watchdog timer
unsigned long now = 0;
unsigned long lastPacket = 0;  //Two 16 bit ints, steering then thrptle 
boolean wdt_isTripped = false; //so  the timer is not tripped continuously

//For sending packet
unsigned long lastSend = 0;
unsigned long sendPeriod = 20000; //in microsoconds (1/Hz*1000000)
const int outGoingPacketLength = 13; //|ch1 LSB|ch1 MSB|ch2 LSB|ch2 MSB|ch3 LSB|ch3 MSB|ch4 LSB|ch4 MSB|ch5 LSB|ch5 MSB|ch6 LSB|ch6 MSB|PEC|

// For reading the controller
volatile unsigned long ch_1_rising, ch1_duty_cycle;
void ch1_rising_interrupt();
void ch1_falling_interrupt();

volatile unsigned long ch_2_rising, ch2_duty_cycle;
void ch2_rising_interrupt();
void ch2_falling_interrupt();

volatile unsigned long ch_3_rising, ch3_duty_cycle;
void ch3_rising_interrupt();
void ch3_falling_interrupt();

volatile unsigned long ch_4_rising, ch4_duty_cycle;
void ch4_rising_interrupt();
void ch4_falling_interrupt();

volatile unsigned long ch_5_rising, ch5_duty_cycle;
void ch5_rising_interrupt();
void ch5_falling_interrupt();

volatile unsigned long ch_6_rising, ch6_duty_cycle;
void ch6_rising_interrupt();
void ch6_falling_interrupt();

//Function for converting input (-1000 to 1000) to microseconds (1000 to 2000)
int calculateHardwareValues(int input){ 
  input = constrain(input, -1000, 1000);
  return map(input, -1000, 1000, 1000, 2000);
}

void setup() {
  // Configure throttle and steering PWM outputs and set drive system to neutral/off
  steering.attach(steering_pin);
  throttle.attach(throttle_pin);
  steering.writeMicroseconds(1500);
  throttle.writeMicroseconds(1500);

  attachInterrupt(digitalPinToInterrupt(ch1_pin), ch1_rising_interrupt, RISING);
  attachInterrupt(digitalPinToInterrupt(ch2_pin), ch2_rising_interrupt, RISING);
  attachInterrupt(digitalPinToInterrupt(ch3_pin), ch3_rising_interrupt, RISING);
  attachInterrupt(digitalPinToInterrupt(ch4_pin), ch4_rising_interrupt, RISING);
  attachInterrupt(digitalPinToInterrupt(ch5_pin), ch5_rising_interrupt, RISING);
  attachInterrupt(digitalPinToInterrupt(ch6_pin), ch6_rising_interrupt, RISING);

  pinMode(led, OUTPUT);
 
  while (!Serial); //Wait to connect to computer

  lastPacket = millis(); //start watchdog timer for the first packet

}

void loop() {
  //Serial.println("CH1: " + String(ch1_duty_cycle) + " CH2: " + String(ch2_duty_cycle) + " CH3: " + String(ch3_duty_cycle) + " CH4: " + String(ch4_duty_cycle) + " CH5: " + String(ch5_duty_cycle) + " CH6: " + String(ch6_duty_cycle));
  
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
  if (packetComplete) {      //If packet is valid

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

  //Sanity check on throttle values
  if(throttleVal > 2100 || throttleVal < 900){ 
    throttleVal = 1500;
  }
   
  //update steering/throttle to new vlaues
  throttle.writeMicroseconds(constrain(throttleVal, 1000, 2000));
  steering.writeMicroseconds(constrain(steeringVal, 1000, 2000));

  //Send packet to computer
  if(micros()-lastSend > sendPeriod){
    //Outgoing packet structure: |ch1 LSB|ch1 MSB|ch2 LSB|ch2 MSB|ch3 LSB|ch3 MSB|ch4 LSB|ch4 MSB|ch5 LSB|ch5 MSB|ch6 LSB|ch6 MSB|PEC|
    byte outGoingPacket[outGoingPacketLength];
    outGoingPacket[0] = (byte)(ch1_duty_cycle & 0b0000000011111111);
    outGoingPacket[1] = (byte)(ch1_duty_cycle >> 8);
    outGoingPacket[2] = (byte)(ch2_duty_cycle & 0b0000000011111111);
    outGoingPacket[3] = (byte)(ch2_duty_cycle >> 8);
    outGoingPacket[4] = (byte)(ch3_duty_cycle & 0b0000000011111111);
    outGoingPacket[5] = (byte)(ch3_duty_cycle >> 8);
    outGoingPacket[6] = (byte)(ch4_duty_cycle & 0b0000000011111111);
    outGoingPacket[7] = (byte)(ch4_duty_cycle >> 8);
    outGoingPacket[8] = (byte)(ch5_duty_cycle & 0b0000000011111111);
    outGoingPacket[9] = (byte)(ch5_duty_cycle >> 8);
    outGoingPacket[10] = (byte)(ch6_duty_cycle & 0b0000000011111111);
    outGoingPacket[11] = (byte)(ch6_duty_cycle >> 8);

    outGoingPacket[12] = (byte)(outGoingPacket[0]^outGoingPacket[1]^outGoingPacket[2]^outGoingPacket[3]^outGoingPacket[4]^outGoingPacket[5]^outGoingPacket[6]^outGoingPacket[7]^outGoingPacket[8]^outGoingPacket[9]^outGoingPacket[10]^outGoingPacket[11]); //PEC byte
    
    Serial.write(outGoingPacket, outGoingPacketLength);
  }
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
}
