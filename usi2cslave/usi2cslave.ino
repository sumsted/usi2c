/*
 * Test harness simulating US MB1242 I2C interface
 * The 1242 has two I2C addresses, an even addr defaulted to 224, and an odd address + 1, default 225
 * The even address is the address the sensor reads the range and change address commands from.
 * The odd address is the address the sensor makes available the location bytes, high and low.
 * To get the current range:
 * 1. master sends 81 to even address , default 224
 * 2. master waits at least 100ms as it takes this long to get sensor ping THIS IS A BLOCK
 * 3. master reads from odd address, def 225, the two location bytes
 * 4. master combines high and low bytes to get int representing the range in cm
 * other notes
 * - may want to 3d print a horn for sensor
 * - need to ensure multiple sensors don't fire at same time
 * - as sensor ping is blocking can we run pings in a separate process on rio like auto mode in wpilib us?
 * - other option for auto ping is running ic2 interface on jetson and using redis to provide last pinged loc
 * - but if jetson goes down, no us readings, some risk
 * - will us only be used for autonomous
 */

#include <Wire.h>

#define LED 13
#define I2C_ADDRESS 224

#define READ_SENSOR 81
#define ADDR_UNLOCK_1 170
#define ADDR_UNLOCK_2 165

int currentDistance = 20;
int lastDistanceRead = 0;

void setup() {
    Serial.begin(57600);
    Serial.println("slave starting");
    pinMode (LED, OUTPUT);

    Wire.begin(I2C_ADDRESS);
    Wire.onReceive(receiveEvent); // default 224 addr
    Wire.onRequest(requestEvent); // default 225 addr
}

/*
 * on the real bot this is would be the i2c addr even number that receives a reading or change address command
 */
void receiveEvent(int bytes) {
    Serial.println("receive event");
    processCommand(bytes);
}

/*
 * for a master to read the location they will access the read addr + 1, an odd num above the read addr
 */
void requestEvent(){
    Serial.println("request event");
    sendSensorData();
}

/*
 * simulate distance in cm
 */
void loop() {
    digitalWrite(LED, HIGH);
    delay(50);
    digitalWrite(LED, LOW);
    delay(50);
    currentDistance++;
    if(currentDistance > 750){
        currentDistance = 20;
    }
}


void processCommand(int bytes){
    if(bytes > 0){
        byte command = Wire.read();    // read one character from the I2C
        if(command == READ_SENSOR){
            readSensorCommand();
        } else if(bytes == 3){
            byte secondByte = Wire.read();
            if(command == ADDR_UNLOCK_1 && secondByte == ADDR_UNLOCK_2){
                byte newAddress = Wire.read();
                changeAddressCommand(newAddress);
            }
        }
    }
}

void readSensorCommand(){
    Serial.print("reading sensor:");
    Serial.println(currentDistance);
    lastDistanceRead = currentDistance;
}

void sendSensorData(){
    Serial.print("lastDistanceRead send:");
    Serial.println(lastDistanceRead);
    byte bytes[2];
    bytes[0] = lastDistanceRead & 0xff;
    bytes[1] = (lastDistanceRead>>8) & 0xff;
    Serial.println(bytes[0]);
    Serial.println(bytes[1]);
    Wire.write(bytes,2);
}

void changeAddressCommand(byte newAddress){
}