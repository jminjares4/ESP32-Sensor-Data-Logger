# ESP-IDF Default README

## How to Use Example

Before project configuration and build, be sure to set the correct chip target using `idf.py set-target <chip_name>`.

### Hardware Required

* A development board with Espressif SoC (e.g., ESP32-DevKitC, ESP-WROVER-KIT, etc.)
* A USB cable for Power supply and programming

Some development boards use an addressable LED instead of a regular one. These development boards include:

| Board                | LED type             | Pin                  |
| -------------------- | -------------------- | -------------------- |
| ESP32-C3-DevKitC-1   | Addressable          | GPIO8                |
| ESP32-C3-DevKitM-1   | Addressable          | GPIO8                |
| ESP32-S2-DevKitM-1   | Addressable          | GPIO18               |
| ESP32-S2-Saola-1     | Addressable          | GPIO18               |
| ESP32-S3-DevKitC-1   | Addressable          | GPIO48               |

See [Development Boards](https://www.espressif.com/en/products/devkits) for more information about it.

### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## Troubleshooting

* If the LED isn't blinking, check the GPIO or the LED type selection in the `Example Configuration` menu.

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
