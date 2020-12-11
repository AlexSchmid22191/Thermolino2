#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <LiquidCrystal.h>
#include <Adafruit_MAX31856.h>

void initialize_display(LiquidCrystal &display);
void update_display(double t_cold_junction, double t_hot_junction, byte tc_type, byte fault, LiquidCrystal &display);
void display_temperatures(double t_cold_junction, double t_hot_junction, LiquidCrystal&display);
void display_errors(byte fault, LiquidCrystal &display);
void display_tc_type(byte tc_type, LiquidCrystal &display);

#endif