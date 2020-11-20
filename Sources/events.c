/*
 * events.c
 *
 *  Created on: Nov 20, 2020
 *      Author: user
 */
#include <hidef.h>
#include "derivative.h"
#include "events.h"

volatile unsigned char flag_inputCapture = 0;

volatile unsigned int overflow_Count = 0;

void Timer1_init(void){
    /* initialisation of timer status and control register 
       and SC_channel register
    */

   // clock initialisation ( bus rate 8MHz )

	TPM1SC_CLKSB=0;
	TPM1SC_CLKSA=0;
	TPM1SC_TOIE = 1; //timer overflow input enable (enables the output interrupt request)
	TPM1SC_CPWMS = 0; // disabled the edge-aligned PWM
	
	TPM1SC_PS = 6; // configure the pre-scale
	
	TPM1MOD = MODULO_TOPE_TIMER; // configure the module register of timer (16bits)
	
	
	TPM2SC_TOIE =1; //timer overflow input enable (enables the output interrupt request)
	TPM2SC_PS = 7; // configure the pre-scale
	TPM2SC_CPWMS = 0;
	TPM2MOD = 0;
   
   
   
   
    /* **** Configuration of timer channels ****  */
   
   /* Configuration of timer channel 0 */
   /* in charge of the bottle sensor */
   
    TPM1C0SC_CH0IE = 1; // interrupt enable

    // select (Input Capture) Timer Mode
    TPM1C0SC_MS0A = 0;
    TPM1C0SC_MS0B = 0;

    // capture on rising and falling edge
    TPM1C0SC_ELS0A = 1;
    TPM1C0SC_ELS0B = 1; // edge level selector
    
    
    /* Configuration of timer channel 1 */
    /* manages the on/off button  (switch) */
    
    TPM1C1SC_CH1IE = 1; // start/stop pin
    
	// select (Input Capture) Timer Mode
	TPM1C1SC_MS1A = 0;
	TPM1C1SC_MS1B = 0;
	// capture on falling edge
	TPM1C1SC_ELS1A = 0;
	TPM1C1SC_ELS1B = 1;
	
	
	/* Configuration of timer channel 2 */
	/* manages the Borra_cuentas button */
	
	TPM1C2SC_CH2IE = 1; // start/pin
	
	// select (Input Capture) Timer Mode
	TPM1C2SC_MS2A = 0;
	TPM1C2SC_MS2B = 0;
	// capture on falling edge only
	TPM1C2SC_ELS2A = 0;
	TPM1C2SC_ELS2B = 1;
	
	/* Configuration of timer channel 3 */
	/* manages the Cuenta_Standard button */
	
	TPM1C3SC_CH3IE = 1; // Cuenta_Standard
	
	// select (Input Capture) Timer Mode
	TPM1C3SC_MS3A = 0;
	TPM1C3SC_MS3B = 0;
	// capture on falling edge only
	TPM1C3SC_ELS3A = 0;
	TPM1C3SC_ELS3B = 1;
		
	/* Configuration of timer channel 4 */
	/* manages the Cuenta_stand_mas button */
	
	TPM1C4SC_CH4IE = 1; // Cuenta_stand_mas
	
	// select (Input Capture) Timer Mode
	TPM1C4SC_MS4A = 0;
	TPM1C4SC_MS4B = 0;
	// capture on rising edge only
	TPM1C4SC_ELS4A = 0;
	TPM1C4SC_ELS4B = 1;
	
	
	/* Configuration of timer channel 5 */
	/* manages the Cuenta_stand_menos button */
	
	TPM1C5SC_CH5IE = 1; // Cuenta_stand_menos
	
	// select (Input Capture) Timer Mode
	TPM1C5SC_MS5A = 0;
	TPM1C5SC_MS5B = 0;
	// capture on falling edge only
	TPM1C5SC_ELS5A = 0;
	TPM1C5SC_ELS5B = 1;
	
	
	/* Configuration of timer channel 6 */
	/* manages the Average_Defectuosas button */
	
	TPM2C0SC_CH0IE = 1; // Average_Defectuosas
	
	// select (Input Capture) Timer Mode
	TPM2C0SC_MS0A = 0;
	TPM2C0SC_MS0B = 0;
	// capture on falling edge only
	TPM2C0SC_ELS0A = 0;
	TPM2C0SC_ELS0B = 1;
	
	

    /*** activate the bus clocks ****/
	
    TPM1SC_CLKSA = 1;
    TPM1SC_CLKSB = 0;// BUS RATE CLOCK
    
    
    TPM2SC_CLKSA = 1;
    TPM2SC_CLKSB = 0;
}


