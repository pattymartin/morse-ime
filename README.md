# Morse IME

<img align="right" width="25%" src="https://i.imgur.com/dpVOpvP.png">

The Morse IME application is a morse code keyboard for wearable devices running
Tizen OS, such as the Samsung Galaxy Watch.

- [Installation](#installation)
- [Using the Keyboard](#using-the-keyboard)
- [Morse Code Table](#morse-code-table)
- [Troubleshooting](#troubleshooting)

## Installation

To install the application on your smartwatch, you will first need to install
[Tizen Studio](https://developer.tizen.org/ko/development/tizen-studio).

See [this page][1] for instructions on building and running an application with
Tizen Studio. In case of issues, refer to the [Troubleshooting](
#troubleshooting) section below.

[1]: https://developer.tizen.org/ko/development/training/native-application/getting-started/creating-your-first-tizen-wearable-native-application

Once installed, the keyboard can be enabled from Settings > General > Input >
Default keyboard.

## Using the Keyboard

Input can be entered simply by tapping in Morse code on the large button in the
center of the keyboard, or by rotating the bezel on the watch
(counter-clockwise for a dot, clockwise for a dash). The keyboard also
includes buttons for shift, backspace, return, and space, although these can
also be done by entering dots and dashes.

By default, a tap lasting less than 0.25 seconds will be interpreted as a
*dot*, and a tap longer than 0.25 seconds will be interpreted as a *dash*. The
keyboard will consider input to be finished after a delay of 0.5 seconds.

## Morse Code Table

Character | Morse       |Character | Morse       |Character | Morse       |Character | Morse       |
:--------:|:-----------:|:--------:|:-----------:|:--------:|:-----------:|:--------:|:-----------:|
Shift     | **····−·**  |L         | **·−··**    |1         | **·−−−−**   |/         | **−··−·**   |
Space     | **··−−**    |M         | **−−**      |2         | **··−−−**   |(         | **−·−−·**   |
Backspace | **−−−−**    |N         | **−·**      |3         | **···−−**   |)         | **−·−−·−**  |
Return    | **·−·−**    |O         | **−−−**     |4         | **····−**   |&         | **·−···**   |
A         | **·−**      |P         | **·−−·**    |5         | **·····**   |:         | **−−−···**  |
B         | **−···**    |Q         | **−−·−**    |6         | **−····**   |;         | **−·−·−·**  |
C         | **−·−·**    |R         | **·−·**     |7         | **−−···**   |=         | **−···−**   |
D         | **−··**     |S         | **···**     |8         | **−−−··**   |+         | **·−·−·**   |
E         | **·**       |T         | **−**       |9         | **−−−−·**   |-         | **−····−**  |
F         | **··−·**    |U         | **··−**     |0         | **−−−−−**   |_         | **··−−·−**  |
G         | **−−·**     |V         | **···−**    |.         | **·−·−·−**  |"         | **·−··−·**  |
H         | **····**    |W         | **·−−**     |,         | **−−··−−**  |$         | **···−··−** |
I         | **··**      |X         | **−··−**    |?         | **··−−··**  |@         | **·−−·−·**  |
J         | **·−−−**    |Y         | **−·−−**    |'         | **·−−−−·**  |          |             |
K         | **−·−**     |Z         | **−−··**    |!         | **−·−·−−**  |          |             |

## Troubleshooting

If your watch will not connect to Tizen Studio using Tizen's instructions, try
this instead. From the watch settings:
1. Turn Bluetooth **off** (Settings > Connections)
2. Turn Wi-Fi **on** (Settings > Connections)
3. Ensure that the watch and the computer running Tizen Studio are connected to
   the same Wi-Fi network.
4. Turn debugging **on** (Settings > About watch)
5. Turn developer mode **on** (Settings > About watch > Software, tap *Software
   version* 5 times)
6. Restart the watch
7. Connect to the watch from the Device Manager in Tizen Studio
