#ifndef PWM_H
#define PWM_H

class pwm{  
  public:
  
  void init(){
  set_OUTPUT_pB(11); 
   //Configuram timer 2, prescalar de 8, non invertor si mod fast PWM
  TCCR2A |= (1 << WGM21) | (1 << WGM20) | (1 << COM2A1); 
  TCCR2B |= (1 << CS21);
  }
  pwm () {};
  void setDC(int dutyCycle){
    OCR2A  = dutyCycle;
  }
};


#endif