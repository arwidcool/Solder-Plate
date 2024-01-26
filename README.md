# Solder Reflux Hotplate Project

This repository contains the source code and design files for a Solder Reflux Hotplate, an essential tool for soldering SMD components. The project is built around the ATmega4809 microcontroller and includes a custom PCB design. It is programmed using PlatformIO, an advanced ecosystem for IoT development.

## Features

- Precise temperature control for soldering SMD components. with support of **up to 6 thermistors**.
- Custom PCB design tailored for efficient heat distribution.
- User-friendly interface for easy operation.
- usb programming
- wifi via esp8266-12f module (with wifi version).
- PlatformIO integration for streamlined development.
- Based on Reflow Hotplate by DerSpatz with some improvements

## Getting Started

### Prerequisites

- PlatformIO Core or PlatformIO IDE.
- ATmega4809 microcontroller.
- Basic soldering tools and electronic assembly skills.
- Access to a PCB manufacturing service.

### Hardware Setup

1. **PCB Fabrication:** Fabricate the PCB using the provided design files.
2. **Component Assembly:** Solder the components onto the PCB as per the schematic.
3. **Power Supply Connection:** Connect a suitable power supply.

### Preparing the ATmega4809

Before programming the ATmega4809, it needs to be flashed with a default bootloader.

1. **Bootloader Flashing:**
   - U can use a spare arduino for the initial updi bootloader flash

2. **MicroUSB Programming:**
   - After flashing the bootloader, the ATmega4809 can be programmed via the microUSB connection.

### Software Installation

1. **Clone the Repository:**
   ```bash
   git clone https://github.com/yourusername/solder-reflux-hotplate.git
   cd solder-reflux-hotplate
2. **Import in vscode**


### Usage

Power On: Connect the hotplate to the power source.
Set Temperature: Use the interface to set the desired temperature.
Start Soldering: Place your PCB and components on the hotplate.

### Customization

Feel free to modify the firmware and PCB design to suit your specific needs.

### Contributing
Contributions are welcome. Please follow standard procedures for contributing to open-source projects.

