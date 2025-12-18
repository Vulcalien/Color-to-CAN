# Color-to-CAN
This firmware offers a CAN interface for the
[TCS34725]()
color sensor, controlled via *I2C* by a
[STM32L432KC](https://www.st.com/en/microcontrollers-microprocessors/stm32l432kc.html)
microcontroller. The project is built on top of the
[NuttX](https://nuttx.apache.org/) embedded operating system.

## Building
Retrieve all git submodules:

```sh
git submodule update --init --depth=1
```

Run `make ID=<sensor-id>` to compile the firmware. On success, the file
`submodules/nuttx/nuttx.bin` is generated. Flash this file into the
microcontroller by running `make program ID=<sensor-id>` (which uses
OpenOCD) or using another flashing software.

## Usage
The firmware communicates using CAN messages. The CAN ID of these
messages is split in two parts:
| Bits | Meaning      |
| ---- | ------------ |
| 0-4  | Sensor ID    |
| 5-11 | Message Type |

The sensor ID must be in range [0, 31]. If the sensor ID is 0, the
message is interpreted as broadcast, that is, the message is addressed
to all sensors present.

The message type can be one of the following constants. Read the header
file [color2can.h](include/color2can.h) for details on each message
type.
- COLOR2CAN_CONFIG_MASK_ID
- COLOR2CAN_RANGE_MASK_ID
- COLOR2CAN_SAMPLE_MASK_ID

The sensor needs to be configured at least once. To do so, send a
'config' message. To request a sample, send an empty 'sample' message
with the _RTR_ bit set.

## License
The source code of the application, contained in the `firmware/apps`
directory, is licensed under the GNU General Public License, either
version 3 of the License or any later version.

### Dependencies
This project depends on the following, each licensed under their
respective terms:
- the NuttX operating system, included as a git submodule
- files in the `firmware/config` directory
