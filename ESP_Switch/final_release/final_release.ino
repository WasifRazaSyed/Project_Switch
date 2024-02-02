#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <ESP8266WebServer.h>

// *variables and functions for measuring current from here 
#include   <Filters.h>  
#define ACS_Pin A0                       

float ACS_Value;                             
float testFrequency = 50;                    
float windowLength = 40.0/testFrequency;     
float intercept = 0; 
float slope = 0.0752;   
float   Amps_TRMS; 
unsigned long printPeriod   = 1000; 
unsigned   long previousMillis = 0;
unsigned long current_time;
String current_="0";
int consolidate=0;

RunningStatistics inputStats;

void getCurrent()
{
  current_time=millis();
  while(millis()-current_time<=1500)
  {
    ACS_Value = analogRead(ACS_Pin);   // read the analog in value:
    inputStats.input(ACS_Value);
    Amps_TRMS = intercept + slope * inputStats.sigma();
    if(Amps_TRMS>=0.5)
    {
      Serial.println("Consolidating");
      consolidate++;
    }
    else if(Amps_TRMS<0.5)
    {
      consolidate=0;
    }
  }
  if(consolidate>=3)
  {
    current_="1";
  }
  else
  {
    current_="0";
  }
}
// till above*

// *http server code from here 
const int relay=D8;
const int network_status = D6;
const int server_status = D7;
const int network_reset = D5;
const int server_reset = D0;
String status_="0";
String online_="1";
String mac="";

ESP8266WebServer server(80);

const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<body>
<center>
Click to turn <a href="/on">led on</a><br>
Click to turn <a href="/off">led off </a><br>
Click to get <a href="/status">status</a><br>
Click to get <a href="/connect">connect</a><br>
Click to get <a href="/current">current</a><br>
Click to get <a href="/online">online</a><br>
Click to get <a href="/getmac">getmac</a><br>
Click to <a href="/reset">reset</a><br>
<form action="/setmac" method="GET">
  <label for="mac">MAC Address:</label>
  <input type="text" id="mac" name="mac" placeholder="Enter MAC Address">
  <input type="submit" value="Set MAC">
  <br>
</form>
<hr>
</center>
<script>
const body0=document.getElementsByTagName("body");
body0[0].style.display="none";
</script>
</body>
</html>
)=====";


void handleRoot() {
  Serial.println("You called the root page");
  String s = MAIN_page;
  server.send(200, "text/html", s);
}

void on() {
  online_="1";
  Serial.println("On page");
  digitalWrite(D4, LOW);  
  digitalWrite(relay, LOW);
  server.send(200, "text/html", "true");
}

void off() {
  online_="0";
  Serial.println("Off page");
  digitalWrite(D4, HIGH); 
  digitalWrite(relay, HIGH);
  server.send(200, "text/html", "false");
}

void status() {
  Serial.println("status page");
  server.send(200, "text/html", status_);
}

void connect() {
  status_="1";
  digitalWrite(server_status, HIGH);
  Serial.println("connect page");
  server.send(200, "text/html", "connected");
}

void current() {
  getCurrent();
  Serial.println("current page");
  server.send(200, "text/html", current_);
}

void online() {
  Serial.println("online page");
  server.send(200, "text/html", online_);
}

void setMac() {
  Serial.println("SET MAC PAGE");
  if (server.hasArg("mac")) {
    mac = server.arg("mac");
    Serial.print("Received MAC Address: ");
    Serial.println(mac);

    // You can store the received MAC address in a global variable or perform any other actions here.
    // For example, you can save it to a global variable or EEPROM.
  } else {
    Serial.println("No MAC Address provided.");
  }

  server.send(200, "text/html", "MAC Address Set: " + mac);
}

void getMac()
{
  server.send(200, "text/html", mac);
}

