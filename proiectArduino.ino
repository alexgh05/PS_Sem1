#include "NVMWrite.h"
#include "TempSensor.h"
#include "PWM.h"
#include <LiquidCrystal.h>

// LCD pin mapping pentru D1 Robot Shield (LCD este pre-conectat la acești pini)
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// Pin Definitions
const int sensorPin = A1;  // Senzor de temperatură conectat pe A1
const int buttonPin = A0;  // Butoane multiplexate pe A0

// Setarea temperaturii (setpoint)
uint16_t setTemperature = 22; // Temperatura țintă
bool selecting = false;

// Variabile pentru gestionarea timpului
unsigned long lastButtonRead = 0;  // Timpul ultimei citiri a butoanelor
const unsigned long buttonInterval = 200;  // Interval pentru debounce (200ms)
unsigned long lastLCDUpdate = 0;  // Timpul ultimei actualizări pe LCD
const unsigned long lcdUpdateInterval = 2000;  // Interval de actualizare LCD (1s)

// Variabile pentru gestionarea timpului total
unsigned long startMillis = 0;  // Momentul de pornire
unsigned long uptime = 0;  // Timpul total scurs (în secunde)

NVMWrite Nvm;
TempSensor Temp;
pwm myPwm;


double eroare = 0, suma_erori = 0, eroare_anterioara = 0, derivativa = 0, output;
double dt = 0.1;  //Timp de  esantionare de 0.1S
double T_set=Nvm.rt_set(), T_inc=Nvm.rt_incalzire(), T_men=Nvm.rt_mentinere(), T_rac=Nvm.rt_racire(), kp=Nvm.rt_K_p(), ki=Nvm.rt_K_l(), kd=Nvm.rt_K_d();
double Temp_interm, pas_crestere, pas_scadere, temp_initial;
char temp_dor[15], temp_cur[15], timpRam[15];
int timpRamas=3000,moving_sp;//timpul ramas si un target spre set temperature care sa nu se regleze direct la acela

float calcTimeToReachTemp(float tempStart, float tempTarget, float totalTime) {
  // Dacă temperatura țintă este mai mică sau egală cu temperatura de început, returnează 0
  if (tempTarget <= tempStart) return 0; 

  float tempDifference = tempTarget - tempStart;
  
  // Definirea ratei de schimbare a temperaturii pentru creștere
  float tempRate = (30.0 - 25.0) / 6.0;  // Creșterea temperaturii per secundă
  
  // Dacă temperatura țintă este mai mică decât temperatura de start (scădere), dublăm timpul
  if (tempTarget < tempStart) {
    tempRate = (25.0 - 20.0) / 6.0; // Definirea ratei de scădere a temperaturii
    return (tempDifference / tempRate) * 2;  // Dublăm timpul pentru scădere
  }
  
  // Calculul pentru creștere
  return tempDifference / tempRate;    
}

void setup() {
  Serial.begin(9600);
  Nvm.wK_d(0.1);
  Nvm.wK_l(0.1);
  Nvm.wK_p(0.1);
  if (Nvm.rt_incalzire() <= 0) {
    Nvm.wt_incalzire(300);
    Serial.println("T_incalzire set to default (300)");
}

if (Nvm.rt_mentinere() <= 0) {
    Nvm.wt_mentinere(500);
    Serial.println("T_mentinere set to default (5000)");
}

if (Nvm.rt_racire() <= 0) {
    Nvm.wt_racire(300);
    Serial.println("T_racire set to default (300)");
}

  // Citire valori EEPROM
  T_inc = Nvm.rt_incalzire();
  T_rac = Nvm.rt_racire();
  T_men = Nvm.rt_mentinere();
  myPwm.init();
  lcd.begin(16, 2);
  lcd.print("Initializing...");
  delay(2000);
  lcd.clear();
  // Configurare pini pentru bec și ventilator
  DDRD|=(1<<3);
  DDRD|=(1<<2);
   lcd.print("Device is On");
  delay(2000);
  T_inc=Nvm.rt_incalzire();
  T_rac=Nvm.rt_racire();
  T_men=Nvm.rt_mentinere();
 
}

  void loop(){
  unsigned long currentMillis = millis();
  uint16_t temperature= Temp.get_temp(1);

// Pornim becul și ventilatorul
if (temperature < setTemperature){
  PORTD|=(1<<3); // Pornim ventilatorul
  PORTD&=~(1<<2);
}
else if(temperature > setTemperature)
  {
    PORTD|=(1<<2);
    PORTD&=~(1<<3);
  }
  else
  {
    PORTD&=~(1<<2);
    PORTD&=~(1<<3);
  }


  // Actualizare LCD la intervale regulate
  if (currentMillis - lastLCDUpdate >= lcdUpdateInterval) {
    lastLCDUpdate = currentMillis;
    updateLCD(temperature);
  }

  // Gestionare butoane la intervale regulate
  if (currentMillis - lastButtonRead >= buttonInterval) {
    lastButtonRead = currentMillis;
    handleButtons();
  }
}

