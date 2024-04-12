#include "globals.h"
#include "telemetry.h"
#include "circ_buf.h"
#include <Servo.h>
#include "helper.h"
#include <WiFi.h>
#include <WiFiUdp.h>

#ifndef STASSID
#define STASSID "DangerZone"
#define STAPSK "epicrobots"
#endif
unsigned int localPort = 8888;  // local port to listen on

// buffers for receiving and sending data
char packetBuffer[UDP_TX_PACKET_MAX_SIZE + 1];  // buffer to hold incoming packet,
char ReplyBuffer[] = "acknowledged\r\n";        // a string to send back

WiFiUDP Udp;

static uint8_t tel_buf[TEL_BUFFER_SIZE];
static circ_buf_t tel_cbuf = {tel_buf, 0, 0, TEL_BUFFER_SIZE};

Servo leftMagnet;
Servo rightMagnet;

void init_telemetry()
{
  WiFi.begin(STASSID, STAPSK);
  leftMagnet.attach(6);
  rightMagnet.attach(7);

    while (WiFi.status() != WL_CONNECTED) {
      Serial.print('.');
      delay(500);
  }

  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());
  Serial.printf("UDP server on port %d\n", localPort);
  Udp.begin(localPort);

}

bool check_for_telemetry()
{
  // if(0 != Serial.available())
  // {
  //   uint8_t data = (uint8_t)Serial.read();
  //   append_byte(&tel_cbuf, data);
  //   return true;
  // }
  int packetSize = Udp.parsePacket();
  if (packetSize) {
    Serial.printf("Received packet of size %d from %s:%d\n    (to %s:%d)\n", packetSize, Udp.remoteIP().toString().c_str(), Udp.remotePort(), Udp.destinationIP().toString().c_str(), Udp.localPort());

    // read the packet into packetBufffer
    int n = Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
    packetBuffer[n] = 0;
    Serial.print("size :");
    Serial.println(n);

    append_buffer(&tel_cbuf, (uint8_t *) packetBuffer, n);
    // send a reply, to the IP address and port that sent us the packet we received
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write(ReplyBuffer);
    Udp.endPacket();
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
  Serial.println(command->commandID);
  switch(command->commandID)
  {
    case DRIVE_CMD: 
    {
      int16_t differential = command->data.drive_command.right * 255;
      int16_t left = command->data.drive_command.left * 255;
      int16_t right = left;

      left = left + differential;
      right = right - differential;

      if (left > 255) left = 255;
      if (left < -255) left = -255;
      if (right > 255) right = 255;
      if (right < -255) right = 255;
      if (left < 0)
      {
      analogWrite(18, -left);
      analogWrite(19, 0);
      }
      else
      {
        analogWrite(18, 0);  
          analogWrite(19, left);   
      }
      if (right < 0)
      {
      analogWrite(20, -right);
      analogWrite(21, 0);
      }
      else
      {
          analogWrite(20, 0);
          analogWrite(21, right);   
      }

      Serial.println(command->data.drive_command.left);
      Serial.println(command->data.drive_command.right);

      Serial.println(left);
      Serial.println(right);
      break;
    }
    case BUTTON_CMD:{
      uint32_t buttons = command->data.button_command.buttons;
      if (buttons & 0x01)
      {
        leftMagnet.writeMicroseconds(1750);
      }
      else
      {
        leftMagnet.writeMicroseconds(1250);
      }
      break;
    }
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