#include "uart.h"

// Private function prototypes

void echo_received_data(char received_data);

// Main function body
int main() {
	
	uart_initialize();
	
	char *message = "Hello World.\n";
	int i;
	for(i = 0; i < 13; i++) {
		uart_transmit(message + i);
	}
	
	uart_set_callback_data_received(&echo_received_data);
	while(1);
}

// Private function bodies

void echo_received_data(char received_data) {
	uart_transmit(&received_data);
}
