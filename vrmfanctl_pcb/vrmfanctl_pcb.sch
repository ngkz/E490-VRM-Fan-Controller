EESchema Schematic File Version 4
LIBS:vrmfanctl_pcb-cache
EELAYER 26 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L MCU_Microchip_ATtiny:ATtiny85-20PU U1
U 1 1 5D2BE157
P 3950 1950
F 0 "U1" H 3421 1996 50  0000 R CNN
F 1 "ATtiny85-20PU" H 3421 1905 50  0000 R CNN
F 2 "Package_DIP:DIP-8_W7.62mm" H 3950 1950 50  0001 C CIN
F 3 "http://ww1.microchip.com/downloads/en/DeviceDoc/atmel-2586-avr-8-bit-microcontroller-attiny25-attiny45-attiny85_datasheet.pdf" H 3950 1950 50  0001 C CNN
	1    3950 1950
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x15 J1
U 1 1 5D2D5518
P 950 1900
F 0 "J1" H 868 975 50  0000 C CNN
F 1 "SATA Power Male" H 868 1066 50  0000 C CNN
F 2 "MyFootprint:SATA_Power_PCBEdge" H 950 1900 50  0001 C CNN
F 3 "~" H 950 1900 50  0001 C CNN
	1    950  1900
	-1   0    0    1   
$EndComp
$Comp
L Device:C_Small C1
U 1 1 5D2DA103
P 2500 1900
F 0 "C1" H 2592 1946 50  0000 L CNN
F 1 "100n" H 2592 1855 50  0000 L CNN
F 2 "Capacitor_THT:C_Disc_D5.0mm_W2.5mm_P2.50mm" H 2500 1900 50  0001 C CNN
F 3 "~" H 2500 1900 50  0001 C CNN
	1    2500 1900
	1    0    0    -1  
$EndComp
Wire Wire Line
	1150 2000 1400 2000
Wire Wire Line
	1400 2000 1400 1900
Wire Wire Line
	1400 1800 1150 1800
Wire Wire Line
	1150 1900 1400 1900
Connection ~ 1400 1900
Wire Wire Line
	1400 1900 1400 1800
Wire Wire Line
	1150 2200 1400 2200
Wire Wire Line
	1400 2300 1400 2200
Wire Wire Line
	1150 2300 1400 2300
Connection ~ 1400 2200
Wire Wire Line
	1150 2100 1400 2100
Wire Wire Line
	1400 2100 1400 2200
Text Label 1150 1800 0    50   ~ 0
5V
Text Label 1150 2100 0    50   ~ 0
GND
Text Label 1150 1900 0    50   ~ 0
5V
Text Label 1150 2000 0    50   ~ 0
5V
Text Label 1150 2200 0    50   ~ 0
GND
Text Label 1150 2300 0    50   ~ 0
GND
Text Notes 50   600  0    50   ~ 0
ThinkPad E490's SATA power connector only has 5V pins and four of GND pins.
Wire Wire Line
	3950 2550 3950 2650
NoConn ~ 1150 1200
NoConn ~ 1150 1300
NoConn ~ 1150 1400
NoConn ~ 1150 1500
NoConn ~ 1150 1600
NoConn ~ 1150 2400
NoConn ~ 1150 2500
NoConn ~ 1150 2600
NoConn ~ 1150 1700
$Comp
L power:PWR_FLAG #FLG01
U 1 1 5D2F5126
P 1100 3150
F 0 "#FLG01" H 1100 3225 50  0001 C CNN
F 1 "PWR_FLAG" H 1100 3323 50  0000 C CNN
F 2 "" H 1100 3150 50  0001 C CNN
F 3 "~" H 1100 3150 50  0001 C CNN
	1    1100 3150
	1    0    0    -1  
$EndComp
$Comp
L power:VCC #PWR01
U 1 1 5D2F56B3
P 1100 3200
F 0 "#PWR01" H 1100 3050 50  0001 C CNN
F 1 "VCC" H 1118 3373 50  0000 C CNN
F 2 "" H 1100 3200 50  0001 C CNN
F 3 "" H 1100 3200 50  0001 C CNN
	1    1100 3200
	-1   0    0    1   
$EndComp
$Comp
L power:PWR_FLAG #FLG02
U 1 1 5D2F5DC2
P 1550 3150
F 0 "#FLG02" H 1550 3225 50  0001 C CNN
F 1 "PWR_FLAG" H 1550 3323 50  0000 C CNN
F 2 "" H 1550 3150 50  0001 C CNN
F 3 "~" H 1550 3150 50  0001 C CNN
	1    1550 3150
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR02
U 1 1 5D2F6399
P 1550 3200
F 0 "#PWR02" H 1550 2950 50  0001 C CNN
F 1 "GND" H 1555 3027 50  0000 C CNN
F 2 "" H 1550 3200 50  0001 C CNN
F 3 "" H 1550 3200 50  0001 C CNN
	1    1550 3200
	1    0    0    -1  
$EndComp
Wire Wire Line
	1550 3200 1550 3150
