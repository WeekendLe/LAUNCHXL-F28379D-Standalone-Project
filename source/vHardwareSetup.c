/*
 * vHardwareSetup.c
 *
 *  Created on: June 25, 2026
 *      Author: LeNguyenAnhTuan
 */

#include "project.h"

// Constant declare
#define CPU_TIMER1_FREQUENCY 1UL  // 1 s

// Public variables

// Function Prototype
void prvHardwareSetupClock(void);
void prvHardwareSetupGPIO(void);
void prvHardwareSetupEPWM(void);
void prvHardwareSetupADC(void);
void prvHardwareSetupDAC(void);
void prvHardwareSetupSCI(void);
void prvHardwareSetupTimer(void);
void prvHardwareSetupInterrupt(void);

// vHardwareSetup.c
void vHardwareSetup( void )
{
    DINT;   // Disable interrupt
    prvHardwareSetupClock();
    prvHardwareSetupGPIO(); // Setup MCU physical pins and function of those pins
    prvHardwareSetupEPWM();
    prvHardwareSetupADC();
    prvHardwareSetupDAC();
    prvHardwareSetupSCI();  // Setup SCI (Serial Communication Interface) or UART
    prvHardwareSetupTimer();
    prvHardwareSetupInterrupt();
    EINT;   // Enable interrupt
}

// pvrHardwareSetupClock
void prvHardwareSetupClock( void )
{
    InitSysCtrl();  // Setup sysclk as 200 MHz and enable peripheral clock
    
    // Set LSPCLK frequency to 100 MHz
    EALLOW;
    ClkCfgRegs.LOSPCP.bit.LSPCLKDIV = 0b001;    // 200 / 2 = 100 MHz
    EDIS;

}

// prvHardwareSetupGPIO
void prvHardwareSetupGPIO( void )
{
    InitGpio();

    // Configure GPIO31 and GPIO34
    EALLOW;                                     // GpioCtrlRegs is protected
    GpioCtrlRegs.GPAMUX2.bit.GPIO31 = 0UL;        // Ensure GPIO31 is in GPIO Mode
    GpioCtrlRegs.GPADIR.bit.GPIO31  = 1UL;        // Set GPIO31 as output

    GpioCtrlRegs.GPBMUX1.bit.GPIO34 = 0UL;        // Ensure GPIO31 is in GPIO Mode
    GpioCtrlRegs.GPBDIR.bit.GPIO34  = 1UL;        // Set GPIO31 as output
    EDIS;                                       // Disable edit for protection

    GpioDataRegs.GPADAT.bit.GPIO31 = 1UL;         // Write 1 to GPIO31
    GpioDataRegs.GPBDAT.bit.GPIO34 = 1UL;         // Write 1 to GPOP34

    // Configure GPIO0 and GPIO1 pins for EPWM1
    EALLOW;
    GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 1UL;         // Configure GPIO0 as EPWM1A (refer to LAUNCHXL-F28379D data sheet for MUX info)
    GpioCtrlRegs.GPAMUX1.bit.GPIO1 = 1UL;         // Configure GPIO1 as EPWM1B

    GpioCtrlRegs.GPAPUD.bit.GPIO0 = 0UL;          // 1: Disable pull-up (when there is gate driver circuit, to save power)
    GpioCtrlRegs.GPAPUD.bit.GPIO1 = 0UL;          // 0: Enable pull-up
    EDIS;

    // Configure GPIO42 and GPIO43 for SCIA TX and RX
    EALLOW;
    GpioCtrlRegs.GPBGMUX1.bit.GPIO43 = 0b11;    // Configure GPIO43 as SCIARX
    GpioCtrlRegs.GPBMUX1.bit.GPIO43 = 0b11;     
    GpioCtrlRegs.GPBQSEL1.bit.GPIO43 = 0b11;    // Input qualification type: Async
    
    GpioCtrlRegs.GPBMUX1.bit.GPIO42 = 0b11;     // Configure GPIO42 as SCIATX
    GpioCtrlRegs.GPBGMUX1.bit.GPIO42 = 0b11;       
    EDIS;
    
    DELAY_US(1000 * 1000);                      // Delay to power up
}

