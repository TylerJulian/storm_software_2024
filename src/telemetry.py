import ctypes
import enum


@enum.unique
class CommandIDs(enum.Enum):
    invalid_command = 0x00
    drive_command = 0x01
    button_command = 0x02
    long_command = 0x03


class BaseStruct(ctypes.LittleEndianStructure):
    _pack_ = 1

    @property
    def fields(self):
        ret = []
        for field in self._fields_:
            field_obj = getattr(self, field[0].strip("_"))
            if isinstance(field_obj, BaseStruct):
                s_ret = field_obj.fields
                ret.append(field[0].strip("_"), s_ret)
            else:
                ret.append(field[0].strip("_"), field_obj)
        return ret

    @property
    def fields_dict(self):
        ret = {}
        for field in self._fields_:
            field_obj = getattr(self, field[0].strip("_"))
            if isinstance(field_obj, BaseStruct):
                ret[field[0].strip("_")] = field_obj.fields
            else:
                ret[field[0].strip("_")] = field_obj

        return ret


class BaseUnion(ctypes.Union):
    @staticmethod
    def get_field_name_by_id(field_id: int):
        return BaseUnion._fields_[field_id][0].strip("_")


class JoystickStruct(BaseStruct):
    _fields_ = [
        ("forward", ctypes.c_float),
        ("horizontal", ctypes.c_float),
    ]
class SpeedButtonStruct(BaseStruct):
    _fields_ = [
        ("speed_multiplier", ctypes.c_float),
        ("_buttons", ctypes.c_uint32),
    ]

    @property
    def buttons(self):
        return self._buttons

    @buttons.setter
    def buttons(self, value):
        if isinstance(value, int):
            self._buttons = value
        elif isinstance(value, tuple):
            self._buttons = self._buttons | (value[0] << value[1])
        else:
            raise Exception("INVALID buttons setting")

class DataUnion(BaseUnion):
    _fields_ = [
        ("invalid_command", ctypes.c_uint64),
        ("stick_control",  JoystickStruct),
        ("speed_button_control", SpeedButtonStruct),
        ("long_placeholder", ctypes.c_uint64),
    ]


class CommandStruct(BaseStruct):
    _fields_ = [
        ("sync", ctypes.c_uint16),
        ("_command_id", ctypes.c_uint8),
        ("data", DataUnion),
        ("_checksum", ctypes.c_uint16)
    ]

    @property
    def raw(self):
        self.sync = 0x55aa
        self.checksum

        return bytearray(self)

    @property
    def command_id(self):
        return CommandIDs(self._command_id).name

    @command_id.setter
    def command_id(self, command):
        self._command_id = command.value

    @property
    def checksum(self):
        packet = bytearray(self)[:-2]  # don't perform checksum on checksum bytes.
        ret = 0
        for byte in packet:
            ret = ret + byte

        ret = ret & 0xffff  # return only 16 bits
        self._checksum = ret
        return ret
