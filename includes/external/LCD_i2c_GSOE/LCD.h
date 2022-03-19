//Verwendet SoftwareI2C von:
/* 
 * mbed Library to use a software master i2c interface on any GPIO pins
 * Copyright (c) 2012 Christopher Pepper
 * Released under the MIT License: http://mbed.org/license/mit
 */
//Anpassungen in SoftwareI2C.cpp: _frequency_delay = 3;

#include "mbed.h"
#include "SoftwareI2C.h"    
class lcd
{   
    private:
    uint8_t Adresse;//=0x3F;
    //DigitalOut *ok;
    //DigitalOut *nok;
    //PortOut *po;
    //DigitalIn *t;
    SoftwareI2C *i2c;
    uint8_t wert;
    public:
    lcd(void);
    void clear(void);
    void sendeByte(char b,uint8_t rw, uint8_t rs );
    void sendeNippel(char b,uint8_t rw, uint8_t rs );
    void init(void);
    void cursorpos(uint8_t pos);
    void warte(void);
    int printf(const char *format, ...);


};
