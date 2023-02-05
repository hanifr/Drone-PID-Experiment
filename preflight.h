#ifndef PREFLIGHT_H
#define PREFLIGHT_H

void startup();
void interrupt_registers_startup();
void motors_startup();
void check_gyro_coherence();
bool security_check();

#endif