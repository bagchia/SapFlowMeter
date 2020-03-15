/*****************************************************************
* Sapflow_ADC.h
* Josh Barksdale
* March 2020
* This library contains a single function which reads the
* temperature in Celsius from 3 different RTDs using an MCP3424
* ADC for the SapFlow project. It uses protothreads. It is based
* on the MCP342x library by B@tto.
******************************************************************/
#ifndef SAPADC_H
#define SAPADC_H

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <pt.h>
#include <Wire.h>

struct temperature{
    double upper;   //temperature in deg C at upper probe
    double lower;   //temperature in deg C at lower probe
    double heater;  //temperature in deg C at heater probe
};

enum register_mask{
  RDY=1<<7,  // result ready
  CHAN=3<<5, // Channel 0-3
  CON=1<<4,  // Continuous/One-Shot
  DEPTH=3<<2,  // Bit depth 12-18
  PGA=3<<0,   // Gain of 1-8
};

int get_temp(struct pt *pt, uint8_t i2c_addr, struct temperature *dest);

double rtd_calc(int32_t raw);

int32_t measure(uint8_t addr, uint8_t channel);

#endif