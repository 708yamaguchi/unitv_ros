# unitv_ros

Publish UnitV recognition result as ROS topic via M5Stack

## Overview

Communication:

  UnitV <-(UART)-> M5Stack <-(Bluetooth/Wi-Fi/USB)-> PC

Published topics:

- `/unitv_image` (`sensor_msgs/Image`)

  UnitV Image compressed to jpg

- `/unitv_image/rects` (`jsk_recognition_msgs/RectArray`)

  Object position rectangle

- `/unitv_image/class` (`jsk_recognition_msgs/ClassificationResult`)

  Object class

- `/draw_rects/output` (`sensor_msgs/Image`)

  UnitV image with recognition result

Visualization of `/draw_rects/output`:

![unitv_ros_demo](https://user-images.githubusercontent.com/19769486/132894050-84d7a79d-da3a-4057-8487-025188f65515.gif)

## Setup

In this section, you can use yolov2 recognition.

1. Create ROS package

    ```bash
    mkdir ~/catkin_ws/src -p
    cd ~/catkin_ws/src
    git clone https://github.com/708yamaguchi/unitv_ros.git
    rosdep install --ignore-src --from-path . -y -r
    cd ~/catkin_ws
    catkin build
    ```

2. Prepare [M5Stack](https://shop.m5stack.com/collections/m5-controllers/products/basic-core-iot-development-kit) and [UnitV](https://shop.m5stack.com/products/unitv-ai-camera?variant=34757279809700)

3. Install [Arduino IDE](https://www.arduino.cc/en/software/), [kflash](https://github.com/sipeed/kflash_gui/releases/download/v1.5.3/kflash_gui_v1.5.3_linux.tar.xz) and [MaixPy IDE](https://dl.sipeed.com/shareURL/MAIX/MaixPy/ide/_)

    - Download arduino IDE file and install it under home directory like `~/arduino-x.y.z`.
    - About kflash and MaixPy IDE, see [unitv quick start](https://docs.m5stack.com/en/quick_start/unitv/unitv_quick_start_maixpy)

4. Burn firmwares into UnitV

    - Make sure that UnitV is connected to your computer via USB Type-C
    - Burn [yolov2.kfpkg](https://github.com/708yamaguchi/unitv_ros/blob/master/data/yolov2.kfpkg) with kflash
    - Burn [yolov2.py](https://github.com/708yamaguchi/unitv_ros/blob/master/firmware/yolov2/yolov2.py)  as boot.py with MaixPy (Tools -> Save open script to board(boot.py))

5. Install M5Stack package in Arduino IDE (Tools -> Manage Libraries -> M5Stack)

6. Create rosserial_arduino libraries

    ```bash
    cd ~/arduino-x.y.z/libraries
    rm -rf ros_lib
    rosrun rosserial_arduino make_libraries.py .
    ```

    See also [rosserial_arduino](http://wiki.ros.org/rosserial_arduino/Tutorials/Arduino%20IDE%20Setup)

7. Burn [yolov2.ino](https://github.com/708yamaguchi/unitv_ros/blob/master/firmware/yolov2/yolov2.ino) into M5Stack

    - Launch Arduino IDE

      ```bash
      ~/arduino-x.y.z/arduino
      ```

    - Before burning, change `#define BLUETOOTH` line according to your connection type

    - There are 3 connection types between M5Stack and computer: Bluetooth, Wi-Fi and USB

    - Currently, USB connection is deprecated. This is because USB connection cannot use baud rate >= 115200. Baud rate 57600 is so slow that UnitV image cannot be transferred.

    - If you use wifi, fill SSID and Password into [wifi.h](https://github.com/708yamaguchi/unitv_ros/blob/master/firmware/yolov2/include/wifi.h) before burning.

    - **Be careful** not to upload your password!

    - To burn to M5Stack(`/dev/ttyUSB*`), you should belong to dialout group.

      ```bash
      sudo adduser $USER dialout
      ```

## Run

1. Setup Connection.

    - Connect M5Stack and UnitV with Grove cable.

    - If you use Bluetooth connection, exec following commands. For detail, see [sktometometo's repo](https://github.com/sktometometo/M5Stack_Bluetooth_rosserial_example).

      ```bash
      sudo rfcomm bind 1 <Bluetooth MAC Address of M5Stack>
      sudo stty -F /dev/rfcomm1 115200 cs8
      ```

    - Bluetooth MAC Address is printed on Arduino IDE Serial Monitor when M5Stack is started.

2. Launch program

    First, source setup.bash

    ```bash
    source ~/catkin_ws/devel/setup.bash
    ```

    - For Bluetooth,

      ```bash
      roslaunch unitv_ros yolov2.launch baud:=115200 port:=/dev/rfcomm1
      ```

    - For Wi-Fi,

      ```bash
      roslaunch unitv_ros yolov2.launch baud:=115200 port:=tcp
      ```

    - For USB(deprecated),

      ```bash
      roslaunch unitv_ros yolov2.launch baud:=57600 port:=(USB DEVICE like /dev/ttyUSB0)
      ```

3. See recognition result

    ```bash
    rosrun image_view image_view image:=/draw_rects/output
    ```

## Trouble shooting

If visualization stops or does not start, follow the below instructions:

- Make sure that M5Stack and UnitV are connected with Grove cable
- Unplug the USB cable from M5Stack
- Double-click the main button to sleep M5Stack
- Plug the USB cable to M5Stack or click the main button to boot the M5Stack

If you want to use other recognition model, check following:

- Find or create `.kfpkg` file, which includes UnitV bin file and recognition model.
  - Find the models from [sipeed models](https://dl.sipeed.com/shareURL/MAIX/MaixPy/model) and [MaixHub](https://www.maixhub.com/)
  - Create the model at [MaixHub Model Training](https://www.maixhub.com/ModelTraining)
  - `.kfpkg` file is the same as zip file, so you can use the `.kfpkg` file just like zip file.
- Write script for recognition.
  - Sample MicroPython code for UnitV are in [sipeed GitHub](https://github.com/sipeed/MaixPy_scripts/tree/master/machine_vision)
  - You also need to edit launch file for visualization in ROS. Useful visualization nodes are [draw_classification_result.py](https://jsk-docs.readthedocs.io/projects/jsk_recognition/en/latest/jsk_perception/nodes/draw_classification_result.html), [draw_rects.py](https://jsk-docs.readthedocs.io/projects/jsk_recognition/en/latest/jsk_perception/nodes/draw_rects.html) and [label_image_decomposer.py](https://jsk-docs.readthedocs.io/projects/jsk_recognition/en/latest/jsk_perception/nodes/label_image_decomposer.html)
