/*
The first function is an interrupt service routine 
(ISR) called PCINT2_vect. It is triggered when one 
of the pins on port D (D4, D5, D6, D7) changes state. 
Depending on the pin that caused the interrupt, 
the function updates one of four variables with 
the time since the last interrupt. These variables 
are used in the second function to calculate desired 
angle rates for the drone.

The second function is called map_radio_channels_to_angle_rates(). 
It takes the radio channel commands (values stored in an array 
called radioChannelCommands) and calculates desired angle rates 
for the X, Y, and Z axes based on these commands. The calculations 
involve some scaling and reversing, and there are upper and lower 
bounds for each axis to limit the range of the values. The desired 
angle rates are then stored in three variables: desired_angle_rate_x, 
desired_angle_rate_y, and desired_angle_rate_z.

The third function is called check_for_motor_arm(). It checks if 
the throttle value is above a certain threshold, and if so, it 
checks if the yaw value is within a certain range. If the yaw 
value is within this range, it sets a flag called motorArmed to 
true. Otherwise, it sets the flag to false.
*/

/* https://forum.arduino.cc/t/struggling-to-have-isr-pcint2_vect-trigger-a-class-function/390909/13 */

ISR(PCINT2_vect) {
    current_count = micros();
  
    if(PIND & B00100000 ){     // pin D4 --> B00010000  CH 1   PITCH                                        
      if(CH1_LS == 0){                                               
        CH1_LS = 1;                                                   
        counter_1 = current_count;                                             
      }
    }
    else if(CH1_LS == 1){                                           
      CH1_LS = 0;                                                     
      radioChannelCommands[PITCH] = current_count - counter_1;                       
    }
  
    if(PIND & B00010000){     // pin D5 --> B00100000   CH 2    ROLL
      if(CH2_LS == 0){                      
        CH2_LS = 1;                     
        counter_2 = current_count;             
      }
    }
    else if(CH2_LS == 1){                       
      CH2_LS = 0;                              
      radioChannelCommands[ROLL] = current_count - counter_2; 
    }
  
    if(PIND & B10000000 ){     // pin D7 --> B10000000  CH 3      THROTTLE                                
      if(CH3_LS == 0){                                             
        CH3_LS = 1;                                                  
        counter_3 = current_count;                                               
      }
    }
    else if(CH3_LS == 1){                                             
      CH3_LS = 0;                                                    
      radioChannelCommands[THROTTLE] = current_count - counter_3;                           
  
    }
   
    if(PIND & B01000000 ){      // pin D6  --> B10000000  CH 4     YAW          
      if(CH4_LS == 0){                                               
        CH4_LS = 1;                                                   
        counter_4 = current_count;                                              
      }
    }
    else if(CH4_LS == 1){                                             
      CH4_LS = 0;                                                  
      radioChannelCommands[YAW] = current_count - counter_4;                          
    }
   
  }

