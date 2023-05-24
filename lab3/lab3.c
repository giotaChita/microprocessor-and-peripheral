#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "uart.h" 
#include "leds.h"
#include "queue.h"
#include "timer.h"
#include <stdbool.h>
#include "platform.h"
#include "gpio.h"
#include "delay.h"
//#include "stm32f4xx_usart.h" // for trial of usart_cmd and disable_irq? Answer: nope

#define P_DHT PA_1
#define UP_TEMP_THRESHOLD 27.8
#define DOWN_TEMP_THRESHOLD 26.3

#define P_DHT PA_1

static bool first_time_aem = true; //flag to stop programm until the user gives his aem fisrt
static int count_butt = 0; 			//holds how many times button was pressed
static Queue rx_queue;					//Queue
static int aem;									//aem
static char uart_input[6];			//queue for aem
static uint8_t last_index = 0; 	//for 4 or 5 digits in aem
static int mode = 1; 						// 3modes 1 for aem , 2 for 1st time button and 3 for > 1 press
int period = 1;									//for everything
int sum_last_digits = 0;				//holds the sum of the last 2 digits of aem
static int counter = 1;					//GENERAL COUNTER
uint8_t response_from_dht = 0; 	

//for DHT11
uint8_t Rh_1, Rh_2, Temp_1, Temp_2;
uint8_t sum_byte5 = 0;



void DHT11_init(void)  //init the pin which data wire is connected to.
{
	gpio_set_mode(P_DHT, Input);
}

void DHT11_start(void){
		gpio_set_mode(P_DHT, Output);
		gpio_set(P_DHT, 0);
		delay_us(20000); // try biggern number than 18ms 
		gpio_set(P_DHT, 1);
		delay_us(40); // 20 - 40us 
		gpio_set_mode(P_DHT, Input);
}

uint8_t DHT11_check_response(void){
	uint8_t response = 0;
	delay_us(40);
	//printf("the gpio is %d\n",gpio_get(P_DHT));
	if(!(gpio_get(P_DHT))){
		//printf("I AM FUCKING IN\n");
		delay_us(80);
		if(gpio_get(P_DHT)){
			//printf("I AM FUCKING IN again\n");
			response = 1;
		}
		else{
			response = -1;
		}
		//delay_us(80); 	//
	}
	while(gpio_get(P_DHT));
	return response;
}

uint8_t DHT11_read(void){
	//uint8_t connection_error = 1;
	uint8_t i,j;
	for (j=0; j < 8; j++){
		//delay_us(55); // datasheet 50sec 
		  while(!gpio_get(P_DHT));  // wait for the pin to go high
			delay_us(40);   // wait for 35 us
			if (!gpio_get(P_DHT))   // if the pin is low
			{
				i&= ~(1<<(7-j));   // write 0 (26-28us)
			}
			else i|= (1<<(7-j));  // if the pin is high, write 1 (70us)
			while (gpio_get(P_DHT));  // wait for the pin to go low
		} 
// if delay > 50us{ then error connection
//		else {
//			connection_error = -1;
//			return connection_error;
		//}
	//}
	return i;
}

float DHT_GetTemperature(void){
	//printf("START\n");
	DHT11_start(); //send request to start measure
	//printf("END OF START\n");
	response_from_dht = DHT11_check_response();
	//printf("check response done, einai toso %d \n", response_from_dht);
	//if(response_from_dht < 0){
	//	printf("error in response");
	//}
	Rh_1 = DHT11_read();
	//printf("phra prvto byte einai toso %d\n", Rh_1);
	Rh_2 = DHT11_read();
	//printf("phra deytero byte einai toso %d\n", Rh_2);
	Temp_1 = DHT11_read();
	//printf("phra trito byte einai toso %d\n", Temp_1);
	Temp_2 = DHT11_read();
	//printf("phra tetarto byte einai toso %d\n", Temp_2);
	sum_byte5 = DHT11_read();
	//printf("phra pempto byte einai toso %d\n", sum_byte5);

	
	if((Rh_1 + Rh_2 + Temp_1 + Temp_2) == sum_byte5){
		return (float)(Temp_1 + (float)Temp_2/10.0);	
	}
	else{
		return -1000.0; // for error 
	}
}

void turn_on_led_isr(){				//opens the led and printf
    leds_set(1, 0, 0);					
		uart_print("led is ON\n");
		printf("led is ON\n");
}

void turn_off_led_isr(){			//close the led and print
	leds_set(0,0,0);
	uart_print("Led is OFF\n");
	printf("led is OFF\n");
}

void toggle_led_isr(){				//toggle led every seconds
	do{
		printf("its toggle now one part, counter is %d \n", counter);
		int temporary_counter = counter;
		if (counter % 2){
			gpio_toggle(P_LED_R);		//in case of closed led 
			printf("led is %d\n", gpio_get(P_LED_R));
		}
		else{
			gpio_toggle(P_LED_R);
			printf("led is %d\n", gpio_get(P_LED_R));
		}
		if(counter == period - 1)
			break;
		while(counter == temporary_counter );
	}while(counter < period);
}

