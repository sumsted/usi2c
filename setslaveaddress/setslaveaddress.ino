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

byte currentAddress = 0;
byte baseAddress = 96;
byte newAddress = 0;
byte offset = 0;

long lastChangeButtonPress = 0;
long lastWriteButtonPress = 0;

byte doWriteAddress = 0;

void setup() {
    Wire.begin();
    Serial.begin(57600);
    Serial.println("start");

    currentAddress = sendFindDevice();

    pinMode (LED_1, OUTPUT);
    pinMode (LED_2, OUTPUT);

    pinMode(BUTTON_1, INPUT);
    pinMode(WRITE_BUTTON, INPUT);

    attachInterrupt(INT0, changeAddressCommand, CHANGE);
    attachInterrupt(INT1, writeAddressCommand,  CHANGE);
}

void loop() {
    if(doWriteAddress == 1){
        sendWriteAddress();
        doWriteAddress = 0;
    }
    delay(100);
}

void changeAddressCommand(){
    long buttonPress = millis();
    if(buttonPress > lastChangeButtonPress+1000){
        Serial.print("new address will be ");
        offset++;
        byte factor = offset % 4;
        newAddress = baseAddress + factor*10;
        Serial.println(newAddress);

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

void writeAddressCommand(){
    long buttonPress = millis();
    if(buttonPress > lastWriteButtonPress+1000){
        doWriteAddress = 1;
        lastWriteButtonPress = buttonPress;
    }
}

void sendWriteAddress(){
    Serial.print("writing new address ");
    Serial.print(newAddress);
    Serial.print(" to old address ");
    Serial.println(currentAddress);
    Serial.flush();
    Wire.beginTransmission(currentAddress);
    Wire.write(ADDR_UNLOCK_1);
    Wire.write(ADDR_UNLOCK_2);
    Wire.write(newAddress);
    Wire.endTransmission();
    currentAddress = sendFindDevice();
    Serial.print("done writing address, new address ");
    Serial.println(currentAddress);
}

int sendFindDevice(){
    byte address;
    int bytesWritten;
    int bytesAvailable;
    for(address=2;address<128;address+=2){
        Wire.beginTransmission(address);
        bytesWritten = Wire.write(READ_SENSOR);
        Wire.endTransmission();
        Wire.requestFrom(address | READ_OFFSET, 2);
        bytesAvailable = Wire.available();
        if(bytesWritten == 1 && bytesAvailable == 2){
            Serial.print("device found at address: ");
            Serial.println(address);
            return address;
        }
    }
}
