/*
author: Adesh Shah
web page: AdeshShah.com
arduino:  1.0.3
discription: 
    Wordfeed is an attemt to improve vocabulary. 
    It uses Arduino Uno R3, ethernet sheild, DS1307 IC for Time.
    I have tested this app on ubuntu-13.04 using LAN cable('shared to other computers' connection from network settings).
    Don't pull request to my site so frequent. (not more than 1 request a min) 
    Kindly, report issues and suggestions

first release: 15:04, 6/Aug/13
*/
#include <LiquidCrystal.h>
#include <Wire.h>
#include "RTClib.h"
#include <SPI.h>//for SD
#include <Ethernet.h>

int LDR_Pin = A0; //LDR sensor input
int lcdlight = 9; //brighness pin 16*2LCD
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);//LCD pin config
RTC_DS1307 rtc; //clock instance
byte mac[] = {  0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02 };//mac address ethernet
char serverName[] = "www.adeshshah.com";//server to connect using DNS
EthernetClient client;
int d,x=1,word_per_day=20,i=0;//keep track of the day to display 20 words a day
//store x in EEPROM
void setup() {
  pinMode(lcdlight, OUTPUT);
  Wire.begin();
  rtc.adjust(DateTime(__DATE__, __TIME__));
  lcd.begin(16, 2);  
  lcd.print("Adesh's Lab!");
  
  //set day tracking variable..
  DateTime now = rtc.now();
  d=now.day();
  
  //Serial.begin(9600);
  // while (!Serial) {
  // ; // wait for serial port to connect. Needed for Leonardo only
  //}
  

  if (Ethernet.begin(mac) == 0) {
    lcd.clear(); 
    lcd.print("DHCP failed.");
    lcd.setCursor(0,1);
    lcd.print("press reset!");
    //set it up again 
    //Serial.println("Failed to configure Ethernet using DHCP");
    // no point in carrying on, so do nothing forevermore:
    //while(true);
  }
  //Serial.println(Ethernet.localIP());
  delay(1000);
}

void loop()
{
  int backlight = map(analogRead(LDR_Pin), 0, 1023, 10, 255);
  analogWrite(lcdlight, backlight);
  
  //night mode -- fetch word if its morning(ambienlty )
  if(backlight>30){ 
   int temp = x+i;
   if(x>4814)temp=temp-4814;
   if(fetchWord(temp)!=0)i--; //5 Second
  }
  
  fetchTime();
  fetchTime();
  fetchTime();
  fetchTime();
  fetchTime();//5 iteration -- 5*1=5 Second
    
  i++;
  if(i>(word_per_day-1))i=0;
  day_track(); //check day change
}

int fetchWord(int count){
  lcd.clear();
  lcd.print("Adesh's Lab!");
  lcd.setCursor(0, 1);
  lcd.print("fetching word...");
  
  String raw = "";
  String currentLine = "";
  
  //Serial.print("Connecting... ");

  if (client.connect(serverName, 80)) {
    //Serial.println("Connected.");
    String temp = "GET /word/index.php?count="+String(count)+" HTTP/1.1";
    //Serial.println(temp);
    client.println(temp);
    client.println("Host: www.adeshshah.com");
    client.println();
  } 
  else {
    //Serial.println("Connection Failed.");
    lcd.clear();
    lcd.print("Adesh's Lab!");
    lcd.setCursor(0, 1);
    lcd.print("fetching failed!");
  }
  
  while(client.connected()){
    if (client.available()) {      
      char inChar = client.read();
      currentLine += inChar;
      if (inChar == '\n') {
        if ( currentLine.startsWith("<word>")){
           raw=currentLine;
           if(raw!=""){
            String wrd,def;
            wrd = raw.substring(raw.indexOf("<word>")+6,raw.indexOf("</word>"));
            def = raw.substring(raw.indexOf("<def>")+5,raw.indexOf("</def>"));
            raw=""; 
            lcd.clear();
            lcd.print(count);lcd.print(".");
            lcd.print(wrd);
            lcd.setCursor(0,1);
            int len = def.length();
            //lcd.print(len);            
            if(len>17){
                def=def.substring(0,def.indexOf("; "));
                  lcd.print(def);
                  delay(3000);
            }
            else{
              lcd.print(def);
              delay(3000);
            } //or simply print
           }
        }
        currentLine = "";
      }
    }
  }
  if (!client.connected()) {
     //Serial.println("disconnecting.");
     client.stop();
  }  

  //delay(1000);
  return(0);
}

void fetchTime(){//consider taking 1sec to get exectuted each iteration
  DateTime now = rtc.now();
  lcd.clear(); 
  lcd.print("Adesh's Lab!");
  lcd.setCursor(0, 1);if (now.hour() < 10) lcd.print("0");lcd.print(now.hour());lcd.setCursor(2, 1);lcd.print(" ");
  lcd.setCursor(2, 1);lcd.print(":");
  lcd.setCursor(3, 1);if (now.minute() < 10) lcd.print("0");lcd.print(now.minute());lcd.setCursor(5, 1);lcd.print("  ");
  lcd.setCursor(7, 1);if (now.day() < 10) lcd.print("0");lcd.print(now.day());lcd.setCursor(9, 1);lcd.print("/");lcd.setCursor(10, 1);
  switch (now.month()) {
      case 1:
        lcd.print("Jan");
        break;
      case 2:
        lcd.print("Feb");
        break;
      case 3:
        lcd.print("Mar");
        break;
      case 4:
        lcd.print("Apr");
        break;
      case 5:
        lcd.print("May");
        break;
      case 6:
        lcd.print("Jun");
        break;
      case 7:
        lcd.print("Jul");
        break;
      case 8:
        lcd.print("Aug");
        break;
      case 9:
        lcd.print("Sep");
        break;
      case 10:
        lcd.print("Oct");
        break;
      case 11:
        lcd.print("Nov");
        break;
      case 12:
        lcd.print("Dec");
        break;
      default:
        lcd.print("   ");   
    }
    delay(500);
    lcd.setCursor(2, 1);lcd.print(" ");
    delay(500);
}

void day_track(){
  DateTime now = rtc.now();
  if(d!=now.day()){
    d=now.day();
    x=x+word_per_day;
    if(x>4814)x=1;
  }
}
