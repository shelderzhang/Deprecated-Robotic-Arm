//ArmMainV4-2.c
//created by: parker, alec, drue
//created on 2-11-15
//updated: 4-20-15 alec (blaze it)
//
//Update content: Added in content for endefector communication.
//Changed struct_xfer so that we can now receive 3 structs instead of 1. Modified all of main so that it works with the
//new struct_xfer and it can distribute the data structs meant for endefector to where it needs to go. Also, fixed delay
//function so it now is a lot more precise.
//
//Update 4-8-15 -- added in constants for ID's and the actuator limits
//Update 4-15-15 -- set up ability for the arm and wrist to move up and down with dual dynamixel movement.
//WARNING: CLOCKWISE/CC MOVEMENT IS NOT FUNCTIONAL! todo: get this working
//RoveSoHard


#include "armMain.h"


void main()
{

	struct arm_control_struct armData;
	struct gripper_control_struct gripperData;
	struct drill_Controls drillData;
	receiveStruct receiveData;
	uint16_t actuatorPos;

	resetStruct(&receiveData, RECEIVE_STRUCT_SIZE);

    initPositions(&actuatorPos);
    delay(DELAY);
	while(1)
	{

		if(recv_struct(UART_MOTHER, &receiveData))
		{
			delay(1);
			switch(receiveData.id)
			{
			case ARM_STRUCT_ID:
				armStructCompare(&armData, &receiveData);
				memcpy(&armData, &receiveData, receiveData.size);

				if(armData.reset)
					resetStruct(&armData, ARM_STRUCT_SIZE);
				
				else if(armData.wristUp){
					wristUp(256);
				}
				else if(armData.wristDown){
					wristDown(256);
				}
				else if(armData.wristClockwise){
					wristClockwise(256);
				}
				else if(armData.wristCounterClockwise){
					wristCounterClockwise(256);
				}
				else if(armData.elbowUp){
					elbowUp(256);
				}
				else if(armData.elbowDown){
					elbowDown(256);
				}
				else if(armData.elbowClockwise){
					elbowClockwise(256);
				}
				else if(armData.elbowCounterClockwise){
					elbowCounterClockwise(256);
				}
				else if(armData.actuatorForward){
					actuatorForward(&actuatorPos);
				}
				else if(armData.actuatorReverse){
					actuatorReverse(&actuatorPos);
				}
				else if(armData.baseClockwise){
					baseClockwise(256);
				}
				else if(armData.baseCounterClockwise){
					baseCounterClockwise(256);
				}
				delay(DELAY);
				break;

			case GRIPPER_STRUCT_ID:
				memcpy(&gripperData, &receiveData, receiveData.size);
				//send_struct(UART_ENDE, &gripperData, INST_OTHER, GRIPPER_STRUCT_ID);
				delay(DELAY);
				break;



			case DRILL_STRUCT_ID:
				memcpy(&drillData, &receiveData, receiveData.size);
				//send_struct(UART_ENDE, &drillData, INST_OTHER, DRILL_STRUCT_ID);
				delay(DELAY);
				break;
			}
		}

		handled = 0;
		flushUart(UART_MOTHER);
		delay(1);
	}
}

void delay(int time)
{
	SysCtlDelay(time * (SysCtlClockGet()/1000));//1 time is 3/100 seconds
}

/************************************************************************************
Hey, look at me
Yes, you
Try using dynoTurn here instead of dynoTurn
It will probably work a lot better
!!!!!!!Do not forget to stop the movement after the while loop conditions are met
Or you're gonna have a bad time!!!!!!!!!!!!!!!!!!!!!!!!!
These probably need to be re-prototyped to take speed input so you can use the same
function for moving and stopping.
************************************************************************************/

void wristClockwise(int16_t speed){
	dynoTurn(UART_DYNAMIXEL, WRIST_DYNOA_ID, TURN_CLOCKWISE, speed);
	delay(DELAY);
	dynoTurn(UART_DYNAMIXEL, WRIST_DYNOB_ID, TURN_CLOCKWISE, speed);
	delay(DELAY);
}

