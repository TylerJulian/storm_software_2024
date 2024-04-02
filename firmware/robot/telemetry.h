#ifndef TELEMETRY_H
#define TELEMETRY_H

#define DATA_UNION_SIZE 8u
#define COMMAND_STRUCT_SIZE 13u
#define TEL_BUFFER_SIZE 256u

#pragma pack(push, 1)
typedef struct trigger_struct{
  float_t left;
  float_t right;
}trigger_struct_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef union data_union{
  uint64_t invalid_command;
  trigger_struct_t drive_command;
  uint64_t long_command;
}data_union_t;
#pragma pack(pop)
static_assert(DATA_UNION_SIZE == sizeof(data_union_t));

#pragma pack(push, 1)
typedef struct command_struct{
  uint16_t sync;
  uint8_t commandID;
  data_union_t data;
  uint16_t checksum;
}command_struct_t;
#pragma pack(pop)
static_assert(COMMAND_STRUCT_SIZE == sizeof(command_struct_t));

enum command_ids
{
  DRIVE_CMD = 0x01,
  BUTTON_CMD = 0x02,
};

bool check_for_telemetry();
void process_telemetry();
uint16_t calculate_checksum(uint8_t * buf, uint8_t len);
void execute_telemetry(command_struct_t * command);
void init_telemetry();
#endif