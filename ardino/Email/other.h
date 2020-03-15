#include <HTTPClient.h>

String sendRequest()
{
  String response;
  if(WiFi.status()== WL_CONNECTED){   //Check WiFi connection status
 
   HTTPClient http;   
 
   http.begin("http://192.168.4.1/capture");  //Specify destination for HTTP request
   http.addHeader("Content-Type", "text/plain");             //Specify content-type header
 
   int httpResponseCode = http.GET();   //Send the actual POST request
 
   if(httpResponseCode>0){
 
    String repsonse = http.getString();                       //Get the response to the request
 
    Serial.println(httpResponseCode);   //Print return code
    Serial.println(response);           //Print request answer
 
   }else{
 
    Serial.print("Error on sending Get: ");
    Serial.println(httpResponseCode);
   }
 
   http.end();  //Free resources
  }
   return response;
}
