#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>

extern const byte SEL_PB;

void initialize_display();
void update_display(double t_cold_junction, double t_hot_junction, byte tc_type, byte fault);
void display_tc_type(byte tc_type);

#endif