void wristCounterClockwise(int16_t speed){
	dynoTurn(UART_DYNAMIXEL, WRIST_DYNOA_ID, TURN_COUNTERCLOCKWISE, speed);
	delay(DELAY);
	dynoTurn(UART_DYNAMIXEL, WRIST_DYNOB_ID, TURN_COUNTERCLOCKWISE, speed);
	delay(DELAY);
}

void wristUp(int16_t speed){
    dynoTurn(UART_DYNAMIXEL, WRIST_DYNOA_ID, TURN_COUNTERCLOCKWISE, speed);
    delay(DELAY);
    dynoTurn(UART_DYNAMIXEL, WRIST_DYNOB_ID, TURN_CLOCKWISE, speed);
	delay(DELAY);
}

void wristDown(int16_t speed){
	dynoTurn(UART_DYNAMIXEL, WRIST_DYNOA_ID, TURN_CLOCKWISE, speed);
	delay(DELAY);
	dynoTurn(UART_DYNAMIXEL, WRIST_DYNOB_ID, TURN_COUNTERCLOCKWISE, speed);
	delay(DELAY);
}

void elbowCounterClockwise(int16_t speed){
    dynoTurn(UART_DYNAMIXEL, ELBOW_DYNOA_ID, TURN_CLOCKWISE, speed);
	delay(DELAY);
	dynoTurn(UART_DYNAMIXEL, ELBOW_DYNOB_ID, TURN_CLOCKWISE, speed);
	delay(DELAY);
}

void elbowClockwise(int16_t speed){
	dynoTurn(UART_DYNAMIXEL, ELBOW_DYNOA_ID, TURN_COUNTERCLOCKWISE, speed);
	delay(DELAY);
	dynoTurn(UART_DYNAMIXEL, ELBOW_DYNOB_ID, TURN_COUNTERCLOCKWISE, speed);
	delay(DELAY);
}

void elbowDown(int16_t speed){
	dynoTurn(UART_DYNAMIXEL, ELBOW_DYNOA_ID, TURN_COUNTERCLOCKWISE, speed);
	delay(DELAY);
	dynoTurn(UART_DYNAMIXEL, ELBOW_DYNOB_ID, TURN_CLOCKWISE, speed);
	delay(DELAY);
}

void elbowUp(int16_t speed){
	dynoTurn(UART_DYNAMIXEL, ELBOW_DYNOA_ID, TURN_CLOCKWISE, speed);
	delay(DELAY);
	dynoTurn(UART_DYNAMIXEL, ELBOW_DYNOB_ID, TURN_COUNTERCLOCKWISE, speed);
	delay(DELAY);
}

void actuatorForward(uint16_t *pos){
	if(*pos >= (ACTUATOR_FORWARD_LIMIT + ACTUATOR_INC)) //The normal operation. If it's so that the next increment WONT push it beyond
	{											//or put it right at the limit, proceed as normal
		*pos -= ACTUATOR_INC;
		setMotor(UART_ACTUATOR, *pos);
		delay(DELAY);
	}

	else if(*pos > ACTUATOR_FORWARD_LIMIT) //if it turns out that our increments aren't a nice division of the limit and so it's below the limit but the next increment will
	{									  //still push it beyond it, just set it equal to the limit
		*pos = ACTUATOR_FORWARD_LIMIT;
		setMotor(UART_ACTUATOR, *pos);
		delay(DELAY);
	}
						//else, we're at the limit, no operation
}

void actuatorReverse(uint16_t *pos){
	if(*pos <= (ACTUATOR_REVERSE_LIMIT - ACTUATOR_INC)) //The normal operation. If it's so that the next increment WONT push it beyond
	{											//or put it right at the limit, proceed as normal
		*pos += ACTUATOR_INC;
		setMotor(UART_ACTUATOR, *pos);
		delay(DELAY);
	}

	else if(*pos < ACTUATOR_REVERSE_LIMIT) //if it turns out that our increments aren't a nice division of the limit and so it's below the limit but the next increment will
	{									  //still push it beyond it, just set it equal to the limit
		*pos = ACTUATOR_REVERSE_LIMIT;
		setMotor(UART_ACTUATOR, *pos);
		delay(DELAY);
	}
						//else, we're at the limit, no operation
}

