#pragma once

#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFi.h>

#define ONE_WIRE_BUS 4   // Data pripojene na D4
#define LDR 34           // Fotoodpor pripojeny na D34

volatile bool Casovac=true;
float POZ_TEPLOTA=25.0;

const char* ssid     = "DodoESP32";
const char* password = "Abcd1234";
WiFiServer server(4321);

struct SnimaceTmp {
  float TeplotaOkruhu;
  DeviceAddress Snimac;   
} ;

struct Rele{
  bool Stav;
  bool Manual;
  int  GPIO;
} ;

SnimaceTmp PoleSnimaceTmp[3]={{0,{ 0x28, 0xAC, 0x2C, 0x1E, 0x0E, 0x00, 0x00, 0x3F}},{0,{ 0x28, 0x3D, 0x10, 0x20, 0x0E, 0x00, 0x00, 0x41}},{0,{ 0x28, 0x97, 0xD7, 0x1E, 0x0E, 0x00, 0x00, 0xFC}}};
Rele PoleRele[4]={{false,true,27},{false,true,26},{false,true,25},{false,true,33}};
int Svetlo;

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

//Casovac
hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

// Variable to store the HTTP request
String header;

void IRAM_ATTR onTimer() {
  portENTER_CRITICAL_ISR(&timerMux);
    Casovac=true;
  portEXIT_CRITICAL_ISR(&timerMux);
}

void PosliHlavicku(WiFiClient client) { 
 client.println("<!DOCTYPE HTML><html>");
 client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">");
 client.println("<style>html {font-family: Arial; display: inline-block; margin: 0px auto; text-align: center;}");
 client.println("h2 { font-size: 3.0rem; }");
 client.println("p { font-size: 3.0rem; }");
 client.println(".units {font-size: 1.2rem; }");
 client.println(".ds-labels{font-size: 1.5rem; vertical-align:middle; padding-bottom: 15px;}");
 client.println(".myDiv {  border: 5px outset red;   background-color: lightblue; 	  text-align: center; } </style>");
 client.println("<script language=\"Javascript\" type=\"text/javascript\"> function gotolink(a) \{ window.location = a;\}</script> </head>");
};

void Zobraz_stranku(WiFiClient client){
    // Display the HTML web page
    PosliHlavicku(client);
    client.println("<body><h2>ESP32 Teploty / Ventily</h2>");
    for (int i=0;i<4;i++){
       if (i<3){
        client.println("<p><span class=\"ds-labels\">Okruh "+String(i+1)+":</span> <span>"+  String(PoleSnimaceTmp[i].TeplotaOkruhu,2) +"</span><sup class=\"units\">&deg;C</sup>");
        Serial.println(PoleSnimaceTmp[i].TeplotaOkruhu);        
       }
       else
       {  //Fotoodpor
        client.println("<p><span class=\"ds-labels\">Osvetlenie:</span>");
        client.println("<span>"+ String(Svetlo) +"</span>");
       }
      
       client.println("<span class=\"ds-labels\">, Stav: "+ String((PoleRele[i].Stav) ?"ON":"OFF")  +"</span></p>");
       Serial.println("Rele: "+String(i));
       Serial.println(PoleRele[i].Manual);
       Serial.println(PoleRele[i].Stav);
       client.println("<div class=\"myDiv\">");
       client.println("<p><input class=\"button\" type=\"radio\" name=\"toggle"+String(i)+"\" id=\"auto\" "+String((PoleRele[i].Manual) ?"":"checked")+" /><label for=\"auto\"><a href=\"/A1B2C3/"+String(i)+"/auto\">Auto</a></label>");
       client.println("   <input class=\"button\" type=\"radio\" name=\"toggle"+String(i)+"\" id=\"manual\" "+String((PoleRele[i].Manual) ?"checked":"")+" /><label for=\"manual\"><a href=\"/A1B2C3/"+String(i)+"/manual\">Manual</a> </label> </p>");
       if (PoleRele[i].Manual){
         client.println("<p><input class=\"button\" type=\"radio\" name=\"toggleM"+String(i)+"\" id=\"on\"  "+String((PoleRele[i].Stav) ?"checked":"")+" /><label for=\"on\"><a href=\"/A1B2C3/"+String(i)+"/on\">On</a></label>");
         client.println("   <input class=\"button\" type=\"radio\" name=\"toggleM"+String(i)+"\" id=\"off\" "+String((PoleRele[i].Stav) ?"":"checked")+" /><label for=\"off\"><a href=\"/A1B2C3/"+String(i)+"/off\">Off</a></label></p>");
       }
       client.println("</div>");
    }

    client.println("</body></html>");
     // The HTTP response ends with another blank line
    client.println();
}

