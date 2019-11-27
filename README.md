# ARC1
Firmware repository for the ARC series board. This repository has code for the following major functions:
1. SPI Slave SAMD21
2. Read PWM
3. Write PWM
4. Digital LED toggle
5. Read UART for GPS module

## Setup
Download the latest version of Atmel Studio 7 on a windows 10 computer and connect you Ateml ICE serial debugger. Once connected connect to the SWD header on ARCC and proceed to verify that the chip is recognized. Next ssh into the jetson tx2 using the `ssh-keygen -R <jetson ip>` to recreate the key as needed. For the Jetson car the password should always be `jetsoncar`.

IGNORE THIS STEP - NOT VERIFIED
Setup SPI on the Jetson TX2 by following this guide: https://elinux.org/Jetson/TX2_SPI YOU DO NOT NEED TO GO THROUGH THE STEPS OF REBUILDING THE KERNEL Just do the `Verifying SPIDev Module` step to make sure. Start with the steps that follow that check.

To view the current version of Jetpack and L4T on your Jetson TX2 device use the `cat /etc/nv_tegra_release` command

## Documentation sources
[TX2 Device Tree](https://elinux.org/Jetson/TX2_DTB)

[Good Explanation of SPI setup process for Jetson TX1](https://www.jetsonhacks.com/2017/04/04/gpio-and-spi-nvidia-jetson-tx1/)

[Python SPI Dev](https://github.com/doceme/py-spidev)

[Logic Analyzer Software](https://www.saleae.com/downloads/)


https://learn.adafruit.com/using-atsamd21-sercom-to-add-more-spi-i2c-serial-ports/creating-a-new-spi

http://ww1.microchip.com/downloads/en/AppNotes/00002465A.pdf

http://asf.atmel.com/docs/latest/samd10/html/asfdoc_sam0_sercom_spi_mux_settings.html

https://github.com/rm-hull/spidev-test

`C:\Users\maste\AppData\Local\Arduino15\packages\arduino\tools\CMSIS-Atmel\1.2.0\CMSIS\Device\ATMEL Add in Arduino Core toolchain for GNU C/C++ compiler`. See Image


https://community.atmel.com/forum/atmel-studio-7-create-project-arduino-sketch-build-failure

https://codeblog.vurdalakov.net/2017/04/how-to-find-hex-bin-elf-generated-by-arduino-ide.html
## Raspberry Pi SPI
https://www.raspberrypi.org/documentation/hardware/raspberrypi/spi/README.md

https://www.raspberrypi.org/forums/viewtopic.php?t=56669

https://pypi.org/project/spidev/

