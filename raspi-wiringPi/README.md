# WeatherRX868 for Raspberry Pi

Adaption of the WeatherRX868 Arduino library for the RaspberryPi.

The software controls the ELV RX868 rf 868.35 MHz receiver module to read and decode data of wireless weather sensores.

The source code of the decoder is the same as for the Arduino. However the part to control the hardware differs. It is based on the [WiringPi library][WiringPi] and uses a fixed sampling rate instead of the interrupt driven approach of the Arduino.

## Hardware

* Raspberry Pi
* [RX868 receiver][RX868]

## Wiring

    Raspberry Pi         RX868
    GPIO WiringPi      
    13   GPIO2 __________ DATA
     9   GND ____________ GND
    17   3V3 ____________ +UB
                       |_ EN  

## Software

    sudo apt install git build-essential wiringpi
    git clone https://github.com/skaringa/WeatherRX868.git
    cd raspi-WiringPi
    make
    sudo ./rxdec

## License

Copyright 2015, 2020 Martin Kompf

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

[TempHygroTX868]: https://github.com/skaringa/TempHygroTX868 "Arduino library to control the ELV TX868 rf transmitter module to send temperature and humidity values over the air."
[RX868]: https://de.elv.com/elv-empfangsmodul-rx868sh-dv-86835-mhz-131300 "ELV Empfangsmodul RX868SH-DV, 868,35 MHz"
[WiringPi]: http://wiringpi.com/ "WiringPi GPIO Interface library for the Raspberry Pi"
