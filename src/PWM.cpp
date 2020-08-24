#include <avr/io.h>
#include <avr/interrupt.h>

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
