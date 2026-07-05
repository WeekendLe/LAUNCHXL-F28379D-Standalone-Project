/*
 *  SciDriverF28379D.h
 *  
 *  Created on: July 02, 2026
 *      Author: Le-Nguyen Anh-Tuan
 */

#ifndef SCIDRIVERF28379D_H_
#define SCIDRIVERF28379D_H_

// Include headers
#include "project.h"

// Define buffer size
#define TX_BUFFER_SIZE 256U
#define RX_BUFFER_SIZE 128U
#define CMD_BUFFER_SIZE 64U

// Define buffer size for string conversion functions (include sign and null)
// "65535\0"
// "123456"
#define UINT16_TO_STRING_SIZE 6U
 
// "-32768\0"
// "1234567"
#define INT16_TO_STRING_SIZE 7U
 
// "4294967295\0"
// "12345678901"
#define UINT32_TO_STRING_SIZE 11U
 
// "-2147483648\0"
// "123456789012"
#define INT32_TO_STRING_SIZE 12U
 
// "18446744073709551615\0"
// "123456789012345678901"
#define UINT64_TO_STRING_SIZE 21U
 
// "-9223372036854775808\0"
// "123456789012345678901"
#define INT64_TO_STRING_SIZE 21U

// "-999999999.9999\0"    // Number limit +-999999999.9999
// "1234567890123456"
#define FRACTIONAL_TO_STRING_SIZE 16U

// Defined in SciDriverF28379D.c
extern char cUint16String[UINT16_TO_STRING_SIZE];
extern char cInt16String[INT16_TO_STRING_SIZE];
extern char cUint32String[UINT32_TO_STRING_SIZE];
extern char cInt32String[INT32_TO_STRING_SIZE];
extern char cUint64String[UINT64_TO_STRING_SIZE];
extern char cInt64String[INT64_TO_STRING_SIZE];
extern char cFractionalString[FRACTIONAL_TO_STRING_SIZE];

// Function prototype
// Add drop the byte into hardware TX Buffer
void vWriteSCI(void);

// Add each char into the software TX Ring Buffer
void vPutCharTxRingBuffer(unsigned char ucData);

// Message to send -> Add the string into the Software TX Ring Buffer
void vMsgToSend(const char * const pcMsg);

// Process the "raw" software RX Ring Buffer, and update the command buffer if possible
bool xReadSCI(char * const pcBuffer, uint16_t usBufferSize);

// Welcome banner print
void vPrintWelcomeBanner(void);

// Convert number to string functions
void vUint16ToString(uint16_t value, char *buffer);
void vInt16ToString(int16_t value, char *buffer);
void vUint32ToString(uint32_t value, char *buffer);
void vInt32ToString(int32_t value, char *buffer);
void vUint64ToString(uint64_t value, char *buffer);
void vInt64ToString(int64_t value, char *buffer);
void vFractionalToString(double value, char *buffer);

// Interrupt Handler Prototype is declare in project.h

#endif /* SCIDRIVERF28379D_H_ */
