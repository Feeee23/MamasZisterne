#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <EMailSender.h>
#include <EEPROM.h>
#include <string.h>
//////eigene Libs
#include <EigHTML.h>
#include <klassen.h>
#include <Sensor.h>
#include<WiFiCredential.h> //Alle WiFi anmeldungen hier

const char* AdminID="123";
const char* AdminPW="456";
char* Berechtigung="none";
long AdminZeit=0;
char buffer[100];
String Key;

WiFiServer server(80);///AsyncWebServer server(80); //Server instanz, Port?

String header;// Variable to store the HTTP request
unsigned long currentTime = millis();// Current time
unsigned long previousTime = 0; // Previous time
const long timeoutTime = 2000; // Define timeout time in milliseconds (example: 2000ms = 2s)

int j=0; //Sommer/Winterzeit
unsigned int letzteAbfrage =0; //für die Tägliche Abfrage

struct Fuellstand* Liste=NULL; //Liste erstellen

WiFiUDP ntpUDP; //Fuer die Package´s die von den Zeit Server zu holen sind
long utcOffsetInSeconds = 3600; // offset von UTF zeit (zur Witerzeit, zur Sommerzeit erhöhen auf 7200)
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds); //Adresse und Client für die Zeit desfinieren

EMailSender emailSend("girkeszisterne@gmail.com", "PW_ÄNDERN"); //Email
const char* empfaenger = "g.girke@gmx.de";
const char* empfaenger2="girkeszisterne@gmail.com"; //zum selbst emailen
EMailSender::EMailMessage messageVoll;
EMailSender::EMailMessage messageLeer;
EMailSender::EMailMessage messageAnfrage;

const int SpeicherPos=301;
int LED =15;

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  pinMode(LED, OUTPUT);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    analogWrite(LED, HIGH);
    Serial.print(".");
  }
  analogWrite(LED, LOW);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("WiFi Failed!");
    return;
  }
  Serial.println();
  Serial.print("IP Address: "); //IP Adresse ausgeben
  Serial.println(WiFi.localIP());

  EEPROM.begin(SpeicherPos+1); //Dauerspeicher reservieren (20*15+2 byte) 
  timeClient.begin();
  server.begin();
  //Emails Voll/Leer
  messageLeer.subject="Zisterne ist fast leer";
  messageLeer.message="In der Zisterne nur noch weniger als 500l drin.";
  messageVoll.subject="Zisterne ist fast voll";
  messageVoll.message="Die Zisterne ist über 200cm voll";

  if(Liste==NULL){ //Keine Messwerte vorhanden 
    if((EEPROM.read(SpeicherPos))>=1){//check den EEPROM
      getValueEEPROM(SpeicherPos, &Liste);//Einlesen
      KartenRechner(Liste);//geb den Eintägen noch die Liter
    } else{//sonst leere Karte erstellen
      Serial.println("gustav");
      erstellen(&Liste, 0, 0, 0, false, 0);
      EEPROM.write(SpeicherPos, 1);
      EEPROM.commit();
      
    }
  }
}

