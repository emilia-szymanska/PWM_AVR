#include <avr/io.h>

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