// Funcție pentru actualizarea LCD-ului
void updateLCD(float temperature) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temperature);
  lcd.print((char)176); // Simbol pentru °C
  lcd.print("C   ");

  // Calculăm uptime-ul și timpul rămas
  uptime = (millis() - startMillis) / 1000; // Convertim uptime-ul în secunde
  

  lcd.setCursor(0, 1); // Mutăm pe linia a doua

 if (temperature < setTemperature) {
  lcd.print("TInc: ");
  timpRamas = calcTimeToReachTemp(temperature, setTemperature, T_inc);
  moving_sp = temperature * (T_inc - timpRamas) / T_inc;
} else if (temperature == setTemperature) {
  lcd.print("TMen: ");
  timpRamas = T_men - uptime;
} else if (temperature > setTemperature) {
  lcd.print("TRac: ");
  timpRamas = calcTimeToReachTemp(setTemperature, temperature, T_rac);
  moving_sp = temperature - temperature * (T_rac - timpRamas) / T_rac;
}


  // Afișăm timpul rămas în format MM:SS
  int min = timpRamas / 60;
  int sec = timpRamas % 60;
  lcd.print(min);
  lcd.print("m:");
  lcd.print(sec);
  lcd.print("s   ");
}



// Funcție pentru gestionarea butoanelor
void handleButtons() {
  int value = analogRead(buttonPin);

  // Detectăm intervalul valorilor pentru fiecare buton
  if (value < 50) {
    // Buton Dreapta
    if (selecting) setTemperature += 0.5;
  } else if (value < 200) {
    // Buton Sus
    if (selecting) setTemperature += 1.0;
  } else if (value < 400) {
    // Buton Jos
    if (selecting) setTemperature -= 1.0;
  } else if (value < 600) {
    // Buton Stânga
    if (selecting) setTemperature -= 0.5;
  } else if (value < 800) {
    // Buton Select
    selecting = !selecting;  // Comutăm între moduri
  }

  // Actualizare afișaj în modul de selectare
  if (selecting) {
    lcd.setCursor(0, 1);
    lcd.print("Set Temp: ");
    lcd.print(setTemperature);
    lcd.print((char)176);
    lcd.print("C   ");
  }
}

void calculPID_si_scrierePWM(float temperature) {
  eroare = Temp_interm - temperature;                              // Calculează eroarea
  suma_erori += eroare * dt;                                       // Integrarea erorii
  derivativa = (eroare - eroare_anterioara) / dt;                  // Derivata erorii
  output = (kp * eroare) + (ki * suma_erori) + (kd * derivativa);  // Calculul final al PID

  // Limităm output-ul să nu depășească intervalul 0-255
  if (output > 255) output = 255;
  else if (output < 0) output = 0;


  // Salvăm eroarea curentă ca fiind ultima pentru următoarele calcule ale output-ului
  eroare_anterioara = eroare;

  delay(1);  // Pauză pentru a permite citirea serialului

  // Setăm PWM-ul
  myPwm.setDC(output);
}