void baseClockwise(int16_t speed){
    dynoTurn(UART_DYNAMIXEL, BASE_ID, 0, speed);
	delay(DELAY);
}

void baseCounterClockwise(int16_t speed){
    dynoTurn(UART_DYNAMIXEL, BASE_ID, 1, speed);
	delay(DELAY);
}

//compares the values with arm -- which contains data still from last transmission -- and receive
// -- which is the new commands. If it is found that the previous struct arm has a variable set that
//received does not have set, it means that function should no longer move in the new command and
//calls a function for that movement to stop.
void armStructCompare(struct arm_control_struct *arm, receiveStruct *received)
{
	//if((received -> genParam1 == 0) && (arm -> reset == 1))
		//do fudge all
	if((received -> genParam2 == 0) && (arm -> wristUp == 1))
		wristUp(0);
	else if((received -> genParam3 == 0) && (arm -> wristDown == 1))
		wristDown(0);
	else if((received -> genParam4 == 0) && (arm -> wristClockwise == 1))
		wristClockwise(0);
	else if((received -> genParam5 == 0) && (arm -> wristCounterClockwise == 1))
		wristCounterClockwise(0);
	else if((received -> genParam6 == 0) && (arm -> elbowUp == 1))
		elbowUp(0);
	else if((received -> genParam7 == 0) && (arm -> elbowDown == 1))
		elbowDown(0);
	else if((received -> genParam8 == 0) && (arm -> elbowClockwise == 1))
		elbowClockwise(0);
	else if((received -> genParam9 == 0) && (arm -> elbowCounterClockwise == 1))
		wristCounterClockwise(0);
	else if((received -> genParam12 == 0) && (arm -> baseClockwise == 1))
		baseClockwise(0);
	else if((received -> genParam13 == 0) && (arm -> baseCounterClockwise == 1))
		wristCounterClockwise(0);
}


//Initializes the hardware. Sets the clock to 16 mhz, the 4 UART modules up -- motherboard and actuator with 115200 buad rates,
//dynamixel line with 57600, endetrucker with 9600 -- with 1 stop bit and no parity, and initialize all of the pins that we use
//and setup most to transmit to output, ones used for UART aside.
void initHardware()
{

	//
	// Set the clocking to run directly from the main oscillator at 16 mhz
	// The following code:
	// -sets the system clock divider to 1
	// -sets the system clock to use an oscillator
	// -uses the main oscillator
	// -configures for use of 16 MHz crystal/oscillator input
	//
	SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |SYSCTL_XTAL_16MHZ);

	//uart1: dynamixel

	//
	// Enable the peripherals used by this example.
	// The UART itself needs to be enabled, as well as the GPIO port
	// containing the pins that will be used.
	//
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);

	//
	// Configure the GPIO pin muxing for the UART function.
	// This is only necessary if your part supports GPIO pin function muxing.
	// Study the data sheet to see which functions are allocated per pin.
	//
	GPIOPinConfigure(GPIO_PC4_U1RX);
	GPIOPinConfigure(GPIO_PC5_U1TX);

	//
	// Since GPIO C4 and C5 are used for the UART function, they must be
	// configured for use as a peripheral function (instead of GPIO).
	GPIOPinTypeUART(GPIO_PORTC_BASE, GPIO_PIN_4 | GPIO_PIN_5);

	//
	// Configure the UART for 57600, 8-N-1 operation.
	// This function uses SysCtlClockGet() to get the system clock
	// frequency.  This could be also be a variable or hard coded value
	// instead of a function call.
	//

	UARTConfigSetExpClk(UART1_BASE, SysCtlClockGet(), 57600, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

	//uart 2: motherboard
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART2);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
	GPIOPinConfigure(GPIO_PD6_U2RX);
	GPIOPinConfigure(GPIO_PD7_U2TX);
	GPIOPinTypeUART(GPIO_PORTD_BASE, GPIO_PIN_6 | GPIO_PIN_7);
	UARTConfigSetExpClk(UART2_BASE, SysCtlClockGet(), 115200, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

	//uart 3: endefector
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART3);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
	GPIOPinConfigure(GPIO_PC6_U3RX);
	GPIOPinConfigure(GPIO_PC7_U3TX);
	GPIOPinTypeUART(GPIO_PORTC_BASE, GPIO_PIN_6 | GPIO_PIN_7);
	UARTConfigSetExpClk(UART3_BASE, SysCtlClockGet(), 9600, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));


	//Uart 7: motor controller
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART7);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
	GPIOPinConfigure(GPIO_PE0_U7RX);
	GPIOPinConfigure(GPIO_PE1_U7TX);
	GPIOPinTypeUART(GPIO_PORTE_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	UARTConfigSetExpClk(UART7_BASE, SysCtlClockGet(), 115200, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_3); //tri-state-buffer enable
	GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_3, GPIO_PIN_3);

	//interrupt setups
	IntSetup();

}

