/*
 *  SciDriverF28379D.c
 *  
 *  Created on: July 02, 2026
 *      Author: Le-Nguyen Anh-Tuan
 */

#include "project.h"
#include "SciDriverF28379D.h"

// Transmit Software Ring Buffer
static volatile unsigned char ucTxBuffer[TX_BUFFER_SIZE];
static volatile uint16_t usTxHead = 0U;
static volatile uint16_t usTxTail = 0U;

// Receive Software Ring Buffer
static volatile unsigned char ucRxBuffer[RX_BUFFER_SIZE];  
static volatile uint16_t usRxHead = 0U;
static volatile uint16_t usRxTail = 0U;

// Global arrays/buffers to store the converted string
char cUint16String[UINT16_TO_STRING_SIZE];
char cInt16String[INT16_TO_STRING_SIZE];
char cUint32String[UINT32_TO_STRING_SIZE];
char cInt32String[INT32_TO_STRING_SIZE];
char cUint64String[UINT64_TO_STRING_SIZE];
char cInt64String[INT64_TO_STRING_SIZE];
char cFractionalString[FRACTIONAL_TO_STRING_SIZE];

// Dedicated Print Welcome Banner
void vPrintWelcomeBanner( void )
{
    uint16_t i = 0U;
	
	vMsgToSend("===========================================\r\n");
	vMsgToSend("    LAUNCHXL-F28379D STANDALONE PROJECT\r\n");
	vMsgToSend("===========================================\r\n");
	vMsgToSend(" [AUTHOR]: LE-NGUYEN ANH-TUAN (WEEKEND LE)\r\n");
	vMsgToSend(" [DATE]: 26 JUNE 2026\r\n");
	vMsgToSend("=====================V=====================\r\n");
	
	for ( i = 0U; i < TX_BUFFER_SIZE; i++)
	{
		vWriteSCI();
		DELAY_US(100U);
	}
	
	i = 0U;
	vMsgToSend("                   /\\_/\\                 \r\n");
	vMsgToSend("            -WL-  ( o.o ) -WL-             \r\n");
	vMsgToSend("                   > ^ <                   \r\n");
	for ( i = 0U; i < TX_BUFFER_SIZE; i++)
	{
		vWriteSCI();
		DELAY_US(100U);
	}
}

// SCIA RX ISR
interrupt void vSCIA_RX_InterruptHandler( void )
{
	// Read the received word from the hardware UART receive buffer
	uint16_t usReceivedWord = SciaRegs.SCIRXBUF.all;

	// Extract the lower 8 bit (as the data is only 8 bit but the buffer is 16 bit register)
	unsigned char ucRxByte = (unsigned char) (usReceivedWord & 0x00FFU);

	// Advance RxHead to next position
	uint16_t usNextRxHead = usRxHead + 1U;
	if (usNextRxHead > (RX_BUFFER_SIZE - 1U))
	{
		usNextRxHead = 0U;
	}

	if( usNextRxHead != usRxTail)
	{
		ucRxBuffer[usRxHead] = ucRxByte;
		usRxHead = usNextRxHead;

		// Put the receive char back to software TxRingBuffer (echo back)
		vPutCharTxRingBuffer(ucRxByte);
	}
	// If the Rx FIFO is full then the char is dropped
	
	// Clear interrupt flag
	SciaRegs.SCIFFRX.bit.RXFFOVRCLR = 1U;   // Clear Overflow flag
    SciaRegs.SCIFFRX.bit.RXFFINTCLR = 1U;   // Clear Interrupt flag

    PieCtrlRegs.PIEACK.all |= PIEACK_GROUP9;
}

