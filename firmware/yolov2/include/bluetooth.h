// Please see
// https://github.com/sktometometo/M5Stack_Bluetooth_rosserial_example

#include <Arduino.h>
#include "BluetoothSerial.h"

class BluetoothHardware {
public:
  BluetoothHardware()
  {
  }

  void init()
  {
    bluetooth_serial.begin("ESP32ROS");
  }

  void init(char *name)
  {
    bluetooth_serial.begin(name);
  }

  int read(){
    return bluetooth_serial.read();
  }

  void write(const uint8_t* data, int length)
  {
    bluetooth_serial.write(data, length);
  }

  unsigned long time()
  {
    return millis();
  }

protected:
  BluetoothSerial bluetooth_serial;
};
