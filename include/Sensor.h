#include <Arduino.h>
#include <SR04.h>

int Trig=5; //Entspricht D1
int Echo=4; //Entspricht D2 
SR04 sr04= SR04(Trig, Echo);

int Sensor(){                            //Schuckt den Sensor an
    int A = sr04.Distance(); //Messe dreimal hintereinander um Feler zu minimiern
    int B = sr04.Distance();
    int C = sr04.Distance();
    int D = (A + B + C) / 3; //Durchschnitt der 3 Werte ermitteln
    int F = 190 - D; //Die Höhe des Sensors (240cm) minus den Messwert ergibt den Füllstand
    if(F<0){
        F=0;
    }
    return F;
}

int Rechner(int FS){
    float l=0.0;
    if (FS<140){ //under 1,4m ist die Zisterne Zylindrisch
        l=100*3.1415*FS/10;
    }else{ //Darueber Kegelstumpf
    l=4400+(FS/10-14)*314.15-36.65*(FS/10-14)*(FS/10-14)+1.43*(FS/10-14)*(FS/10-14)*(FS/10-14); //max wert Zylindrsich plus Zylinderstumpf formel (r2 durch h ausgedrueckt) !!!Achtung Rundungen auf Dezimeter verfälschen das Ergebniss
    //l=4400+((FS-140)*31415-366.5*(FS-140)*(FS-140)+1.43*(FS-140)*(FS-140)*(FS-140))/1000; // genauer?
    }
    return (int) l; //Literanzahl zurueckgeben                            //rechnet die Liter aus
}

void KartenRechner(struct Fuellstand* Liste){ //Rechnet für alle karten die Liter aus
    struct Fuellstand* F;
    F=Liste;
    while(F->next!=NULL){
        F->liter=Rechner(F->hoehe);
        F=F->next;
    }
}