// Drop bytes from software TX ring buffer to hardware TX FIFO -> Send
void vWriteSCI( void )
{
	/* 
	** CRITICAL SECTION START: Temporary disable RX interrupt
	** Reason: The RX Interrupt that is function as echo back will write byte to software TX buffer,
	** that also modified the usTxHead and usTxTail
	*/
	uint16_t usRxIerState = PieCtrlRegs.PIEIER9.bit.INTx1;
	PieCtrlRegs.PIEIER9.bit.INTx1 = 0U;
	
	// Check if the hardware TX FIFO is not full
	// AND if the software buffer is not empty
	while ((SciaRegs.SCIFFTX.bit.TXFFST < 16U) && (usTxHead != usTxTail))
	{
		/*
		** Feed bytes from software TX ring buffer to hardware TX FIFO
		*/
		SciaRegs.SCITXBUF.all = ucTxBuffer[usTxTail];
		
		// Advance tail pointer circularly
    	uint16_t usNextTxTail = usTxTail + 1U;
    	if( usNextTxTail > (TX_BUFFER_SIZE - 1U) )
    	{
       		usNextTxTail = 0U;
    	}
    	usTxTail = usNextTxTail;
	}

	volatile uint16_t i;
	for(i = 0U; i < 20; i++)
	{
    	asm(" NOP");
	}
	
	/* CRITICAL SECTION END: Restore RX interrupt */
	PieCtrlRegs.PIEIER9.bit.INTx1 = usRxIerState;
}

// Put the message into the Software TX Ring Buffer
void vPutCharTxRingBuffer( unsigned char ucData )
{
	uint16_t usNextTxHead = usTxHead + 1U;	// Calculate the next slot in the Ring Buffer
	if (usNextTxHead > (TX_BUFFER_SIZE - 1U)) // Wrap around for Ring Buffer Implementation
	{
		usNextTxHead = 0U;
	}
	/*
     * Check for software buffer overflow.
     * If the next head would equal the tail, the buffer is full and
     * the byte is silently dropped – this ensures the function never blocks 
     * (fire‑and‑forget behaviour) => Risk of overflow data if sending to fast
     */
	if ( usNextTxHead != usTxTail )
	{
		ucTxBuffer[usTxHead] = ucData;	// write to current slot
		usTxHead = usNextTxHead;	// Move the pointer to next slot
	}
}

// Message to send to the Software TX Ring Buffer
// Basically what the string to send
// Example: vMsgToSend(" string to send ");
void vMsgToSend( const char * const pcMsg )
{
	uint16_t usIdx = 0U;

	/*
    ** CRITICAL SECTION START: protect the loop that writes a whole string
    ** into the software TX ring buffer. 
	** Disable RX interrupt (echo path)
    ** once before the loop and restore after the loop, avoiding repeated
    ** enable/disable inside the iteration.
    */
	uint16_t usRxIerState = PieCtrlRegs.PIEIER9.bit.INTx1;
	PieCtrlRegs.PIEIER9.bit.INTx1 = 0U;

	while( pcMsg[usIdx] != '\0' )
	{
		vPutCharTxRingBuffer( (unsigned char)pcMsg[usIdx] );
		usIdx = usIdx + 1U;
	}

	/* CRITICAL SECTION END: Restore RX interrupt */
	PieCtrlRegs.PIEIER9.bit.INTx1 = usRxIerState;
}


