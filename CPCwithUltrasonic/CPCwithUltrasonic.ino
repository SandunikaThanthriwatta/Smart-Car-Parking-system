#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Bonezegei_HCSR04.h> // Library for Ultrasonic Sensor

LiquidCrystal_I2C lcd(0x27, 20, 4);

#define IR_SENSOR_PIN_1 PINB0      // Slot 1 Sensor 1 pin (using Arduino pin numbers)
#define IR_SENSOR_PIN_2 PINB1     // Slot 1 Sensor 2 pin (using Arduino pin numbers)
#define ULTRASONIC_TRIGGER_PIN 11   // Ultrasonic sensor trigger pin
#define ULTRASONIC_ECHO_PIN 10      // Ultrasonic sensor echo pin

Bonezegei_HCSR04 ultrasonic(ULTRASONIC_TRIGGER_PIN, ULTRASONIC_ECHO_PIN);

#define LED_PIN_1 PIND2 // LED 1 pin 2
#define LED_PIN_2 PIND3 // LED 2 pin 3
#define LED_PIN_3 PIND4 // LED 3 pin 4
#define SERVO_PIN PB5 // Servo control pin connected to Pin 13



void servoWrite(uint16_t angle);

void setup()
{
  lcd.init(); // initialize the lcd
  lcd.backlight();
  lcd.setCursor(3, 0);
  lcd.print("Hello, world!");
  lcd.setCursor(2, 1);
  lcd.print("Ywrobot Arduino!");

  DDRB |= (1 << SERVO_PIN); // Set SERVO_PIN (Pin 9) as output
  DDRB &= ~((1 << IR_SENSOR_PIN_1) | (1 << IR_SENSOR_PIN_2));
  DDRD |= (1 << LED_PIN_1) | (1 << LED_PIN_2) | (1 << LED_PIN_3);

  PORTB |= (1 << IR_SENSOR_PIN_1) | (1 << IR_SENSOR_PIN_2);


  Serial.begin(9600);
  pinMode(ULTRASONIC_TRIGGER_PIN, OUTPUT);
  pinMode(ULTRASONIC_ECHO_PIN, INPUT);
}

void loop()
{
  while (1)
  {
    uint8_t sensor1_state = !(PINB & (1 << IR_SENSOR_PIN_1));
    uint8_t sensor2_state = !(PINB & (1 << IR_SENSOR_PIN_2));

    int distance=ultrasonic.getDistance(); // Get distance from ultrasonic sensor
  Serial.print("Distance:");
  Serial.print(distance);
  Serial.println(" cm");
    lcd.setCursor(3, 0);
    lcd.clear();
    lcd.print("Slot1: ");
    lcd.print(sensor1_state ? "Available  " : "Not available");

    lcd.setCursor(2, 1);
    lcd.print("Slot2: ");
    lcd.print(sensor2_state ? "Available  " : "Not available");

    if (sensor1_state)
    {
      PORTD |= (1 << LED_PIN_1); // Turn on LED 1 if Sensor 1 is active
    }
    else
    {
      PORTD &= ~(1 << LED_PIN_1); // Turn off LED 1 if Sensor 1 is not active
    }

    if (sensor2_state)
    {
      PORTD |= (1 << LED_PIN_2); // Turn on LED 2 if Sensor 2 is active
    }
    else
    {
      PORTD &= ~(1 << LED_PIN_2); // Turn off LED 2 if Sensor 2 is not active
    }

if (distance > 0 && distance < 30) // If distance is positive and below 30 cm
{
  PORTD |= (1 << LED_PIN_3); // Turn on LED 3
  servoWrite(90); // Turn on servo motor
}
else
{
  PORTD &= ~(1 << LED_PIN_3); // Turn off LED 3
  servoWrite(0); // Turn off servo motor
}


    // Add a delay to avoid rapid toggling due to sensor noise
    _delay_ms(500);
  }
}

void servoWrite(uint16_t angle)
{
  // Map angle (0-180) to pulse width (1000-2000 us)
  uint16_t pulse_width = 1000 + ((uint32_t)angle * 1000) / 180;

  // Generate the PWM signal for the servo
  PORTB |= (1 << SERVO_PIN);
  _delay_us(pulse_width);
  PORTB &= ~(1 << SERVO_PIN);
  _delay_us(20000 - pulse_width);

  // Adjust the delay loop if needed for better accuracy
}
