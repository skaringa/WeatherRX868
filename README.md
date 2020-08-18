# WeatherRX868

![runtest](https://github.com/skaringa/WeatherRX868/workflows/runtest/badge.svg)

Arduino library to receive and decode data of wireless weather sensors manufactured by [ELV], like the S 300, ASH 2200, and KS 300. The communication protocol is also compatible to self-made sensors based on the [TempHygroTX868 arduino library][TempHygroTX868].

The software uses the ELV RX868 868.35 MHz receiver module to receive the data of the sensors, decodes it and prints it in human readable or LogView format compatible to the USB-WDE1.

As a bonus, the decoder runs on the Raspberry Pi as well. See sub-directory ```raspi-wiringPi```.

![Arduino Nano with RX868](https://www.kompf.de/tech/images/nano-rx868.jpg)

## Hardware

* [RX868 receiver][RX868]
* Arduino Uno, Nano, Pro Mini etc.

## Wiring

    Arduino        RX868
     D3 ___________ DATA
     GND __________ GND
     5V/3V3 _______ +UB (The receiver works with 3.3V and 5V)
                 |_ EN

## Setup

* Install this library:
  * Download WeatherRX868.zip from [releases page][release]
  * Or clone this git repo into the libraries folder of your Arduino Installation
  * Follow the steps described at [Installing Additional Arduino Libraries](https://www.arduino.cc/en/guide/libraries)
* Connect [RX868 receiver][RX868] to Arduino
* Open the sketch from File - Examples - WeatherRX868
* Upload the sketch to the Arduino
* View output on the serial console at 57600 bps


## Description of the input rf sensor data

Source: [dc3yc]

* On-off keying with constant length of pulses of 1220.7µs
* One pulse codes a logical 0 or 1
* Logical 0: 854.5µs on and 366.2µs off
* Logical 1: 366.2µs on and 854.5µs off
* Data is grouped into nibbles of 4 bit with LSB first. Each nibble is terminated with 1.

### Thermo/Hygro sensor like S 300 and ASH 2200

Data      | Meaning
--------- | -------------
0000000   | synchronisation (up to 10 bits)
1         | start bit
10001     | sensor type: 1
AAA       | address (0..7)
S1        | sign of temperature, S==0 => positive
TTTT1     | temperature digit with weight 0.1
TTTT1     | temperature digit with weight 1
TTTT1     | temperature digit with weight 10
HHHH1     | humidity digit with weight 0.1
HHHH1     | humidity digit with weight 1
HHHH1     | humidity digit with weight 10
QQQQ1     | check: XOR of data from sensor type to check - must be 0
SSSS1     | sum of data from sensor type to check + 5 AND 0xF

#### Example

    0000000110001011010001100001010010110110011101011110110001

Thermo/Hygro Sensor (type 1), address 6, temperature 20.8 °C, rel. humidity 59.6 %

### Kombi sensor KS 300

Data      | Meaning
--------- | -------------
0000000   | synchronisation (up to 10 bits)
1         | start bit
11101     | sensor type: 7
1R0       | rain detector R==1 => it is raining
S1        | sign of temperature, S==0 => positive
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
QQQQ1     | check: XOR of data from sensor type to check - must be 0
SSSS1     | sum of data from sensor type to check + 5 AND 0xF

#### Example

    11101100011110100011100010001101101000010000100001101111110100001101111000110111

Kombi Sensor (type 7), temperature 18.7 °C, rel. humidity 68 %, wind speed 0.0 km/h, rainfall 125

## Description of the output data

The sample sketch WeatherRX868 contains two different functions to send the decoded date to
the serial port. You may use the one that fits your needs or code another one.

__printDecoderOutput__: Prints data in human readable format that should be self-explaining.

__handleLogViewOutput__: Prints data in LogView format that is compatible to the USB-WDE1.
The data is printed line by line, each line contains one field per sensor value, the fields are separated by semicolon __;__

Data field | Meaning
---------- | -------
$1;        | start of record
1;         | state (always 1)
;          | timeStamp (always empty)
 dd.d;     | temperature sensor address 0
 dd.d;     | temperature sensor address 1
 dd.d;     | temperature sensor address 2
 dd.d;     | temperature sensor address 3
 dd.d;     | temperature sensor address 4
 dd.d;     | temperature sensor address 5
 dd.d;     | temperature sensor address 6
 dd.d;     | temperature sensor address 7
dd.d;      | humidity sensor address 0
dd.d;      | humidity sensor address 1
dd.d;      | humidity sensor address 2
dd.d;      | humidity sensor address 3
dd.d;      | humidity sensor address 4
dd.d;      | humidity sensor address 5
dd.d;      | humidity sensor address 6
dd.d;      | humidity sensor address 7
dd.d;      | temperature Kombisensor
dd.d;      | humidity Kombisensor
dd.d;      | wind speed
ddd;       | amount of rainfall
d;         | rain detector
0\r\n      | end of record

### Example

    $1;1;;27.7;24.7;;;21.2;;25.4;;0.0;43.9;;;64.9;;54.9;;30.0;33.0;0.0;42;0;0



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
[release]: https://github.com/skaringa/WeatherRX868/releases/latest
[dc3yc]: http://www.dc3yc.homepage.t-online.de/protocol.htm
