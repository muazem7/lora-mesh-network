# LoRa Mesh Network

## Overview

This project implements a mesh network using LoRa technology, designed for reliable device-to-device communication in environments with limited connectivity. It features message handling, device management, and integration with external services.

## Features

- LoRa-based mesh networking
- Device connection and management
- Message sending, receiving, and acknowledgment
- Integration with Firebase for device data storage
- Modular code structure for easy extension

## Getting Started

### Prerequisites

- Arduino-compatible hardware with LoRa support
- PlatformIO or Arduino IDE
- Required libraries: `LoRa`, `ArduinoJson`, and others as specified in the code

### Installation

1. Clone the repository:
   ```
   git clone https://github.com/muazem7/lora-mesh-network
   ```
2. Install dependencies using PlatformIO or Arduino IDE.
3. Configure your board and LoRa pins in `board_pins.h`.

### Usage

- Build and upload the firmware to your device.
- Devices will automatically connect and form a mesh network.
- Messages are sent and received according to device type and connectivity.

## Project Structure

- `src/` - Main source code & header files

## License

This project is licensed under the BSD 2-Clause License. See the [LICENSE](LICENSE) file for details.
