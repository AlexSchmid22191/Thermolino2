// Implements display drawing functionalities for the Thermolino 2L (Black PCB, Oled Display)

#include <Arduino.h>
#include <Wire.h>
#include <display/display.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_MAX31856.h>
#include <Fonts/FreeMonoBold12pt7b.h>
#include <Fonts/FreeMono9pt7b.h>

// Rewrite this with the old logic

const byte SEL_PB = 2;

const byte SCREEN_WIDTH = 128;
const byte SCREEN_HEIGHT = 64;
const byte SCREEN_ADDRESS = 0x3C;

void display_temperatures(double t_cold_junction, double t_hot_junction);
void display_errors(byte fault);

static Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void initialize_display()
{
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  display.clearDisplay();
  
  // Partition the display nicely
  display.drawFastHLine(0, 31, 128, SSD1306_WHITE);
  display.drawFastHLine(0, 32, 128, SSD1306_WHITE);
  display.drawFastVLine(63, 32, 32, SSD1306_WHITE);
  display.drawFastVLine(64, 32, 32, SSD1306_WHITE);
  
  display.display();
}

void update_display(double t_cold_junction, double t_hot_junction, byte tc_type, byte fault)
{
  static unsigned long last_update = millis();
  if(millis() - last_update < 1000) return;

  // Clear the top area and the left bottom area
  display.writeFillRect(0, 0, 128, 31, SSD1306_BLACK);
  display.writeFillRect(0, 33, 63, 31, SSD1306_BLACK);

  // Working variables for positioning
  int16_t x, y;
  uint16_t w, h;

  if(!fault)
  {
    //  Draw thermocouple temperature
    char temp_string[8];
    dtostrf(t_hot_junction, 4, 2, temp_string);
    display.setFont(&FreeMonoBold12pt7b);
    display.setTextColor(SSD1306_WHITE);
    display.getTextBounds(temp_string, 0, 0, &x, &y, &w, &h);
    x = (128 - w) / 2;
    y = (31 + h) / 2;
    display.setCursor(x, y);
    display.print(temp_string);
  }

  else
  {
    display_errors(fault);
  }

  // Draw cold junction temperature
  char temp_string[8];
  dtostrf(t_cold_junction, 4, 2, temp_string);
  display.setFont(&FreeMono9pt7b);
  display.setTextColor(SSD1306_WHITE);
  display.getTextBounds(temp_string, 0, 33, &x, &y, &w, &h);
  x = (63 - w) / 2;
  y = (95 + h) / 2;
  display.setCursor(x, y);
  display.print(temp_string);

  display.display();
  last_update = millis();
}


void display_tc_type(byte tc_type)
{
  // Clear out the right bottom area
  display.writeFillRect(65, 33, 63, 31, SSD1306_BLACK);
    
  char tc_str[2];
  switch (tc_type)
  {
  case MAX31856_TCTYPE_K:
    strcpy(tc_str, "K");
    break;
  case MAX31856_TCTYPE_S:
    strcpy(tc_str, "S");
    break;
  case MAX31856_TCTYPE_E:
    strcpy(tc_str, "E");
    break;
  case MAX31856_TCTYPE_J:
    strcpy(tc_str, "J");
    break;
  case MAX31856_TCTYPE_R:
    strcpy(tc_str, "R");
    break;
  case MAX31856_TCTYPE_B:
    strcpy(tc_str, "B");
    break;
  case MAX31856_TCTYPE_T:
    strcpy(tc_str, "T");
    break;
  case MAX31856_TCTYPE_N:
    strcpy(tc_str, "N");
    break;
  case MAX31856_VMODE_G8:
    strcpy(tc_str, "");
    break;
  case MAX31856_VMODE_G32:
    strcpy(tc_str, "");
    break;
  default:
    strcpy(tc_str, "");
    break;
  }

  // Working variables for positioning
  int16_t x, y;
  uint16_t w, h;

  // Draw TC type
  display.setFont(&FreeMonoBold12pt7b);
  display.setTextColor(SSD1306_WHITE);
  display.getTextBounds(tc_str, 65, 33, &x, &y, &w, &h);
  x = (193 - w) /2;
  y = (95 + h) / 2;
  display.setCursor(x, y);
  display.print(tc_str);
  display.display();
}



void display_errors(byte fault)
{
  int16_t x, y;
  uint16_t w, h;

  char errorstring[10];
  snprintf(errorstring, sizeof(errorstring), "Error %u", fault);

  display.setFont(&FreeMono9pt7b);
  display.setTextColor(SSD1306_WHITE);
  display.getTextBounds(errorstring, 0, 0, &x, &y, &w, &h);
  x = (128 - w) / 2;
  y = (31 + h) / 2;
  display.setCursor(x, y);
  display.print(errorstring);
};