void reset() {
  online_="1";
  status_="0";
  mac.clear();
  digitalWrite(D4, LOW);  
  digitalWrite(relay, LOW);
  digitalWrite(server_status, LOW);
  Serial.println("reset page");
  server.send(200, "text/html", "reset");
}

//till here

void setup()
{
  Serial.begin( 115200 );   
  pinMode(ACS_Pin,INPUT);  
  pinMode(D4, OUTPUT); //built in led
  pinMode(relay, OUTPUT);
  pinMode(server_reset, INPUT);
  pinMode(network_reset, INPUT);
  pinMode(network_status, OUTPUT);
  pinMode(server_status, OUTPUT);

  digitalWrite(server_status, LOW);
  digitalWrite(network_status, LOW);
  digitalWrite(D4, LOW);  
  digitalWrite(relay, LOW);

  inputStats.setWindowSecs( windowLength ); //current sensor 
  // wifi portal code from here
  WiFiManager wm;
  wm.setCustomHeadElement("<script>window.addEventListener('load', function() {var arr=document.getElementsByTagName(\"form\"); arr[1].remove(); arr[2].remove(); const wrap= document.querySelector('.wrap'); const h1=wrap.querySelector('h1'); const h3 = wrap.querySelector('h3'); wrap.removeChild(h3); wrap.removeChild(h1); const newH1= document.createElement('h1');newH1.textContent = \"Smart Switch-WiFi Manager\";newH1.style.textAlign='center';newH1.style.marginTop= \"50px\";newH1.style.marginBottom=\"50px\"; wrap.insertBefore(newH1, wrap.firstChild); const p=document.createElement('p'); p.textContent=\"Press the Configure WiFi button to set the credentials of the network your machine is connected to.\"; wrap.insertBefore(p, wrap.children[2].nextSibling);const note=document.createElement('p'); note.textContent=\"Note: To reset the network configuration, press the network reset button on you smart switch\"; note.style.color='red'; wrap.insertBefore(note, wrap.children[3].nextSibling); var arr1=document.getElementsByTagName(\"form\"); arr1[1].remove();}); </script>");
  wm.setAPStaticIPConfig(IPAddress(10, 10, 10, 10), IPAddress(10, 10, 10, 1), IPAddress(255, 255, 255, 0));
    
  if (!wm.autoConnect("Smart Switch", "gateway10point1."))
  {
  }
  // wifi portal code till above
  else
  {
    if(WiFi.status()==WL_CONNECTED)
    {
      IPAddress ip;
      String gateway=WiFi.gatewayIP().toString();
      int index=gateway.lastIndexOf('.');
      if(index!=-1)
      {
        String IP=gateway.substring(0, index);
        IP+=".47";
        ip.fromString(IP);
        Serial.print("Made IP: ");Serial.println(IP);
        Serial.print("TO BE IP: ");Serial.println(ip);
      }
      delay(400);
      WiFi.config(ip, WiFi.gatewayIP(), WiFi.subnetMask());
      Serial.print("IP: ");
      Serial.println(WiFi.localIP());
      digitalWrite(network_status, HIGH);

      server.on("/", handleRoot);
      server.on("/on", on);
      server.on("/off", off);
      server.on("/status", status);
      server.on("/connect", connect);
      server.on("/current", current);
      server.on("/online", online);
      server.on("/getmac", getMac);
      server.on("/setmac", setMac);
      server.on("/reset", reset);

      server.begin();
    }
  }
}

void loop()
{
  server.handleClient();
  if(digitalRead(server_reset)==LOW)
  {
    digitalWrite(server_status, LOW);
    Serial.println("Server_Reset");
    online_="1";
    status_="0";
    mac.clear();
    digitalWrite(D4, LOW);  
    digitalWrite(relay, LOW);
  }
  if(digitalRead(network_reset)==LOW)
  {
    digitalWrite(network_status, LOW);
    Serial.println("Network_Reset");
    WiFi.disconnect();
    ESP.restart();
  }
}

//http server code till above