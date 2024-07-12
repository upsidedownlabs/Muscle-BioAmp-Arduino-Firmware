# Muscle-BioAmp-Firmware
Firmware for Muscle BioAmp hardware from Upside Down Labs

| No. | Program| Description |
| ---- | ---- | ---- |
|1 | [FixedSampling](1_FixedSampling)| Sample from ADC at a fixed rate for easy processing of signal.|
|2 | [EMGFilter](2_EMGFilter)| A 74.5 - 149.5 Hz band pass filter sketch for clean Electromyography.|
|3 | [EMGEnvelop](3_EMGEnvelop)| EMG signal envelop detection for robotics and biomedical applications.|
|4 | [ClawController](4_ClawController)| Servo Claw Controller.|
|5 | [ServoControl](5_ServoControl)| Controlling the Servo Motors with EMG signals.|
|6 | [LEDBarGraph](6_LEDBarGraph)| LED bar graph representing EMG Amplitude.|
|7 | [MuscleStrengthGame](7_MuscleStrengthGame)| Measure your muscle strength. |
|8 | [EMGScrolling](8_EMGScrolling)| Scrolling through screen using hands. |
|9 | [2Channel_LCDBarGraph](9_2Channel_LCDBarGraph)| LCD bar graph representing EMG Amplitude using 2 Channel |


Compatibility of various boards with different sensors
<table>
    <thead>
        <tr>
            <th>No.</th>
            <th>Development Board</th>
             <th>Maximum ADC Resolution</th>
            <th>Sensor</th>
            <th>Compatibility</th>
        </tr>
    </thead>
    <tbody>
        <tr>
            <td rowspan="6">1</td>
            <td align= "center" rowspan="6">Arduino UNO R3<br>&<br>Maker UNO</td>
            <td align = "center" rowspan="6">10 bits</td>
            <td>Bioamp EXG Pill</td>
            <td align = "center" >✅</td>
        </tr>
        <tr>
            <td>Muscle Bioamp Candy</td>
            <td align = "center" >✅</td>
        </tr>
        <tr>
            <td>Muscle Bioamp Patchy</td>
            <td align = "center" >✅</td>
        </tr>
        <tr>
            <td>Muscle Bioamp Blip</td>
            <td align = "center" >✅</td>
        </tr>
        <tr>
            <td>Muscle Bioamp Biscute</td>
            <td align = "center" >✅</td>
        </tr>
        <tr>
            <td>Muscle Bioamp Shield</td>
            <td align = "center" >✅</td>
        </tr>
        <tr>
            <td rowspan="6">2</td>
            <td align = "center" rowspan="6">Arduino UNO R4 Minima<br>&<br>Arduino UNO R4 Wifi</td>
            <td align = "center"rowspan="6">14 bits</td>
            <td>Bioamp EXG Pill</td>
            <td align = "center" >✅</td>
        </tr>
        <tr>
            <td>Muscle Bioamp Candy</td>
            <td align = "center" >✅</td>
        </tr>
        <tr>
            <td>Muscle Bioamp Patchy</td>
            <td align = "center" >✅</td>
        </tr>
        <tr>
            <td>Muscle Bioamp Blip</td>
            <td align = "center" >✅</td>
        </tr>
        <tr>
            <td>Muscle Bioamp Biscute</td>
            <td align = "center" >✅</td>
        </tr>
        <tr>
            <td>Muscle Bioamp Shield</td>
            <td align = "center" >✅</td>
        </tr>
        <tr>
            <td rowspan="6">3</td>
            <td align = "center" rowspan="6">Raspberry Pi Pico</td>
            <td align = "center" rowspan="6">12 bits</td>
            <td>Bioamp EXG Pill</td>
            <td align = "center" >❌</td>
        </tr>
        <tr>
            <td>Muscle Bioamp Candy</td>
            <td align = "center" >✅</td>
        </tr>
        <tr>
            <td>Muscle Bioamp Patchy</td>
            <td align = "center" >❌</td>
        </tr>
        <tr>
            <td>Muscle Bioamp Blip</td>
            <td align = "center" >❌</td>
        </tr>
        <tr>
            <td>Muscle Bioamp Biscute</td>
            <td align = "center" >✅</td>
        </tr>
        <tr>
            <td>Muscle Bioamp Shield</td>
            <td align = "center" >❌</td>
        </tr>
       <tr>
             <td rowspan="6">4</td>
            <td align= "center" rowspan="6" >ESP32 C3 MINI</td>
           <td align = "center" rowspan="6">12 bits</td>
            <td>Bioamp EXG Pill</td>
            <td align = "center" >❌</td>
        </tr>
        <tr>
            <td>Muscle Bioamp Candy</td>
            <td align = "center" >✅</td>
        </tr>
        <tr>
            <td>Muscle Bioamp Patchy</td>
            <td align = "center" >❌</td>
        </tr>
        <tr>
            <td>Muscle Bioamp Blip</td>
            <td align = "center" >❌</td>
        </tr>
        <tr>
            <td>Muscle Bioamp Biscute</td>
            <td align = "center" >✅</td>
        </tr>
        <tr>
            <td>Muscle Bioamp Shield</td>
            <td align = "center" >❌</td>
        </tr>
        <tr>
             <td rowspan="6">5</td>
            <td align= "center" rowspan="6" >Arduino Due</td>
           <td align = "center" rowspan="6">12 bits</td>
            <td>Bioamp EXG Pill</td>
            <td align = "center" >✅</td>
        </tr>
        <tr>
            <td>Muscle Bioamp Candy</td>
            <td align = "center" >✅</td>
        </tr>
        <tr>
            <td>Muscle Bioamp Patchy</td>
            <td align = "center" >✅</td>
        </tr>
        <tr>
            <td>Muscle Bioamp Blip</td>
            <td align = "center" >✅</td>
        </tr>
        <tr>
            <td>Muscle Bioamp Biscute</td>
            <td align = "center" >✅</td>
        </tr>
        <tr>
            <td>Muscle Bioamp Shield</td>
            <td align= "center">:x:</td>
        </tr>
        <tr>
             <td rowspan="6">6</td>
            <td align= "center" rowspan="6" >Arduino Nano</td>
           <td align = "center" rowspan="6">10 bits</td>
            <td>Bioamp EXG Pill</td>
            <td align = "center" >✅</td>
        </tr>
        <tr>
            <td>Muscle Bioamp Candy</td>
            <td align = "center" >✅</td>
        </tr>
        <tr>
            <td>Muscle Bioamp Patchy</td>
            <td align = "center" >✅</td>
        </tr>
        <tr>
            <td>Muscle Bioamp Blip</td>
            <td align = "center" >✅</td>
        </tr>
        <tr>
            <td>Muscle Bioamp Biscute</td>
            <td align = "center" >✅</td>
        </tr>
        <tr>
            <td>Muscle Bioamp Shield</td>
            <td align = "center" >❌</td>
        </tr>
    </tbody>
