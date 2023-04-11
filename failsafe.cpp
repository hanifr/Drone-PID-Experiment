/*
motors_synch() is a function that contains a while loop that 
waits until the difference between the current value of micros() 
and the previous value of prevMotorWrite is greater than or 
equal to 4000. It appears that this function is intended to 
synchronize the motors.

show_alarm() is a function that turns on the alarm LED for 
500 milliseconds, turns it off for 500 milliseconds, and 
then returns. It seems to be a simple way of indicating 
that an alarm has been triggered.
*/
void motors_synch(){
  
    while(micros() - prevMotorWrite < 4000){};
     
  }
  
  void show_alarm(){
    
    digitalWrite(ALARM_LED, HIGH);
    delay(500);
    digitalWrite(ALARM_LED, LOW);
    delay(500);
       
  }
  