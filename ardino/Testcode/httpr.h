#include "WiFi.h"
#include <FS.h>
#include <SPIFFS.h>

#define BOUNDARY "xx---xx--xx" 
#define TIMEOUT 250000
#define BUFSIZE 100

uint8_t buffer[BUFSIZE];
const char* host = "knock.aksharsolanki.com";
String body(String content , String message);
String header(size_t length);


void sendpicture()
{
  File file = SPIFFS.open("/photo.jpg", FILE_READ);
  if (!file) {
      Serial.println("Failed to open file in reading mode");
      return;
  }
  String bodyTxt =  body("message","Picture");
  String bodyPic =  body("imageFile","Picture");
  String bodyEnd =  String("--")+BOUNDARY+String("--\r\n");
  size_t allLen = bodyTxt.length()+bodyPic.length()+file.size()+bodyEnd.length();
  String headerTxt =  header(allLen);
  const int httpPort = 80;
  WiFiClient client;
  if (!client.connect(host, httpPort)) {
       Serial.println("connection failed");
       file.close();
       return;
  }
  Serial.print("yay");

  client.print(headerTxt+bodyTxt+bodyPic);
  Serial.print(headerTxt+bodyTxt+bodyPic);
  while (file.available()){
    int cc = file.read(buffer, BUFSIZE);
    client.write(buffer, cc);
    Serial.print((char*)buffer);
  }
  Serial.print("\r\n"+bodyEnd);
  client.print("\r\n"+bodyEnd);
  
  file.close();

  delay(20);
  long tOut = millis() + TIMEOUT;
  while(client.connected() && tOut > millis()) 
  {
   if (client.available()) 
   {
     String serverRes = client.readStringUntil('\r');
     Serial.println(serverRes);
   }
  }
  Serial.println("isdj");
  return;
}

String header(size_t length)
{
  String  data;
      data =  F("POST /images HTTP/1.1\r\n");
      data += F("cache-control: no-cache\r\n");
      data += F("Content-Type: multipart/form-data; boundary=");
      data += BOUNDARY;
      data += "\r\n";
      data += F("User-Agent: PostmanRuntime/6.4.1\r\n");
      data += F("Accept: */*\r\n");
      data += F("Host: ");
      data += host;
      data += F("\r\n");
      data += F("accept-encoding: gzip, deflate\r\n");
      data += F("Connection: keep-alive\r\n");
      data += F("Content-length: ");
      data += String(length);
      data += "\r\n";
      data += "\r\n";
    return(data);
}
String body(String content , String message)
{
  String data;
  data = "--";
  data += BOUNDARY;
  data += F("\r\n");
  if(content=="imageFile")
  {
    data += F("Content-Disposition: form-data; name=\"file\"; filename=\"picture.jpg\"\r\n");
    data += F("Content-Transfer-Encoding: binary\r\n");
    data += F("Content-Type: image/jpeg\r\n");
    data += F("\r\n");
  }
  else
  {
    data += "Content-Disposition: form-data; name=\"" + content +"\"\r\n";
    data += "\r\n";
    data += "help";
    data += "\r\n";
  }
   return(data);
}
