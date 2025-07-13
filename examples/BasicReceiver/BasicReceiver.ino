#include <TFAReceiver.h>

#define RXPIN 22 //Pin where 433Mhz receiver is connected

TFAReceiver receiver(RXPIN); 

void setup() {
  Serial.begin(115200);
  receiver.begin();

  // optional: setting temperature unit. Default: Celsius
  //receiver.setTemperatureUnit(0); //Fahrenheit
  //receiver.setTemperatureUnit(1); //Celsius
}

void loop() {
  receiver.checkBuf();  // checks if buffer is valid

  if (receiver.isAvailable()) {
    TFAResult res = receiver.getResult();

    Serial.print("Type: "); Serial.println(res.type);
    Serial.print("ID: "); Serial.println(res.id);
    Serial.print("Battery OK: "); Serial.println(res.battery);
    Serial.print("Channel: "); Serial.println(res.channel);
    Serial.print("Temperature: "); Serial.println(res.temperature);
    Serial.print("Humidity: "); Serial.println(res.humidity);
    Serial.println("-----------");
  }
}
