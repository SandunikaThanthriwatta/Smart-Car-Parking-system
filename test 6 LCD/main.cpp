#define F_CPU 16000000UL
#define BAUD 9600
#define UBRR_VAL ((F_CPU / (BAUD * 16UL)) - 1)

#include <avr/io.h>
#include "i2c.h"
#include "LCD_I2C.h"
#include <util/delay.h>
// Define input pins for IR sensors
#define Slot_1_IR PINB5  // Slot 1 Sensor
#define Slot_2_IR PINB4  // Slot 2 Sensor
#define Entrance_IR PINB1  // Entrance Sensor
#define Exit_IR PINB0  // Exit Sensor

void lcd_msg(const char* message);

// Define output pins for the LEDs
#define Slot_1_LED_PIN PIND7  // Slot 1 is occupied or not
#define Slot_2_LED_PIN PIND6  // Slot 2 is occupied or not
#define Gate_LED_PIN PIND5  // Slot 2 is occupied or not

void USART_Init(unsigned int ubrr) {
	// Set baud rate
	UBRR0H = (unsigned char)(ubrr >> 8);
	UBRR0L = (unsigned char)ubrr;
	
	// Enable receiver and transmitter
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);
	
	// Set frame format: 8 data bits, 1 stop bit
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void USART_Transmit(unsigned char data) {
	// Wait for empty transmit buffer
	while (!(UCSR0A & (1 << UDRE0)));

	// Put data into buffer, sends the data
	UDR0 = data;
}

int main(void)
{
		USART_Init(UBRR_VAL);
		// Set LED pins as outputs
		DDRD |= (1 << Slot_1_LED_PIN) | (1 << Slot_2_LED_PIN) | (1 << Gate_LED_PIN);

		// Set IR sensor pins as inputs
		PORTB &= 0B11011111; // Slot 1 Sensor
		PORTB &= 0B11101111; // Slot 2 Sensor
		PORTB &= 0B11111101; // Entrance Sensor
		PORTB &= 0B11111110; // Exit Sensor
	
	
	i2c_init();
	i2c_start();
	i2c_write(0x70); // Set the correct address of the I2C module
	lcd_init();
	

	char message1[] = "Slot 1 avalible";
	char message2[] = "Slot 2 avalible";
	char message3[] = "Slot 1 Not avalible";
	char message4[] = "Slot 2 Not avalible";



	
	
		while (1)
		{
			// Check the state of Slot_1_IR and turn on Slot_1_LED_PIN if it's high (1)
			if (PINB & (1 << Slot_1_IR))
			{
				PORTD |= (1 << Slot_1_LED_PIN);  // Turn on Slot_1_LED_PIN
				USART_Transmit('1'); // Send '1' to indicate occupied
				lcd_cmd(0x80);
				lcd_msg(message1);
			}
			else
			{
				PORTD &= ~(1 << Slot_1_LED_PIN); // Turn off Slot_1_LED_PIN
				USART_Transmit('0'); // Send '0' to indicate unoccupied
				lcd_cmd(0x80);
				lcd_msg(message3);
			}

			// Check the state of Slot_2_IR and turn on Slot_2_LED_PIN if it's high (1)
			if (PINB & (1 << Slot_2_IR))
			{
				PORTD |= (1 << Slot_2_LED_PIN);  // Turn on Slot_2_LED_PIN
				USART_Transmit('2'); // Send '2' to indicate occupied
				lcd_cmd(0xC3);
				lcd_msg(message2);
			}
			else
			{
				PORTD &= ~(1 << Slot_2_LED_PIN); // Turn off Slot_2_LED_PIN
				USART_Transmit('0'); // Send '0' to indicate unoccupied
				lcd_cmd(0xC3);
				lcd_msg(message4);
			}
			

			
			if ( (PINB & (1 << Entrance_IR)) || (PINB & (1 << Exit_IR)) )
			{
				PORTD |= (1 << Gate_LED_PIN);  // Turn on Gate LEDcPIN
				USART_Transmit('3'); // Send '0' to indicate unoccupied
			}
			else
			{
				PORTD &= ~(1 << Gate_LED_PIN); // Turn Gate LED PIN
				USART_Transmit('0'); // Send '0' to indicate unoccupied
			}

			_delay_ms(200); // Add a small delay to avoid rapid LED toggling due to bouncing
		}

		return 0;
	
}
