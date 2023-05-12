/*
 * debug.h
 *
 *  Created on: Dec 4, 2014
 *      Author: Minh
 */

#ifndef USER_DEBUG_H_
#define USER_DEBUG_H_
#include <osapi.h>//Required for os_printf

#if defined(MQTT_DEBUG_ON)

#define MQTT_INFO( format, ... ) os_printf( format, ## __VA_ARGS__ ) //this does not print in c-files (missing Serial)
#define DEBUG(msg) Debug(msg)
#define DEBUG_B(buf, len) Debug_b(buf, len)
#else
#define MQTT_INFO( format, ... )
#define DEBUG(msg)
#define DEBUG_u32(msg)
#define DEBUG_B(buf, len)
#endif

void Debug(const char *msg);
void Debug_b(uint8_t* buf, uint16_t len);

#endif /* USER_DEBUG_H_ */
