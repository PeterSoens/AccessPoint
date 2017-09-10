# AccessPoint
This is a guide to creating a home security checkpoint utilizing internet of things based devices.  When an RFID tag is scanned by the acces point, the response is compared to a database of users stored on a Raspberry Pi.  If the card is among the allowed users, then the person will be granted access, otherwise they will not.  A picture is also taken and stored by the Raspberry Pi everytime a tag is read.

## Equipment

I used the following parts for this project:

## Raspberry Pi 3

I used two for this project.
You can buy them [here](https://www.amazon.com/Raspberry-Model-1-2GHz-64-bit-quad-core/dp/B01CD5VC92/ref=sr_1_3?s=pc&ie=UTF8&qid=1505071457&sr=1-3&keywords=raspberry+pi+3)

## Raspberry Pi Camera

I chose to buy the one without the installed Infrared Light filter.  This allows for better lowlight images.  You can find the one I bought [here](https://www.amazon.com/LANDZO-Raspberry-Pi-Camera-Module/dp/B074JZ4KN2/ref=sr_1_1?ie=UTF8&qid=1505071809&sr=8-1-spons&keywords=raspberry+pi+camera+v2&psc=1)

You will also need two power cables, one for each Pi.  You can buy them [here](https://www.amazon.com/TOOGOO-micro-USB-Charger-Adapter-Raspberry/dp/B071VD251G/ref=sr_1_7?ie=UTF8&qid=1505072053&sr=8-7&keywords=r+pi+power+cable)

## Touchscreen for Raspberry Pi

I bought [this](https://www.amazon.com/Raspberry-Pi-7-Touchscreen-Display/dp/B0153R2A9I/ref=sr_1_4?s=electronics&ie=UTF8&qid=1505072109&sr=1-4&keywords=raspberry+pi+touchscreen) one.

## Mifare RC522 RF Scanner and tags

You can buy one [here](https://www.amazon.com/Gowoops-RFID-Kit-Arduino-Raspberry/dp/B01KFM0XNG/ref=sr_1_fkmr0_1?s=electronics&ie=UTF8&qid=1505071575&sr=1-1-fkmr0&keywords=rfid+r+pi)

## Wemos D1 R1

I bought mine off of aliexpress [here](https://www.aliexpress.com/item/Upgraded-WeMos-D1-R2-WiFi-UNO-Development-Board-Based-ESP8266/32706199462.html?spm=2114.search0104.3.34.ab9MUf&ws_ab_test=searchweb0_0,searchweb201602_3_5460015_10152_10065_10151_10130_10068_5560015_5470015_10307_10137_10060_10155_10154_10056_10055_10054_10059_5490015_100031_10099_10338_5380015_10103_10102_440_10052_10053_10107_10050_10142_10051_10324_10325_10326_10084_513_10083_10080_10082_10081_10178_10110_10111_10112_10113_10114_143_10312_10313_10314_5570015_10078_10079_10073_5550015,searchweb201603_21,ppcSwitch_3&btsid=78133445-0218-452b-af48-84799fbef45a&algo_expid=6a994604-33a0-4dbd-8ae5-2999f371ae2b-3&algo_pvid=6a994604-33a0-4dbd-8ae5-2999f371ae2b)

## Battery and cable

The battery is used to power the wemos and RFID Reader.  You can find one [here](https://www.amazon.com/eeco-Portable-10000mAh-External-Technology/dp/B071GL2G5M/ref=sr_1_2?ie=UTF8&qid=1505072342&sr=8-2-spons&keywords=phone+battery&psc=1)

To charge the battery you will need a [usb wall charger](https://www.amazon.com/Charger-Vifigen-2-Pack-Universal-Samsung/dp/B01NBM85SX/ref=sr_1_3?ie=UTF8&qid=1505072455&sr=8-3&keywords=usb+plug) and usb to [usb-micro cable](https://www.amazon.com/StarTech-1-Feet-Mini-USB-Cable/dp/B002L5U7N2/ref=sr_1_6?s=electronics&ie=UTF8&qid=1505072291&sr=1-6&keywords=usb+to+mini)  

# Procedure

The information is sent from the microcontroller to the Rasbperry Pi using MQTT (Message Queue Telemetry Transport) protocol. And so one of the Raspberry Pi's (the one not connected to the touchscreen) will need to have an MQTT broker installed. I chose to use [Mosquitto](https://mosquitto.org/).