interrupt 9 void TPM1InputCapture_ISR(void){

    /* attends the interrupt request of timer when 
    there is a falling or rising edge

    It happens when the bottle pass through the sensor
    */
   // clear the interrupt request of CHOF (channel 0 flag)
   
   TPM1C0SC; //1 step, read the ch0 SC register
   TPM1C0SC_CH0F = 0; // 2 step, clear CH0F
   flag_inputCapture = 1; // indicates the main program the input capture

}

interrupt 10 void TPM1OnOFFButton_ISR(void){
	/* attends the interrupt request of timer when 
	there is a falling or rising edge

	It happens when the on/off button is pressed
	*/
	// clear the interrupt request of CHOF (channel 1 flag)
	TPM1C1SC;//1 step, read the ch1 SC register
	TPM1C1SC_CH1F = 0; // 2 step, clear CH1F
	flag_inputCapture = 2;// indicates the main program the input capture with On/off button
	
}

interrupt 11 void borrarCuentas_ISR(void){
	/* attends the interrupt request of timer when 
		there is a  rising edge
	It happens when the Borra_cuentas button is pressed
	*/
	// clear the interrupt request of CHOF (channel 1 flag)
	TPM1C2SC;//1 step, read the ch1 SC register
	TPM1C2SC_CH2F = 0; // 2 step, clear CH1F
	flag_inputCapture = 3;// indicates the main program the input capture with Borra_Cuentas
		
}


interrupt 12 void cuentaEstandar_ISR(void){
	/* attends the interrupt request of timer when 
		there is a  rising edge

		It happens when the cuentaEstandar button is pressed
		*/
		// clear the interrupt request of CHOF (channel 1 flag)
		TPM1C3SC;//1 step, read the ch1 SC register
		TPM1C3SC_CH3F = 0; // 2 step, clear CH1F
		flag_inputCapture = 4;// indicates the main program the input capture with Borra_Cuentas
		
}
interrupt 13 void CuentaEstandarMas_ISR(void){
	/* attends the interrupt request of timer when 
		there is a  rising edge

		It happens when the CuentaEstandarMas button is pressed
		*/
		// clear the interrupt request of CHOF (channel 1 flag)
		TPM1C4SC;//1 step, read the ch1 SC register
		TPM1C4SC_CH4F = 0; // 2 step, clear CH1F
		flag_inputCapture = 5;// indicates the main program the input capture with Borra_Cuentas	
}

interrupt 14 void CuentaEstandarMenos_ISR(void){
	/* attends the interrupt request of timer when 
		there is a  rising edge

		It happens when the CuentaEstandarMenos button is pressed
		*/
		// clear the interrupt request of CHOF (channel 1 flag)
		TPM1C5SC;//1 step, read the ch1 SC register
		TPM1C5SC_CH5F = 0; // 2 step, clear CH1F
		flag_inputCapture = 6;// indicates the main program the input capture with Borra_Cuentas	
}

interrupt 16 void averageDefectuosas_ISR(void){
	/* attends the interrupt request of timer when 
		there is a  rising edge

		It happens when the Average_Defectuosas button is pressed
		*/
		// clear the interrupt request of CHOF (channel 1 flag)
		TPM2C0SC;//1 step, read the ch1 SC register
		TPM2C0SC_CH0F = 0; // 2 step, clear CH1F
		flag_inputCapture = 7;// indicates the main program the input capture with Borra_Cuentas	
}



interrupt 15 void TPM1_Overflow_ISR(void){

    /* attends the interrupt request of timer when 
    there is an overflow in the counter
    
    it occurs when the count reach the module value
    
     */
    //Clear TOF in two steps
    TPM1SC;
    TPM1SC_TOF = 0;
    overflow_Count++; // increase overflows counter

}




