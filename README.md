# CuissonTuning
Tuning my cooktop, because I can.

Using a [Proline IH234P](https://reunion.darty-dom.com/p/proline-ih234p-noir#), a 220VAC->5VDC power supply, 2 220V relays, RGB LED strips, and an AVR microcontroller (current target is an AtMega328 using an Arduino Uno, but I want to deploy the final software on an AtTiny85).

The AVR microcontroller and the relays are powered with the 5V provided by the tactile board of the cooktop. The 220VAC->5VDC power supply is powered on with the input 220V through the relays, to generate the 5V for LED strips only when they are used.

The AVR microcontroller spies the communication between the cooktop tactile board and power board, to detect:
- General power on/off
- Cooking power on the 2 sides (front and back)
- Presence/absence of pane above each side

To do so, the communication protocol between the 2 cooktop boards has been reverse engineered:
- Physically, its is a 1 wire bi-directionnal communication with binary levels between 0V and 5V. Default state is high (5V).
- One of the 2 boards acts as a master, and the other board is a slave (I think the master is the power board, but in fact I don't care to know which is which)
- Data is exchanged between the 2 sides at 19200 bits/s (~52µs bit period)
- When the cooktop is on, the master iniates communication by sending a ~10ms low pulse. Then, the line is released to the slave which sends 5 consecutive low/high pulses at 19200 bit/s. Then the slave sends 1 byte like an UART protocol, with 1 start low bit, 8 data bits (MSB first), and 1 high stop bit. The line is then given back to the master which eventually sends a response (also using UART protocol), containing 1 or multiple consecutive bytes. The size of the master response depends on the byte sent by the slave. MAster responses seem to be less than 16 bytes.
- The master iniates a communication slot every 20ms
- It seems the slave usually loops its byte command every 11 messages, thus a complete communication loop is executed every 220ms.
- However, sometimes the slave sends additional byte commands, thus increasing the communication period loop above 220ms.
- When the slave sends the 0x80 command byte, the master response contains a flag indicating if the cooktop is on or off.
- When the slave sends the 0x20 command byte, the master response contains the front side cooking power. Same with the 0x1F command byte, but for the back side.
- When the slave sends the 0x6A command byte, the master response contains other information about the front side, including some status bits used to detect presence/absence of a pane. Same with the 0xE9 command byte, but for the back side.

Example of several exchanges between the tactile and power boards of the cooktop :
![trame](https://github.com/user-attachments/assets/c92243a6-a673-4346-98fb-9915d4477c33)


Same example of the 9 consecutive exchanges but all synchronized with their initial reset pulses:
![trames](https://github.com/user-attachments/assets/0ebcae87-d76c-4283-83ad-bdb98dd060fd)


# Demo
https://github.com/user-attachments/assets/2e445db2-d366-4176-8674-0e94bcc6eb2f
