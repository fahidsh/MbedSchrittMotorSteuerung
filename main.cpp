
#include "mbed.h"
#include "LCD.h"
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

// LCD CONTROLLER MIT NAME mylcd
lcd mylcd;

// PINOUT FÜR MOTOR, WIR NUTZEN PORT-C UND DAVON DIE PINS 7 BIS 11
PortOut Motor(PortC,0b111100000000);
// MOTOR STEPPING ALS ARRAY, BINÄR SIGNALE
int Motorlauf[]={0b0001,0b0011,0b0010,0b0110,0b0100,0b1100,0b1000,0b1001};
// MOTOR SCHRITTE
int Position = 0;
// VERSCHIEBUNG VON BITS (falls wir nicht die anfang pins verwenden und nicht alle pins in unsere array eingeben wollen)
int Verschiebung = 8;

// PLATZHALTER FÜR LETZTE NACHRICHT, DER AUF LCD ANGEZEIGT WIRD
std::string letzteMessage = "irgend ein string";
// LETZTE TAKT NUMMER, (wird benutzt um Taktnummer auf LCD anzugeigen)
int PositionAlt = 0;

/**
    Dieser mehode wird verwendet ein Meldeung/ Nachricht anzuzeigen
    eine nachricht wird an dieser Methode gegeben und dann wird von hier gesteuert, wo und wir diese Nachricht
    angezeigt wird.
*/
int messageAnzeigen(const char* message, ...) {
    va_list arg;
    va_start(arg, message);
    // maximal Nachricht Lange, printf style formatierung möglich
    char messageToPrint[250];
    // formatierte Nachricht in eine Char Array übernehmen
    vsprintf(messageToPrint, message, arg);
    va_end(arg);
    // das nachricht von Char-Array in string übernehmen, für das string Vergleich
    std::string neuMessage = messageToPrint;
    // wenn der alte Nachricht in letzteMessage nicht gleich wie jetzige Nachricht neuMessage ist
    if(neuMessage.compare(letzteMessage) != 0){
        mylcd.clear();
        mylcd.cursorpos(0);
        // zeige das Nachricht auf LCD
        mylcd.printf("%s", messageToPrint);
        // druck das Nachricht in Serial Console
        printf("%s\n", messageToPrint);
        // weil das neuMassage jetzt angezeigt worden ist, alte Nachricht in letzteMessage übernehmen
        letzteMessage = messageToPrint;
        // diese ist nutzlich um der takt/poisition wert anzuzeigen wenn stop gedruckt wird
        PositionAlt--;
    }
    // zeige das aktuelle takt/Position in zweite zeile of lcd
    // nur wann der takt zahl sich geändert hat
    if(Position != PositionAlt){
        mylcd.cursorpos(64);
        mylcd.printf("Position: %d", Position);
        PositionAlt = Position;
    }

    // Limitiere die Position variable zu ein Max Grenze
    int Grenze = 24000;
    // wenn der Position variable zu groß/klein geworden ist, setze es gleich 0
    if(Position > Grenze || Position < -Grenze) { Position = 0; }
    return 0;
    
}

int doDreheRechts () {
    Motor = Motorlauf[Position%8]<<Verschiebung;
    Position++;
    messageAnzeigen("Drehe Rechts");
    ThisThread::sleep_for(SLEEP_TIME);    
    leds[0] = leds[2] = AUS;
    leds[1] = EIN;
    return 0;
}

int doDreheLinks () {
    Motor = Motorlauf[Position%8]<<Verschiebung;
    Position--;
    messageAnzeigen("Drehe Links");
    ThisThread::sleep_for(SLEEP_TIME);    
    leds[0] = leds[1] = AUS;
    leds[2] = EIN;
    return 0;
}

int doStop () {
    Motor = Motorlauf[Position%8]<<Verschiebung;
    messageAnzeigen("Gestoppt");
    ThisThread::sleep_for(SLEEP_TIME);    
    leds[1] = leds[2] = AUS;
    leds[0] = EIN;
    return 0;
}









/* INIT Methode */
void init(){

    messageAnzeigen("");

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