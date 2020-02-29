#include "ESP32_MailClient.h"
#include <WiFiAP.h>
#include <WiFiClient.h>"
#include "ESPAsyncWebServer.h"
//AP newtowrk stuff
const char *ssidA = "travis";
const char *passwordA = "";

//WiFiServer server(80);
AsyncWebServer server(80);

//Network Password
const char* ssid = "SCU-Guest";
const char* password = "";

//email stuff
#define emailSenderAccount    "tle2@.edu"    
#define emailSenderPassword "zqseuywzwnfulytb"
#define emailRecipient        "tle2@scu.edu"
#define smtpServer            "smtp.gmail.com"
#define smtpServerPort        465
#define emailSubject          "ESP32 Test"

SMTPData smtpData;

// Callback function to get the Email sending status
void sendCallback(SendStatus info);

void setup(){
  //pinMode(4, OUTPUT);
  ledcSetup(0, 2500, 8);
  ledcAttachPin(4, 0);
  pinMode(19, INPUT);
  Serial.begin(115200);
  Serial.println();
  
  Serial.print("Connecting");

  WiFi.softAP(ssidA, passwordA);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(200);
  }

  Serial.println(WiFi.localIP());
  
  server.on("/hello", HTTP_GET, [](AsyncWebServerRequest *request){
    Makeasound();
    request->send(200, "text/plain", "Hello World");
  });
  
  server.begin();
  Serial.println();

  /*
  Serial.println();
  Serial.println("WiFi connected.");
  Serial.println();
  Serial.println("Preparing to send email");
  Serial.println();
  
  // Set the SMTP Server Email host, port, account and password
  smtpData.setLogin(smtpServer, smtpServerPort, emailSenderAccount, emailSenderPassword);

  // For library version 1.2.0 and later which STARTTLS protocol was supported,the STARTTLS will be 
  // enabled automatically when port 587 was used, or enable it manually using setSTARTTLS function.
  //smtpData.setSTARTTLS(true);

  // Set the sender name and Email
  smtpData.setSender("ESP32", emailSenderAccount);

  // Set Email priority or importance High, Normal, Low or 1 to 5 (1 is highest)
  smtpData.setPriority("High");

  // Set the subject
  smtpData.setSubject(emailSubject);

  // Set the message with HTML format
  smtpData.setMessage("<div style=\"color:#2f4468;\"><h1>Hello World!</h1><p>- Sent from ESP32 board</p></div>", true);
  // Set the email message in text format (raw)
  //smtpData.setMessage("Hello World! - Sent from ESP32 board", false);

  // Add recipients, you can add more than one recipient
  smtpData.addRecipient(emailRecipient);
  //smtpData.addRecipient("YOUR_OTHER_RECIPIENT_EMAIL_ADDRESS@EXAMPLE.com");

  smtpData.setSendCallback(sendCallback);

  //Start sending Email, can be set callback function to track the status
  if (!MailClient.sendMail(smtpData))
    Serial.println("Error sending Email, " + MailClient.smtpErrorReason());

  //Clear all data from Email object to free memory
  smtpData.empty();
  */
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
  }
  else
  {
    ledcWrite(0,0);
  }
  delay(100);
}

// Callback function to get the Email sending status
void sendCallback(SendStatus msg) {
  // Print the current status
  Serial.println(msg.info());

  // Do something when complete
  if (msg.success()) {
    Serial.println("----------------");
  }
}

void Makeasound()
{
  ledcWrite(0,200);
  delay(3000);
  ledcWrite(0,0);
}
