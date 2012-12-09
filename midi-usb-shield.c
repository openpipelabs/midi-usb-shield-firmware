/**
 * Copyright (c) 2012 Xulio Coira <xulioc@gmail.com>. All rights reserved.
 *
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "midi-usb-shield.h"

int16_t received;
uint8_t command, channel, data1, data2;

/** LUFA MIDI Class driver interface configuration and state information. This structure is
 *  passed to all MIDI Class driver functions, so that multiple instances of the same class
 *  within a device can be differentiated from one another.
 */
USB_ClassInfo_MIDI_Device_t MIDI_Interface =
	{
		.Config =
			{
				.StreamingInterfaceNumber = 1,
				.DataINEndpoint           =
					{
						.Address          = MIDI_STREAM_IN_EPADDR,
						.Size             = MIDI_STREAM_EPSIZE,
						.Banks            = 1,
					},
				.DataOUTEndpoint          =
					{
						.Address          = MIDI_STREAM_OUT_EPADDR,
						.Size             = MIDI_STREAM_EPSIZE,
						.Banks            = 1,
					},
			},
	};

void USB_MIDI_Send_Command(unsigned char channel, unsigned char command, unsigned char data2, unsigned char data3){

	MIDI_EventPacket_t MIDIEvent = (MIDI_EventPacket_t)
		{
			.Event       = MIDI_EVENT(0, command),

			.Data1       = command | channel,
			.Data2       = data2,
			.Data3       = data3,
		};

	MIDI_Device_SendEventPacket(&MIDI_Interface, &MIDIEvent);
	MIDI_Device_Flush(&MIDI_Interface);

}

/** Main program entry point. This routine contains the overall program flow, including initial
 *  setup of all components and the main program loop.
 */

int main(void)
{
	SetupHardware();

	sei();

	for (;;)
	{
		wdt_reset();

		/* RECEIVE FROM UART AND SEND TO USB */
		if (Serial_IsCharReceived()){

			MIDI_LED_ON();

			received = Serial_ReceiveByte();
			command = (uint8_t)received & 0xF0;
			channel = (uint8_t)received & 0x0F;

			switch(command){

			// 1 BYTE COMMANDS
			case 0xC0: //ProgramChange
			case 0xD0: //AfterTouchChannel
				while( !Serial_IsCharReceived() );
				data1 = (uint8_t)Serial_ReceiveByte();
				USB_MIDI_Send_Command(channel, command, data1, 0);
				break;

			// 2 BYTE COMMANDS
			case 0x80: //NoteOff
			case 0x90: //NoteOn
			case 0xA0: //AfterTouchPoly
			case 0xB0: //ControlChange
			case 0xE0: //PitchBend
				while( !Serial_IsCharReceived() );
				data1 = (uint8_t)Serial_ReceiveByte();
				while( !Serial_IsCharReceived() );
				data2 = (uint8_t)Serial_ReceiveByte();
				USB_MIDI_Send_Command(channel, command, data1, data2);
				break;

			//SYSTEM
			case 0xF0:
				if ( channel==0 ){
					//Sysex
					//TODO
				}else if ( channel >= 6 ){
					//Tune Request
					//End of Exclusive
					//System Real Time
					USB_MIDI_Send_Command(channel, command, 0, 0);
				}else if ( channel==1 || channel==3 ){
					//MIDI Time Code Quarter Frame
					//Song Select
					while( !Serial_IsCharReceived() );
					data1 = (uint8_t)Serial_ReceiveByte();
					USB_MIDI_Send_Command(channel, command, data1, 0);
				}else if ( channel==2 ){
					//Song Position Pointer
					while( !Serial_IsCharReceived() );
					data1 = (uint8_t)Serial_ReceiveByte();
					while( !Serial_IsCharReceived() );
					data2 = (uint8_t)Serial_ReceiveByte();
					USB_MIDI_Send_Command(channel, command, data1, data2);
				}
				break;
			}
			MIDI_LED_OFF();
		}


		MIDI_EventPacket_t ReceivedMIDIEvent;
		while (MIDI_Device_ReceiveEventPacket(&MIDI_Interface, &ReceivedMIDIEvent))
		{

		}


		MIDI_Device_USBTask(&MIDI_Interface);
		USB_USBTask();
	}
}

/** Configures the board hardware and chip peripherals for the demo's functionality. */
void SetupHardware(void)
{
	/* Set high watchdog in order to LUFA to do the job */
	wdt_enable(WDTO_4S);

	/* Disable clock division */
	clock_prescale_set(clock_div_1);

	/* LEDs */
	DDRC |= (1<<6); //PC6 MIDI
	DDRC |= (1<<7); //PC7 USB

	MIDI_LED_OFF();
	USB_LED_OFF();

	/* Hardware Initialization */
	USB_Init();

    // Initialize the serial USART driver before first use, with 9600 baud (and no double-speed mode)
    Serial_Init(31250, false);

}

/** Event handler for the library USB Connection event. */
void EVENT_USB_Device_Connect(void)
{
	USB_LED_ON();
}

/** Event handler for the library USB Disconnection event. */
void EVENT_USB_Device_Disconnect(void)
{
	USB_LED_OFF();
}

/** Event handler for the library USB Configuration Changed event. */
void EVENT_USB_Device_ConfigurationChanged(void)
{
	bool ConfigSuccess = true;
	ConfigSuccess &= MIDI_Device_ConfigureEndpoints(&MIDI_Interface);

}

/** Event handler for the library USB Control Request reception event. */
void EVENT_USB_Device_ControlRequest(void)
{
	MIDI_Device_ProcessControlRequest(&MIDI_Interface);
}