void loop() {
  if(Liste->Zaehler>1600){ //wenn der Speicher mit Instanzen voll ist Restarte
    ESP.reset();
  }
  timeClient.update();
  
  if(hour(timeClient.getEpochTime())==2 && timeClient.getEpochTime()>(letzteAbfrage+4000)){ //wenn es 2uhr ist und mind 4000s später als die letzte Automatische Abfrage (>1h)
    letzteAbfrage=timeClient.getEpochTime();
    int F=Sensor();// Abfrage starten
    int L=Rechner(F);
    timeClient.update();
    unsigned int Unix=(timeClient.getEpochTime()+j); //Sommer/Winterzeit 
    erstellen(&Liste, F, L, Unix, true, SpeicherPos);
    messageAnfrage.subject="Anfrage Zisternenfüllstand"; //Tägliche email an sich selber damit das Google Konto nocht die Einstellungen ändert
    messageAnfrage.message=EmailAnfrage(Liste);
    EMailSender::Response resp=emailSend.send(empfaenger2, messageAnfrage);
    if (L<500){ //wenn fast leer
      EMailSender::Response resp=emailSend.send(empfaenger, messageLeer);
    } else if (F>200){ //wenn fast voll
      EMailSender::Response resp=emailSend.send(empfaenger, messageVoll);
    } 
}

  WiFiClient client = server.available();
  if (client){
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    currentTime = millis();
    previousTime = currentTime;
    while (client.connected() && currentTime - previousTime <= timeoutTime) { // loop while the client's connected and not timed out
      currentTime = millis();         
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
      
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            delay(1000);
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            if((header.indexOf("logout")>=0 &&strcmp(Berechtigung, "none")!=0)||((timeClient.getEpochTime()-AdminZeit)>300)){
              Berechtigung="none";
              //continue;
            }            
            if (header.indexOf("GET /6/on") >= 0) { //Sommer/Winterzeit button //Wenn Button aktiv -> Sommerzeit
               j=3600;
            }else if(header.indexOf("GET /6/off") >= 0){
              j=0;
            }
            if (header.indexOf("GET /5/on") >= 0) { //Start Button
              int F=Sensor();// Abfrage starten
              int L=Rechner(F);
              timeClient.update();
              unsigned int Unix=(timeClient.getEpochTime()+j); //Sommer/Winterzeit 
              erstellen(&Liste, F, L, Unix, true, SpeicherPos);
            }
            if (header.indexOf("GET /7/on") >= 0){ //email Button
              messageAnfrage.subject="Anfrage Zisternenfüllstand";
              int F=Sensor();// Abfrage starten
              int L=Rechner(F);
              timeClient.update();
              unsigned int Unix=(timeClient.getEpochTime()+j); //Sommer/Winterzeit 
              erstellen(&Liste, F, L, Unix, true, SpeicherPos);
              messageAnfrage.message=EmailAnfrage(Liste);
              EMailSender::Response resp=emailSend.send(empfaenger, messageAnfrage);
            }
            if(header.indexOf("GET /stat/on")>=0){ //Statistik Seite
              client.println(SendStat(getStatValue(Liste)));
              continue; 
            }      
            if(header.indexOf("GET /8/on")>=0){ //Anmelde Seite
              client.println(AnmeldeSeite);
              continue;
            }
            if(header.indexOf("GET /reset")>=0){ //reset über Software
              Serial.println("reset");
              ESP.reset();
            }
            if(header.indexOf("GET /DataDump")>=0){
              client.println("---------------------------Speicher Werte---------------------------\n");
              client.println(getAdminValue(Liste, SpeicherPos));
              client.println("---------------------------Alle Tabellen Werte---------------------------\n");
              client.println(getTabellenWerte(Liste));
              client.println("---------------------------Alle Diagramm Werte---------------------------\n");
              client.println(getDiagramWerte(Liste));
              continue;
            }
            if(header.indexOf("GET /Speicherleeren")>=0){ //EEPROM Speicher leer machen
              Serial.println("Speicher Leeren");
              EEPROM.write(SpeicherPos, 1); // Speicher leeren
              for(int q=0;q<SpeicherPos;q++){
                EEPROM.write(q, 0);
              }
              EEPROM.commit();
            }       
            if(strcmp(Berechtigung, "admin")==0)
            {
              client.println(SendAdminBereich(getAdminValue(Liste, SpeicherPos), header.c_str()));
              continue;
            }else{
              client.println(SendStandartseite(getDiagramWerte(Liste), getTabellenWerte(Liste),j));
            }
            
            String respon; //prüfe Admin eingabe
            respon=client.readString();
            if(respon.indexOf("Benutzer")>=0){
              char buff[100];
              respon.toCharArray(buff,100);
              char* resp=strchr(buff, 61)+1;
              int diff=strchr(resp, 38)-resp;
              char UserN[diff];
              memcpy(UserN, resp, diff);
              char* PW=strchr(resp, 61)+1;
              if(strcmp(PW,AdminPW)==0&&strcmp(UserN, AdminID)==0){
                Berechtigung="admin";
                AdminZeit=timeClient.getEpochTime();
                delay(100);
                continue;
              }
            }
          } else { // if you got a newline, then clear currentLine
              currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
/* ToDo
* Abweichung
* Stat Seite
*/

/*Überprüfen
* Eventuell Gmail nochmal aktivieren
* autoemail höhe
* emailadresse
*/
