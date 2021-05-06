// Implements display drawing functionalities for the Thermolino 2 (Black PCB)


#include <SPI.h>
#include <Wire.h>
#include <LiquidCrystal.h>
#include <Adafruit_MAX31856.h>

#include "display.h"

const byte VFD_RS = A5;
const byte VFD_EN = A4;
const byte VFD_D0 = 4;
const byte VFD_D1 = A3;
const byte VFD_D2 = 5;
const byte VFD_D3 = A2;
const byte VFD_D4 = 6;
const byte VFD_D5 = A1;
const byte VFD_D6 = 7;
const byte VFD_D7 = A0;

const byte SEL_PB = 2;

static LiquidCrystal display(VFD_RS, VFD_EN, VFD_D0, VFD_D1, VFD_D2, VFD_D3, VFD_D4, VFD_D5, VFD_D6, VFD_D7);

#define DEG_SYM 7
byte degree[8] = 
{
  B01100,
  B10010,
  B10010,
  B01100,
  B00000,
  B00000,
  B00000,
  B00000
};


void display_temperatures(double t_cold_junction, double t_hot_junction);
void display_errors(byte fault);


void initialize_display()
{
  display.createChar(7, degree);
  display.begin(16, 2);
  display.clear();
  display.setCursor(0, 0);
  display.print("TC: ");
  display.setCursor(12, 0);
  display.write(DEG_SYM);
  display.print("C");
  display.setCursor(0, 1);
  display.print("CJ: ");
  display.setCursor(12, 1);
  display.write(DEG_SYM);
  display.print("C");
};

void update_display(double t_cold_junction, double t_hot_junction, byte tc_type, byte fault)
{
  static bool needs_reinit = false;
  static unsigned long last_update = millis();
  if (millis() - last_update > 1000)
  {
    if(fault)
    {
      display_errors(fault);
      needs_reinit = true;
    }
    else
    {
      if(needs_reinit)
      {
        initialize_display();
        display_tc_type(tc_type);
        needs_reinit = false;
      }
      display_temperatures(t_cold_junction, t_hot_junction);
    }
    last_update = millis();
  }
}


void display_tc_type(byte tc_type)
{
  display.setCursor(15, 0);
  switch (tc_type)
  {
  case MAX31856_TCTYPE_K:
    display.print("K");
    break;
  case MAX31856_TCTYPE_S:
    display.print("S");
    break;
  case MAX31856_TCTYPE_E:
    display.print("E");
    break;
  case MAX31856_TCTYPE_J:
    display.print("J");
    break;
  case MAX31856_TCTYPE_R:
    display.print("R");
    break;
  case MAX31856_TCTYPE_B:
    display.print("B");
    break;
  case MAX31856_TCTYPE_T:
    display.print("T");
    break;
  case MAX31856_TCTYPE_N:
    display.print("N");
    break;
  case MAX31856_VMODE_G8:
    break;
  case MAX31856_VMODE_G32:
    break;
  default:
    break;
  }
};

void display_temperatures(double t_cold_junction, double t_hot_junction)
{
    char temp_string[8];

    dtostrf(t_hot_junction, 7, 2, temp_string);
    display.setCursor(4, 0);
    display.print(temp_string);

    dtostrf(t_cold_junction, 7, 2, temp_string);
    display.setCursor(4, 1);
    display.print(temp_string);
};


void display_errors(byte fault)
{
  display.clear();
  if (fault & MAX31856_FAULT_CJRANGE)
  {
      display.setCursor(0,0);
      display.print("CJ-R");
  }
  if (fault & MAX31856_FAULT_TCRANGE)
  {
    display.setCursor(6,0);
    display.print("TC-R");
  }
  if (fault & MAX31856_FAULT_CJHIGH)
  {
    display.setCursor(0, 1);
    display.print("CJ-H");
  }
  if (fault & MAX31856_FAULT_CJLOW)
  {
    display.setCursor(0, 1);
    display.print("CJ-L");
  }
  if (fault & MAX31856_FAULT_TCHIGH)
  {
    display.setCursor(6, 1);
    display.print("TC-H");
  }
  if (fault & MAX31856_FAULT_TCLOW)
  {
    display.setCursor(6, 1);
    display.print("TC-L");
  }
  if (fault & MAX31856_FAULT_OVUV)
  {
    display.setCursor(12, 0);
    display.print("OVUV");
  }
  if (fault & MAX31856_FAULT_OPEN)
  {
    display.setCursor(12, 1);
    display.print("OPEN");
  }
};