import ctypes
import enum


@enum.unique
class CommandIDs(enum.Enum):
    invalid_command = 0x00
    drive_command = 0x01
    long_command = 0x02


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


class TriggerStruct(ctypes.Structure):
    _fields_ = [
        ("left", ctypes.c_float),
        ("right", ctypes.c_float),
    ]


class DataUnion(BaseUnion):
    _fields_ = [
        ("invalid_command", ctypes.c_uint64),
        ("trigger_control", TriggerStruct),
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
