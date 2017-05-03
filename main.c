#define F_CPU 1000000
#include <avr/io.h>
#include<avr/wdt.h>
#include<avr/interrupt.h>
#include<util/delay.h>

volatile long dist_a = 0;
volatile long dist_b = 0;
volatile unsigned char up = 0;
volatile uint32_t running = 0;
volatile uint32_t timercounter =0;
int turn = 0;
int k=0;

ISR(TIMER0_OVF_vect)
{
    if (up) {
        timercounter++;
    }
}

void steruj()
{
		if(PIND & (1<<PD4)){
			if(dist_a>35){
					OCR1A = 255;
					OCR1B = 200;
			}
			else if(dist_a>15&&dist_a<=35){
					OCR1A = 90;
					OCR1B = 90;
			}
			else{
					OCR1A = 0;
					OCR1B = 0;
			}
		}
		if(PIND & (1<<PD5)){
			if(dist_b>35){
					OCR1A = 255;
					OCR1B = 200;
			}
			else if(dist_b>15&&dist_b<=35){
					OCR1A = 90;
					OCR1B = 90;
			}
			else{
					OCR1A = 0;
					OCR1B = 0;
			}
		}
}

SIGNAL(INT0_vect){
    if(running){ 
        if (up == 0) { 
            up = 1;
            timercounter = 0;            
            TCCR0 |= (0 << CS02)|(0 << CS01)|(1 << CS00); 
            TCNT0 = 0; 
        } else {
            up = 0;
            dist_a = (timercounter*256+TCNT0)/58;
            steruj();
            running = 0;
        }
    }
}

SIGNAL(INT1_vect){
    if(running){
        if (up == 0) {
            up = 1;
            timercounter = 0;            
            TCCR0 |= (0 << CS02)|(0 << CS01)|(1 << CS00);
            TCNT0 = 0;
        } else {
            up = 0;
            dist_b = (timercounter*256+TCNT0)/58;
            steruj();
            running = 0;
        }
    }
}

void send_trigger()
{
    PORTD = 0x00;
    _delay_us(5);
    PORTD = 0x01;
    running = 1;
    _delay_us(10);
    PORTD = 0x00;
}
void send_trigger2()
{
    PORTD = 0x00;
    _delay_us(5);
    PORTD = 0x02;
    running = 1;
    _delay_us(10);
    PORTD = 0x00;
}
int main()
{
    DDRD = 0x03;
	
	
	PORTD = 0x00;
	
	DDRD &= ~(1 << PD4);
	DDRD &= ~(1 << PD5);
	DDRD &= ~(1 << PD6);
	DDRD &= ~(1 << PD7);
	
	DDRC |= (1 << PC0);
	DDRC |= (1 << PC1);
	DDRC |= (1 << PC2);
	DDRC |= (1 << PC3);
	
	DDRB |= (1 << PB1);
	DDRB |= (1 << PB2);
 
	TCCR1A |= (1<<WGM10);                       
    TCCR1B |= (1<<WGM12); 
    TCCR1A |= (1<<COM1A1)|(1<<COM1B1) ;         
    TCCR1B |= (1<<CS10)|(1<<CS11);            
    OCR1A = 0;       
    OCR1B = 0;         
	

    MCUCR |= (0 << ISC01) | (1 << ISC00);
    GICR |= (1 << INT0); 
	
	MCUCR |= (0 << ISC11) | (1 << ISC10); 
    GICR |= (1 << INT1); 

    TIMSK |= (1 << TOIE0); 
    sei(); 
    while(1)
    {
		//kontaktron
		if(PINC & (1<<PC5)){
			k=1;
		}
		else{
			k=0;
		}
		//pilot
		if(k==1){
        if(running == 0) {
            _delay_ms(60);
            send_trigger();
			_delay_ms(60);
            send_trigger2();
        }
		if(PIND & (1<<PD4)){
		PORTC &= ~(1 << PC0);
		PORTC &= ~(1 << PC2);
		PORTC |= (1 << PC1);
		PORTC |= (1 << PC3);
		
		if(running == 0) {
        _delay_ms(60);
        send_trigger();
        }
		}
		else if(PIND & (1<<PD5)){
		PORTC &= ~(1 << PC1);
		PORTC &= ~(1 << PC3);
		PORTC |= (1 << PC0);
		PORTC |= (1 << PC2);
		
		if(running == 0) {
		_delay_ms(60);
        send_trigger2();
        }
		}
		else if(PIND & (1<<PD6)){
		PORTC &= ~(1 << PC0);
		PORTC &= ~(1 << PC3);
		PORTC |= (1 << PC2);
		PORTC |= (1 << PC1);
		
		OCR1A = 255;
		OCR1B = 255;
		}
		else if(PIND & (1<<PD7)){
		PORTC &= ~(1 << PC1);
		PORTC &= ~(1 << PC2);
		PORTC |= (1 << PC3);
		PORTC |= (1 << PC0);
		
		OCR1A = 255;
		OCR1B = 255;
		}
		else{
		PORTC &= ~(1 << PC0);
		PORTC &= ~(1 << PC1);
		PORTC &= ~(1 << PC2);
		PORTC &= ~(1 << PC3);
		}
		}
	else{
		PORTC &= ~(1 << PC0);
		PORTC &= ~(1 << PC1);
		PORTC &= ~(1 << PC2);
		PORTC &= ~(1 << PC3);
	}
    }
}