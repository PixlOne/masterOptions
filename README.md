# Logitech MX Master Options

This is intended to be a daemon and configuration utility for the Logitech MX Master on Linux.


**NOTE:** This is currently in a very early stage of development. You may want to use [libratbag](https://github.com/libratbag/libratbag) instead.

What works so far:

* Automatic device detection
* Automatic button divertion (pre-defined in Diverter.h)
* Listens for diverted button events (prints press/release events)
* Gesture detection
* Device connection/sleep event handlers

What's planned:

* Proper button remapping (in progress)
* Configuration files (in progress)
* SmartShift settings
* udev rules, running as a service
* CLI configuration utility (GUI too maybe?)
* Supporting more devices (low priority)
* ...

## Building

This project requires a C++14 compiler, cmake, libevdev, and [libhidpp](https://github.com/cvuchener/hidpp)

To build this project, run:

```
mkdir build
cd build
cmake ..
make
```

Installation is currently not possible.
