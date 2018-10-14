/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/* Helpful Resources 
GPIO https://community.atmel.com/sites/default/files/forum_attachments/PIN-IO-SAMD21-SAMR21_RevA_0.pdf
SPI
UART
I2C
PWM
*/

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * Bare minimum empty user application template
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# Minimal main function that starts with a call to system_init()
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include <asf.h>

//IO Port Definitions
#define LED0 PORT_PB03 //(1ul<<3)
#define LED1 PORT_PA27 //(1ul<<27)

int main (void)
{
	system_init();
	delay_init();
	
	REG_PORT_DIRSET0 = LED0; //Sets pin direction to output 
	REG_PORT_DIRSET0 = LED1; //Sets pin direction to output 

	/* Insert application code here, after the board has been initialized. */
	while (1)
	{
		REG_PORT_OUTTGL0 = LED0; // toggle PORT_PB03
		delay_s(1); // delay for 1 second
		REG_PORT_OUTTGL0 = LED1; // toggle PORT_PA27
		delay_s(1); // delay for 1 second
	}
}
