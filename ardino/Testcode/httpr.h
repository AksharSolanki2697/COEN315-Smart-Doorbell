#include <Wifi.h>
#include <HTTPClient.h>

void sendPostData()
{
  //got from https://techtutorialsx.com/2017/05/20/esp32-http-post-requests/
if(WiFi.status()== WL_CONNECTED){   //Check WiFi connection status
   HTTPClient http;   

   http.begin("http://172.31.154.186:5000/");  //Specify destination for HTTP request
   http.addHeader("Content-Type", "text/plain");             //Specify content-type header
   delay(2000);
   int httpResponseCode;   //Send the actual POST request
   httpResponseCode= http.POST("q=1");
   if(httpResponseCode>0){
 
    String response = http.getString();                       //Get the response to the request
 
    Serial.println(httpResponseCode);   //Print return code
    Serial.println(response);           //Print request answer
 
   }else{
 
    Serial.print("Error on sending POST: ");
    Serial.println(httpResponseCode);
 
   }
 
   http.end();  //Free resources
 
 }else{
 
    Serial.println("Error in WiFi connection");   
 
 }
  
}
