# SmartBox_firmware

The main purpose of this project is to create an IoT device with simple functionalities, such as LED control, VBAT measurement, and more. Additionally, all information will be logged via Bluetooth Low Energy. The project is being developed using the Nordic Thingy:53 IoT prototyping platform with the nrf5340 microcontroller, containing two Cortex-M33 cores. The application core is proposed for functional features, and the network core is proposed for Bluetooth communication.

![image](https://github.com/user-attachments/assets/e0ee9319-ed88-4ec2-8973-d4d5af46eb1b)

# TODO list
- [x] BLE
- [x] RGB diode, PWM
- [x] Battery condition, ADC
- [x] Interprocessor communication, IPC

# Description of functionalities

## RGB LED

Lighting of the RGB LED can be adjusted by sending data via Bluetooth. The images show an example of usage with the nRF Connect application on a mobile device.

<img src="https://github.com/user-attachments/assets/561d11cd-e155-44d9-a9d3-3de80ee77682" height="435"/>
<img src="https://github.com/user-attachments/assets/2273c3ef-3071-43b8-9727-ddb4c1cf7e64" height="435"/>
<img src="https://github.com/user-attachments/assets/27eaaca5-a704-42cc-8819-a9084d6740be" height="435"/>

## Battery condition

Battery condition as a percentage can be received via Bluetooth.

<img src="https://github.com/user-attachments/assets/a32d0135-d1d2-458c-8802-81555ce12514" height="435"/>
