# WindSensor Base 

Reads data transmitted on 2.4GHz RF from a windsensor device and converts it into NMEA2000 Wind sentences for use on a NMEA2000 bus. The RF uses JDY-40 modules https://www.electrodragon.com/w/images/0/05/EY-40_English_manual.pdf based on a BEKEN RF Chip https://www.mikrocontroller.net/attachment/381910/BT-WiFi-52rf5541.pdf. Which claims a range of upto 120M (tested through walls and metal roofing at 40M no problems). The Base uses a ESP32 module (WROOM) with Wifi and BT disabled consuming about 80mA at 5V. CAN transcevers are MCP2562FD chips using 3.3V logic levels and 5V CAN driver voltage which seems to be compatible with most marine equipment expecting CANH at arround 4.5v.

The same setup could be used for any other RF Serial -> CANBus sensor, and the JDY-40 can switch RF channels relatively quickly, however for this application probably not fast enough to bridge Wind Readings at 10Hz. Reading data at 9600 1 line every 0.1s on a fixed RF channel works no problem as the underlying RF transport runs at 2Mb/s, but the JDY-40 wont switch reliably between channels using AT commands, see the doc on the ED-40 manual.


# TODO

[ ] Implement CLI with configuration settings.
[ ] Test on NMEA2000 bus
