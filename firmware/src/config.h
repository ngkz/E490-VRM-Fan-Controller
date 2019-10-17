/*
 *  ThinkPad E490 VRM Cooling Mod Controller Firmware
 *  Copyright (C) 2019  Kazutoshi Noguchi
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef __CONFIG_H_
#define __CONFIG_H_

#define BETA               3346       //thermistor B-Constant or Beta value
#define T0                 22.5       //[℃]
#define R0                 10400      //thermistor resistance at T0[℃]
#define R1                 4620       //actual R1 resistance [Ω]
#define ADC_GAIN_COMP      1.0026672f // ADC gain error compensation
                                      // 4.88V = 1022.5LSB (Vcc=4.88V)
#define ADC_OFFSET_ERROR   1.2239757f // ADC offset error [LSB]
                                      // 8.3mV = 1/2LSB (Vcc=4.93V)

#define T_FAN_STOP 50         //once the fan started, the fan won't stop until heatsink temperature drops below this.
#define T_FAN_START 55        //if heatsink temperature is greater than or equal to this, the fan starts up.
                              //the fan accelarates as temperature rises.
#define T_FAN_FULL_SPEED 70   //if heatsink temperature is greater than or equal to this, the fan will run at full speed (MAX_DUTY).
#define MIN_DUTY 12           //minimal duty that the fan can start from dead stop, 30%
#define MAX_DUTY 39           //maximum duty that the fan allows, 100%


#endif // __CONFIG_H_