void ObsluhaKlienta(WiFiClient client) {
  Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
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
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            Serial.println(header);
            int j =0;
            j=header.indexOf("A1B2C3/")+7; 
                  
            if (j>8) {
              String Pokyn;
              Pokyn=header.substring(j+2,j+4);
              Serial.println("Poziadavka: "+Pokyn) ;
              j=(header.substring(j,j+1)).toInt();
              Serial.println("Poziadavka2: "+String(j)) ;
              if (Pokyn=="on"){ 
                 PoleRele[j].Manual=true;
                 PoleRele[j].Stav=true;
                 digitalWrite(PoleRele[j].GPIO,LOW);
              }
              if (Pokyn=="of"){ 
                 PoleRele[j].Manual=true;
                 PoleRele[j].Stav=false;
                 digitalWrite(PoleRele[j].GPIO,HIGH);
              }
              if (Pokyn=="au"){ 
                 PoleRele[j].Manual=false;
                 Casovac;
              }
              if (Pokyn=="ma"){ 
                 PoleRele[j].Manual=true;
                 Casovac;
              }
              Serial.println("Rele po pokyne: "+String(j));
              Serial.println(PoleRele[j].Manual);
              Serial.println(PoleRele[j].Stav);

            }
            
            Zobraz_stranku(client);
            
            // Break out of the while loop
            break;
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
  Serial.println("Client odpojeny.");
}

void setup(){
  // start serial port
  Serial.begin(115200);
  // Start up the onewire library
  sensors.begin();
  
  for (int i=0 ; i<4;i++){
    pinMode(PoleRele[i].GPIO, OUTPUT);
    digitalWrite(PoleRele[i].GPIO, HIGH);
  }
  // Connect to Wi-Fi network with SSID and password
  Serial.print("Setting AP (Access Point)…");
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  server.begin();
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 10000000, true);  //nacita data zo senzorov kazdych 10 sec
  timerAlarmEnable(timer);
}

void loop(){ 

  WiFiClient client = server.available();   // Listen for incoming clients  
  if (client) {
    ObsluhaKlienta(client);
  }
  if(Casovac){
      portENTER_CRITICAL(&timerMux);
      Casovac=false;
      portEXIT_CRITICAL(&timerMux);
      sensors.requestTemperatures(); // Send the command to get temperatures
      for (int i=0;i<3;i++){
        PoleSnimaceTmp[i].TeplotaOkruhu = sensors.getTempC(PoleSnimaceTmp[i].Snimac);
      }
      Svetlo = analogRead(LDR);      
      for (int i=0;i<4;i++){        
        if (PoleRele[i].Manual==false){
          if (i==3){
              if (Svetlo<700){
               PoleRele[i].Stav=true;
               digitalWrite(PoleRele[i].GPIO, LOW); 
              }
              else{
               PoleRele[i].Stav=false;
               digitalWrite(PoleRele[i].GPIO, HIGH); 
              }
          }
          else{
            if(PoleSnimaceTmp[i].TeplotaOkruhu>POZ_TEPLOTA){
              PoleRele[i].Stav=false;
              digitalWrite(PoleRele[i].GPIO, HIGH); 
            }
            else{
              PoleRele[i].Stav=true;
              digitalWrite(PoleRele[i].GPIO, LOW); 
            }
          }
        }
      }
  } 
}






