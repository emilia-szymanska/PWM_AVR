#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define tbi(port, pin) (port) ^= _BV(pin)
#define sbi(port, pin) (port) |= _BV(pin)
#define cbi(port, pin) (port) &= ~_BV(pin)

#define MAX_servo 12.75    //max deviation in % (normally 2.55ms/20ms)
#define MIN_servo 2.75     //min deviation in % (normally 0.55ms/20ms)
#define MAX_degrees 180

void uart_config()
{
  UBRR0H = 0x00;                          //USART baud rate register high
  UBRR0L = 0x10;                          //USART baud rate register low
  UCSR0C |= (1<<UCSZ01) | (1<< UCSZ00);   //8-bit character size
  UCSR0B |= (1<< RXEN0) | (1<< TXEN0);    //Receiver & Transmitter enable
}

void USART_Transmit(unsigned char data)
{
  while (!(UCSR0A & (1<<UDRE0)));     //Wait for empty transmit buffer
  UDR0 = data;                        //Put data into buffer, sends the data
}

unsigned char USART_Receive()
{
  while (!(UCSR0A & (1<<RXC0)));    //Wait for data to be received
  return UDR0;                      //Get and return received data from buffer
}

void init_timer1(int frequency)
{
  TIMSK1 |= (1<<OCIE1B) | (1<<OCIE1A);  //Timer/Counter1, Output Compare A & B Match Interrupt Enable
  TCCR1B |= (1<<WGM12);                 //Clear Timer on Compare Match (CTC) Mode
  sei();

  if(frequency < 4)                     //to small prescaler
  {
    TCCR1B |= (1<<CS12);                //prescaler = 256
    OCR1A = 16000000/(256 * frequency) - 1;
  }
  else 
  {
    if(frequency > 100000)              //too big prescaler
    {
      TCCR1B |= (1<<CS11);              //prescaler = 8
      OCR1A = 16000000/(8 * frequency) - 1;
    }
    else
    {
      TCCR1B |= (1<<CS11) | (1<<CS10);  //prescaler = 64
      OCR1A = 16000000/(64 * frequency) - 1;
    }
  }
}

void set_duty_cycle(double duty_cycle)        //in % 
{
  OCR1B = ((OCR1A + 1) * duty_cycle)/100 - 1; //setting the duty cycle based on % value
}

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