/*
** Read the receive message and store it in a pre-defined buffer
** Example:
** uint16_t ucExampleBufferSize = 16;
** unsigned char ucExampleBuffer[16];
** xReadSCI( (char)*ucExampleBuffer, ucExampleBufferSize)
** *NOTE: This function return TRUE if the message is stored successfully
*/
bool xReadSCI(char * const pcBuffer, uint16_t usBufferSize)
{
	bool xResult = false;

	// Sanity check:
	if( (pcBuffer == NULL) || (usBufferSize == 0U))
	{
		return false;
	}
	xResult = false;

	/*
    ** CRITICAL SECTION START: protect the code that process the software RX Ring Buffer
	** Disable RX interrupt
    */
	uint16_t usRxIerState = PieCtrlRegs.PIEIER9.bit.INTx1;
	PieCtrlRegs.PIEIER9.bit.INTx1 = 0U;

	// If the RX Ring Buffer is empty -> Nothing to process
	if (usRxHead == usRxTail)
	{
		/* CRITICAL SECTION END: Restore RX interrupt */
		PieCtrlRegs.PIEIER9.bit.INTx1 = usRxIerState;
		return false;
	}

	/*
	** First, scan the whole RX Ring Buffer line terminator character and measure message length
	** Terminator character can be:
	** - One character: "\n" or "\r"
	** - Two character: "\r\n" or "\n\r"
	*/
	uint16_t usIdx = usRxTail;
	uint16_t usMsgLen = 0U;
	uint16_t usTerminatorLen = 0U;

	while( usIdx != usRxHead )
	{
		unsigned char ucCh = ucRxBuffer[usIdx];

		if( (ucCh == (unsigned char)'\r') || (ucCh == (unsigned char)'\n'))
		{
			usTerminatorLen = 1U;
			uint16_t usNextIdx = usIdx + 1U;
			
			// Move the Idx to next slot to check if there is another terminator
			if( usNextIdx > (RX_BUFFER_SIZE - 1U) )
			{
				usNextIdx = 0U;
			}
			
			if( usNextIdx != usRxHead)
			{
				unsigned char ucNextCh = ucRxBuffer[usNextIdx];
				if( ((ucCh == (unsigned char)'\r') && (ucNextCh == (unsigned char)'\n')) ||
					((ucCh == (unsigned char)'\n') && (ucNextCh == (unsigned char)'\r')))
				{
					usTerminatorLen = 2U;
				}
			}
			break;
		}
		
		usMsgLen = usMsgLen + 1U;
		usIdx = usIdx + 1U;
		if( usIdx > (RX_BUFFER_SIZE - 1U))
		{
			usIdx = 0U;
		}
	}

	// If no terminator, the line have not fill in the RX Ring Buffer yet
	if( usTerminatorLen == 0U)
	{
		/* CRITICAL SECTION END: Restore RX interrupt */
		PieCtrlRegs.PIEIER9.bit.INTx1 = usRxIerState;
		return false;
	}

	// If the receiving line larger than provided buffer -> flush the usRxTail to dismiss this cmd
	if( usMsgLen >= usBufferSize )
	{
		usRxTail = usRxTail + usMsgLen + usTerminatorLen;
		if( usRxTail > (RX_BUFFER_SIZE - 1) )
		{
    		usRxTail = usRxTail - (usRxTail / RX_BUFFER_SIZE) * RX_BUFFER_SIZE;
		}
		
		/* CRITICAL SECTION END: Restore RX interrupt */
		PieCtrlRegs.PIEIER9.bit.INTx1 = usRxIerState;
		return false;
	}

	/*
	** Second, copy the msg from the RX Ring Buffer in to the provided buffer
	*/
	uint16_t usCopyCount = usMsgLen;
	uint16_t usLastIdx = 0U;
	
	while( usCopyCount > 0U )
	{
		pcBuffer[usLastIdx] = (unsigned char)ucRxBuffer[usRxTail];
		usLastIdx = usLastIdx + 1U;

		// Advance tail circularly
		usRxTail = usRxTail + 1U;
		if( usRxTail > (RX_BUFFER_SIZE - 1U) )
		{
			usRxTail = 0U;
		}

		usCopyCount = usCopyCount - 1U;
	}

	pcBuffer[usLastIdx] = (unsigned char)'\0';

	// Skip the terminator character
	uint16_t i = 0U;
	for( i = 0U; i < usTerminatorLen; i++ )
	{
		usRxTail = usRxTail + 1U;
		if( usRxTail > (RX_BUFFER_SIZE - 1U) )
		{
			usRxTail = 0U;
		}
	}
	
	xResult = true;

	/* CRITICAL SECTION END: Restore RX interrupt */
	PieCtrlRegs.PIEIER9.bit.INTx1 = usRxIerState;

	return xResult;
}

