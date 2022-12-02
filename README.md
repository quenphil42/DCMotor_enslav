# DCMotor_enslav
This project is based on a practical training at ENSEA. The project objectives are to servo by speed and current a DC Motor with an STM32.

We will be using the NUCLEO-G474RE board with a custom made shield. The shield is required to control the inverter.
The team used Doxygen as documentation generation software.
User defined values are set via wildcards (ex: int a = 256; ==> int a = BUFFER_SIZE;)
User defined Variables are written with a capital letter on each word for easy identification (ex: int userDefinedVariable;)

## 1. Config 
To start off, we configure the NUCLEO pins to the configuration givven right bellow:
![image](https://user-images.githubusercontent.com/113909661/195544779-ca40b81b-0b3d-45dd-8a04-904a3a217c6b.png)

On the custom made PCB, we need to connect the pins as following:
![image](https://user-images.githubusercontent.com/113909661/205322873-f4ddeada-e1db-4ac6-acd8-8282e608288a.png)

Remember to plug in the DC adapter for the custom board. 

## 2. UART setup for character echoing

We setup the UART2 to give the user an echo of what key is pressed.


