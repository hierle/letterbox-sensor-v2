# LoRaWan EU868MHz Letterbox Sensor v2
sensing letters in your letterbox via two infrared proximity sensors (HSDL9100),
sending out status via lora / lorawan, using RAK3172 (based on STM32WLE5CC) CPU/RF, powered by a CR2032 battery.

![Lorawan letterbox sensor v2](https://github.com/hierle/letterbox-sensor-v2/blob/main/misc/letterbox-sensor-v2.png?raw=true)<br>
![Lorawan letterbox sensor v2](https://github.com/hierle/letterbox-sensor-v2/blob/main/misc/letterbox-sensor-v2-w.png?raw=true)

This project was inspired by the heise ct briefkasten sensor project<br>
https://www.heise.de/ratgeber/IoT-Netz-LoRaWAN-Briefkastensensor-mit-hoher-Reichweite-selber-bauen-4417179.html<br>
https://github.com/jamct/radio-mailbox<br>
and is a direct successor of version 1: https://github.com/hierle/letterbox-sensor


Contents:
- ./PCB/ :                  kicad PCB files
- ./letterbox-sensor-v2/ :  lorawan letterbox sensor sketch
- ./misc/ :                 payload decoder, sample http integration cgi

An extended web ui by Peter is available here: https://github.com/pbiering/letterbox-sensor-web

Compiled and flashed with ArduinoIDE with RAKWireless STM32 extensions


Features:
- using RAKWireless RAK3172 based on STM32WLE5CC
- deep sleep at (measured) 5uA including battery voltage divider and tantalium caps
- two proximity sensors HSDL9100
- USB-C interface for flashing and serial monitor, via FT230XS-R USB-UART
- onboard 868MHz PCB antenna after TI document http://www.ti.com/lit/an/swra227e/swra227e.pdf
- lorawan activation either ABP or OTAA
- battery voltage measurment thru voltage divider (2x 10MOhm)


Changes to v1:
- replaced ATTiny and RFM95 with RAK3172, much more CPU power and memory, much simpler code
- support for OTAA lorawan activation
- much simpler software stack, using ArduinoIDE with RAKWireless STM32 extensions and API usage
- added USB-C interface, no more special programming interface needed
- added second HSDL9100 for better physical letterbox coverage
- added two (optional) supporting tantalium caps for extended battery life
- removed temperature, as not easily avail on STM32WLE5CC / RAK API
- removed trimmers


BOM (digikey.de):

| Count | Part number        | Description             | Usage                    |
|-------|--------------------|-------------------------|--------------------------|
| 1     | RAK3172-8-SM-NI    | CPU/RF-Lora  NO IPEX!   | CPU/RF module            |
| 2     | CS1213AGF260       | Tactile switch          | Boot/Reset               |
| 2     | HSDL-9100-021/024  | Proximity Sensor        | Proximity sensors        |
| 2     | CRGCQ1206F33R      | RES 33 Ohm 1206         | Proximity sens LED       | 
| 2     | CRGCQ1206F1M0      | RES 1M Ohm 1206         | Proximity sens diode     |
| 1     | BU2032SM-FH-GTR    | CR2032 COIN CELL HOLDER | Battery                  |
| 2     | TAJE477M010RNJ     | CAP tant 470u           | Battery supporting caps  |
| 1     | LQG18HN1N8S00D     | IND 1.8NH 0603          | Antenna matching         |
| 1     | GRM1885C1H2R7CA01D | CAP 2.7PF 0603          | Antenna matching         |
| 3     | GRM188R72A104KA35D | CAP CER 0.1u 0603       | Suppressor caps          |
| 2     | GRM188D71A106KA73D | CAP CER 10u 0603        | Suppressor caps          |
| 1     | RAHUC31AUTR        | USB-C connector         | USB                      |
| 1     | FT230XS-R          | FTDI USB Serial UART    | USB UART                 |
| 1     | MI0805K601R-10     | Ferrite bead 0805       | USB power                |
| 2     | AC1206FR-075K1L    | RES 5.1k Ohm 1206       | USB power sens           |
| 2     | CRGCQ1206F22R      | RES 22 Ohm 1206         | USB lines                |
| 1     | GRM188R72A104KA35D | DIP switch              | Power switch USB/Battery |
| 2     | AA3528LSECKT/J4    | LED orange              | Status/USB status        |
| 2     | CRGCQ1206F220R     | RES 220 Ohm 1206        | Status LEDs              |
| 2     | RC1206FR-0710ML    | RES 10M Ohm 1206        | Battery voltage measure  |


RAK3172 documentation:

https://docs.rakwireless.com/Product-Categories/WisDuo/RAK3172-Module/Overview/


How to compile:

Follow the instructions on<br>
https://docs.rakwireless.com/Product-Categories/WisDuo/RAK3172-Module/Quickstart/<br>
in short:<br>
- install ArduinoIDE from arduino.cc
- add the following URL to the boards manager URLs section in preferences:
https://raw.githubusercontent.com/RAKWireless/RAKwireless-Arduino-BSP-Index/main/package_rakwireless.com_rui_index.json
- install the "RAKwireless RUI STM32 Boards" from the Boards Manager
- select "WisDuo RAK3172 Evaluation Board" board
- connect the board, make sure it's powered on (DIP switch)
- select the corresponding serial port
- open the sketch folder
- adjust config.h and lorawan.h
- compile and flash


For configuration adjust config.h and lorawan.h

config.h:
- PERIOD: the time intervall you want the sensor to send it's data, in minutes, default is 30 minutes
- THRESHOLD: the threshold for the sensor readings (empty/full), can later be "overwritten" in e.g. HTML integration
- USE_OTAA: if true, OTAA will be used instead of ABP, so we do not have to worry about frame counters after reset / battery change
- DATARATE and TXPOWER: adjust to your needs, your packets should be reliably received, w/o wasting to much power
- see also the comments for each entry in config.h

lorawan.h:
- set your lorawan keys for either ABP or OTAA
- OTAA_APPEUI or JOINEUI is only needed to select a specific JOIN server, in private networks can be set to any value


Additional notes:
- you will need a *DATA* USB-C cable, a charging only cable will not work, as it's missing the data pins ;-)
- the DIP switch allows to power the device directly from USB, but when using the battery in production environment (letterbox), it should be switched off, not to waist power on the FT230 USB-UART chip


Letterbox sensor v2 lorawan payload package byte structure:

|byte|    0     |  1   2 | 3   4 | 5   6 |    7    |
|----|----------|--------|-------|-------|---------|
|    |0x00=empty|battery |sensor1|sensor2|threshold|
|    |0xFF=full |voltage |raw    |raw    |         |


Todos:
- read battery voltage and temperature from STM32
- see sections 3.18.1 and 3.18.3 in: https://www.st.com/resource/en/datasheet/stm32wle5c8.pdf


Have fun!