/*===========================================================================*/
/* Number‑to‑string conversion functions                                    */
/*===========================================================================*/

/*
** vUint16ToString()
** Converts a 16‑bit unsigned integer to a null‑terminated decimal string.
** Buffer must be at least UINT16_TO_STRING_SIZE (6) bytes.
**/
void vUint16ToString( uint16_t value, char *buffer )
{
	uint16_t i = 0U;
	uint16_t j = 0U;
	char temp[UINT16_TO_STRING_SIZE];

	if( value == 0U )
	{
		buffer[0U] = '0';
		buffer[1U] = '\0';
		return;
	}

	// value = 123
	while( value > 0U )
	{
		temp[i] = '0' + (value % 10U);
		value = value / 10U;
		i = i + 1U;
	}
	// temp = '321'
	// Reserve the ordering to store to actual buffer
	while( i > 0U )
	{
		i = i - 1U;
		buffer[j] = temp[i];
		j = j + 1U;
	}
	buffer[j] = '\0';
	// Buffer = '123\0'
}

/*
** vInt16ToString()
** Converts a 16‑bit signed integer to a decimal string.
** Buffer must be at least INT16_TO_STRING_SIZE (7) bytes.
**/
void vInt16ToString( int16_t value, char *buffer )
{
	// Make use of vUint16ToString to help
	if( value < 0 )
	{
		buffer[0] = '-';
		// Convert to absolute value
		// Cannot write: value = - value;
		// Consider edge case: -(-32768) = 32768 > 32767 (maximum int16_t)
		// => Promote to int32_t then negate the negative
		uint16_t usAbsValue = (uint16_t)(-(int32_t)value);
		// Pass the pointer to buffer[1] as buffer[0] = '-'
		vUint16ToString(usAbsValue, buffer + 1);
	}
	else
	{
		vUint16ToString( (uint16_t)value, buffer);
	}
}
 
/*
** vUint32ToString()
** Converts a 32‑bit unsigned integer to a decimal string.
** Buffer must be at least UINT32_TO_STRING_SIZE (11) bytes.
**/
void vUint32ToString( uint32_t value, char *buffer )
{
	uint16_t i = 0U;
	uint16_t j = 0U;
	char temp[UINT32_TO_STRING_SIZE];

	if( value == 0U )
	{
		buffer[0U] = '0';
		buffer[1U] = '\0';
		return;
	}

	while( value > 0U )
	{
		temp[i] = '0' + (value % 10U);
		value = value / 10U;
		i = i + 1U;
	}

	// Reserve the ordering to store to actual buffer
	while( i > 0U )
	{
		i = i - 1U;
		buffer[j] = temp[i];
		j = j + 1U;
	}
	buffer[j] = '\0';
}

/*
** vInt32ToString()
** Converts a 32‑bit signed integer to a decimal string.
** Buffer must be at least INT32_TO_STRING_SIZE (12) bytes.
**/
void vInt32ToString(int32_t value, char *buffer)
{
    if (value < 0)
    {
        buffer[0] = '-';
        /* Promote to 64‑bit to safely negate -2147483648 */
        uint32_t ulAbsValue = (uint32_t)(-(int64_t)value);
        vUint32ToString(ulAbsValue, buffer + 1);
    }
    else
    {
        vUint32ToString((uint32_t)value, buffer);
    }
}

/*
** vUint64ToString()
** Converts a 64‑bit unsigned integer to a decimal string.
** Buffer must be at least UINT64_TO_STRING_SIZE (21) bytes.
**/
void vUint64ToString( uint64_t value, char *buffer )
{
	uint16_t i = 0U;
	uint16_t j = 0U;
	char temp[UINT64_TO_STRING_SIZE];

	if( value == 0U )
	{
		buffer[0U] = '0';
		buffer[1U] = '\0';
		return;
	}

	while( value > 0U )
	{
		temp[i] = '0' + (value % 10U);
		value = value / 10U;
		i = i + 1U;
	}

	// Reserve the ordering to store to actual buffer
	while( i > 0U )
	{
		i = i - 1U;
		buffer[j] = temp[i];
		j = j + 1U;
	}
	buffer[j] = '\0';
}

