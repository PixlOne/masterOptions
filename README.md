## Note: This project is no longer being developed here, please visit https://github.com/PixlOne/logiops for further developments.

# Logitech MX Master Options

This is intended to be a daemon and configuration utility for the Logitech MX Master on Linux.


**NOTE:** This is currently a work in progress. You may want to use [libratbag](https://github.com/libratbag/libratbag) instead.

What works so far:

* Automatic device detection & divertion
* Gesture detection
* Redivertion on disconnect
* Button remapping
* Configuration files

What's planned:

* Mapping gestures to relative axes
* Configuration documentation
* Smartshift, DPI, hires scrolling config
* udev rules, running as a service
* Properly documenting code

Possible ideas:

* Universal Logitech HID++ config daemon
* CLI configuration utility (GUI too maybe?)

## Building

This project requires a C++14 compiler, cmake, libevdev, libconfig, and [libhidpp](https://github.com/cvuchener/hidpp)

To build this project, run:

```
mkdir build
cd build
cmake ..
make
```

Installation is currently not implemented.
