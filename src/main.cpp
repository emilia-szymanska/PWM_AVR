//commented includes as these libraries are included in PWM.cpp and uart.cpp
//#include <avr/io.h>
//#include <avr/interrupt.h>
#include <util/delay.h>
#include "PWM.cpp"
#include "uart.cpp"

#define tbi(port, pin) (port) ^= _BV(pin)
#define sbi(port, pin) (port) |= _BV(pin)
#define cbi(port, pin) (port) &= ~_BV(pin)

#define MAX_servo 12.75    //max deviation in % (normally 2.55ms/20ms)
#define MIN_servo 2.75     //min deviation in % (normally 0.55ms/20ms)
#define MAX_degrees 180

void set_servo_position(int degrees)
{
  double DS = ((degrees*1.0)/MAX_degrees)*(MAX_servo - MIN_servo);    //converting degrees into %
  DS = DS + MIN_servo;
  set_duty_cycle(DS); 
}

int main()
{
  init_timer1(50);
  uart_config();
  unsigned char angle = 0, read;
  DDRD |= (1<<PD5);    //PWM PD5 as Output
  PORTD = 0x00;
  while(1)
  {
    read = USART_Receive();
    
    if(read != '\n') angle = angle * 10 + (read - '0');   //converting chars (e.g. '2' '3' '4') into numbers (e.g. 234)
    else 
    {
      set_servo_position(angle);
      angle = 0;
    }
  }

  return 0;
}

ISR (TIMER1_COMPA_vect)       //interruption, comparing to A
{
  sbi(PORTD, PD5);
}

ISR (TIMER1_COMPB_vect)       //interruption, comparing to B
{
  cbi(PORTD, PD5);
}