/*
** vInt64ToString()
** Converts a 64‑bit signed integer to a decimal string.
** Buffer must be at least INT64_TO_STRING_SIZE (21) bytes.
**/
void vInt64ToString( int64_t value, char *buffer )
{
	if (value < 0)
    {
        buffer[0] = '-';
        /* Cast to unsigned first then negate to safely invert INT64_MIN (-2^63) */
        uint64_t ullAbsValue = -(uint64_t)value;
        vUint64ToString(ullAbsValue, buffer + 1);
    }
    else
    {
        vUint64ToString((uint64_t)value, buffer);
    }
}

/*
** vFractionalToString()
** Converts a double value into string
** MAXIMUM = 999999999.9999
** MINIMUM = -999999999.9999
** Buffer must be at least FRACTIONAL_TO_STRING (16) bytes.
**/
void vFractionalToString( double value, char *buffer)
{
	// Value = (+-)XXXXXXXXX.YYYY
	
	// Check value limit
	if( value > 999999999.9999 )
	{
        const char msg[] = "OVER MAXIMUM";
        uint16_t i = 0U;
        while (msg[i] != '\0')
        {
            buffer[i] = msg[i];
            i++;
        }
        buffer[i] = '\0';
        return;
	}
	if( value < -999999999.9999)
	{
        const char msg[] = "UNDER MINIMUM";
        uint16_t i = 0U;
        while (msg[i] != '\0')
        {
            buffer[i] = msg[i];
            i++;
        }
        buffer[i] = '\0';
        return;
	}

	// Seperate value into:
	// Integer part (+-)XXXXXXXXX
	int32_t lIntPart = (int32_t)value;
	// Fractional part 0.YYYY
	double dFrac = value - (double)lIntPart;
	if( dFrac < 0.0 )
	{
		dFrac = -dFrac;
	}
	// Convert 0.YYYY into YYYY (no rounding)
	uint16_t usFracPart = (uint16_t)( dFrac * 10000.0);
	if( usFracPart > 9999U )
	{
    usFracPart = 9999U;   // clamp to maximum valid fraction
	}

	// Convert Integer part into String
	char cIntString[INT32_TO_STRING_SIZE];
	vInt32ToString(lIntPart, cIntString);

	// Convert Fractional part into String
	char cFracString[UINT16_TO_STRING_SIZE];
	vUint16ToString(usFracPart, cFracString);
	
	// Combine the two parts into output buffer
	uint16_t usBufIdx = 0U;
	uint16_t i = 0U;

	// Add Integer part
	while( cIntString[i] != '\0' )
	{
		buffer[usBufIdx] = cIntString[i];
		usBufIdx = usBufIdx + 1U;
		i = i + 1U;
	}

	// Add the "."
	buffer[usBufIdx] = '.';
	usBufIdx = usBufIdx + 1U;

	// Calculate how many zeros needed in the fractional part
	// Example: 0.000X, 0.00XX, 0.0XXX
	uint16_t usFracLen = 0U;
	
	while( cFracString[usFracLen] != '\0' )
	{
		usFracLen = usFracLen + 1U;
	}

	uint16_t usZeroNeeded = 4U - usFracLen;
	
	// Add the zeros needed
	while( usZeroNeeded > 0U )
	{
		buffer[usBufIdx] = '0';
		usBufIdx = usBufIdx + 1U;
		usZeroNeeded = usZeroNeeded - 1U;
	}

	// Add the fractional part
	i = 0U;
	while( cFracString[i] != '\0' )
	{
		buffer[usBufIdx] = cFracString[i];
		usBufIdx = usBufIdx + 1U;
		i = i + 1U;
	}

	// Add the terminator
	buffer[usBufIdx] = '\0';
}
