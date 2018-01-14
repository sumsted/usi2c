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

byte newAddress = 96;
byte offset = 0;

int lastChangeButtonPress = 0;
int lastWriteButtonPress = 0;

void setup() {
  Wire.begin();
  Serial.begin(57600);
  pinMode (LED_1, OUTPUT);
  pinMode (LED_2, OUTPUT);

  pinMode(BUTTON_1, INPUT);
  pinMode(WRITE_BUTTON, INPUT);

  attachInterrupt(INT0, changeAddress, CHANGE);
  attachInterrupt(INT1, writeAddress, FALLING);
}


void loop() {
    Serial.println(find_device());
    delay(100);
}

void changeAddress(){
    int buttonPress = millis();
    if(buttonPress > lastChangeButtonPress+1000){
        Serial.print("new address will be ");
        offset++;
        offset = offset>3 ? 0 : offset;
        newAddress += offset*10;
        Serial.println(newAddress);

        switch(offset){
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
    int i;
    int buttonPress = 0;
    if(buttonPress > lastWriteButtonPress+1000){
        byte oldAddress = find_device();
        Serial.print("writing new address ");
        Serial.println(newAddress);
        Serial.print(" to old address ");
        Serial.println(oldAddress);
        Wire.beginTransmission(oldAddress);
        Wire.write(ADDR_UNLOCK_1);
        Wire.write(ADDR_UNLOCK_2);
        Wire.write(newAddress);
        Wire.endTransmission();
        for(i=0;i<5;i++){
            digitalWrite(LED_1, 1);
            digitalWrite(LED_2, 1);
            delay(50);
            digitalWrite(LED_1, 0);
            digitalWrite(LED_2, 0);
            delay(50);
        }
        lastWriteButtonPress = buttonPress;
    }
}

int find_device(){
    byte address;
    int bytesWritten;
    int bytesAvailable;
    Serial.println("\nlooking for active i2c addresses");
    for(address=2;address<128;address+=2){
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
            return address;
        }
    }
}
