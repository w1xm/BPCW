#ifndef USART_H_
#define USART_H_

#define USART_BAUDRATE 115200

//Function declarations
void usart_init(void);
void write_char(char);
void write_str(const char *);
void debug_print(const char *, ...) __attribute__ ((format (gnu_printf, 1, 2)));
void print_arr(uint8_t *, uint8_t);

#endif /* USART_H_ */