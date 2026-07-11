/*
 *  project.h
 *  Including all headers file
 *  Declare any function prototypes
 * 
 *  Created on: June 24, 2026
 *      Author: Le-Nguyen Anh-Tuan
 */

#ifndef PROJECT_H_
#define PROJECT_H_

// C library
#include <stdint.h>
#include <stdbool.h>

// F28379D
#include "F2837xD_device.h"
#include "F2837xD_Examples.h"

// Custom drivers
#include "SciDriverF28379D.h"

// Device library

// Function Prototype
void vHardwareSetup(void);

// Interrupt Handler
interrupt void vEPWM1_InterruptHandler(void);
interrupt void vCpuTimer1_InterruptHandler(void);

#endif /* PROJECT_H_ */
