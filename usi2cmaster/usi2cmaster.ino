#include <Wire.h>

#define I2C_ADDRESS 224
#define READ_OFFSET B00000000  // in real work this is 1-
#define READ_SENSOR 81
#define ADDR_UNLOCK_1 170
#define ADDR_UNLOCK_2 165


void setup() {
  Wire.begin();
  Serial.begin(57600);
}

void loop() {
  delay(500);
  int distance = getDistance();
  Serial.println(distance);
}


int getDistance(){
    int distance = 0;
    // send command
    Wire.beginTransmission(I2C_ADDRESS);
    Wire.write(READ_SENSOR);
    Wire.endTransmission();

    // pause
    delay(100);

    // read result
    Wire.requestFrom(I2C_ADDRESS| READ_OFFSET, 2);
    byte highByte = Wire.read();
    byte lowByte = Wire.read();
    distance = lowByte;
    distance = (distance << 8) | highByte;
    return distance;
}