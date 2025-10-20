# QidiBox Filament Tag Reader/Writer

A Flipper Zero application for reading and writing RFID tags used in the QidiBox filament management system.

## Features

- **Read Tags**: Scan existing QidiBox filament tags to view material type and color
- **Write Tags**: Program new tags with material and color information
- Support for 22 material types (PLA, ABS, PETG, TPU, etc.)
- Support for 24 predefined colors

## Installation

1. Download the latest `qidi_filament_nfc.fap` from the [Releases](https://github.com/alexk42/qidi-filament-nfc-flipper/releases) page
2. Copy the `.fap` file to your Flipper Zero's SD card at `/ext/apps/NFC/`
3. Navigate to Apps → NFC → QidiBox Filament on your Flipper Zero

## Building from Source

This app is designed to be built as part of the Flipper Zero firmware build system.

1. Clone this repository into your Flipper Zero firmware's `applications_user` directory:
   ```bash
   cd /path/to/flipperzero-firmware/applications_user
   git clone https://github.com/alexk42/qidi-filament-nfc-flipper.git
   ```

2. Build the application:
   ```bash
   cd /path/to/flipperzero-firmware
   ./fbt fap_qidi_filament_nfc
   ```

3. The compiled `.fap` file will be in `build/f7-firmware-D/.extapps/`

## Technical Details

Specifications based on the [QIDI RFID Tag Guide](https://wiki.qidi3d.com/en/QIDIBOX/RFID).

- **Tag Type**: MIFARE Classic 1K (FM11RF08S chip)
- **Frequency**: 13.56 MHz
- **Protocol**: ISO/IEC 14443-A
- **Authentication**: Default MIFARE keys (FF FF FF FF FF FF)
- **Data Location**: Sector 1, Block 0 (absolute block 4)
- **Data Format**: 3 bytes
  - Byte 0: Material code
  - Byte 1: Color code
  - Byte 2: Manufacturer code (set to 1)

## Supported Materials

PLA, PLA Matte, PLA Silk, PLA Plus, ABS, ASA, PETG, TPU, PA (Nylon), PC, PVA, HIPS, PP, PLA-CF, PETG-CF, PA-CF, PET-CF, ABS-GF, Wood, and more.

## Supported Colors

White, Black, Gray, Red, Orange, Yellow, Green, Blue, Purple, Brown, Natural, Transparent, Silver, Gold, and more.

## License

MIT License - see [LICENSE](LICENSE) file for details.

## Author

Alex Kilimnik

## Disclaimer

This is an unofficial third-party application. It is not affiliated with, endorsed by, or supported by Qidi Technology.

## Version

0.2 - Hexadecimal fix (current)
0.1 - Initial release
