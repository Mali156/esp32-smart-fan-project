// Member B: OLED display (SSD1306, 128x64)

#ifndef DISPLAY_MODULE_H
#define DISPLAY_MODULE_H

void initDisplay();     // call once in setup()
void updateDisplay();   // call every 200 ms
void nextPage();        // switch between page 1 (fan) and page 2 (environment)

#endif
