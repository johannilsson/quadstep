/*
 
 created by Aaron Weiss, SparkFun electronics 2011
 modified by John Miller, johnjamesmiller.com 2012
 modified by Johan Nilsson, markupartist.com 2013
 Beerware - do what you wish with this ish
 
 */

#include <Wire.h>
#include "Adafruit_MCP23017.h"

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "quadstep.h"

#define STEPMIN	800

quadstep::quadstep() {
  _using_mcp = false;
  //make sure the step lines are low on startup
  doDigitalWrite(_step_pin, LOW);
  Serial.begin(9600);
}

void quadstep::set_mcp(Adafruit_MCP23017 mcp) {
  _using_mcp = true;
  _mcp = mcp;
}

/////////////////////////////////////////////////////////
///////   Pin connections ///////////////////////////////
/////////////////////////////////////////////////////////
void quadstep::set_enable_pin(int enable_pin) {
  doPinMode(enable_pin, OUTPUT);
  doDigitalWrite(enable_pin, HIGH);
  _enable_pin = enable_pin;
}

void quadstep::set_direction_pin(int direction_pin) {
  doPinMode(direction_pin, OUTPUT);
  doDigitalWrite(direction_pin, LOW);
  _direction_pin = direction_pin;
}

void quadstep::set_step_pin(int step_pin) {
	doPinMode(step_pin, OUTPUT);
	_step_pin = step_pin;
}

void quadstep::set_microstep_select_pins(int ms1_pin,int ms2_pin,int ms3_pin) {
  doPinMode(ms1_pin, OUTPUT);
  doPinMode(ms2_pin, OUTPUT);
  doPinMode(ms3_pin, OUTPUT);
  _ms1_pin = ms1_pin;
  _ms2_pin = ms2_pin;
  _ms3_pin = ms3_pin;
}

/////////////////////////////////////////////////////////
///////   Motor settings  ///////////////////////////////
/////////////////////////////////////////////////////////

bool quadstep::run() {
  if (_current_position != _target_position) {

    step();

    if (_direction == DIRECTION_CW) {
      _current_position += 1;
    } else {
      _current_position -= 1;
    }

    return true;
  }
  disable();
  return false;
}

int quadstep::getStepsToGo() {
  return _target_position - _current_position;
}

int quadstep::getCurrentPosition() {
  return _current_position;
}

void quadstep::go(step_modes_t step_size, int number_of_steps, int torque) {
  if (_target_position != number_of_steps) {
    _target_position = number_of_steps;
    _step_size = step_size;
    _torque = torque;
    _direction = getDirection(number_of_steps);

  
    set_direction(_target_position);
    set_speed(_step_size, _torque);
    set_microstep_format(_step_size);
    
    enable();
  }
}

void quadstep::stall() {
  enable();
}

direction_t quadstep::getDirection(int number_of_steps) {
  // Or otherway around?
  direction_t dir = (number_of_steps > 0) ? DIRECTION_CW : DIRECTION_CCW;
  return dir;
}

void quadstep::set_direction(int number_of_steps) {
  bool dir = (number_of_steps > 0) ? HIGH : LOW;
  doDigitalWrite(_direction_pin, dir);
}

void quadstep::set_speed(step_modes_t step_size, int torque) {
  pulse_width = STEPMIN + (torque * 260) / step_size;
}

void quadstep::set_microstep_format(step_modes_t step_size) {
  if (step_size == FULL) {
    doDigitalWrite(_ms1_pin, LOW);
    doDigitalWrite(_ms2_pin, LOW);
    doDigitalWrite(_ms3_pin, LOW);
  } else if (step_size == HALF) {
    doDigitalWrite(_ms1_pin, HIGH);
    doDigitalWrite(_ms2_pin, LOW);
    doDigitalWrite(_ms3_pin, LOW);
  } else if (step_size == QUARTER) {
    doDigitalWrite(_ms1_pin, LOW);
    doDigitalWrite(_ms2_pin, HIGH);
    doDigitalWrite(_ms3_pin, LOW);
  } else if (step_size == EIGHTH) {
    doDigitalWrite(_ms1_pin, HIGH);
    doDigitalWrite(_ms2_pin, HIGH);
    doDigitalWrite(_ms3_pin, LOW);
  } else if (step_size == SIXTEENTH) {
    doDigitalWrite(_ms1_pin, HIGH);
    doDigitalWrite(_ms2_pin, HIGH);
    doDigitalWrite(_ms3_pin, HIGH);
  } else{
    Serial.println("error: incorrect value for step_size");
  }
}

void quadstep::enable() {
  doDigitalWrite(_enable_pin, LOW);
}

void quadstep::step() {
  doDigitalWrite(_step_pin, HIGH);
  delayMicroseconds (pulse_width); //low time
  doDigitalWrite(_step_pin, LOW);
  delayMicroseconds(pulse_width); // high time
}

void quadstep::disable() {
  doDigitalWrite(_enable_pin, HIGH);
}

void quadstep::doDigitalWrite(int pin, int value) {
  if (!_using_mcp) {
    digitalWrite(pin, value);
  } else {
    _mcp.digitalWrite(pin, value);
  }
}

void quadstep::doPinMode(int pin, int value) {
  if (!_using_mcp) {
    pinMode(pin, value);
  } else {
    _mcp.pinMode(pin, value);
  }
}
