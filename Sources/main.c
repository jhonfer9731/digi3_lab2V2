#include <hidef.h>
#include "derivative.h"
#include "events.h"



unsigned int time_edge_1, time_edge_2; //variables for indicating the current timer count when falling and rising edge occurs

unsigned long time_pulse_width; // time of bottle going across the sensor (this time measures indirectly the bottle diameter)




void Port_Init( void ){
    /* Ports configuration */
	
	
	
    PTADD = 0xFF; // output port
    PTAD = 0x00; // clear port (actuators)

}




void asignarMensaje(char *mensaje,char *nuevoMensaje){
	
	unsigned char contador = 0;
	while(nuevoMensaje[contador] != '\0'){
		mensaje[contador] = nuevoMensaje[contador];
		contador++;
	}
	
}

void asignarNumero(unsigned int A, char* mensaje){
	
	mensaje[2] = (A/10)+48;
	mensaje[3] = (A%10)+48;
		
}




void main(void){

    char firstEdge_flag = 0; // indicates the fist and second edge of the IC interrupt
    unsigned char movimiento = 0; // initial state of  conveyor belt
    unsigned int numeroBotellas = 0;
    unsigned char defecto = 0; // flag for wrong bottles
	unsigned int numeroBotellasStd = 0;
    unsigned int cuentaStandardMas = 0;
    unsigned int cuentaStandardMenos = 0;
    unsigned int averageDefectuosas  = 0;
    unsigned int botellasDefectuosas  = 0;
    unsigned int variableSeleccion = 0;  // it has the number of one bottle category
    const unsigned int min = 30000; // Min & Max to define the size range for Standard bottles.
	const unsigned int max = 50000; 
	unsigned int tmax = 0; //Counter for the 10 seconds
	
	char mensaje[65]= "  00   Numero de Botellas Std"; 
	
	
    // system initialisation
    SOPT1_COPT  = 0; // disable the WatchDog
    Timer1_init();
    Port_Init(); 

    //Enable interrupts
    EnableInterrupts;
   
    
    /**** Assigned input ports **/
    /*
     * 
     * 
    TImer 1: 
    
    PTED[2] = ch0 -> t1,t2 rising/fallin edge
    PTED[3] = ch1 -> on/off btn
    PTFD[0] = ch2 -> erase counts
    PTFD[1] = ch3 -> # standard bottles
    PTFD[2] = ch4 -> # stardard +
    PTFD[3] = ch5 -> # stardard -
    
    Timer 2:
    PTFD[4] = ch0 -> #deficient bottles average
    
     */


    for(;;){

        __asm WAIT;// an interruption wake the MCU up
        

        //check if the interrupt is an Input Capture
        if(flag_inputCapture == 1){
            flag_inputCapture = 0;
            // verify if it is first or second edge
            if(firstEdge_flag == 0 && movimiento){
                overflow_Count = 0; // reset the overflow counter
                firstEdge_flag = 1; 
                time_edge_1 = TPM1C0V; // save the current count of the timer
                PTAD_PTAD0 = 1; // the bottle just got captured by sensor (LED)
                PTAD_PTAD1 = 0;
            }
            else if(firstEdge_flag == 1 && movimiento){

                firstEdge_flag = 0; 
                time_edge_2 = TPM1C0V; // save the current count of the timer
                PTAD_PTAD0 = 0;
                PTAD_PTAD1 = 1; // the bottle just left the sensor (LED)
                time_pulse_width = (long) (time_edge_2 + overflow_Count*MODULO_TOPE_TIMER - time_edge_1); // calculates the time the bottle takes to go across the sensor
                if (time_pulse_width>=min && time_pulse_width<=max){
					numeroBotellasStd++;
					defecto=0;
					asignarMensaje(mensaje,"      Numero de botellas estandar");
					asignarNumero(numeroBotellasStd,mensaje);
					
				}
				else if(max<time_pulse_width){
					cuentaStandardMas++;
					botellasDefectuosas++;
					defecto=1;
				}
				else if(time_pulse_width<min){
					cuentaStandardMenos++;
					botellasDefectuosas++;
					defecto=1;
				}
				else{
					/* it never goes into */
					botellasDefectuosas++;
					defecto=1;
				}
				PTAD_PTAD4 = defecto; /* LED is active */ //PTAD[4] indicates the non-standard status
                overflow_Count = 0; // reset the overflow counter
                numeroBotellas++; // increases the bottle counter

            }
        }
        else if(flag_inputCapture == 2){
        	// on/off button was pressed
        	flag_inputCapture = 0;
        	if(!movimiento){
        		// on state
				PTAD_PTAD2 = 1; // PTAD[2] activates the conveyor belt
				movimiento = 1;
        	}else{
        		// off state
				PTAD_PTAD2 = 0; // PTAD[2] stops the conveyor belt
				movimiento = 0;
				PTAD_PTAD0 = 0;
				PTAD_PTAD1 = 0; // the bottle just left the sensor (LED)
				defecto = 0;
				PTAD_PTAD4 = 0;
        	}
        	PTAD_PTAD3 = movimiento; //PTAD[3] indicates the movement status
        	
        	
        }
        // it indicates that Borrar_cuentas button was pressed
        else if(flag_inputCapture == 3){
        	flag_inputCapture = 0;
        	if(!movimiento){ 
        		numeroBotellas = 0; // the number of bottles is reset
        	}
        }
        //the cuentaEstandar button is pressed
        else if(flag_inputCapture == 4){
        	flag_inputCapture = 0;
        	variableSeleccion = numeroBotellasStd;
        	asignarMensaje(mensaje,"      Numero de botellas estandar");
			asignarNumero(numeroBotellasStd,mensaje);
        }
        //It happens when the CuentaEstandarMas button is pressed
        else if(flag_inputCapture == 5){
			flag_inputCapture = 0;
			variableSeleccion = cuentaStandardMas;
			asignarMensaje(mensaje,"      Numero de botellas estandar mas");
			asignarNumero(variableSeleccion,mensaje);
        }
        //It happens when the CuentaEstandarMenos button is pressed
        else if(flag_inputCapture == 6){
			flag_inputCapture = 0;
			variableSeleccion = cuentaStandardMenos;
			asignarMensaje(mensaje,"      Numero de botellas estandar menos");
			asignarNumero(variableSeleccion,mensaje);
        }
        //It happens when the Average_Defectuosas button is pressed
        else if(flag_inputCapture == 7){
			flag_inputCapture = 0;
			if(numeroBotellas){
				variableSeleccion = (botellasDefectuosas*100)/numeroBotellas;
			}else{
				variableSeleccion = 0;
			}
			asignarMensaje(mensaje,"    %  Porcentaje botellas defectuosas");
			asignarNumero(variableSeleccion,mensaje);
        }
        else if(overflow_Count > 2){ // Enter when 
        	overflow_Count = 0;
        	PTAD_PTAD2 = 0; // PTAD[2] stops the conveyor belt
        	movimiento = 0;
        	PTAD_PTAD3 = movimiento; //PTAD[3] indicates the movement status
        	PTAD_PTAD0 = 0;
			PTAD_PTAD1 = 0;
			defecto = 0;
			PTAD_PTAD4 = 0;
			asignarMensaje(mensaje,"Tiempo de espera maximo alcanzado, el sistema se apagara ....");
        }
    }



}