// prvHardwareSetupEPWM
void prvHardwareSetupEPWM( void )
{
    // Configure peripheral clock for EPWM
    EALLOW;
    ClkCfgRegs.PERCLKDIVSEL.bit.EPWMCLKDIV = 1UL; // 200 MHz / 2 = 100 MHz
    EDIS;
    
    // Configure EPWM1 module (normal PWM with interrupt and served as SOC signal for ADC)
    EPwm1Regs.TBCTL.bit.CTRMODE = 3U;            // Freeze counter operation (effectively stop the EPWM module)
    EPwm1Regs.TBCTL.bit.HSPCLKDIV = 0U;          // High speed time base clock pre-scale (= 0, i.e. divided by 1)
    EPwm1Regs.TBCTL.bit.CLKDIV = 0U;             // Time base clock pre-scale (= 0, i.e. divided by 1)
    // EPWM counter value calculation:
    // (EPWMCLK / (HSPCLKDIV x CLKDIV)) / (PwmFrequecy) Ex: (100e6/(1x1))/(10e3) = 10000
    EPwm1Regs.TBPRD = 10000U;                     // Set the counter value to result to the desire PWM frequency
    EPwm1Regs.TBCTL.bit.PHSEN = 0U;              // Disable phase loading (use to synchronize with other EPWM module)
    EPwm1Regs.TBPHS.bit.TBPHS = 0U;              // Phase is 0
    EPwm1Regs.TBCTR = 0U;                        // Clear the counter (ensure that counter start at 0)
    EPwm1Regs.TBCTL.bit.SYNCOSEL = 1U;           // Set EPWM1 as master to sync other EPWMx module; for slave EPWMx, use PHSEN bits

    // Setup shadow register load on ZERO
    EPwm1Regs.CMPCTL.bit.SHDWAMODE = 0U;
    EPwm1Regs.CMPCTL.bit.LOADAMODE = 0U;
    EPwm1Regs.CMPCTL.bit.SHDWBMODE = 0U;
    EPwm1Regs.CMPCTL.bit.LOADBMODE = 0U;

    // Set compare value (duty)
    EPwm1Regs.CMPA.bit.CMPA = 500U;              // Set compare A value
    EPwm1Regs.CMPB.bit.CMPB = 250U;              // Set compare B value

    // Set action
    EPwm1Regs.AQCTLA.bit.ZRO = 2U;               // Set PWM1A to output high
    EPwm1Regs.AQCTLA.bit.CAU = 1U;               // Set PWM1A to output low
    EPwm1Regs.AQCTLB.bit.ZRO = 1U;               // Set PWM1B to output low
    EPwm1Regs.AQCTLB.bit.CBU = 2U;               // Set PWM1B to output high

    // Setup ADC SOC
    EPwm1Regs.ETSEL.bit.SOCAEN = 0U;             // Disable the ADC Start of Conversion A Pulse
    EPwm1Regs.ETSEL.bit.SOCASEL = 1U;            // Determine when a EPWM1SOCA pulse will be generated
    EPwm1Regs.ETSEL.bit.SOCAEN = 1U;             // Enable the ADC Start of Conversion A Pulse
    EPwm1Regs.ETPS.bit.SOCAPRD = 1U;             // Generate the EPWM1SOCA pulse on the first event

    // Enable EPWM1 Interrupt
    EPwm1Regs.ETSEL.bit.INTSEL = 1U;             // Enable INT whentime-base counter equal to zero
    EPwm1Regs.ETSEL.bit.INTEN = 1U;              // Enable EPWM1_INT generation
    EPwm1Regs.ETPS.bit.INTPRD = 1U;              // Generate an interrupt on the first event

    // Enable PWM1 module
    //EPwm1Regs.TBCTL.bit.CTRMODE = 0;            // Un-freeze counter operation, power up, and enter count-up mode (can be used outside of this function in main)

}

