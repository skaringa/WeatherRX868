# TempHygroRX868

Receiver and Decoder for data of wireless weather sensors manufactured by [ELV], like the S 300, ASH 2200, and KS 300. The communication protocol is also compatible to self-made sensors based on the [TempHygroTX868 arduino library][TempHygroTX868].

The software uses the ELV RX868 rf 868.35 MHz receiver module to receive the data of the sensors, decodes it and prints it in human readable or LogView format compatible to the USB-WDE1.

The software runs on the Arduino platform as well on the Raspberry Pi.

## Prerequisites

* [RX868 receiver][RX868]
* Arduino (Uno, Nano, Pro Mini etc.) or Raspberry Pi
* Only for Raspberry Pi: [WiringPi library][WiringPi]

## Wiring

### Arduino

    Arduino        RX868
     D3 ___________ DATA
     GND __________ GND
     5V/3V3 _______ +UB (The receiver works with 3.3V and 5V)
                 |_ EN

### Raspberry Pi GPIO

    Raspberry Pi         RX868
    GPIO WiringPi      
    13   GPIO2 __________ DATA
     9   GND ____________ GND
    17   3V3 ____________ +UB
                       |_ EN

## Installation


## Description of the input rf data

Source: [dc3yc]

* On-off keying with constant length of pulses of 1220.7µs
* One pulse codes a logical 0 or 1
* Logical 0: 854.5µs on and 366.2µs off
* Logical 1: 366.2µs on and 854.5µs off
* Data is grouped into nibbles of 4 bit with LSB first. Each nibbles is terminated with 1.

### Thermo/Hygro sensor like S 300 and ASH 2200

Data      | Meaning
--------- | -------------
00000001  | synchronisation (up to 10 0 bits)
10001     | sensor type: 1
AAA       | address (0..7)
S1        | sign of temperature, 0 = positive
TTTT1     | temperature digit with weight 0.1
TTTT1     | temperature digit with weight 1
TTTT1     | temperature digit with weight 10
HHHH1     | humidity digit with weight 0.1
HHHH1     | humidity digit with weight 1
HHHH1     | humidity digit with weight 10
QQQQ1     | check: XOR of data from type to check must be 0
SSSS1     | sum of data from type to check + 5 AND 0xF

### Kombi sensor KS 300

Data      | Meaning
--------- | -------------
00000001  | synchronisation (up to 10 0 bits)
11101     | sensor type: 7
1R0       | rain detector 1 = it is raining
S1        | sign of temperature, 0 = positive
TTTT1     | temperature digit with weight 0.1
TTTT1     | temperature digit with weight 1
TTTT1     | temperature digit with weight 10
HHHH1     | humidity digit with weight 1
HHHH1     | humidity digit with weight 10
WWWW1     | wind speed digit with weight 0.1
WWWW1     | wind speed digit with weight 1
WWWW1     | wind speed digit with weight 10
CCCC1     | amount of rainfall bit 0..4
CCCC1     | amount of rainfall bit 5..8
CCCC1     | amount of rainfall bit 9..12
XXXX1     | ?
QQQQ1     | check: XOR of data from type to check must be 0
SSSS1     | sum of data from type to check + 5 AND 0xF

## Description of the output serial data



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

[ELV]: https://www.elv.de
[TempHygroTX868]: https://github.com/skaringa/TempHygroTX868 "Arduino library to control the ELV TX868 rf transmitter module to send temperature and humidity values over the air."
[RX868]: https://de.elv.com/elv-empfangsmodul-rx868sh-dv-86835-mhz-131300 "ELV Empfangsmodul RX868SH-DV, 868,35 MHz"
[WiringPi]: http://wiringpi.com/ "WiringPi GPIO Interface library for the Raspberry Pi"
[dc3yc]: http://www.dc3yc.homepage.t-online.de/protocol.htm
