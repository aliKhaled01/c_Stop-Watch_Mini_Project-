/*
 *stop_watch.c
 *Created on: May 5, 2022
 *Author: lenovo
 */
#include <avr/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
/******************************************************Prototypes*******************************************************************/
void Int0_Init(void);
/*
 * Input:void
 * output:void
 * Description:Function To Initialize The external interrupt 0 to work with falling edge and activates the internal pull up and \
 * the module interrupt
 */
void Int1_Init(void);
/*
 * Input:void
 * output:void
 * Description:Function To Initialize The external interrupt 1 to work with rising edge and activates the module interrupt enable
 */
void Int2_Init(void);
/*
 * Input:void
 * output:void
 * Description:Function To Initialize The external interrupt 2 to work with falling edge and activates the internal pull up and \
 * the module interrupt enable
 */
void Timer1_CTC_Init(void);
/*
 * Input:void
 * Output:void
 * Description: Function to Initialize The Timer To Work In The CTC Mode With OCR1A Top Value
 * 				Set The Prescaler To 1024
 * 				Set The Compare Register OCR1A To 1000
 * 				Clear TCNT1
 * 				Enables The Compare Match Interrupt
 * 				As Fcpu = 1MHZ Then Tick Time = 1msec
 * 				So We Get Compare Match Interrupt Request (If I Bit Is Enabled) After Every 1000ms(1s)
 *
 */
void Seg1_Display(unsigned char value ,unsigned char seg_no);
/*
 * Input:the value to be displayed (minutes or seconds or hours)
 * Input:the segment number to display the right most (first) digit of the value on it
 * Output:void
 * Description:Function to enable the sevensegment whose number is given in seg_no variable and disable other sevensegments and\
 * takes the first digit of the (seconds,hours,minutes) and display this digit on the corresponding sevensegment whose number is
 * given in the seg_no variable
 *
 */
void Seg2_Display(unsigned char value ,unsigned char seg_no);
/*
 * Input:the value to be displayed (minutes or seconds or hours)
 * Input:the segment number to display the left most (second) digit of the value on it
 * Output:void
 * Description:Function to enable the sevensegment whose number is given in seg_no variable and disable other sevensegments and\
 * takes the second digit of the (seconds,hours,minutes) and display this digit on the corresponding sevensegment whose number is\
 * given in the seg_no variable
 *
 */
/***********************************************Global Variables*********************************************************/
unsigned char seconds=0;
unsigned char minutes=0;
unsigned char hours=0;
/*****************************************************Functions*******************************************************/
void Int0_Init(void)
{
	DDRD&=~(1<<PD2);
	/*
	 * pin 2 at port d (int0) is input
	 */
	PORTD|=(1<<PD2);
	/*
	 * activate internal pull up
	 */
	MCUCR|=(1<<ISC01);
	MCUCR&=~(1<<ISC00);
	/*
	 * work with falling edge
	 */
	GICR|=(1<<INT0);
	/*
	 * activate the module interrupt enable
	 */
}
void Int1_Init(void)
{
	DDRD&=~(1<<PD3);
	/*
	 * pin 3 at port d (int1) is input
	 */
	MCUCR|=(1<<ISC11)|(1<<ISC10);
	/*
	 * work with rising edge
	 */
	GICR|=(1<<INT1);
	/*
	 * activate the module interrupt enable
	 */
}
void INT2_Init(void)
{
	DDRB&=~(1<<PB2);
	/*
	 * pin 2 at port b (int2) is input
	 */
	PORTB|=(1<<PB2);
	/*
	 * activate internal pull up
	 */
	MCUCSR&=~(1<<ISC2);
	/*
	 * work with falling edge
	 */
	GICR|=(1<<INT2);
	/*
	 * activate the module interrupt enable
	 */
}
void Timer1_CTC_Init(void)
{
	TCNT1=0;
	/*
	 * clear the timer
	 */
	OCR1A=1000;
	/*
	 * compare register set to 1000
	 *
	 */
	TIMSK|=(1<<OCIE1A);
	/*
	 * enable the interrupt compare match module
	 *
	 */
	TCCR1A=(1<<FOC1A);
	TCCR1B=(1<<WGM12)|(1<<CS10)|(1<<CS12);
	/*
	 * 1024 prescaler
	 * work with ctc_mode (ocr1a on top)
	 * As Fcpu=1MHZ
	 * So tick time = 1msec
	 * So compare match after 1000ms(1s)
	 *
	 */

}