</table>

**NOTE:** 
1. GREEN Tick (✅) signifies that the development board works on the same voltage level as the sensor, thus it is compatible.
2. RED cross (❌) shows that the development board and sensor work on different voltages, thus they are not directly compatible. 

The issue:<br>
Some of the sensors mentioned above are having a minimum required voltage of 5V. Thus, they will not work properly when their VCC terminal is connected to a development board's VCC that is working on a lower voltage level (For example: 3.3V). 

How to resolve the issue?<br>
It's not recommended but to resolve this issue you can use external 5V supply for sensor, use common ground and create a resistor divider to convert the higher voltage output of the sensor to lower voltage level so as to make it compatible with the development board. This step is crucial to prevent the sensor to harm the analog pins of development board.<br>

![29cd87b6-1133-4b6e-b7ac-5b7ffa1a7d91](https://github.com/Aryan-Prakhar/Muscle-BioAmp-Arduino-Firmware/assets/97614212/1a3765af-205d-463e-9560-d68d43c711c5)
<br>


## Examples

1. **EMG Filter**

    A band pass filter for EMG signals between 74.5 Hz and 149.5 Hz

    <img src="2_EMGFilter/EMGFilter.png" height="300" width="400">
    

2. **EMG Envelop**

    EMG signal envelop detection for robotics and biomedical applications.

    <img src="3_EMGEnvelope/EMGEnvelope.png" height="300" width="400">
