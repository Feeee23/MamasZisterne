#include <Arduino.h>
#include <string.h>
#include <TimeLib.h>
#include <EEPROM.h>

struct Fuellstand { //Struct erstellen
    int hoehe;
    int liter;
    int unix;
    int Zaehler;
    struct Fuellstand* next;
};

void erstellen(struct Fuellstand** Liste, int F, int L, unsigned int Unix, bool Speichern, int SpeicherPos){ //Element erstellen
    void *speicher=NULL;
    struct Fuellstand* F1 ;
    //Speichern=false;/////////////////////////////////Speichern deaktivieren
    speicher=malloc(sizeof(struct Fuellstand));
    F1=(struct Fuellstand*) speicher;
    F1->hoehe=F;
    F1->liter=L;
    F1->unix=Unix;
    if(*Liste==NULL){
        F1->next=NULL;
    } else
    {
        F1->next=*Liste;
    }
    *Liste=F1;
    if(F1->next!=NULL){ //Gib jedem eine Unike Nummer
        F1->Zaehler=F1->next->Zaehler+1;
    } else{ //wenn erster eintrag
        F1->Zaehler=1;
    }
    if(Speichern){//die Werte auch noch in den EEPROM schreiben
        int Pos=(EEPROM.read(SpeicherPos)-1)*15; //lese die aktuelle Speicherpos aus
        int i=0;
        char buffer[20];
        itoa(F1->hoehe, buffer, 10);//itoa
        for(i=0; i<=4; i++){
            EEPROM.write(Pos+i, buffer[i]);   //scheibe die höhe dahin
        }
        itoa(F1->unix, buffer, 10);//itoa
        for(i=5; i<15; i++){
            EEPROM.write(Pos+i,buffer[i-5]);   //scheibe UNIX 5 stellen weiter
        }
        EEPROM.write(SpeicherPos,EEPROM.read(SpeicherPos)+1); //erhöhe die Speicherpos um einen eintrag
        if(EEPROM.read(SpeicherPos)-1==21){ //Überlauf
            EEPROM.write(SpeicherPos, 1);
        }
        EEPROM.commit();
    }
}

String getDiagramWerte(struct Fuellstand* Liste ){ //Element ausgeben, bisher noch falschrum links das neuste
    String s;
    struct Fuellstand* F;
    F=Liste;
    int Anzahl=20;
    int i=0;
    int liter[Anzahl];
    unsigned int unix[Anzahl];
    for(i=0; i<Anzahl; i++){ //speicher alle werte umgegkehrt in Arrays damit die Ausgabe richtig rum ist
        liter[Anzahl-1-i]=F->liter;
        unix[Anzahl-1-i]=F->unix;
        F=F->next;
    }
    for(i=0; i<Anzahl; i++){
        String k="['";
        if(day(unix[i])<10){
            k+=0;
        }
        k+=day(unix[i]);
        k+=".";
        if(month(unix[i])<10){
            k+=0;
        }
        k+=month(unix[i]);
        k+=".";
        k+=year(unix[i]);
        k+="', ";
        k+=liter[i]; //Liter
        k+="],\n";
        s+=k;
        //Zielformat['19.09.2020', 200],
    }
    return s;
}
String getTabellenWerte(struct Fuellstand* Liste ){ //Element ausgeben
    String s;
    struct Fuellstand* F;
    F=Liste;
    while(F->next!=NULL){
        String k="<tr>\n    <td style='border-top:hidden;border-bottom:hidden;border-left:hidden;'> </td>\n   <td> ";
        if (day(F->unix)<10){
            k+=0;
        }
        k+=day(F->unix); //Datum
        k+=".";
        if (month(F->unix)<10){
            k+=0;
        }
        k+=month(F->unix);
        k+=".";
        if (year(F->unix)<10){
            k+=0;
        }
        k+=year(F->unix);
        k+=" ";
        if (hour(F->unix)<10){
        k+=0;
        }
        k+=hour(F->unix);//Uhrzeit
        k+=":";
        if (minute(F->unix)<10){
        k+=0;
        }
        k+=minute(F->unix);
        k+="</td>\n     <td>";
        k+=F->hoehe; //höhe
        k+="cm</td>\n   <td>";
        k+=F->liter; //Liter
        k+="l</td>\n    <td>";
        k+=F->liter-F->next->liter; //delta zum vortag
        k+="l</td>\n    <td style='border-top:hidden;border-bottom:hidden;border-right:hidden;'> </td></tr>\n";
        s+=k;
        //Zielformat
//                <tr>
//                    <td style='border-top:hidden;border-bottom:hidden;border-left:hidden;'> </td>
//                    <td>19.09.2020 11:23</td>
//                    <td> 135cm</td>
//                    <td>4963l</td>
//                    <td>150l</td> //delta Vortag
//                    <td style='border-top:hidden;border-bottom:hidden;border-right:hidden;'> </td>
//                </tr>
        F=F->next;
    }
    return s;
}

