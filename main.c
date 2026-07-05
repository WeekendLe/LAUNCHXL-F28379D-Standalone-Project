/* LAUNCHXL_F28379D Standalone project setup
** by LE-NGUYEN ANH-TUAN
**       (WEEKEND LE)
** 
** NOTICE: This project is setup for the Launch Pad - LAUNCHXL_F28379D
**
**/

/**
** main.c
**/

#include "project.h"

// Constant

// Command Buffer
volatile unsigned char ucCmdBuffer[CMD_BUFFER_SIZE];	// Store valid command

// Global variables
uint16_t ADCA = 0U;
uint16_t DACB = 1000U;
uint32_t cnt = 10UL;

/*
*	main.c
*/
void main( void )
{
	// Hardware Setup
	vHardwareSetup(); 

	GpioDataRegs.GPADAT.bit.GPIO31 = 0U;
	EPwm1Regs.TBCTL.bit.CTRMODE = 0U;	// Enable EPWM1
	GpioDataRegs.GPBDAT.bit.GPIO34 = 0U;
	CpuTimer1Regs.TCR.bit.TSS = 0U;	// Start the timer
	
	// Init Complete
	vPrintWelcomeBanner();
	
	for(;;)
	{
		
	}
}

interrupt void vEPWM1_InterruptHandler( void )
{
	ADCA = AdcaResultRegs.ADCRESULT0;
	DacbRegs.DACVALS.all = DACB;
	cnt++;
	
	// CLEAR interrupt flag
    EPwm1Regs.ETCLR.bit.INT = 1U;
    PieCtrlRegs.PIEACK.all |= PIEACK_GROUP3;
}

interrupt void vCpuTimer1_InterruptHandler( void )
{
	// Clear CPU TIMER1 Overflow Flag 
	// (Just status clear -> Do no affect interrupt functionality)
	CpuTimer1Regs.TCR.bit.TIF = 1U;

	vPrintWelcomeBanner();
	// Take a snapshot of the ADCA
	uint16_t usAdca = ADCA;
	vMsgToSend("ADCA: ");
	vUint16ToString(usAdca, cUint16String);
	vMsgToSend(cUint16String);
	vMsgToSend("\n\r");
	// Send it
	vWriteSCI();
	// Toggle LED
	GpioDataRegs.GPBTOGGLE.bit.GPIO34 = 1U;
}
