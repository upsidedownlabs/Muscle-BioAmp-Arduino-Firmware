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
|7 | [MuscleStrengthGame](7_MuscleStrengthGame)| Measure your muscle strength |

Compatibility of various boards with different sensors
<table>
    <thead>
        <tr>
            <th>No.</th>
            <th>Development Board</th>
            <th>Sensor</th>
            <th>Compatibility</th>
        </tr>
    </thead>
    <tbody>
        <tr>
            <td rowspan="6">1</td>
            <td rowspan="6">Arduino UNO R3<br>Maker UNO</td>
            <td>Bioamp EXG Pill</td>
            <td>✅</td>
        </tr>
        <tr>
            <td>Muscle Bioamp Candy</td>
            <td>✅</td>
        </tr>
        <tr>
            <td>Muscle Bioamp Patchy</td>
            <td>✅</td>
        </tr>
        <tr>
            <td>Muscle Bioamp Blip</td>
            <td>✅</td>
        </tr>
        <tr>
            <td>Muscle Bioamp Biscute</td>
            <td>✅</td>
        </tr>
        <tr>
            <td>Muscle Bioamp Shield</td>
            <td>✅</td>
        </tr>
        <tr>
            <td rowspan="6">2</td>
            <td rowspan="6">Arduino UNO R4 Minima<br>Arduino UNO R4 Wifi</td>
            <td>Bioamp EXG Pill</td>
            <td>✅</td>
        </tr>
        <tr>
            <td>Muscle Bioamp Candy</td>
            <td>✅</td>
        </tr>
        <tr>
            <td>Muscle Bioamp Patchy</td>
            <td>✅</td>
        </tr>
        <tr>
            <td>Muscle Bioamp Blip</td>
            <td>✅</td>
        </tr>
        <tr>
            <td>Muscle Bioamp Biscute</td>
            <td>✅</td>
        </tr>
        <tr>
            <td>Muscle Bioamp Shield</td>
            <td>✅</td>
        </tr>
        <tr>
            <td rowspan="6">3</td>
            <td rowspan="6">Raspberry Pi Pico</td>
            <td>Bioamp EXG Pill</td>
            <td>✅</td>
        </tr>
        <tr>
            <td>Muscle Bioamp Candy</td>
            <td>✅</td>
        </tr>
        <tr>
            <td>Muscle Bioamp Patchy</td>
            <td>❌</td>
        </tr>
        <tr>
            <td>Muscle Bioamp Blip</td>
            <td>❌</td>
        </tr>
        <tr>
            <td>Muscle Bioamp Biscute</td>
            <td>✅</td>
        </tr>
        <tr>
            <td>Muscle Bioamp Shield</td>
            <td>❌</td>
        </tr>
    </tbody>
</table>

**NOTE:**  GREEN Tick (✅) signifies that the board works on the same voltage level as the sensor thus it is compatible and tested with the sensor. RED cross :x: shows that the board and sensor work on different voltages thus they are not directly compatible. The sensor will not work when it's VCC is connected to a development board VCC that is working on lower voltage due to the minimum required voltage (5V) for the sensor to work properly. It's not recommended but to resolve this issue you can use external 5V supply for sensor, use common ground, and create a resistors divider to convert higher voltage output of the sensor to lower voltage level of development board so the sensor doesn't harm the development board's analog pins.

**NOTE:**  GREEN Tick (✅) signifies that the board works on the same voltage level as the sensor , thus it is compatible and tested with the sensor (for better compatibilty better a board and a sensor, their required or working voltage level must be same) . RED cross :x: shows that the board and sensor work on different voltages thus they are not directly compatible. The senors have a minimum required voltage of 5V. Thus, they will not work properly when their VCC terminal is connected to a development board's VCC that is working on a lower voltage level.  It's not recommended but to resolve this issue you can use external 5V supply for sensor, use common ground and create a resistor divider to convert the higher voltage output of the sensor to lower voltage level of development board so that the sensor doesn't harm the development board's analog pins.


## Examples

1. **EMG Filter**

    A band pass filter for EMG signals between 74.5 Hz and 149.5 Hz

    <img src="2_EMGFilter/EMGFilter.png" height="300" width="400">
    

2. **EMG Envelop**

    EMG signal envelop detection for robotics and biomedical applications.

    <img src="3_EMGEnvelope/EMGEnvelope.png" height="300" width="400">
