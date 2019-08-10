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
P 3100 1950
F 0 "U1" H 2571 1996 50  0000 R CNN
F 1 "ATtiny85-20PU" H 2571 1905 50  0000 R CNN
F 2 "Package_DIP:DIP-8_W7.62mm" H 3100 1950 50  0001 C CIN
F 3 "http://ww1.microchip.com/downloads/en/DeviceDoc/atmel-2586-avr-8-bit-microcontroller-attiny25-attiny45-attiny85_datasheet.pdf" H 3100 1950 50  0001 C CNN
	1    3100 1950
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
P 1600 2400
F 0 "C1" H 1692 2446 50  0000 L CNN
F 1 "100n" H 1692 2355 50  0000 L CNN
F 2 "Capacitor_THT:C_Disc_D5.0mm_W2.5mm_P2.50mm" H 1600 2400 50  0001 C CNN
F 3 "~" H 1600 2400 50  0001 C CNN
	1    1600 2400
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
Text Notes 750  800  0    50   ~ 0
ThinkPad E490's SATA power connector only has 5V pins and four of GND pins.
Wire Wire Line
	3100 2550 3100 2650
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
Wire Wire Line
	1400 2300 1400 2600
Wire Wire Line
	3100 1300 3100 1350
Text Label 3750 1950 0    50   ~ 0
FG
Text Label 3750 1850 0    50   ~ 0
SCL
Text Label 3750 1650 0    50   ~ 0
SDA
$Comp
L power:GND #PWR04
U 1 1 5D329DC0
P 1500 2650
F 0 "#PWR04" H 1500 2400 50  0001 C CNN
F 1 "GND" H 1505 2477 50  0000 C CNN
F 2 "" H 1500 2650 50  0001 C CNN
F 3 "" H 1500 2650 50  0001 C CNN
	1    1500 2650
	1    0    0    -1  
$EndComp
$Comp
L power:VCC #PWR03
U 1 1 5D32A0F0
P 1850 2150
F 0 "#PWR03" H 1850 2000 50  0001 C CNN
F 1 "VCC" H 1867 2323 50  0000 C CNN
F 2 "" H 1850 2150 50  0001 C CNN
F 3 "" H 1850 2150 50  0001 C CNN
	1    1850 2150
	1    0    0    -1  
$EndComp
$Comp
L Device:D D3
U 1 1 5D392244
P 1600 2000
F 0 "D3" V 1646 1921 50  0000 R CNN
F 1 "D" V 1555 1921 50  0000 R CNN
F 2 "Diode_THT:D_T-1_P5.08mm_Horizontal" H 1600 2000 50  0001 C CNN
F 3 "~" H 1600 2000 50  0001 C CNN
	1    1600 2000
	0    -1   -1   0   
$EndComp
$Comp
L Device:D D2
U 1 1 5D392B20
P 1600 1700
F 0 "D2" V 1646 1621 50  0000 R CNN
F 1 "D" V 1555 1621 50  0000 R CNN
F 2 "Diode_THT:D_T-1_P5.08mm_Horizontal" H 1600 1700 50  0001 C CNN
F 3 "~" H 1600 1700 50  0001 C CNN
	1    1600 1700
	0    -1   -1   0   
$EndComp
$Comp
L Device:D D1
U 1 1 5D392E56
P 1600 1400
F 0 "D1" V 1646 1321 50  0000 R CNN
F 1 "D" V 1555 1321 50  0000 R CNN
F 2 "Diode_THT:D_T-1_P5.08mm_Horizontal" H 1600 1400 50  0001 C CNN
F 3 "~" H 1600 1400 50  0001 C CNN
	1    1600 1400
	0    -1   -1   0   
$EndComp
Wire Wire Line
	3950 1850 3950 1750
Wire Wire Line
	3700 1850 3950 1850
NoConn ~ 3700 2150
Text Notes 1700 2250 0    50   ~ 0
around 3V
$Comp
L power:GND #PWR0101
U 1 1 5D3CB935
P 3100 2650
F 0 "#PWR0101" H 3100 2400 50  0001 C CNN
F 1 "GND" H 3105 2477 50  0000 C CNN
F 2 "" H 3100 2650 50  0001 C CNN
F 3 "" H 3100 2650 50  0001 C CNN
	1    3100 2650
	1    0    0    -1  
$EndComp
$Comp
L power:VCC #PWR0102
U 1 1 5D3CC093
P 3100 1300
F 0 "#PWR0102" H 3100 1150 50  0001 C CNN
F 1 "VCC" H 3117 1473 50  0000 C CNN
F 2 "" H 3100 1300 50  0001 C CNN
F 3 "" H 3100 1300 50  0001 C CNN
	1    3100 1300
	1    0    0    -1  
$EndComp
Wire Wire Line
	1600 1200 1600 1250
Wire Wire Line
	1600 2150 1600 2300
Connection ~ 1600 2150
Wire Wire Line
	1600 2600 1600 2500
Wire Wire Line
	1600 2150 1850 2150
