#include <Wire.h>

#define I2C_ADDRESS 224

#define READ_SENSOR 81
#define ADDR_UNLOCK_1 170
#define ADDR_UNLOCK_2 165

void setup() {
  Wire.begin();
  Serial.begin(57600);
  Serial.print("start master");
}

void loop() {
  Serial.print("x: ");
  Serial.println(x);
  delay(500);
  distance = getDistance();
  Serial.print(distance);
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
    Wire.requestFrom(I2C_ADDRESS|B00000001, 2);
    highByte = Wire.read();
    lowByte = Wire.read();
    distance = (highByte * 256) + lowByte;
    return distance;
}