void Seg1_Display(unsigned char value , unsigned char seg_no)
{
	unsigned char digit1;
	/*
	 * define variable to hold the first digit of the number to be displayed
	 */
	seg_no=6-seg_no;
	/*
	 * to make the right most sevensegment which connected to PA5 with number 1
	 */
	PORTA=(1<<seg_no);
	/*
	 * enable this sevensegment and disable the others
	 */
	digit1 = value%10;
	/*
	 * get the first digit of the number
	 */
	PORTC=PORTC&0XF0|digit1&0x0F;
	/*
	 * insert the first 4 bits in the digit variable into the first 4 bits in portc
	 */

}
void Seg2_Display(unsigned char value , unsigned char seg_no)
{
	unsigned char digit2;
	/*
	 * define variable to hold the second digit of the number to be displayed
	 */
	seg_no=6-seg_no;
	/*
	 * to make the right most sevensegment which connected to PA5 with number 1
	 */
	PORTA=(1<<seg_no);
	/*
	 * enable this sevensegment and disable the others
	 */
	value/=10;
	digit2 = value%10;
	/*
	 * get the second digit of the number
	 */
	PORTC=PORTC&0XF0|digit2&0x0F;
	/*
	 * insert the first 4 bits of the digit variable into the first 4 bits in portc
	 */

}
/***********************************************Interrupt Service Routines**********************************************/
ISR(INT0_vect)
{
	seconds=0;
	minutes=0;
	hours=0;
	/*
	 * reset the timer
	 */
}
ISR(INT1_vect)
{
	TCCR1B&=~(1<<CS10)&~(1<<CS12)&~(1<<CS11);
	/*
	 * pause the timer
	 */
}
ISR(INT2_vect)
{
	TCCR1B|=(1<<CS10)|(1<<CS12);
	/*
	 * resume the timer
	 */
}
ISR(TIMER1_COMPA_vect)
{
	if(minutes==59&&seconds==59&&hours==99)
	{
		minutes=0;
		seconds=0;
		hours=0;
	}
	if(seconds<59)
	{
		seconds++;
	}
	else if(seconds==59&&minutes<59)
	{
		seconds =0;
		minutes ++;
	}
	else if(minutes==59&&seconds==59&&hours<99)
	{
		minutes=0;
		seconds=0;
		hours++;

	}

}

/******************************************************Main Application Function***************************************************/
int main()
{
	DDRC|=0X0F;
	/*
	 * first 4 bits in portc are outputs
	 */
	PORTC&=~0X0F;
	/*
	 * clear first 4 bits in portc
	 */
	DDRA|=0X1F;
	/*
	 * first 5 bits in port a are outputs
	 */
	PORTA&=~0X1F;
	/*
	 * clear first 5 bits in port a
	 */
	Int0_Init();
	Int1_Init();
	INT2_Init();
	Timer1_CTC_Init();
	/*
	 * start timer
	 */
	SREG|=(1<<7);
	/*
	 * enable the global interrupt (I BIT)
	 */
	while(1)
	{

		Seg1_Display(seconds,1);
		_delay_us(5);
		Seg2_Display(seconds,2);
		_delay_us(5);
		Seg1_Display(minutes,3);
		_delay_us(5);
		Seg2_Display(minutes,4);
		_delay_us(5);
		Seg1_Display(hours,5);
		_delay_us(5);
		Seg2_Display(hours,6);
		_delay_us(5);
		/*
		 * Display The Time On The 6 SevenSegments
		 */

	}
}
