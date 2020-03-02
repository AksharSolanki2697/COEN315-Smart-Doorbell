#include "ESP32_MailClient.h"
#include <WiFiAP.h>
#include <WiFiClient.h>"
#include "ESPAsyncWebServer.h"
#include "functions.h"
#include "other.h"

AsyncWebServer server(80);

//Network Password
const char* ssid = "SCU-Guest";
const char* password = "";

//AP newtowrk stuff
const char *ssidA = "travis";
const char *passwordA = "";

//timer stuff:
volatile int interruptCounter;
volatile int iniCount;
hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

// Callback function to get the Email sending status
void sendCallback(SendStatus info);
void IRAM_ATTR logouput();
void startTimer();

void setup(){
  //Setting up pins
  //pinMode(4, OUTPUT);
  ledcSetup(0, 2500, 8);
  ledcAttachPin(4, 0);
  pinMode(19, INPUT);

  //set up serial
  Serial.begin(115200);
  Serial.println();

  //setup softAp
  WiFi.softAP(ssidA, passwordA);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  
  //setting up wifi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(200);
  }
  Serial.println("this is the local IP");
  Serial.println(WiFi.localIP());

  //set generate request
  server.on("/email", HTTP_GET, [](AsyncWebServerRequest *request){
    generateEmail();
    request->send(200, "text/plain", "Hello World");
  });
  
  //other method
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    uint8_t a[4408] = sendRequest();
    request->send(200, "text/plain", (const char *)a);
  });
  server.begin();
  Serial.println();
}

void loop() {
  /*
  WiFiClient client = server.available();
  if (client)
  {
    client.stop();
    Serial.println("Client");
  }
  */
  if (digitalRead(19) == HIGH)
  {
    ledcWrite(0,300);
    if(interruptCounter <= 0 && iniCount <= 0)
    {
      portENTER_CRITICAL(&timerMux);
      interruptCounter++;
      iniCount++;
      portEXIT_CRITICAL(&timerMux);
      startTimer();
    }
  }
  else
  {
    ledcWrite(0,0);
  }
  if(iniCount >0 && interruptCounter <=0)
  {
      portENTER_CRITICAL(&timerMux);
      iniCount--;
      portEXIT_CRITICAL(&timerMux);
      Serial.println("supposed to send email.");
      stopTimer();
  }
  delay(100);
}
void startTimer()
{
  //cited from here https://github.com/espressif/arduino-esp32/issues/1313
  if(timer == NULL)
  {
    timer = timerBegin(0,80,true);
    timerAttachInterrupt(timer, &logouput, true);
  }
  timerAlarmWrite(timer, 5000000, true);

  yield();
  timerAlarmEnable(timer);
}

void stopTimer()
{
  if(timer != NULL)
  {
    timerAlarmDisable(timer);
    timerDetachInterrupt(timer);
    timerEnd(timer);
    timer = NULL;
  }
}

void IRAM_ATTR logouput()
{
  Serial.println("this should only be called every 5sec.");
  portENTER_CRITICAL_ISR(&timerMux);
  interruptCounter--;
  portEXIT_CRITICAL_ISR(&timerMux);
  
}
