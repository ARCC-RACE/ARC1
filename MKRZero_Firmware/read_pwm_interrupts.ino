//ch1
void ch1_rising_interrupt(){
  ch_1_rising = micros();
  attachInterrupt(digitalPinToInterrupt(ch1_pin), ch1_falling_interrupt, FALLING);
}

void ch1_falling_interrupt(){
  ch1_duty_cycle = micros()-ch_1_rising;
  attachInterrupt(digitalPinToInterrupt(ch1_pin), ch1_rising_interrupt, RISING);
}

//ch2
void ch2_rising_interrupt(){
  ch_2_rising = micros();
  attachInterrupt(digitalPinToInterrupt(ch2_pin), ch2_falling_interrupt, FALLING);
}

void ch2_falling_interrupt(){
  ch2_duty_cycle = micros()-ch_2_rising;
//  detachInterrupt(digitalPinToInterrupt(ch2_pin));
  attachInterrupt(digitalPinToInterrupt(ch2_pin), ch2_rising_interrupt, RISING);
}

//ch3
void ch3_rising_interrupt(){
  ch_3_rising = micros();
  attachInterrupt(digitalPinToInterrupt(ch3_pin), ch3_falling_interrupt, FALLING);
}

void ch3_falling_interrupt(){
  ch3_duty_cycle = micros()-ch_3_rising;
  attachInterrupt(digitalPinToInterrupt(ch3_pin), ch3_rising_interrupt, RISING);
}

//ch4
void ch4_rising_interrupt(){
  ch_4_rising = micros();
  attachInterrupt(digitalPinToInterrupt(ch4_pin), ch4_falling_interrupt, FALLING);
}

void ch4_falling_interrupt(){
  ch4_duty_cycle = micros()-ch_4_rising;
  attachInterrupt(digitalPinToInterrupt(ch4_pin), ch4_rising_interrupt, RISING);
}

//ch5
void ch5_rising_interrupt(){
  ch_5_rising = micros();
  attachInterrupt(digitalPinToInterrupt(ch5_pin), ch5_falling_interrupt, FALLING);
}

void ch5_falling_interrupt(){
  ch5_duty_cycle = micros()-ch_5_rising;
  attachInterrupt(digitalPinToInterrupt(ch5_pin), ch5_rising_interrupt, RISING);
}

//ch6
void ch6_rising_interrupt(){
  ch_6_rising = micros();
  attachInterrupt(digitalPinToInterrupt(ch6_pin), ch6_falling_interrupt, FALLING);
}

void ch6_falling_interrupt(){
  ch6_duty_cycle = micros()-ch_6_rising;
  attachInterrupt(digitalPinToInterrupt(ch6_pin), ch6_rising_interrupt, RISING);
}
