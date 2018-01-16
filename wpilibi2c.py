import time
import smbus


class I2C:
    """ simulate """
    class Port:
        kOnboard = 0
        kMXP = 1

    def __init__(self, port, address):
        self.port = port
        self.address = address
        self.node = smbus.SMBus(port)

    def readOnly(self, count):
        bytes = self.node.read_i2c_block_data(self.address, 0, count)
        return bytes

    def writeBulk(self, values):
        for b in values:
            self.node.write_byte(self.address, b)


if __name__ == '__main__':
    """
    So the pi does not see 224, looks like rpi is dropping leading
    bit (128) from address. Tested with 244 and got 0x74 on rpi as well
    224 = 0xE0 = 11100000
    96  = 0x60 = 01100000
    on raspberry pi use command to identify i2c connections
    pi@raspberrypi:~/apps/usi2c $ sudo i2cdetect -y 1
     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f
    00:          -- -- -- -- -- -- -- -- -- -- -- -- -- 
    10: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
    20: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
    30: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
    40: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
    50: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
    60: 60 -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
    70: -- -- -- -- -- -- -- --   
    Dk why, maybe don't set I2C address on sensors below 128 and
    assume leading bit always set.
    
    I thought this might be the trailing read/write bit missing from address.
    But this is the leading bit that is missing. I see the trailing 
    read/write bit. ex:
    arduino and sensor = 225 = 0xE1 = 11100001
    raspberry pi sees  =  97 = 0X61 = 01100001
    
    still feels like the read bit issue but the issue we see is masking of the 128 bit not shifting to the right
    96 we see should be half of 224, but isn't, it's 128 less, weird.
    
    Figure 2: 8-bit addresses. Some vendors incorrectly provide two 8-bit slave addresses for their device, 
    one to write to the device and one to read from the device. This 8-bit number actually encodes the 7-bit 
    slave address and the read/write bit. Since Total Phase's products use 7-bit addressing, it is important to 
    only use the top 7 bits of the address as the slave address.
    https://www.totalphase.com/support/articles/200349176-7-bit-8-bit-and-10-bit-I2C-Slave-Addressing
    
    I think we should assume all addresses are less than 128. This will probably work on all masters.
    Use test code to check for active addresses.
    """
    ADDRESS = 0x60
    READ_OFFSET = 0  # 1 in real life
    RANGE_COMMAND = 81
    RANGE_DELAY = .08

    # assuming
    # from wpilib import I2C
    commandI2C = I2C(I2C.Port.kOnboard, ADDRESS)
    locationI2C = I2C(I2C.Port.kOnboard, ADDRESS + READ_OFFSET)

    # tell slave to ping
    commandI2C.writeBulk([RANGE_COMMAND])

    # wait
    time.sleep(RANGE_DELAY)

    # read the results
    buffer = locationI2C.readOnly(2)
    distance = buffer[1]
    distance = (distance << 8) | buffer[0]

    print(distance)