String EmailAnfrage(struct Fuellstand* Liste){
    String s= "Hallo, die Zisterne hat aktuell einen Füllstand von: ";
    s+=Liste->hoehe;
    s+="cm, das sind ca.: ";
    s+=Liste->liter;
    s+="Liter. Die Abfrage war am ";
    if (day(Liste->unix)<10){
        s+=0;
    }
    s+=day(Liste->unix); //Datum
    s+=".";
    if (month(Liste->unix)<10){
        s+=0;
    }
    s+=month(Liste->unix);
    s+=".";
    if (year(Liste->unix)<10){
        s+=0;
    }
    s+=year(Liste->unix);
    s+=" ";
    if (hour(Liste->unix)<10){
        s+=0;
    }
    s+=hour(Liste->unix);//Uhrzeit
    s+=":";
    if (minute(Liste->unix)<10){
        s+=0;
    }
    s+=minute(Liste->unix);

    return s;
}

int AnzahlMesswerte(struct Fuellstand* Liste){// gibt die Anzahl der Messwerte zurück //ungenutzt
     struct Fuellstand* F;
     F=Liste;
     int i=0;
     while (F->next!=NULL){
         i++;
         F=F->next;
     }
     return i;
}

void getValueEEPROM(int SpeicherPos, struct Fuellstand** Liste){
    int Z=EEPROM.read(SpeicherPos);//-1; //Schaue an welcher angefangen werden muss zu lesen. 0 bedeutet leer
    int Sprung =15; //jeder eintrag benötigt 15byte
    int AktuellePos=(Z-1)*Sprung; 
    //Serial.println(AktuellePos);
    int i=0;
    int k=0;//zählt wie oft schon gelesen wurde
    char buff1[20];
    char buff2[20];
    
    for(k=0; k<20; k++){ //gehe zu jedem Eintrag
        AktuellePos=AktuellePos+Sprung; //eins hinter dem neusten müsste das älteste Stehen
        //Serial.println("Günter");
        //Serial.println(AktuellePos);
        if(AktuellePos>=SpeicherPos){ //evtl überlauf
            AktuellePos=0;
            //Serial.println("T1");
        }
        /*if(char(EEPROM.read(AktuellePos))==NULL){ //wenn nix mehr drinsteht geh zum nächsten
            Serial.println("T2");
            continue;
        }*/
        for(i=0; i<=4; i++){
            buff1[i]=char(EEPROM.read(AktuellePos+i)); //lese die höhe aus
            //Serial.print(buff1);
        }
        for(i=5; i<15; i++){
            buff2[i-5]=char(EEPROM.read(AktuellePos+i)); //lese UNIX aus
            //Serial.print(buff2);
        }
        int h=atoi(buff1);//mach ints aus den chars
        int UNIX=atoi(buff2); 
        erstellen(Liste,h, 0, UNIX, false, 0);//erstelle eine karte   
            
    }
}

String getAdminValue(struct Fuellstand* Liste, int SpeicherPos){
    String s;
    s+="<p>Letzte Abfrage: ";
    s+=Liste->hoehe;
    s+="cm</p>";
    s+="<p>Entspricht: ";
    s+=Liste->liter;
    s+="Litern</p>";
    s+="<p>UNIX: ";
    s+=Liste->unix;
    s+="</p>";
    s+="<p>Anzahl: ";
    s+=Liste->Zaehler;
    s+="</p><br>";
    int i=0;
    s+="<p>SpeicherPos, SpeicherWert</p>";
    for(i=0; i<=SpeicherPos; i++){
        if(i==0){
            s+="<p>-----1-----</p>";
        }
        s+="<p>";
        s+=i;
        s+=" , ";
        s+=EEPROM.read(i);
        s+="</p>";
        if(((i+1)%15)==0){
            s+="<p>-----";
            s+=((i+1)/15)+1;
            s+="-----</p>";
        }
    }
    
    return s;
    /* Ziel
        <p>Letzte Abrage:</p>
        <p>Entspricht Litern</p>
        <p>UNIX</p>
        <p>Anzahl</p>
        <p>Die Verschiebung betraegt</p>
    */
}