void map_radio_channels_to_angle_rates(){
    
    desired_angle_rate_x = 0;  //default value for value included in the deadband
    
    if( radioChannelCommands[PITCH] > DEADBAND_UPPER_BOUND ){ desired_angle_rate_x = ( radioChannelCommands[PITCH] - DEADBAND_UPPER_BOUND ) * SW_PITCH_REVERSE; }         //X axis value -- pitch 
    if( radioChannelCommands[PITCH] < DEADBAND_LOWER_BOUND ){ desired_angle_rate_x = ( radioChannelCommands[PITCH] - DEADBAND_LOWER_BOUND ) * SW_PITCH_REVERSE; }         //X axis value -- pitch 
  
    if( desired_angle_rate_x / SETPOINT_PITCH_SPEED_RATE <= MAX_GYRO_VALUE && desired_angle_rate_x / SETPOINT_PITCH_SPEED_RATE >= MIN_GYRO_VALUE ){
      desired_angle_rate_x = desired_angle_rate_x / SETPOINT_PITCH_SPEED_RATE;
    }
    else if( desired_angle_rate_x / SETPOINT_PITCH_SPEED_RATE <= MIN_GYRO_VALUE ){ 
      desired_angle_rate_x = MIN_GYRO_VALUE;  
    }else{
      desired_angle_rate_x = MAX_GYRO_VALUE; 
    }
  
  
    desired_angle_rate_y = 0;  //default value for value included in the deadband
    
    if( radioChannelCommands[ROLL] > DEADBAND_UPPER_BOUND ){ desired_angle_rate_y = ( radioChannelCommands[ROLL] - DEADBAND_UPPER_BOUND ) * SW_ROLL_REVERSE; }            //Y axis value -- roll 
    if( radioChannelCommands[ROLL] < DEADBAND_LOWER_BOUND ){ desired_angle_rate_y = ( radioChannelCommands[ROLL] - DEADBAND_LOWER_BOUND ) * SW_ROLL_REVERSE; }            //Y axis value -- roll 
  
    if( desired_angle_rate_y / SETPOINT_ROLL_SPEED_RATE <= MAX_GYRO_VALUE && desired_angle_rate_y / SETPOINT_ROLL_SPEED_RATE >= MIN_GYRO_VALUE ){
      desired_angle_rate_y = desired_angle_rate_y / SETPOINT_ROLL_SPEED_RATE;
    }
    else if( desired_angle_rate_y / SETPOINT_ROLL_SPEED_RATE <= MIN_GYRO_VALUE ){ 
      desired_angle_rate_y = MIN_GYRO_VALUE;  
    }else{
      desired_angle_rate_y = MAX_GYRO_VALUE; 
    }
  
  
    desired_angle_rate_z = 0;  //default value for value included in the deadband
    
    if(radioChannelCommands[THROTTLE] > THROTTLE_LOWER_THRESHOLD){
      if( radioChannelCommands[YAW] > DEADBAND_UPPER_BOUND ){ desired_angle_rate_z = ( radioChannelCommands[YAW] - DEADBAND_UPPER_BOUND ) * SW_YAW_REVERSE; }            //Z axis value -- yaw 
      if( radioChannelCommands[YAW] < DEADBAND_LOWER_BOUND ){ desired_angle_rate_z = ( radioChannelCommands[YAW] - DEADBAND_LOWER_BOUND ) * SW_YAW_REVERSE; }            //Z axis value -- yaw 
    }
    
    if( desired_angle_rate_z / SETPOINT_YAW_SPEED_RATE <= MAX_GYRO_VALUE && desired_angle_rate_z / SETPOINT_YAW_SPEED_RATE >= MIN_GYRO_VALUE ){
      desired_angle_rate_z = desired_angle_rate_z / SETPOINT_YAW_SPEED_RATE;
    }
    else if( desired_angle_rate_z / SETPOINT_YAW_SPEED_RATE <= MIN_GYRO_VALUE ){ 
      desired_angle_rate_z = MIN_GYRO_VALUE;  
    }else{
      desired_angle_rate_z = MAX_GYRO_VALUE; 
    }
      
  }

void check_for_motor_arm(){

    if( radioChannelCommands[THROTTLE] <= 1050 && radioChannelCommands[YAW] >= 1990 ){
      delay(500);
      if( radioChannelCommands[THROTTLE] <= 1050 && radioChannelCommands[YAW] >= 1990 ){
        reset_pid();
        armed = 1;
      }
    }
  
    if( radioChannelCommands[THROTTLE] <= 1050 && radioChannelCommands[THROTTLE] >= 955 && radioChannelCommands[YAW] <= 1010 ){
      delay(500);
      if( radioChannelCommands[THROTTLE] <= 1050 && radioChannelCommands[YAW] <= 1010 ){
        armed = 0;
      }
    }
  
  }
  
  /* Questa funzione Ã¨ strettamente HW dipendente, infatti per rendere il codice piÃ¹ efficiente e rendere l'esperienza di utilizzo superiore *
   * si agisce direttamente al livello di PORT e quindi registri del micorcontrollore; ciÃ² Ã¨ dovuto anche in gran parte alle prestazioni     *
   * limitate del processore ATMEL 328p                                                                                                      */
void write_motor_pulse(){
    
    prevMotorWrite = micros();
    
    PORTB = B00001111; //Attivo il segnale contemporaneamente sui 4 pin 
  
    if(armed == 1){
      while(PORTB > 0 ){
        time_to_spin = micros() - prevMotorWrite;
        
        if( speedVec[MOTOR_1] <= time_to_spin ){ PORTB &= B11111110; }                //stacco il motore 1 se la pulsazione Ã¨ finita
        if( speedVec[MOTOR_2] <= time_to_spin ){ PORTB &= B11111101; }                //stacco il motore 2 se la pulsazione Ã¨ finita
        if( speedVec[MOTOR_3] <= time_to_spin ){ PORTB &= B11111011; }                //stacco il motore 3 se la pulsazione Ã¨ finita 
        if( speedVec[MOTOR_4] <= time_to_spin ){ PORTB &= B11110111; }                //stacco il motore 4 se la pulsazione Ã¨ finita
      }
    }else{
      while(PORTB > 0 ){
        time_to_spin = micros() - prevMotorWrite;
        
        if( NOT_ARMED_PWM_PULSE <= time_to_spin ){ PORTB &= B00000000; }        //stacco i 4 motori contemporaneamente dato che la pulsazione Ã¨ uguale per tutti
      }
    }
  
  }
