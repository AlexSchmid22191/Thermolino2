#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal.h>
#include <Adafruit_MAX31856.h>
#include "display/display.h"
#include "serialcom/serialcom.h"

//TODO: Change the display init protocol, such that the correct TY_Type is shown after recovery from error

// Pin definitions
const byte MAX_CS = 10;
const byte MAX_DR = 9;

const byte VFD_RS = A1;
const byte VFD_EN = A0;
const byte VFD_D0 = 4;
const byte VFD_D1 = A5;
const byte VFD_D2 = 5;
const byte VFD_D3 = A4;
const byte VFD_D4 = 6;
const byte VFD_D5 = A3;
const byte VFD_D6 = 7;
const byte VFD_D7 = A2;

const byte SEL_PB = 2;

// Devices
Adafruit_MAX31856 maxthermo = Adafruit_MAX31856(MAX_CS);
LiquidCrystal display(VFD_RS, VFD_EN, VFD_D0, VFD_D1, VFD_D2, VFD_D3, VFD_D4, VFD_D5, VFD_D6, VFD_D7);

void poll_pushbutton(const byte button, void (&function)(byte &), byte &argument);
void toggle_tc_type(byte &tc_type);
double ema_filter(double prev_value, double raw_value, const float filter_constant);


void setup()
{
  pinMode(SEL_PB, INPUT_PULLUP);

  Serial.begin(9600);

  maxthermo.begin();
  maxthermo.setConversionMode(MAX31856_CONTINUOUS);
  maxthermo.setNoiseFilter(MAX31856_NOISE_FILTER_50HZ);

  display.begin(16, 2);
  initialize_display(display);
  display_tc_type((byte)MAX31856_TCTYPE_K, display);
}


void loop()
{
  static double t_hot_junction, t_cold_junction;
  static byte tc_type = (byte)MAX31856_TCTYPE_K;
  
  byte fault = maxthermo.readFault();
  if (!digitalRead(MAX_DR))
  {
    t_hot_junction = ema_filter(t_hot_junction, maxthermo.readThermocoupleTemperature(), 0.5);
    t_cold_junction = ema_filter(t_cold_junction, maxthermo.readCJTemperature(), 0.5);
  }
  poll_pushbutton(SEL_PB, toggle_tc_type, tc_type);
  update_display(t_cold_junction, t_hot_junction, tc_type, fault, display);
  poll_serial(t_hot_junction);
}


double ema_filter(double prev_value, double raw_value, const float filter_constant)
{
  return filter_constant * raw_value + (1 - filter_constant) * prev_value;
}

void toggle_tc_type(byte &tc_type)
{
  ++tc_type %= 8;
  maxthermo.setThermocoupleType((max31856_thermocoupletype_t)tc_type);
  display_tc_type(tc_type, display);
}

void poll_pushbutton(const byte button, void (&function)(byte &), byte &argument)
{
  static unsigned long last_pushed = millis();
  if (!digitalRead(SEL_PB) && millis() - last_pushed > 200)
  {
    function(argument);
    last_pushed = millis();
  }
}