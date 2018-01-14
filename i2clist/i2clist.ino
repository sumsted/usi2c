#include <Wire.h>

#define READ_SENSOR 81
#define READ_OFFSET B00000000 // change to 1 for actual sensor

void setup() {
  Wire.begin();
  Serial.begin(57600);
  print_i2c_addresses();
}

void print_i2c_addresses(){
    int address;
    int bytesWritten;
    int bytesAvailable;
    Serial.println("\nlooking for active i2c addresses");
    for(address=2;address<256;address+=2){
        Wire.beginTransmission(address);
        bytesWritten = Wire.write(READ_SENSOR);
        Wire.endTransmission();
        delay(100);
        Wire.requestFrom(address | READ_OFFSET, 2);
        bytesAvailable = Wire.available();
        if(address%10 == 0){
            Serial.print(".");
        }
        if(bytesWritten == 1 && bytesAvailable == 2){
            Serial.print("\t");
            Serial.print(address);
            Serial.print("\t");
        }
    }
}

void loop() {

}
