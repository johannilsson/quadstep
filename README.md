
# Sparkfun Quadstep Arduino Library

This is a fork of @johnjamesmiller
[quadstep](https://github.com/johnjamesmiller/quadstep) library.

This fork requires a MCP23017 io expander and communication is done using I2C
using the [Adafruit-MCP23017 Arduino
Library](https://github.com/adafruit/Adafruit-MCP23017-Arduino-Library).
With this setup only two pins A4 and A5 is required to control the
motors.

The library is also modified to support control of the motors
simultaneously. This should be fairly easy to backport upstream and is
planned.

---
Johan Nilsson <http://www.markupartist.com>