// pvrHarwareSetupADC
void prvHardwareSetupADC ( void )
{
    // ADCA
    EALLOW;
    AdcaRegs.ADCCTL2.bit.PRESCALE = 6U;          // Set ADC Clock Prescaler to 4
    AdcSetMode(ADC_ADCA, ADC_RESOLUTION_12BIT, ADC_SIGNALMODE_SINGLE);  // Using provided function to set resolution and signal mode
    AdcaRegs.ADCCTL1.bit.INTPULSEPOS = 1U;       // Interrupt pulse generation occurs at the end of the conversion
    AdcaRegs.ADCCTL1.bit.ADCPWDNZ = 1U;          // Power up ADCA
    EDIS;
    DELAY_US(1000);                             // Delay to power up ADCA
    EALLOW;
    AdcaRegs.ADCSOC0CTL.bit.ACQPS = 25U;         // Select sample and hold time for this SOC based on the number of system clock
    AdcaRegs.ADCSOC0CTL.bit.CHSEL = 0U;          // Select the channel to be converted (ADCINA0 or pin 30 on the launchpad)
    AdcaRegs.ADCSOC0CTL.bit.TRIGSEL = 5U;        // SOC0 trigger source select (ePWM1 will generate SOCA trigger to SOCA)
    EDIS;
}

// prvHardwareSetupDAC
void prvHardwareSetupDAC ( void )
{
    // Configure for DACOUTB (Pin 70 on LAUNCHXL-F28379D)
    EALLOW;
    DacbRegs.DACCTL.bit.DACREFSEL = 1U;          // ADC VREFHI/VSSA are the reference voltages
    DacbRegs.DACCTL.bit.LOADMODE = 0U;           // Load on next SYSCLK
    DacbRegs.DACOUTEN.bit.DACOUTEN = 1U;         // Enable DAC output
    EDIS;
    DELAY_US(10);                               // Delay for DAC to power up
    DacbRegs.DACVALS.all = 2048U;                // DAC output (0 < DAC < 4095)
}

// prvHardwareSetupSCI
void prvHardwareSetupSCI ( void )
{
    // Set up the SCIA module using GPIO42 and GPIO43 as pinout
    // Set up FIFO for SCIA
    // Enable FIFO enhancements, disable TX interrupt
    SciaRegs.SCIFFTX.bit.SCIRST = 0b1;
    SciaRegs.SCIFFTX.bit.SCIFFENA = 0b1;
    SciaRegs.SCIFFTX.bit.TXFIFORESET = 0b1;
    SciaRegs.SCIFFTX.bit.TXFFST = 0b00000;
    SciaRegs.SCIFFTX.bit.TXFFINT = 0b0;
    SciaRegs.SCIFFTX.bit.TXFFINTCLR = 0b1;
    SciaRegs.SCIFFTX.bit.TXFFIENA = 0b0;
    SciaRegs.SCIFFTX.bit.TXFFIL = 0b00000;
    // Enable RX FIFO
    SciaRegs.SCIFFRX.bit.RXFFOVF = 0b0;
    SciaRegs.SCIFFRX.bit.RXFFOVRCLR = 0b0;
    SciaRegs.SCIFFRX.bit.RXFIFORESET = 0b1;
    SciaRegs.SCIFFRX.bit.RXFFST = 0b00000;
    SciaRegs.SCIFFRX.bit.RXFFINT = 0b0;
    SciaRegs.SCIFFRX.bit.RXFFINTCLR = 0b0;
    SciaRegs.SCIFFRX.bit.RXFFIENA = 0b1;
    SciaRegs.SCIFFRX.bit.RXFFIL = 0b00001;
    
    SciaRegs.SCIFFCT.all = 0x0000U;

    // Configure SCIA module
    
    // *
    // 1 stop bit,  No loopback, No parity, 8 char bits, Async mode, Idle-line protocol
    // *
    SciaRegs.SCICCR.bit.STOPBITS = 0U;
    SciaRegs.SCICCR.bit.PARITY = 0U;
    SciaRegs.SCICCR.bit.PARITYENA = 0U;
    SciaRegs.SCICCR.bit.LOOPBKENA = 0U;
    SciaRegs.SCICCR.bit.ADDRIDLE_MODE = 0U;
    SciaRegs.SCICCR.bit.SCICHAR = 0b111;

    // *
    // SCIA set at 115200 baud
    // *
    /* BRR = (100 000 000 / (115200 * 8)) - 1
    ** BRR = 107.5 = 108 = 0x006C
    ** => SCIHBAUD = 0x00 and SCILBAUD = 0x6C
    */
    SciaRegs.SCIHBAUD.all = 0x00U;
    SciaRegs.SCILBAUD.all = 0x6CU;
    
    // Enable SCIA transmitter and receiver
    SciaRegs.SCICTL1.bit.TXENA = 1U;
    SciaRegs.SCICTL1.bit.RXENA = 1U;
    SciaRegs.SCICTL2.bit.RXBKINTENA = 1U;   // Enable RX interrupt
    SciaRegs.SCICTL1.bit.SWRESET = 1U;  // Release SCIA from reset
}

