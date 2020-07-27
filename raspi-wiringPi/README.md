# TempHygroRX868

Receiver for data of wireless weather sensors with RX868 and Raspberry Pi.

The software controls the ELV RX868 rf 868.35 MHz receiver module to read and decode data of wireless weather sensores.

The communication protocol is compatible to ELV sensors like the S 300 and ASH 2200 and self-made sensors based on the [TempHygroTX868 arduino library][TempHygroTX868].

## Prerequisites

* Raspberry Pi
* [RX868 receiver][RX868]
* [WiringPi library][WiringPi]

## License

Copyright 2015 Martin Kompf

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
[RX868]: http://www.elv.de/output/controller.aspx?cid=74&detail=10&detail2=42432 "ELV Empfangsmodul RX868SH-DV, 868,35 MHz"
[WiringPi]: http://wiringpi.com/ "WiringPi GPIO Interface library for the Raspberry Pi"
