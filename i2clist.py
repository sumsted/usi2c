"""
List all addresses in I2C chain.
"""
import smbus


def get_i2c_addresses(port):
    addresses = []
    node = smbus.SMBus(port)
    for address in range(2, 256, 2):
        try:
            node.write_byte(address, 0x0)
            addresses.append(address)
            print("found: %d" % address)
        except IOError:
            pass
    return addresses


if __name__ == '__main__':
    print("note missing leading bit")
    for b in get_i2c_addresses(1):
        print(bin(b))
