#ifndef CONTROL_H
#define CONTROL_H

ISR(PCINT2_vect);
void map_radio_channels_to_angle_rates();
void check_for_motor_arm();
void write_motor_pulse();

#endif