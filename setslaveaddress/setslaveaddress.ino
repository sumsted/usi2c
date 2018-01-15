#include <Wire.h>

#define LED 13
#define READ_SENSOR 81
#define READ_OFFSET B00000000 // change to 1 for actual sensor

#define BUTTON_1 2
#define WRITE_BUTTON 3

#define LED_1 9
#define LED_2 10

#define ADDR_UNLOCK_1 170
#define ADDR_UNLOCK_2 165

byte baseAddress = 0;
byte newAddress = 0;
byte offset = 0;

long lastChangeButtonPress = 0;
long lastWriteButtonPress = 0;

void setup() {
    Wire.begin();
    Serial.begin(57600);
    Serial.println("start");

    baseAddress = findDevice();

    pinMode (LED_1, OUTPUT);
    pinMode (LED_2, OUTPUT);

    pinMode(BUTTON_1, INPUT);
    pinMode(WRITE_BUTTON, INPUT);

    attachInterrupt(INT0, changeAddress, CHANGE);
    attachInterrupt(INT1, writeAddress,  CHANGE);
}


void loop() {
//    Serial.println(find_device());
    delay(100);
}

void changeAddress(){
    Serial.print("change ");
    long buttonPress = millis();
    Serial.print(buttonPress);
    Serial.print(" - ");
    Serial.println(lastChangeButtonPress);
    if(buttonPress > lastChangeButtonPress+1000){
        Serial.print("new address will be ");
        offset++;
        byte factor = offset % 4;
        newAddress = baseAddress + factor*10;
        Serial.print(newAddress);
        Serial.print(" - ");
        Serial.print(offset);
        Serial.print(" - ");
        Serial.println(factor);

        switch(factor){
            case 0:
                digitalWrite(LED_1, 0);
                digitalWrite(LED_2, 0);
                break;
            case 1:
                digitalWrite(LED_1, 1);
                digitalWrite(LED_2, 0);
                break;
            case 2:
                digitalWrite(LED_1, 0);
                digitalWrite(LED_2, 1);
                break;
            case 3:
                digitalWrite(LED_1, 1);
                digitalWrite(LED_2, 1);
                break;
        }
        lastChangeButtonPress = buttonPress;
    }
}

void writeAddress(){
    Serial.print("write ");
    long buttonPress = millis();
    Serial.print(buttonPress);
    Serial.print(" - ");
    Serial.println(lastWriteButtonPress);
    if(buttonPress > lastWriteButtonPress+1000){
        byte oldAddress = baseAddress;
        Serial.print("writing new address ");
        Serial.println(newAddress);
        Serial.print(" to old address ");
        Serial.println(oldAddress);
        Serial.flush();
        Wire.beginTransmission(oldAddress);
        Wire.write(ADDR_UNLOCK_1);
        Wire.write(ADDR_UNLOCK_2);
        Wire.write(newAddress);
        Wire.endTransmission();

//        baseAddress = findDevice();
        lastWriteButtonPress = buttonPress;
    }
}

int findDevice(){
    byte address;
    int bytesWritten;
    int bytesAvailable;
//    Serial.println("\nlooking for active i2c addresses");
    for(address=2;address<128;address+=2){
        Wire.beginTransmission(address);
        bytesWritten = Wire.write(READ_SENSOR);
        Wire.endTransmission();
        Wire.requestFrom(address | READ_OFFSET, 2);
        bytesAvailable = Wire.available();
        if(bytesWritten == 1 && bytesAvailable == 2){
            return address;
        }
    }
}
