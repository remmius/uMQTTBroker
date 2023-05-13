#include <Arduino.h>
#include "mqtt/debug.h"
void Debug(const char *msg) {
  Serial.println(msg);
}

void Debug_b(uint8_t* buf, uint16_t len){
    uint16_t i;
    uint8_t *ptr = buf;
    for(i = 0; i < len; i++)
    {
      Serial.print((char *)ptr++);
    }
    Serial.print("\n");
}