$Comp
L Device:R R1
U 1 1 5D3AD9FE
P 3900 2550
F 0 "R1" H 3830 2504 50  0000 R CNN
F 1 "2M" H 3830 2595 50  0000 R CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P5.08mm_Horizontal" V 3830 2550 50  0001 C CNN
F 3 "~" H 3900 2550 50  0001 C CNN
	1    3900 2550
	-1   0    0    1   
$EndComp
Wire Wire Line
	3900 2700 3900 3050
Wire Wire Line
	3900 3050 3950 3050
Wire Wire Line
	1600 1200 1400 1200
Wire Wire Line
	1400 1200 1400 1800
Connection ~ 1400 1800
$Comp
L Device:Q_NPN_ECB Q1
U 1 1 5D3A6F9D
P 4150 3050
F 0 "Q1" H 4340 3096 50  0000 L CNN
F 1 "?" H 4340 3005 50  0000 L CNN
F 2 "Package_TO_SOT_SMD:SC-59" H 4350 3150 50  0001 C CNN
F 3 "~" H 4150 3050 50  0001 C CNN
	1    4150 3050
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0103
U 1 1 5D3C9261
P 4650 2150
F 0 "#PWR0103" H 4650 1900 50  0001 C CNN
F 1 "GND" H 4655 1977 50  0000 C CNN
F 2 "" H 4650 2150 50  0001 C CNN
F 3 "" H 4650 2150 50  0001 C CNN
	1    4650 2150
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x04 J4
U 1 1 5D2D1DFB
P 4950 2050
F 0 "J4" H 4900 1700 50  0000 L CNN
F 1 "Pin header L-type female" H 4900 1600 50  0000 L CNN
F 2 "Connector_PinSocket_2.54mm:PinSocket_1x04_P2.54mm_Horizontal" H 4950 2050 50  0001 C CNN
F 3 "~" H 4950 2050 50  0001 C CNN
	1    4950 2050
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0104
U 1 1 5D3C8379
P 4250 3250
F 0 "#PWR0104" H 4250 3000 50  0001 C CNN
F 1 "GND" H 4255 3077 50  0000 C CNN
F 2 "" H 4250 3250 50  0001 C CNN
F 3 "" H 4250 3250 50  0001 C CNN
	1    4250 3250
	1    0    0    -1  
$EndComp
Text Label 4400 2400 0    50   ~ 0
5V
Text Notes 5050 2100 0    50   ~ 0
PWM(BLUE)
Text Notes 5050 2000 0    50   ~ 0
FOO(YELLOW)
Text Notes 5050 2200 0    50   ~ 0
V-(BLACK)
Text Notes 5050 2300 0    50   ~ 0
V+(RED)
Text Notes 5050 1900 0    50   ~ 0
To Fan (KDB0305HA3-CD7)
Wire Wire Line
	1400 2600 1500 2600
Wire Wire Line
	1500 2650 1500 2600
Connection ~ 1500 2600
Wire Wire Line
	1500 2600 1600 2600
Text Label 4400 2050 0    50   ~ 0
PWM_5V
Wire Wire Line
	4750 2050 4250 2050
Text Notes 3700 3550 0    50   ~ 0
SOT? hFE=287 CBE Vf=688mV
$Comp
L Device:R R2
U 1 1 5D39A6A0
P 4500 2600
F 0 "R2" H 4570 2646 50  0000 L CNN
F 1 "47K" H 4570 2555 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P5.08mm_Horizontal" V 4430 2600 50  0001 C CNN
F 3 "~" H 4500 2600 50  0001 C CNN
	1    4500 2600
	1    0    0    -1  
$EndComp
Wire Wire Line
	4250 2050 4250 2800
Wire Wire Line
	4500 2800 4250 2800
Connection ~ 4250 2800
Wire Wire Line
	4250 2800 4250 2850
Wire Wire Line
	4400 2400 4500 2400
Wire Wire Line
	4500 2450 4500 2400
Text Notes 4000 1200 0    50   ~ 0
FG is open-drain. enable internal pull-up.
Text Notes 4000 1500 0    50   ~ 0
To SMBus  DDR4 SO-DIMM Pin 254(SDA), Pin 253(SCL)
Wire Wire Line
	3950 1750 4000 1750
Wire Wire Line
	3700 1650 4000 1650
$Comp
L Connector_Generic:Conn_01x02 J3
U 1 1 5D2D4A68
P 4200 1650
F 0 "J3" H 4300 1650 50  0000 L CNN
F 1 "Pin header L-type male" H 4300 1550 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Horizontal" H 4200 1650 50  0001 C CNN
F 3 "~" H 4200 1650 50  0001 C CNN
	1    4200 1650
	1    0    0    -1  
$EndComp
Wire Wire Line
	4500 2750 4500 2800
Wire Wire Line
	4750 2150 4650 2150
Wire Wire Line
	4500 2400 4750 2400
Wire Wire Line
	4750 2400 4750 2250
Connection ~ 4500 2400
Text Label 3750 1750 0    50   ~ 0
PWM#
Wire Wire Line
	3700 1950 4750 1950
Wire Wire Line
	3700 1750 3900 1750
Wire Wire Line
	3900 1750 3900 2400
NoConn ~ 3700 2050
$EndSCHEMATC