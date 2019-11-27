#include<Servo.h>
#include<SPI.h>

void SERCOM2_Handler()
{
  uint8_t data = 0;
  data =(uint8_t)SERCOM2->SPI.DATA.reg;
  uint8_t interrupts = SERCOM2->SPI.INTFLAG.reg; //Read SPI interrupt register
  if(interrupts & (1<<3))
  {
    SERCOM2->SPI.INTFLAG.bit.SSL = 1; //clear slave select interrupt
  }
  if(interrupts & (1<<2))
  {
    data = SERCOM2->SPI.DATA.reg; //Read data register
    SERCOM2->SPI.INTFLAG.bit.RXC = 1; //clear receive complete interrupt
  }
  if(interrupts & (1<<1))
  {
    SERCOM2->SPI.INTFLAG.bit.TXC = 1; //clear receive complete interrupt
  }
  
  if(interrupts & (1<<0))
  {
    SERCOM2->SPI.DATA.reg = 0xAA;
  }
}

void spiSlave_init()
{

  //  PA08  MISO (SPI)
  //  PA09  SCK (SPI)
  //  PA10  CS0 (SPI)
  //  PA11  MOSI (SPI)
  //  SERCOM 2
  // D DOPO 0x0 / DIPO 0x3 MISO  SCK /SS MOSI
  
  PORT->Group[PORTA].PINCFG[16].bit.PMUXEN = 0x1; //Enable Peripheral Multiplexing for SERCOM2 SPI PA18 Arduino PIN10
  PORT->Group[PORTA].PMUX[8].bit.PMUXE = 0x2; //SERCOM 1 is selected for peripherial use of this pad
  PORT->Group[PORTA].PINCFG[17].bit.PMUXEN = 0x1; //Enable Peripheral Multiplexing for SERCOM2 SPI PA18 Arduino PIN10
  PORT->Group[PORTA].PMUX[8].bit.PMUXO = 0x2; //SERCOM 1 is selected for peripherial use of this pad
  PORT->Group[PORTA].PINCFG[18].bit.PMUXEN = 0x1; //Enable Peripheral Multiplexing for SERCOM2 SPI PA18 Arduino PIN10
  PORT->Group[PORTA].PMUX[9].bit.PMUXE = 0x2; //SERCOM 1 is selected for peripherial use of this pad
  PORT->Group[PORTA].PINCFG[19].bit.PMUXEN = 0x1; //Enable Peripheral Multiplexing for SERCOM2 SPI PA18 Arduino PIN10
  PORT->Group[PORTA].PMUX[9].bit.PMUXO = 0x2; //SERCOM 1 is selected for peripherial use of this pad
    
  
  //Disable SPI 0
  SERCOM2->SPI.CTRLA.bit.ENABLE =0;
  while(SERCOM2->SPI.SYNCBUSY.bit.ENABLE);
  
  //Reset SPI 0
  SERCOM2->SPI.CTRLA.bit.SWRST = 1;
  while(SERCOM2->SPI.CTRLA.bit.SWRST || SERCOM2->SPI.SYNCBUSY.bit.SWRST);
  
  //Setting up NVIC
  NVIC_EnableIRQ(SERCOM2_IRQn);
  NVIC_SetPriority(SERCOM2_IRQn,2);
  
  //Setting Generic Clock Controller!!!!
  GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID(GCM_SERCOM2_CORE) | //Generic Clock 0
            GCLK_CLKCTRL_GEN_GCLK0 | // Generic Clock Generator 0 is the source
            GCLK_CLKCTRL_CLKEN; // Enable Generic Clock Generator
  
  while(GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY); //Wait for synchronisation
  
  
  //Set up SPI Control A Register
  SERCOM2->SPI.CTRLA.bit.DORD = 0; //MSB first
  SERCOM2->SPI.CTRLA.bit.CPOL = 0; //SCK is low when idle, leading edge is rising edge
  SERCOM2->SPI.CTRLA.bit.CPHA = 0; //data sampled on leading sck edge and changed on a trailing sck edge
  SERCOM2->SPI.CTRLA.bit.FORM = 0x0; //Frame format = SPI
  SERCOM2->SPI.CTRLA.bit.DIPO = 0x3; //DATA PAD0 MISO is used as input (slave mode)
  SERCOM2->SPI.CTRLA.bit.DOPO = 0x00; //DATA PAD3 MISO is used as output
  SERCOM2->SPI.CTRLA.bit.MODE = 0x2; //SPI in Slave mode
  SERCOM2->SPI.CTRLA.bit.IBON = 0x1; //Buffer Overflow notification
  SERCOM2->SPI.CTRLA.bit.RUNSTDBY = 1; //wake on receiver complete
  
  //Set up SPI control B register
  //SERCOM2->SPI.CTRLB.bit.RXEN = 0x1; //Enable Receiver
  SERCOM2->SPI.CTRLB.bit.SSDE = 0x1; //Slave Selecte Detection Enabled
  SERCOM2->SPI.CTRLB.bit.CHSIZE = 0; //character size 8 Bit
  //SERCOM2->SPI.CTRLB.bit.PLOADEN = 0x1; //Enable Preload Data Register
  //while (SERCOM2->SPI.SYNCBUSY.bit.CTRLB);  
  
  //Set up SPI interrupts
  SERCOM2->SPI.INTENSET.bit.SSL = 0x1; //Enable Slave Select low interrupt        
  SERCOM2->SPI.INTENSET.bit.RXC = 0x1; //Receive complete interrupt
  SERCOM2->SPI.INTENSET.bit.TXC = 0x1; //Receive complete interrupt
  SERCOM2->SPI.INTENSET.bit.ERROR = 0x1; //Receive complete interrupt
  SERCOM2->SPI.INTENSET.bit.DRE = 0x1; //Data Register Empty interrupt
  //init SPI CLK  
  //SERCOM2->SPI.BAUD.reg = SERCOM_FREQ_REF / (2*4000000u)-1;
  //Enable SPI
  SERCOM2->SPI.CTRLA.bit.ENABLE = 1;
  while(SERCOM2->SPI.SYNCBUSY.bit.ENABLE);
  SERCOM2->SPI.CTRLB.bit.RXEN = 0x1; //Enable Receiver, this is done here due to errate issue
  while(SERCOM2->SPI.SYNCBUSY.bit.CTRLB); //wait until receiver is enabled

}

void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
