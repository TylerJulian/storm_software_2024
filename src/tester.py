from telemetry import *
import unittest


class TestTelemetryStruct(unittest.TestCase):
    def test_read_of_checksum(self):
        t1 = CommandStruct()
        t1.command_id = CommandIDs.long_command
        t1.data.long_placeholder = 0xdeadbeef
        self.assertNotEqual(0x00, t1.checksum)

    def test_raw_checksum(self):
        t2 = CommandStruct()
        t2.command_id = CommandIDs.long_command
        t2.data.long_placeholder = 0xdeadbeef
        self.assertNotEqual(t2.raw[-2:], b'\x00\x00')


if __name__ == '__main__':
    unittest.main()