String getDurchschnitt(struct Fuellstand* Liste, int Fall){ //berechnet die durschnittlilche füllung und Verbrauch
    String s;//="unbekannt"; 
    struct Fuellstand* F1;
    struct Fuellstand* F2;
    F1=Liste;
    ////////////////////////////////////////////
    int SumHinzu=0;
    int SumWeg=0;
    int AnzahlHinzu=0;
    int AnzahlWeg=0;
    F2=F1->next;
    while(F2->next!=NULL){
        if (hour(F1->unix)>0&&hour(F1->unix)<4){ //teste uhrzeit von F1
            if (hour(F2->unix)>0&&hour(F2->unix)<4){ //teste uhrzeit von F2
                if(F1->liter-F2->liter>0){ //wasser hinzu
                    SumHinzu+=F1->liter-F2->liter>0;
                    AnzahlHinzu++;
                } else if (F1->liter-F2->liter<0){ // wasser weg
                    SumWeg+=-(F1->liter-F2->liter); //negieren um positiven wert zu erhalten
                    AnzahlWeg++;
                } //sonderfall Wasser bleibt coonstant ist egal
                F1=F2; //weiter setzten der Variiabeln
                F2=F2->next;
                continue;                
            }else{
                F2=F2->next; //setzte F2 eins weiter
                continue;
            }
        } else{ //setzte F1 auf den nächsen und f2 auch und versuche es nochmal
            F1=F2;
            F2=F2->next;
            continue;
        }   
    }
    switch (Fall) { //Ausgabe
        case 1: //Verbrauch
            if(AnzahlWeg>0){
                s+=SumWeg/AnzahlWeg;
                break; 
            }else{
                s+="keine Angabe moeglich ";
                break;
            }
        case 2://Regen
            if(AnzahlHinzu>0){
                s+=SumHinzu/AnzahlHinzu;
                break; 
            }else{
                s+="keine Angabe moeglich ";
                break;
            }
        case 3:s+=SumWeg;break; //Gesamt Verbrauch
        case 4:s+=SumHinzu;break; //Gesammt Regen
    }
    return s;
}

String getStatValue(struct Fuellstand* Liste){ //bietet die Daten für die Stat website dar

    String s;
    s+="<p>letzte Abfrage: ";
    if (day(Liste->unix)<10){
        s+=0;
    }
    s+=day(Liste->unix); //Datum
    s+=".";
    if (month(Liste->unix)<10){
        s+=0;
    }
    s+=month(Liste->unix);
    s+=".";
    if (year(Liste->unix)<10){
        s+=0;
    }
    s+=year(Liste->unix);
    s+=" ";
    if (hour(Liste->unix)<10){
        s+=0;
    }
    s+=hour(Liste->unix);//Uhrzeit
    s+=":";
    if (minute(Liste->unix)<10){
        s+=0;
    }
    s+=minute(Liste->unix);
    s+="</p>\n <p>Es gibt aktuell ";
    s+=Liste->Zaehler;
    s+=" Messanfragen.</p>\n <p>Durchschnittlich werden am Tag ";
    s+=getDurchschnitt(Liste, 1);//"unbekannt"; //hier noch
    s+="l entnommen.</p>\n <p>Wenn es Regnet dann kommen durchschnittlich ";
    s+=getDurchschnitt(Liste, 2);//"unbekannt";//hier noch
    s+="l am Tag hinzu.\n <p>Insgesamt wurden ";
    s+=getDurchschnitt(Liste, 3); //gesammt entnommen
    s+="l entnommen.</p>\n <p>Insgesamt hat es ";
    s+=getDurchschnitt(Liste, 4); //gesammt geregnet
    s+="l geregnet.</p>\n </p></body></html>";
    return s;
}

