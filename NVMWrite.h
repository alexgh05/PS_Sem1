#ifndef NVMWrite_H
#define NVMWrite_H

#include<EEPROM.h>
#include "MacroAndFunc.h"

class NVMWrite{

  private:
  double t_set,t_incalzire, t_mentinere, t_racire, K_p, K_l, K_d;

  public:
 NVMWrite() {
    EEPROM.get(T_set_adr, t_set);
    if (t_set < 0 || t_set > 40) t_set = 22; // Valoare implicită

    EEPROM.get(T_incalzire_adr, t_incalzire);
    if (t_incalzire <= 0 || t_incalzire > 100000) t_incalzire = 3000;

    EEPROM.get(T_mentinere_adr, t_mentinere);
    if (t_mentinere <= 0 || t_mentinere > 100000) t_mentinere = 5000;

    EEPROM.get(T_racire_adr, t_racire);
    if (t_racire <= 0 || t_racire > 100000) t_racire = 3000;

    EEPROM.get(K_P_adr, K_p);
    if (K_p < 0 || K_p > 10) K_p = 0.1;

    EEPROM.get(K_L_adr, K_l);
    if (K_l < 0 || K_l > 10) K_l = 0.1;

    EEPROM.get(K_D_adr, K_d);
    if (K_d < 0 || K_d > 10) K_d = 0.1;
}


  void wt_set(double value) { EEPROM.put(T_set_adr, value); 
  
  }
  void wt_incalzire(double value) { EEPROM.put(T_incalzire_adr, value); }
  void wt_mentinere(double value) { EEPROM.put(T_mentinere_adr, value); }
  void wt_racire(double value) { EEPROM.put(T_racire_adr, value); }
  void wK_p(double value) { EEPROM.put(K_P_adr, value); }
  void wK_l(double value) { EEPROM.put(K_L_adr, value); }
  void wK_d(double value) { EEPROM.put(K_D_adr, value); }

  double rt_set(){return t_set;}
  double rt_incalzire(){return t_incalzire;}
  double rt_mentinere(){return t_mentinere;}
  double rt_racire(){return t_racire;}
  double rt_K_p(){return K_p;}
  double rt_K_l(){return K_l;}
  double rt_K_d(){return K_d;}
};

#endif