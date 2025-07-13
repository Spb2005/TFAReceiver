# TFA 433 Receiver for Dostmann 30.3208

Use your Arduino to receive temperature and humidity data from the TFA Dostmann 30.3208 remote sensor using a 433 MHz receiver.

This library is based on a fork of [d10i's original code](https://github.com/d10i/TFA).

It may also work with the following devices (not tested):

* Ambient Weather F007TH Thermo-Hygrometer
* Ambient Weather F012TH Indoor/Display Thermo-Hygrometer
* SwitchDoc Labs F016TH

---

## Changes Compared to d10i's Code

* **Interrupt Optimization**:
  The checksum calculation was removed from the interrupt service routine because it triggered the interrupt watchdog timer on ESP32 platforms.
  Instead, the checksum is now verified inside the `loop()` using the `receiver.checkBuf();` function.
  Make sure to call this in your `loop()` function.

* **Cross-Platform Compatibility**:
  Fixed a compilation issue on non-AVR (non-Atmel) platforms:

  ```
  invalid conversion from 'volatile byte*' to 'const byte*' [-fpermissive]
  ```

* **Temperature Unit Selection**:
  Added `receiver.setTemperatureUnit(bool useCelsius);`
  You can choose the temperature unit:

  * `receiver.setTemperatureUnit(true);` → Celsius (default)
  * `receiver.setTemperatureUnit(false);` → Fahrenheit
  * If not set, Celsius is used by default.

---

## Download

GitHub Repository: [https://github.com/Spb2005/TFAReceiver](https://github.com/Spb2005/TFAReceiver)

---

## Additional Info

### Hardware

Any standard 433 MHz receiver module should work.
Cheap models from online retailers are sufficient.

Make sure to connect the receiver to an interrupt-capable pin:

* On Arduino Uno/Nano: use **D2** or **D3**

### Example Usage

A working example is available in the `examples/` directory.
It shows how to receive and read sensor data using this library.

---

## Credits & Inspiration

This library is a modified version of the one by [d10i](https://github.com/d10i/TFA), which itself was inspired by:

* [denxhun/TFA433](https://github.com/denxhun/TFA433): for README format and general library interface structure.
* [robwlakes/ArduinoWeatherOS](https://github.com/robwlakes/ArduinoWeatherOS): for the receiving logic.
* [merbanan/rtl\_433](https://github.com/merbanan/rtl_433): for decoding and packet structure information.
