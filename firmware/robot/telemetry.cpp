#include "globals.h"
#include "telemetry.h"
#include "circ_buf.h"
#include <Servo.h>
#include "helper.h"

static uint8_t tel_buf[TEL_BUFFER_SIZE];
static circ_buf_t tel_cbuf = {tel_buf, 0, 0, TEL_BUFFER_SIZE};

Servo leftMotor;
Servo rightMotor;

void init_telemetry()
{
  leftMotor.attach(16);
  rightMotor.attach(19);
}

bool check_for_telemetry()
{
  if(0 != SerialBT.available())
  {
    uint8_t data = (uint8_t)SerialBT.read();
    append_byte(&tel_cbuf, data);
    return true;
  }
  return false;
}

void process_telemetry()
{
  static uint8_t data;
  static uint8_t command_buf[COMMAND_STRUCT_SIZE];
  static uint8_t cb_size = 0;

  if (0u == pop_byte(&tel_cbuf, &data))
  {
    if (cb_size == 0)
    {
      if (data == 0xAA)
        {
          command_buf[cb_size] = data;
          cb_size++;
        }
    }
    else if (cb_size == 1)
    {
      if (data == 0x55)
      {
        command_buf[cb_size] = data;
          cb_size++;
      }
      else
      {
        cb_size = 0;
      }
    }
    else if((1 < cb_size) && (cb_size< COMMAND_STRUCT_SIZE))
    {
      command_buf[cb_size] = data;
      cb_size++;
    }
    else
    {
      uint16_t * cs = (uint16_t *)&command_buf[COMMAND_STRUCT_SIZE - 2];
      if (calculate_checksum(command_buf, COMMAND_STRUCT_SIZE-2) == *cs)
      {
        execute_telemetry((command_struct_t *)&command_buf);
        Serial.println("YIPPEE!");
      }
      else
      {
        Serial.println("BAD!");
        Serial.println(*cs);
        Serial.println(calculate_checksum(command_buf, COMMAND_STRUCT_SIZE-2));
      }
      cb_size = 0;
    }


  }
}

void execute_telemetry(command_struct_t * command)
{
  switch(command->commandID)
  {
    case DRIVE_CMD: 
      int16_t differential = interpolate_f2uint16(command->data.drive_command.right, -1, 1, 1000, 2000) - 1500;
      uint16_t left = interpolate_f2uint16(command->data.drive_command.left, -1, 1, 1000, 2000);
      uint16_t right = left;

      left = left + differential;
      right = right - differential;
      
      rightMotor.writeMicroseconds((int) left);
      leftMotor.writeMicroseconds((int) right);

      Serial.println(command->data.drive_command.left);
      Serial.println(command->data.drive_command.right);

      Serial.println(left);
      Serial.println(right);
      break;
  }
}


uint16_t calculate_checksum(uint8_t * buf, uint8_t len)
{
  uint16_t checksum = 0;
  for (uint8_t i = 0; i < len; i++)
  {
    checksum = checksum + buf[i];
  }
  return checksum;
}