void button_isr(){					//when button is pressed, led is toggled.
	count_butt++;
	printf("button is pressed, total %d th times\n", count_butt);

	if (count_butt <= 6){ //first time that button is pressed
												//must sum 2 last digits of aem and make same period.
		mode = 2;						//1st time button
		sum_last_digits = (uart_input[last_index]-'0') + (uart_input[last_index - 1]-'0');
		if(sum_last_digits == 2){
			sum_last_digits = 4;
		}
		printf("sum of last 2 digits is %d\n", sum_last_digits);
		period = sum_last_digits;
	}
	else{									//if not first time
		mode = 3;						// >1 time button
		if(count_butt % 2){	//time is odd
			period  = 4;
		}
		else{								//time is even
			period = 3;
		}
	}
}

void uart_rx_isr (uint8_t rx ){
	if(first_time_aem){
		// Store the received character
		queue_enqueue(&rx_queue , rx);
		printf("enqueued this %d \n", rx); //debug
		printf("enqueued this %c \n", rx); //real value that was enqueued
		
		if(rx == 13){ //if ENTER was pressed
			uint8_t enter = 0;
			
			queue_dequeue(&rx_queue, &uart_input[0]);
			queue_dequeue(&rx_queue, &uart_input[1]);
			queue_dequeue(&rx_queue, &uart_input[2]);
			queue_dequeue(&rx_queue, &uart_input[3]);
			queue_dequeue(&rx_queue, &uart_input[4]);
			
			if (!queue_dequeue(&rx_queue, &uart_input[5])){ //if there is not 5th digit, so aem has 4 digits
				last_index = 3;
			}
			else{
				last_index = 4;
			}
			printf("last index is %d\n", last_index);
			printf("and aem is %d%d%d%d\n", uart_input[0]-'0', uart_input[1]-'0', uart_input[2]-'0', uart_input[3]-'0');
			uart_print("i received your AEM part 1\n");
			first_time_aem = false;
		}
	}
}

void timer_callback_isr(){ //every second second
	counter++;
}

control_led_temp(float temp){
	if (temp == -1000.0) {								//in case of error
		uart_print("ERROR reading the temperature\n");
	} 
	else if (temp > UP_TEMP_THRESHOLD){		//if temperature is over high threshold
		turn_on_led_isr();
	} 
	else if (temp < DOWN_TEMP_THRESHOLD){ //if temperature is under high threshold
		turn_off_led_isr();
	}
	else { 																//if temperature is between 20-25 *C
		toggle_led_isr();
	}
}

int main(void){
	//led_init
	leds_init();
	//uart and queue init
	queue_init(&rx_queue , 128);
	uart_init(115200);
	uart_set_rx_callback(uart_rx_isr);
	uart_enable();
	__enable_irq();

	//code here
	uart_print("please give me your AEM\n");
	//uart_rx();
	while(first_time_aem);
	uart_print("i received your AEM part final\n");
	//USART_Cmd(USART2, DISABLE); 					// check? Answer: nope.......
	NVIC_DisableIRQ(USART2_IRQn);						// disable uart_irqs 				
	printf("passed usart_CMD??\n");
	__disable_irq();
//button init
	gpio_set_mode(P_SW, PullUp);
	gpio_set_trigger(P_SW, Rising);
	gpio_set_callback(P_SW, button_isr);
	
	//timer init
	timer_set_callback(timer_callback_isr); //set timer callback function
	timer_init(1000000); //one sec timer
	timer_enable();
	
	//dht11 init
	DHT11_init();
	
	__enable_irq();

	float temp; //holds the temperature 
	period = 2;
	
	do{
		if(counter >= period){
			uart_print("perasan 2sec\n");
			counter = 0;
			
			//pare timh thermokrasias
			temp = DHT_GetTemperature();
			printf("temp is %.2f\n", temp);
			control_led_temp(temp);
			__WFI(); //SOS  
		}
	}while(mode == 1);					//while(!gpio_get(P_SW));
	printf("efyges apo to do_while\n");
	do{
		if (counter >= period){
			printf("twra pernoyn %d sec\n", period);
			uart_print("perasan kapoia sec\n");

			counter = 0;
			temp = DHT_GetTemperature();			// take temperature
			printf("temp is %.2f\n", temp);
			control_led_temp(temp);
			__WFI(); //SOS 
		}
	}while(mode == 2);
	printf("efyges apo to do_while\n");		
	do{
		if (counter >= period){							// take temperature
			printf("twra pernoyn %d sec\n", period);
			uart_print("perasan kapoia alla sec\n");  
			
			counter = 0;
			temp = DHT_GetTemperature();			// take temperature
			printf("temp is %.2f\n", temp);
			control_led_temp(temp); 
			__WFI(); //SOS 
		}
	}while(mode == 3);
	return 1;
}
