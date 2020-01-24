# PiDuino Database Manager

_Arduino on Pi boards, the best of both worlds !_

## Abstract

PiDuino is a C ++ library for Pi boards that allows the use of I/O like GPIO,
I2C, SPI, UART ... with an API as close as possible to the Arduino language.  
The description of Pi cards uses a stored "Object" model in a database that 
allows to add new models of boards easily.  

At this time, the SoC models supported are AllWinner H-Series and Broadcom 
BCM2708 through 2711 which allows it to be used on Raspberry Pi and most Nano Pi, 
Orange Pi and Banana Pi.

pidbm allows you to manage the piduino database, whether local or remote (on a 
server). 

It is a multiplatform software developed with [cppdb](https://sourceforge.net/projects/cppcms)
which allows you to add, modify or delete the different elements that make up 
the PiDuino database (boards, GPIO, connectors, pins, SoC ...).

