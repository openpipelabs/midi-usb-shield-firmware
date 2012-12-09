#ifndef _MIDI_USB_SHIELD_H_
#define _MIDI_USB_SHIELD_H_

	/* Includes: */
		#include <avr/io.h>
		#include <avr/wdt.h>
		#include <avr/power.h>
		#include <avr/interrupt.h>
		#include <stdbool.h>
		#include <string.h>

		#include "Descriptors.h"

		#include <LUFA/Drivers/USB/USB.h>
		#include <LUFA/Drivers/Peripheral/Serial.h>

	/* Function Prototypes: */
		void SetupHardware(void);

		void EVENT_USB_Device_Connect(void);
		void EVENT_USB_Device_Disconnect(void);
		void EVENT_USB_Device_ConfigurationChanged(void);
		void EVENT_USB_Device_ControlRequest(void);

		/* LEDs */
		#define MIDI_LED_ON()       (PORTC |= (1<<6))
		#define MIDI_LED_OFF()      (PORTC &= ~(1<<6))
		#define USB_LED_ON()        (PORTC |= (1<<7))
		#define USB_LED_OFF()       (PORTC &= ~(1<<7))


#endif

