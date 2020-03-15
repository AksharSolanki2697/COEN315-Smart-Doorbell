#include "ESP32_MailClient.h"
#include <WiFiClient.h>
#include <HTTPClient.h>
#include "functions.h"
#include "other.h"
#include "SPIFFS.h"

//Network Password
const char* ssid = "SCU-Guest";
const char* password = "";

//AP newtowrk stuff
const char *ssida = "travis";
const char *passworda = "";

//timer stuff:
volatile int interruptCounter;
volatile int iniCount;
hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

// Callback function to get the Email sending status
void sendCallback(SendStatus info);
void IRAM_ATTR logouput();
void startTimer();
void getPhoto();

void setup(){
  //Setting up pins
  //pinMode(4, OUTPUT);
  ledcSetup(0, 2500, 8);
  ledcAttachPin(4, 0);
  pinMode(19, INPUT);

  //set up serial
  Serial.begin(115200);
  Serial.println();
  
  //setting up wifi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(200);
  }
  Serial.println("this is the local IP");
  Serial.println(WiFi.localIP());
  Serial.println();

  if(!SPIFFS.begin(true)){
    Serial.println("An error has occured while mounting SPIFFS");
    return;
  }
}
void loop() {
  /*
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
      WiFi.disconnect();
      Serial.println("disconnecting--------------");
      Serial.print("Connecting to ");
      Serial.println(ssida);
    
      WiFi.begin(ssida, passworda);
    
      while (WiFi.status() != WL_CONNECTED) {
          delay(500);
          Serial.print(".");
      }
      sendRequest();
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
      getPhoto();
      WiFi.disconnect();
      Serial.println("disconnecting--------------");
      Serial.print("Connecting to ");
      Serial.println(ssid);
    
      WiFi.begin(ssid, password);
    
      while (WiFi.status() != WL_CONNECTED) {
          delay(500);
          Serial.print(".");
      }
      generateEmail();
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
  timerAlarmWrite(timer, 12000000, true);

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

void getPhoto()
{

  File file = SPIFFS.open("/test-file.jpg",FILE_WRITE);
  
  if(!file)
  {
    Serial.println("There was an error opening the file for writing");
    return;
  }

 if ((WiFi.status() == WL_CONNECTED)) { //Check the current connection status
 
    HTTPClient http;
    http.begin("http://192.168.4.1/saved-photo"); //Specify the URL
    int httpCode = http.GET();                                        //Make the request
 
    if(httpCode > 0) {
            // HTTP header has been send and Server response header has been handled
            Serial.printf("[HTTP] GET... code: %d\n", httpCode);

            // file found at server
            if(httpCode == HTTP_CODE_OK) {

                // get lenght of document (is -1 when Server sends no Content-Length header)
                int len = http.getSize();

                // create buffer for read
                uint8_t buff[128] = { 0 };

                // get tcp stream
                WiFiClient * stream = http.getStreamPtr();

                // read all data from server
                while(http.connected() && (len > 0 || len == -1)) {
                    // get available data size
                    size_t size = stream->available();

                    if(size) {
                        // read up to 128 byte
                        int c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));

                        // write it to Serial
                        Serial.write(buff, c);

                         if(file.write(buff,c)){
                            Serial.println("File was written");
                         }else
                         { 
                             Serial.println("File write failed");
                         }
                         

                        if(len > 0) {
                            len -= c;
                        }
                    }
                    delay(1);
                }

                Serial.println();
                Serial.print("[HTTP] connection closed or file end.\n");

            }
        } else {
            Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }

        http.end();
    }
    file.close();

}
