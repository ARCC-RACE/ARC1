/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/* Helpful Resources 
GPIO https://community.atmel.com/sites/default/files/forum_attachments/PIN-IO-SAMD21-SAMR21_RevA_0.pdf
SPI  https://microchipdeveloper.com/32arm:samd21-code-gcc-sercom-spi-master-example
     http://asf.atmel.com/docs/3.34.1/samd21/html/group__asfdoc__sam0__sercom__spi__group.html #asfdoc_sam0_sercom_spi_examples
UART
I2C
PWM http://asf.atmel.com/docs/3.34.1/samd21/html/asfdoc_sam0_tcc_basic_use_case.html
USB http://asf.atmel.com/docs/3.34.1/samd21/html/udi_cdc_quickstart.html
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
#include "delay.h"

//IO Port Definitions
#define LED0 PORT_PB03 //(1ul<<3)
#define LED1 PORT_PA27 //(1ul<<27)
#define steering PIN_PA06E_TCC1_WO0
#define throttle PIN_PA05E_TCC0_WO1
#define encoder PIN_PA01
#define uart_tx PIN_PA12C_SERCOM2_PAD0
#define uart_rx PIN_PA13C_SERCOM2_PAD1

//SPI Definitions 
#define BUF_LENGTH 20

struct spi_module spi_slave_instance; //A globally available software device instance struct to store the SPI driver state while it is in use.

static uint8_t buffer_rx[BUF_LENGTH] = {0x00,}; //Receiving Buffer for SPI
static uint8_t buffer_expect[BUF_LENGTH] = {
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
	0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13
};

volatile bool transfer_complete_spi_slave = false; //A global variable that can flag to the application that the buffer has been transferred.

//A function for configuring the SPI.
void configure_spi_slave(void)
{
	struct spi_config config_spi_slave;
	/* Configure, initialize and enable SERCOM SPI module */
	spi_get_config_defaults(&config_spi_slave);
	config_spi_slave.mode = SPI_MODE_SLAVE;
	config_spi_slave.mode_specific.slave.preload_enable = true;
	config_spi_slave.mode_specific.slave.frame_format = SPI_FRAME_FORMAT_SPI_FRAME;
	config_spi_slave.mux_setting = SPI_SIGNAL_MUX_SETTING_D;
	config_spi_slave.pinmux_pad0 = PINMUX_PA08C_SERCOM0_PAD0;
	config_spi_slave.pinmux_pad1 = PINMUX_PA09C_SERCOM0_PAD1;
	config_spi_slave.pinmux_pad2 = PINMUX_PA10C_SERCOM0_PAD2;
	config_spi_slave.pinmux_pad3 = PINMUX_PA11C_SERCOM0_PAD3;
	spi_init(&spi_slave_instance, SERCOM0, &config_spi_slave);
	spi_enable(&spi_slave_instance);
}

//SPI callback function.
static void spi_slave_callback(struct spi_module *const module)
{
	transfer_complete_spi_slave = true;
}

//A function for configuring the callback functionality of the SPI.
void configure_spi_slave_callbacks(void)
{
	spi_register_callback(&spi_slave_instance, spi_slave_callback, SPI_CALLBACK_BUFFER_RECEIVED);
	spi_enable_callback(&spi_slave_instance, SPI_CALLBACK_BUFFER_RECEIVED);
}


//PWM and TCC Definitions
//Throttle PWM Signal (ESC)
struct tcc_module tcc_throttle;
static void configure_tcc_throttle(void)
{
	struct tcc_config config_tcc;
	tcc_get_config_defaults(&config_tcc, TCC0);
	config_tcc.counter.clock_prescaler = TCC_CLOCK_PRESCALER_DIV256;
	config_tcc.counter.period = 0xEA6; //50Hz
	config_tcc.compare.wave_generation = TCC_WAVE_GENERATION_SINGLE_SLOPE_PWM;
	config_tcc.compare.match[1] = (0xEA6 / 4);
	config_tcc.pins.enable_wave_out_pin[1] = true;
	config_tcc.pins.wave_out_pin[1]        = throttle;
	config_tcc.pins.wave_out_pin_mux[1]    = MUX_PA05E_TCC0_WO1;
	tcc_init(&tcc_throttle, TCC0, &config_tcc);
	tcc_enable(&tcc_throttle);
}

//Steering PWM Signal (Servo)
struct tcc_module tcc_steering;
static void configure_tcc_steering(void)
{
	struct tcc_config config_tcc;
	tcc_get_config_defaults(&config_tcc, TCC1);
	config_tcc.counter.clock_prescaler = TCC_CLOCK_PRESCALER_DIV256;
	config_tcc.counter.period = 0xEA6; //50Hz assuming 48Mhz clock
	config_tcc.compare.wave_generation = TCC_WAVE_GENERATION_SINGLE_SLOPE_PWM;
	config_tcc.compare.match[0] = (0xEA6 / 15); // 2ms (0xEA6 / 20 = 1ms)
	config_tcc.pins.enable_wave_out_pin[0] = true;
	config_tcc.pins.wave_out_pin[0]        = steering;
	config_tcc.pins.wave_out_pin_mux[0]    = MUX_PA06E_TCC1_WO0;
	tcc_init(&tcc_steering, TCC1, &config_tcc);
	tcc_enable(&tcc_steering);
}


int main (void)
{
	system_init();
	delay_init();
	//configure_spi_slave();
	//configure_spi_slave_callbacks();
	//configure_tcc_throttle();
	//configure_tcc_steering();
	
	uint8_t result = 0; //result from SPI 
	
	REG_PORT_DIRSET0 = LED1; // Direction set to OUTPUT
	REG_PORT_DIRSET0 = LED0; // Direction set to OUTPUT
	while (1) {
		REG_PORT_OUTTGL0 = LED1; // toggle 
		REG_PORT_OUTTGL0 = LED0; // toggle 
		delay_s(1); // delay for 1 second
	}
}
