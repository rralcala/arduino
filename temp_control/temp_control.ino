/* Use DHT22 sensor to control a RC300 controlled fireplace
  RC300 is the model of the remote, in this case the fireplace is Heat & Glo
 */
//Libraries
#include <DHT.h>
#include <FireplaceRF.h>

// Constants
#define DEBUG
#define PIN_FIRE_TX 3
#define DHTPIN 7
#define INPIN 6
#define HEAT_LED 8
#define OPER_LED 5
// DHT 22  (AM2302)
#define DHTTYPE DHT22
#define LOWTEMP 21.0
// About how long it can take for a power cycle to complete
#define MAX_POWERCYCLE_MILLIS 28000

FireplaceRF fireplace(PIN_FIRE_TX);
// Initialize DHT sensor for normal 16mhz Arduino
DHT dht(DHTPIN, DHTTYPE);

// Variables
unsigned int runtime_left = 21600;
bool operating = true;
bool heating = false;
bool need_heat = false;

void setup()
{
  dht.begin();
  pinMode(HEAT_LED, OUTPUT);
  pinMode(OPER_LED, OUTPUT);
  pinMode(INPIN, INPUT);
  pinMode(PIN_FIRE_TX, OUTPUT);
  digitalWrite(OPER_LED, operating);
  
  float temp = dht.readTemperature();  
  if (temp < LOWTEMP) {
    need_heat = true;
  }
  #ifdef DEBUG
  Serial.begin(9600);
  Serial.println("setup");
  Serial.println(temp);
  #endif
  
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

void loop()
{
  // button pressed flips the operation switch
  if (digitalRead(INPIN) == LOW) {
    power_toggle();
  }
  if (runtime_left == 0) {
    power_off();
  }
  
  if (heating) {
    runtime_left -= 1;
  } else {
    runtime_left = 21600;  
  }

  float temp = dht.readTemperature();
  #ifdef DEBUG
    Serial.println(temp);
  #endif

  if (temp < LOWTEMP) {
    need_heat = true;
  }
  if (temp > (LOWTEMP + 2.0) ) {
    need_heat = false;
  }

  power_status(need_heat);
  delay(1000);
}

void power_status(bool need_heat) {
  if (operating) {
    if (need_heat != heating) {
      heating = need_heat;
      digitalWrite(HEAT_LED, heating);
      if (heating) {
        #ifdef DEBUG
        Serial.println("Start the fire");
        #endif
        fireplace.on();
        delay(MAX_POWERCYCLE_MILLIS);
        fireplace.setFan(2);
        delay(2000);
        fireplace.setFlame(3);
      } else {
        #ifdef DEBUG
        Serial.println("Stop the fire");
        #endif
        fireplace.off();
      }
    }
  } else if (heating) {
    #ifdef DEBUG
    Serial.println("Stop operation");
    #endif
    heating = false;
    fireplace.off();
    digitalWrite(HEAT_LED, heating);
    delay(MAX_POWERCYCLE_MILLIS);
  }
}

   