#ifndef EVENTS_H_
#define EVENTS_H_

#define MODULO_TOPE_TIMER 30000 // it defines the ceiling of the timer, then the count is reset

extern volatile unsigned char flag_inputCapture; //variable for indicating the main program that the Input Capture interrupt rutine was attended


extern volatile unsigned int overflow_Count; // indicates the number of timer overflows when the bottle goes across the sensor


void Timer1_init(void);


#endif /* EVENTS_H_ */
