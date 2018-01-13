import time
import smbus


class WpilibI2C:
    """ simulate """
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
    ADDRESS = 0x60 # 224 this doesn't feel right
    READ_OFFSET = 0  # 1 in real life
    RANGE_COMMAND = 81
    commandI2C = WpilibI2C(1, ADDRESS)
    locationI2C = WpilibI2C(1, ADDRESS + READ_OFFSET)

    # tell slave to ping
    commandI2C.writeBulk([RANGE_COMMAND])

    # wait
    time.sleep(.1)

    # read the results
    buffer = commandI2C.readOnly(2)
    distance = buffer[1]
    distance = (distance << 8) | buffer[0]

    print(distance)
