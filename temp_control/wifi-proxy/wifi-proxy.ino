#include <ESP8266WiFi.h>

WiFiServer server(80);
bool head = false;
int pos;
char temp[3] = "68";

void setup() {
  Serial.begin(115200);
  WiFi.begin("ssid", "password");
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
  }
  server.begin();
  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());
}

// Passes thorugh output from serial and a number from the wifi to the serial line.
void loop() {
    WiFiClient client = server.available();
    if (client)
    {
      while (client.connected())
      {
        while (client.available())
        {
           char incoming = client.read();
           // This will allow double digit changes with boundaries like >NN<
           // for easy validation.
           if (!head && incoming == '>') {
             head = true;
             pos = 0;
           } else if (head && incoming >= '0' && incoming <= '9' && pos < 2) {
            temp[pos++] = incoming;
           } else if (head && pos == 2 && incoming == '<' ) {
             head = false;
             pos = 0;
             Serial.println(temp);
           } else {
             head = false;
             pos = 0;
            }
           
        }
        while (Serial.available()) {
          String line = Serial.readString();
          client.print(line.c_str());
           
        }
        delay(100);
      }
    }
  
  delay(2000);
}
