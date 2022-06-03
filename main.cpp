
#include "mbed.h"
#include <string>

// ZEIT STEURUNG
// #define NORMAL_TAKT 500ms // Zeit in Milisekunden
#define SLEEP_TIME 2ms // Zeit in Milisekunden zwischen Motortakt

// ENUM FÜR PROGRAM ZUSTÖNDE
enum zustand{stop,rechts,links};
// ENUM INSTANCE FÜR AKTIVE ZUSTAND
zustand aktuellerZustand = zustand::stop;

// ALLE TASTER AN MF-SHIELD IN ARRAY
DigitalIn Taster[] = {PA_1, PA_4, PB_0};
// ALLE LEDS AN MF-SHIELD
DigitalOut leds[] = {PA_5, PA_6, PA_7, PB_6};
const int EIN = 0;  // LED ZUSTAND AN
const int AUS = 1;  // LED ZUSTAND AUS
const int GEDRUCKT = 0; // TASTER ZUSATND GEDRUCKT


// PINOUT FÜR MOTOR, WIR NUTZEN PORT-C UND DAVON DIE PINS 7 BIS 11
PortOut Motor(PortC,0b111100000000);
// MOTOR STEPPING ALS ARRAY, BINÄR SIGNALE
int Motorlauf[]={0b0001,0b0011,0b0010,0b0110,0b0100,0b1100,0b1000,0b1001};
// MOTOR SCHRITTE
int Position = 0;
// Falls der Zahl zu groß oder klein wird, soll es zurückgesetzt werden, bevor die INT grenze erreicht werden kann
int Reset_Position_After = 2147483000; // int Limits: -2147483648 bis 2147483647
// VERSCHIEBUNG VON BITS (falls wir nicht die anfang pins verwenden und nicht alle pins in unsere array eingeben wollen)
int Verschiebung = 8;


int doDreheRechts () {
    Motor = Motorlauf[Position%8]<<Verschiebung;
    Position++;
    ThisThread::sleep_for(SLEEP_TIME);    
    leds[0] = leds[2] = AUS;
    leds[1] = EIN;
    return 0;
}

int doDreheLinks () {
    Motor = Motorlauf[Position%8]<<Verschiebung;
    Position--;
    ThisThread::sleep_for(SLEEP_TIME);    
    leds[0] = leds[1] = AUS;
    leds[2] = EIN;
    return 0;
}

int doStop () {
    Motor = Motorlauf[Position%8]<<Verschiebung;
    ThisThread::sleep_for(SLEEP_TIME);    
    leds[1] = leds[2] = AUS;
    leds[0] = EIN;
    return 0;
}



/* INIT Methode */
void init(){

    // alle LEDs ausschalten
    // LED-4 wird gleich Aus/1 gesetzt und die Wert wird für alle LEDs übernommen
    leds[0] = leds[1] = leds[2] = leds[3] = AUS;

    // Taster-PinMode setzen, optional für Taster an MF-Shield
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

        // Wenn Position is gorßer/gleich Reset_Position_After ORDER Position ist kleiner als minus Reset_Position_After
        // Setze das Position zu 0 
        // Position = (Position >= Reset_Position_After || Position < -1 * Reset_Position_After) ? 0 : Position;
        if(Position >= Reset_Position_After || Position < -1 * Reset_Position_After) {
            Position = 0;
        }

        switch (aktuellerZustand) {

            case zustand::rechts:
                // da die If-Schleife :) jeweils nur eine Anweisung hat, wird verkurzt geschrieben
                // bei eine Anweisung nach der IF, die Klammern {} sind auch optional
                if (Taster[0] == GEDRUCKT) aktuellerZustand = zustand::stop;
                else if (Taster[2] == GEDRUCKT) aktuellerZustand = zustand::links; 
                else doDreheRechts();
                break;

            case zustand::links:
                // es ist aber trotzdem besser immer die Klammern {} zu verwenden
                if (Taster[0] == GEDRUCKT) { aktuellerZustand = zustand::stop; } 
                else if (Taster[1] == GEDRUCKT) { aktuellerZustand = zustand::rechts; } 
                else { doDreheLinks(); }
                break;

            case zustand::stop:
                if (Taster[1] == GEDRUCKT) { aktuellerZustand = zustand::rechts; }
                else if (Taster[2] == GEDRUCKT) { aktuellerZustand = zustand::links; }
                else { doStop(); }
                break;
        }
        

    }


}