// prvHardwareSetupTimer
void prvHardwareSetupTimer( void )
{
    // Using CPU TIMER1 for logging and process SCI message
    // Stop CPU TIMER1
    CpuTimer1Regs.TCR.bit.TSS = 1U;
    // Disable CPU TIMER1 Interrupt during config
    CpuTimer1Regs.TCR.bit.TIE = 0U;
    // Clear CPU TIMER1 Overflow Flag
    CpuTimer1Regs.TCR.bit.TIF = 1U;
    // Stop CPU TIMER1 during a debug's breakpoint/halt
    CpuTimer1Regs.TCR.bit.FREE = 0U;
    CpuTimer1Regs.TCR.bit.SOFT = 0U;

    // Set CPU TIMER1 Prescale Value = 1 + 1
    // SysClk / 2 = 100 MHz
    CpuTimer1Regs.TPR.all = 1U;
    CpuTimer1Regs.TPRH.all = 0U;

    // Set CPU TIMER1 Period
    uint32_t ulTimer1Period = (100000000UL / CPU_TIMER1_FREQUENCY) - 1UL;
    CpuTimer1Regs.PRD.all = ulTimer1Period;

    // Reload CPU TIMER1 Counter
    CpuTimer1Regs.TCR.bit.TRB = 1U;

    // Enable CPU TIMER1 Interrupt
    CpuTimer1Regs.TCR.bit.TIE = 1U;

    // Start CPU TIMER1 - Can be done in main.c
    // CpuTimer1Regs.TCR.bit.TSS = 0U;
}

// prvHarwareSetupInterrupt
void prvHardwareSetupInterrupt( void )
{
    // Clear all interrupts and initialize PIE vector table
    DINT;   // Disable gobal interrupt
    InitPieCtrl();
    IER = 0x0000U;
    IFR = 0x0000U;
    InitPieVectTable();

    // Remap the default ISR functions to user custom ISR (declared in project.h)
    EALLOW;
    PieVectTable.EPWM1_INT = &vEPWM1_InterruptHandler;       // Point to EPWM1 custom ISR
    PieVectTable.SCIA_RX_INT = &vSCIA_RX_InterruptHandler;   // Point to SCIA RX custom ISR
    PieVectTable.TIMER1_INT = &vCpuTimer1_InterruptHandler;  // Point to CPU TIMER1 custom ISR
    EDIS;

    // Check the PIE INTERRUPT TABLE (p. 95 Reference Manual)
    PieCtrlRegs.PIECTRL.bit.ENPIE = 1U; // Enable vector fetching from ePIE block
    PieCtrlRegs.PIEIER3.bit.INTx1 = 1U; // ePWM1 interrupt enable
    PieCtrlRegs.PIEIER9.bit.INTx1 = 1U; // SCIA RX interrupt enable

    // Enale CPU INT Vector (p. 94 Reference Manual)
    IER |= M_INT3;  // Enable interrupt group 3 (PIE output go to CPU INT vector 3)
    IER |= M_INT9;  // Enable interrupt group 9 (PIE output go to CPU INT vector 9)
    IER |= M_INT13; // Enable interrupt vector 13 (which connects directly to the CPU INT vector 13)

    EINT;   // Enable global interrupt 
    ERTM;
}
