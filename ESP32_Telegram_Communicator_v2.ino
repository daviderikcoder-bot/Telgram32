// Replace YOUR_WIFI, YOUR_PASSWORD, YOUR_BOT_TOKEN and YOUR_CHAT_ID
// Pins:
// OLED SDA=22 SCL=21
// ENC CLK=18 DT=19 SW=23

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

const char* ssid="Wifi name here";
const char* password="wifi password here";
const char* BOT_TOKEN="xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
const char* CHAT_ID="xxxxxxxxxx";

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH,SCREEN_HEIGHT,&Wire,-1);

#define CLK 18
#define DT 19
#define SW 23

const char chars[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789 .,!?@#&()-_+/:=<";
int idx=0;
String msg="";

// Encoder state
uint8_t oldAB = 0;
int8_t encDelta = 0;

// Button
bool lastBtn=HIGH;
unsigned long down=0;
bool sent=false;

// Incoming message mode
bool incomingMode = false;
String incomingText = "";
long lastUpdateId = 0;

// Gray-code table
const int8_t table[16]={0,-1,1,0,1,0,0,-1,-1,0,0,1,0,1,-1,0};

void ui(const char* status=""){
 display.clearDisplay();
 display.setTextSize(1);
 display.setTextColor(SSD1306_WHITE);
 display.setCursor(0,0); display.println("Telegram Compose");
 display.drawLine(0,10,127,10,SSD1306_WHITE);
 display.setCursor(0,14); display.println(msg);
 display.drawLine(0,50,127,50,SSD1306_WHITE);
 display.setCursor(0,54);
 if(status[0]) display.print(status);
 else{
   char c=chars[idx];
   display.print("Char: ");
   if(c=='<') display.print("BACK");
   else if(c==' ') display.print("SPACE");
   else display.print(c);
 }
 display.display();
}

void showIncomingMessage(String text){
 display.clearDisplay();
 display.setTextSize(1);
 display.setTextColor(SSD1306_WHITE);
 display.setCursor(0,0);
 display.println("Telegram Message:");
 display.drawLine(0,10,127,10,SSD1306_WHITE);
 display.setCursor(0,14);
 display.println(text);
 display.drawLine(0,50,127,50,SSD1306_WHITE);
 display.setCursor(0,54);
 display.print("Click to return");
 display.display();
}

void sendTelegram(){
 if(msg=="") return;
 WiFiClientSecure client; client.setInsecure();
 HTTPClient http;
 String t="";
 for(char c:msg){
   if(c==' ') t+="%20";
   else t+=c;
 }
 String url="https://api.telegram.org/bot"+String(BOT_TOKEN)+"/sendMessage?chat_id="+String(CHAT_ID)+"&text="+t;
 ui("Sending...");
 if(http.begin(client,url)){
   int code=http.GET();
   http.end();
   if(code==200){ ui("Sent! Click"); sent=true; }
   else ui("HTTP Error");
 }
}

void setup(){
 Serial.begin(115200);

 pinMode(CLK,INPUT_PULLUP);
 pinMode(DT,INPUT_PULLUP);
 pinMode(SW,INPUT_PULLUP);

 Wire.begin(22,21);
 display.begin(SSD1306_SWITCHCAPVCC,0x3C);

 WiFi.begin(ssid,password);
 ui("WiFi...");
 while(WiFi.status()!=WL_CONNECTED) delay(200);

 oldAB = (digitalRead(CLK)<<1) | digitalRead(DT);

 ui();
}

void loop(){

// -------------------- FAST ENCODER POLLING (EVERY 200 µs) --------------------
static unsigned long lastEnc = 0;
if(micros() - lastEnc >= 200){
  lastEnc = micros();

  uint8_t ab = (digitalRead(CLK)<<1) | digitalRead(DT);
  uint8_t state = (oldAB<<2) | ab;
  encDelta += table[state];
  oldAB = ab;

  if(encDelta >= 4){
    encDelta = 0;
    idx = (idx + 1) % (sizeof(chars)-1);
    if(!sent && !incomingMode) ui();
  }
  else if(encDelta <= -4){
    encDelta = 0;
    idx--;
    if(idx < 0) idx = sizeof(chars)-2;
    if(!sent && !incomingMode) ui();
  }
}

// -------------------- CHECK TELEGRAM (NON-BLOCKING) --------------------
static unsigned long lastCheck = 0;
if(!incomingMode && millis() - lastCheck > 1000){
  WiFiClientSecure client; 
  client.setInsecure();
  HTTPClient http;

  String url = "https://api.telegram.org/bot" + String(BOT_TOKEN) +
               "/getUpdates?offset=" + String(lastUpdateId + 1);

  if(http.begin(client, url)){
    int code = http.GET();
    if(code == 200){
      String payload = http.getString();

      int u1 = payload.indexOf("\"update_id\":");
      if(u1 != -1){
        u1 += 12;
        int u2 = payload.indexOf(",", u1);
        long updId = payload.substring(u1, u2).toInt();

        if(updId > lastUpdateId){
          lastUpdateId = updId;

          int t1 = payload.indexOf("\"text\":\"", u2);
          if(t1 != -1){
            t1 += 8;
            int t2 = payload.indexOf("\"", t1);
            incomingText = payload.substring(t1, t2);

            incomingMode = true;
            showIncomingMessage(incomingText);
          }
        }
      }
    }
    http.end();
  }
  lastCheck = millis();
}

// -------------------- INCOMING MESSAGE MODE --------------------
if(incomingMode){
  bool b = digitalRead(SW);
  if(lastBtn && !b){ down = millis(); }
  if(!lastBtn && b){
    incomingMode = false;
    incomingText = "";
    ui(); // back to compose
  }
  lastBtn = b;
  return;
}

// -------------------- BUTTON LOGIC --------------------
bool b=digitalRead(SW);
if(lastBtn && !b){ down=millis(); }

if(!lastBtn && b){
  if(sent){
    sent=false;
    msg="";
    ui();
  }else{
    if(millis()-down>=800){
      sendTelegram();
    }else{
      char c=chars[idx];
      if(c=='<'){
        if(msg.length()) msg.remove(msg.length()-1);
      }else msg+=c;
      ui();
    }
  }
}
lastBtn=b;
}
