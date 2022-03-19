
#include "mbed.h"
#include "LCD.h"
#include <string>
//#include <cstdio>
//#include <stdarg.h>

// Blinking rate in milliseconds
#define NORMAL_TAKT 500ms
#define SLEEP_TIME 10ms
#define EIN 0
#define AUS 1
#define GEDRUCKT 0

enum zustand{stop,rechts,links};
zustand aktuellerZustand = zustand::stop;

DigitalIn Taster[] = {PA_1, PA_4, PB_0};
DigitalOut leds[] = {PA_5, PA_6, PA_7, PB_6};

lcd mylcd;

PortOut Motor(PortC,0b1111);
//int Motorlauf[]={0b0001,0b0011,0b0010,0b0110,0b0100,0b1100,0b1000,0b1001};
int Motorlauf[]={0b0001,0b0010,0b0011,0b0100,0b0101,0b0110,0b0111,0b1000};
int Position = 0;

std::string letzteMessage = "irgend ein string";
int PositionAlt = 0;


int messageAnzeigen(const char* message, ...) {
    va_list arg;
    va_start(arg, message);
    char messageToPrint[250];
    vsprintf(messageToPrint, message, arg);
    va_end(arg);

    std::string neuMessage = messageToPrint;

    if(neuMessage.compare(letzteMessage) != 0){
        mylcd.clear();
        mylcd.cursorpos(0);    
        mylcd.printf("%s", messageToPrint);
        printf("%s\n", messageToPrint);
        letzteMessage = messageToPrint;

        PositionAlt--;
    }

    if(Position != PositionAlt){
        mylcd.cursorpos(64);
        mylcd.printf("Position: %d", Position);
        PositionAlt = Position;
    }

    // Limitiere die Position zu ein Max Grenze
    int Grenze = 24000;
    if(Position > Grenze || Position < -Grenze) { Position = 0; }
    return 0;
    
}

int do_dreheRechts () {
    Motor = Motorlauf[Position%8]<<0;
    Position++;
    messageAnzeigen("Drehe Rechts");
    ThisThread::sleep_for(SLEEP_TIME);    
    leds[0] = leds[2] = AUS;
    leds[1] = EIN;
    return 0;
}

int do_dreheLinks () {
    Motor = Motorlauf[Position%8]<<0;
    Position--;
    messageAnzeigen("Drehe Links");
    ThisThread::sleep_for(SLEEP_TIME);    
    leds[0] = leds[1] = AUS;
    leds[2] = EIN;
    return 0;
}

int do_stop () {
    Motor = Motorlauf[Position%8]<<0;
    messageAnzeigen("Gestoppt");
    ThisThread::sleep_for(SLEEP_TIME);    
    leds[1] = leds[2] = AUS;
    leds[0] = EIN;
    return 0;
}









/* INIT Methode */
void init(){

    messageAnzeigen("");

    leds[0] = leds[1] = leds[2] = leds[3] = AUS;

    Taster[0].mode(PinMode::PullUp);
    Taster[1].mode(PinMode::PullUp);
    Taster[2].mode(PinMode::PullUp);
}












/*
 ###############################################################
 ###############################################################
        main methode
 ###############################################################
 ###############################################################
*/

int main() {
    init();
    



/*
 ###############################################################
        haupt schleife
 ###############################################################
*/
    while(true) {

        
        switch (aktuellerZustand) {

            case zustand::rechts:
                if (Taster[0] == GEDRUCKT) { aktuellerZustand = zustand::stop;  } 
                else if (Taster[2] == GEDRUCKT) { aktuellerZustand = zustand::links; } 
                else { do_dreheRechts(); }
                break;

            case zustand::links:
                if (Taster[0] == GEDRUCKT) { aktuellerZustand = zustand::stop; } 
                else if (Taster[1] == GEDRUCKT) { aktuellerZustand = zustand::rechts; } 
                else { do_dreheLinks(); }
                break;

            case zustand::stop:
                if (Taster[1] == GEDRUCKT) { aktuellerZustand = zustand::rechts; }
                else if (Taster[2] == GEDRUCKT) { aktuellerZustand = zustand::links; }
                else { do_stop(); }
                break;
        }
        

    }


}