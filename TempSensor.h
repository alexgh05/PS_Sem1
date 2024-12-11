#ifndef TempSensor_H
#define Tempsensor_H
#include<avr/io.h>
#include"MacroAndFunc.h"

class TempSensor
{
private:


public:

void init()
{
  DDRC|=(1<<1);
set_INPUT_A0();

    //Setam tensiunea de referinta ca fiind cea interna 5V
    ADMUX = (ADMUX & ~(1 << REFS1)) | (1 << REFS0);

    //Setam prescalar 128 => Frecv 125KHz si dam enable la CAN
    ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0) | (1 << ADEN);

}



uint16_t get_temp(uint8_t ch)
{
uint16_t nr_masuratori=0;
uint32_t sum=0;
  while(nr_masuratori<30)
  {
        ADMUX = (ADMUX & 0xF0 | ch & 0x0F);
        ADCSRA |= (1 << ADSC);
         while (ADCSRA & (1 << ADSC));


  float temperature = (ADC * 5.0 / 1023.0) * 100;
  nr_masuratori++;
  sum=sum+temperature;
  }
  uint16_t temperatureC=(uint16_t)(sum/nr_masuratori);
  return temperatureC;
}

};
#endif