Wire Wire Line
	1100 3200 1100 3150
Connection ~ 1400 2300
Text Label 4600 1950 0    50   ~ 0
FG
Text Label 4600 1850 0    50   ~ 0
SCL
Text Label 4600 1650 0    50   ~ 0
SDA
NoConn ~ 4550 2150
$Comp
L power:GND #PWR0101
U 1 1 5D3CB935
P 2500 2850
F 0 "#PWR0101" H 2500 2600 50  0001 C CNN
F 1 "GND" H 2505 2677 50  0000 C CNN
F 2 "" H 2500 2850 50  0001 C CNN
F 3 "" H 2500 2850 50  0001 C CNN
	1    2500 2850
	1    0    0    -1  
$EndComp
$Comp
L power:VCC #PWR0102
U 1 1 5D3CC093
P 2500 1050
F 0 "#PWR0102" H 2500 900 50  0001 C CNN
F 1 "VCC" H 2517 1223 50  0000 C CNN
F 2 "" H 2500 1050 50  0001 C CNN
F 3 "" H 2500 1050 50  0001 C CNN
	1    2500 1050
	1    0    0    -1  
$EndComp
Connection ~ 1400 1800
$Comp
L Connector_Generic:Conn_01x04 J4
U 1 1 5D2D1DFB
P 5350 2050
F 0 "J4" H 5300 1700 50  0000 L CNN
F 1 "Pin header L-shape female" H 5300 1600 50  0000 L CNN
F 2 "Connector_PinSocket_2.54mm:PinSocket_1x04_P2.54mm_Horizontal" H 5350 2050 50  0001 C CNN
F 3 "~" H 5350 2050 50  0001 C CNN
	1    5350 2050
	1    0    0    -1  
$EndComp
Text Notes 5450 2000 0    50   ~ 0
PWM(BLUE)
Text Notes 5450 2100 0    50   ~ 0
FOO(YELLOW)
Text Notes 5450 2200 0    50   ~ 0
V-(BLACK)
Text Notes 5450 2300 0    50   ~ 0
V+(RED)
Text Notes 5450 1900 0    50   ~ 0
To Fan (KDB0305HA3-CD7)
Text Notes 5300 1100 0    50   ~ 0
FG is open-drain. enable internal pull-up.
Text Notes 5450 1500 0    50   ~ 0
To SMBus  DDR4 SO-DIMM Pin 254(SDA), Pin 253(SCL)
Wire Wire Line
	4550 1650 5150 1650
$Comp
L Connector_Generic:Conn_01x02 J3
U 1 1 5D2D4A68
P 5350 1650
F 0 "J3" H 5450 1650 50  0000 L CNN
F 1 "Pin header L-shape male" H 5450 1550 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Horizontal" H 5350 1650 50  0001 C CNN
F 3 "~" H 5350 1650 50  0001 C CNN
	1    5350 1650
	1    0    0    -1  
$EndComp
Text Label 4600 1750 0    50   ~ 0
PWM
NoConn ~ 4550 2050
Wire Wire Line
	4850 2150 5150 2150
Wire Wire Line
	5000 1850 5000 1750
Wire Wire Line
	5000 1750 5150 1750
Wire Wire Line
	4550 1850 5000 1850
Wire Wire Line
	4850 2650 3950 2650
Wire Wire Line
	4850 2150 4850 2650
Connection ~ 3950 2650
Wire Wire Line
	2500 2650 2500 2850
Wire Wire Line
	3950 2650 2500 2650
Wire Wire Line
	1400 2300 1400 2650
Wire Wire Line
	2500 2000 2500 2650
Connection ~ 2500 2650
Wire Wire Line
	3950 1200 3950 1350
$Comp
L Connector_Generic:Conn_01x01 J2
U 1 1 5D50B05C
P 1900 1200
F 0 "J2" H 1820 975 50  0000 C CNN
F 1 "Pin header L-shape male" H 1820 1066 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x01_P2.54mm_Horizontal" H 1900 1200 50  0001 C CNN
F 3 "~" H 1900 1200 50  0001 C CNN
	1    1900 1200
	-1   0    0    1   
$EndComp
Wire Wire Line
	2500 1050 2500 1200
Connection ~ 2500 1200
Wire Wire Line
	2500 1200 2500 1800
Text Label 2100 1200 0    50   ~ 0
3V3
Wire Wire Line
	2100 1200 2500 1200
Wire Wire Line
	2500 1200 3950 1200
Wire Wire Line
	4950 750  4950 2250
Wire Wire Line
	1400 750  1400 1800
Wire Wire Line
	4950 2250 5150 2250
Wire Wire Line
	4550 1950 4850 1950
Wire Wire Line
	4850 1950 4850 2050
Wire Wire Line
	4850 2050 5150 2050
Wire Wire Line
	4900 1750 4900 1950
Wire Wire Line
	4900 1950 5150 1950
Wire Wire Line
	4550 1750 4900 1750
Wire Wire Line
	1400 750  4950 750 
Wire Wire Line
	1400 2650 2500 2650
$EndSCHEMATC
