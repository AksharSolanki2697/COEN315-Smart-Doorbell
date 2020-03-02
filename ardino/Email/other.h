#include <HTTPClient.h>

uint8_t[] sendRequest()
{
  String response;
  if(WiFi.status()== WL_CONNECTED){   //Check WiFi connection status
 
   HTTPClient http;   
 
   http.begin("http://192.168.4.2");  //Specify destination for HTTP request
   http.addHeader("Content-Type", "text/plain");             //Specify content-type header
 
   int httpResponseCode = http.GET();   //Send the actual POST request
 
   if(httpResponseCode>0){
 
    uint8_t[] = http.getString();                       //Get the response to the request
 
    Serial.println(httpResponseCode);   //Print return code
    Serial.println(response);           //Print request answer
 
   }else{
 
    Serial.print("Error on sending POST: ");
    Serial.println(httpResponseCode);
   }
 
   http.end();  //Free resources
  }
   return response;
}