void resetStruct(void * myStruct, int size){
	int i;
	uint8_t * address = (uint8_t *)myStruct;
	for(i = 0; i < size; i++)
	{
		*(address + i) = 0;
	}//if this seems confusing: creates a pointer to the base address of struct, then increments the pointer up to
}//the size of the struct, IE how many variables are in it IE how many memory blocks the struct encompasses and therefore
//the blocks within its scope represent its variables...while setting them all to 0.

void setMotor(uint32_t uart, uint16_t pos)
{
	uint8_t highByte = ((pos >> 5) & 0b01111111);
	uint8_t lowByte = ((uint8_t)pos & 0b00011111);
	lowByte |= 0b11000000;
	UARTCharPut(uart, lowByte);
	UARTCharPut(uart, highByte);
}

void initPositions(uint16_t * actuatorPos)
{
	switchCom(TX_MODE);
	/**wristVertPos = WRISTA_START_POS;
	*wristHoriPos = WRISTB_START_POS;
	*elbowVertPos = ELBOWA_START_POS;
	*elbowHoriPos = ELBOWB_START_POS;
	*basePos = BASE_START_POS;*/
	actuatorPos = ACTUATOR_START_POS;
	setMotor(UART_ACTUATOR, *actuatorPos);
	delay(DELAY);
	/*dynoTurn(UART_DYNAMIXEL, WRIST_VERT_ID, *wristVertPos);
	delay(DELAY);
	dynoTurn(UART_DYNAMIXEL, WRIST_HORI_ID, *wristHoriPos);
	delay(DELAY);
	dynoTurn(UART_DYNAMIXEL, ELBOW_HORI_ID, *elbowHoriPos);
	delay(DELAY);
	dynoTurn(UART_DYNAMIXEL, ELBOW_VERT_ID, *elbowVertPos);
	delay(DELAY);
	dynoTurn(UART_DYNAMIXEL, BASE_ID, *basePos);
	delay(DELAY);*/
	dynoWheelModeSet(UART_DYNAMIXEL, GLOBAL_ID); //sets them all to multi turn mode for now
}



void UARTMotherIntHandler()
{
	UARTIntClear(UART_MOTHER, UART_INT_RX);
	if(!(handled))
	{
		uartRxBuf[0] = UARTCharGetNonBlocking(UART_MOTHER);
		if(uartRxBuf[0] == STRUCT_START1)
		{
			uint8_t i;
			for(i = 1; i < 3; i++)
			{
				if(!(HWREG(UART_MOTHER + UART_O_FR) & UART_FR_RXFE))//uartCharGetNonBlocking
				{
					uartRxBuf[i] = HWREG(UART_MOTHER + UART_O_DR);
				}
			}

			handled = 1;
		}
	}
}

void IntSetup()
{
	UARTIntEnable(UART_MOTHER, UART_INT_RX); //must enable before registering the function dynamically
	UARTIntRegister(UART_MOTHER, UARTMotherIntHandler);
	UARTFIFOLevelSet(UART_MOTHER, UART_FIFO_TX1_8, UART_FIFO_RX2_8); //dont worry about the tx, it's unused
	IntMasterEnable();
	handled = 0;
}

void flushUart(uint32_t uart)
{
	while(UARTCharsAvail(uart))
		UARTCharGetNonBlocking(uart);
}
