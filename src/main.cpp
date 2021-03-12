#include <Arduino.h>
#include <Adafruit_MAX31856.h>

#include "display/display.h"
#include "serialcom/serialcom.h"

// Pin definitions
const byte MAX_DR = 8;
const byte MAX_CS = 10;

// Thermocouple readout chip
Adafruit_MAX31856 maxthermo = Adafruit_MAX31856(MAX_CS);

void toggle_tc_type(byte &tc_type);
void poll_pushbutton(const byte button, void (&function)(byte &), byte &argument);
double ema_filter(double prev_value, double raw_value, const float filter_constant);


void setup()
{
  pinMode(MAX_DR, INPUT);
  pinMode(SEL_PB, INPUT_PULLUP);

  Serial.begin(9600);

  maxthermo.begin();
  maxthermo.setConversionMode(MAX31856_CONTINUOUS);
  maxthermo.setNoiseFilter(MAX31856_NOISE_FILTER_50HZ);

  initialize_display();
  display_tc_type((byte)MAX31856_TCTYPE_K);
}


void loop()
{
  delay(10);
  static double t_hot_junction, t_cold_junction;
  static byte tc_type = (byte)MAX31856_TCTYPE_K;
  
  byte fault = maxthermo.readFault();
  if (!digitalRead(MAX_DR) or true)
  {
    t_hot_junction = ema_filter(t_hot_junction, maxthermo.readThermocoupleTemperature(), 0.5);
    t_cold_junction = ema_filter(t_cold_junction, maxthermo.readCJTemperature(), 0.5);
  }
  poll_pushbutton(SEL_PB, toggle_tc_type, tc_type);
  update_display(t_cold_junction, t_hot_junction, tc_type, fault);
  poll_serial(t_hot_junction, t_cold_junction);
}


double ema_filter(double prev_value, double raw_value, const float filter_constant)
{
  return filter_constant * raw_value + (1 - filter_constant) * prev_value;
}

void toggle_tc_type(byte &tc_type)
{
  ++tc_type %= 8;
  maxthermo.setThermocoupleType((max31856_thermocoupletype_t)tc_type);
  display_tc_type(tc_type);
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