// Member B: the four buttons K1-K4 on the OLED module
//   K1 UP   : manual mode duty +10 %, PID mode target +100 RPM
//   K2 DOWN : manual mode duty -10 %, PID mode target -100 RPM
//   K3 MODE : change mode (MANUAL -> PID -> MANUAL)
//   K4 PAGE : change OLED page

#ifndef BUTTON_MODULE_H
#define BUTTON_MODULE_H

void initButtons();    // call once in setup()
void checkButtons();   // call in every loop()

#endif
