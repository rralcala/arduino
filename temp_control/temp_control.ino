/* Use DHT22 sensor to control a RC300 controlled fireplace
  RC300 is the model of the remote, in this case the fireplace is Heat & Glo
*/
//Libraries
#include <DHT.h>
#include <FireplaceRF.h>

// Constants
#define DHTPIN 7
// DHT 22  (AM2302)
#define DHTTYPE DHT22
#define HEAT_LED 8
#define INPIN 6
#define LOOP_DELAY_SEC 2
// About how long it can take for a power cycle to complete
#define MAX_POWERCYCLE_MILLIS 30000
#define MAX_RUNTIME 25200
#define OPER_LED 5
#define PIN_FIRE_TX 3

// Initialize DHT sensor for normal 16mhz Arduino
DHT dht(DHTPIN, DHTTYPE);

FireplaceRF fireplace(PIN_FIRE_TX);

// Variables
String incomingTemp("20");
bool heating = false;
bool need_heat = false;
bool operating = true;
float high_temp = 21.5;
float low_temp = 20.0;
float temp = 20.0;
unsigned int runtime_left = MAX_RUNTIME; // Seconds

void setup()
{
  dht.begin();
  pinMode(HEAT_LED, OUTPUT);
  pinMode(OPER_LED, OUTPUT);
  pinMode(INPIN, INPUT);
  pinMode(PIN_FIRE_TX, OUTPUT);
  digitalWrite(OPER_LED, operating);
   
  temp = dht.readTemperature();
  if (temp < low_temp) {
    need_heat = true;
  }
  Serial.begin(115200);
  heating = !need_heat;
  power_status(need_heat);
}

void power_toggle() {
  operating = !operating;
  digitalWrite(OPER_LED, operating);
}

void power_off() {
  operating = false;
  digitalWrite(OPER_LED, operating);
}

void power_status(bool need_heat) {
  if (operating) {
    if (need_heat != heating) {
      heating = need_heat;
      digitalWrite(HEAT_LED, heating);
      if (heating) {

        fireplace.on();
        delay(MAX_POWERCYCLE_MILLIS);
        fireplace.setFan(2);
        delay(2000);
        fireplace.setFlame(3);
      } else {

        fireplace.off();
      }
    }
  } else if (heating) {

    heating = false;
    fireplace.off();
    digitalWrite(HEAT_LED, heating);
    delay(MAX_POWERCYCLE_MILLIS);
  }
}

void loop()
{
  if (Serial.available()) {
    // read the incoming data
    incomingTemp = Serial.readString();
    incomingTemp.trim();
    low_temp = atof(incomingTemp.c_str());
    if (low_temp < 5.0 || low_temp > 40.0) {
      low_temp = 20.0; 
    }
    high_temp = low_temp + 1.5;
    Serial.println("ACK");
  } 
  // button pressed flips the operation switch
  if (digitalRead(INPIN) == LOW) {
    power_toggle();
  }
  if (runtime_left == 0) {
    power_off();
  }

  if (heating) {
    runtime_left -= LOOP_DELAY_SEC;
  } else {
    runtime_left = MAX_RUNTIME;
  }

  temp = dht.readTemperature();

  Serial.print(temp);
  Serial.print(" ");
  Serial.print(operating);
  Serial.print(" ");
  Serial.print(heating);
  Serial.print(" ");
  Serial.println(low_temp);

  if (temp < low_temp) {
    need_heat = true;
  }
  if (temp > high_temp ) {
    need_heat = false;
  }

  power_status(need_heat);
  delay(LOOP_DELAY_SEC * 1000);
}
