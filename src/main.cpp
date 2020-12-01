#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal.h>
#include <Adafruit_MAX31856.h>


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

//Global variables
double t_hot_junction, t_cold_junction;

void poll_pushbutton(const byte button, void (&function)(void));
void toggle_tc_type();
void initialize_display();
void update_display(double t_cold_junction, double t_hot_juction);
double ema_filter(double prev_value, double raw_value, const float filter_constant);

void setup()
{
  pinMode(SEL_PB, INPUT_PULLUP);

  Serial.begin(9600);

  maxthermo.begin();
  maxthermo.setConversionMode(MAX31856_CONTINUOUS);
  maxthermo.setNoiseFilter(MAX31856_NOISE_FILTER_50HZ);
  
  display.begin(16, 2);
  initialize_display();
}

void loop()
{
  if(!digitalRead(MAX_DR))
  {
    t_hot_junction = ema_filter(t_hot_junction, maxthermo.readThermocoupleTemperature(), 0.5);
    t_cold_junction = ema_filter(t_cold_junction, maxthermo.readCJTemperature(), 0.5);
  }
  poll_pushbutton(SEL_PB, toggle_tc_type);
  update_display(t_cold_junction, t_hot_junction);
}

double ema_filter(double prev_value, double raw_value, const float filter_constant)
{
  return filter_constant*raw_value + (1-filter_constant) * prev_value;
}

void initialize_display()
{
  display.setCursor(0, 0);
  display.print("HJ: ");
  display.setCursor(12, 0);
  display.print(char(176));
  display.print("C K");
  display.setCursor(0, 1);
  display.print("CJ: ");
  display.setCursor(12, 1);
  display.print(char(176));
  display.print("C");
}

void update_display(double t_cold_junction, double t_hot_juction)
{
  static unsigned long last_update = millis();
  if (millis() - last_update > 1000)
  {
    display.setCursor(6, 0);
    display.print(t_hot_junction);
    display.setCursor(6, 1);
    display.print(t_cold_junction);
    last_update = millis();
  }
}

void toggle_tc_type()
{
  static byte tc_type = (byte)MAX31856_TCTYPE_K;

  ++tc_type %=8;
  maxthermo.setThermocoupleType((max31856_thermocoupletype_t)tc_type);

  display.setCursor(15, 0);
  switch(tc_type)
  {
    case MAX31856_TCTYPE_K: display.print("K"); break;
    case MAX31856_TCTYPE_S: display.print("S"); break;
    case MAX31856_TCTYPE_E: display.print("E"); break;
    case MAX31856_TCTYPE_J: display.print("J"); break;
    case MAX31856_TCTYPE_R: display.print("R"); break;
    case MAX31856_TCTYPE_B: display.print("B"); break;
    case MAX31856_TCTYPE_T: display.print("T"); break;
    case MAX31856_TCTYPE_N: display.print("N"); break;
    case MAX31856_VMODE_G8: break;
    case MAX31856_VMODE_G32: break;
    default: break;
  }
}

void poll_pushbutton(const byte button, void (&function)(void))
{
  static unsigned long last_pushed = millis();
  if(!digitalRead(SEL_PB) && millis() - last_pushed > 200)
  {
    function();
    last_pushed = millis();
  }
}