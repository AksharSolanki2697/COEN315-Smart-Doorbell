#include "WiFi.h"
#include <FS.h>
#include <SPIFFS.h>

#define BOUNDARY "---------xx---xx--------" 
#define TIMEOUT 20000
const char* host = "data.sparkfun.com";
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

  client.print(headerTxt+bodyTxt+bodyEnd);
  while (file.available()){
    client.write(file.read());
  }
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
  return;
}

String header(size_t length)
{
  String  data;
      data =  F("POST /ln/bot.php HTTP/1.1\r\n");
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
      data += F("content-length: ");
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
    data += F("Content-Disposition: form-data; name=\"imageFile\"; filename=\"picture.jpg\"\r\n");
    data += F("Content-Type: image/jpeg\r\n");
    data += F("\r\n");
  }
  else
  {
    data += "Content-Disposition: form-data; name=\"" + content +"\"\r\n";
    data += "\r\n";
    data += message;
    data += "\r\n";
  }
   return(data);
}
