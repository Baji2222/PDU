#include <reg_sfr.h>

#include <stdio.h>

// Get Character Non-Blocking: Reads a character from UART without waiting if no data available
// Returns 0xFF if no character received, otherwise returns the received character
char getchar_nb(void){
	char c;	
	while(!RI){
		return(0xFF);
	}
	RI=0; 
	c=SBUF0; 
	return c; 
}

/* unused
// Get Character Blocking: Waits indefinitely for a character to be received from UART
// Returns the received character after clearing the receive interrupt flag
char getchar(void) 
{ 
    char c;	
	while(!RI);
	RI=0; 
	c=SBUF0; 
	return c; 
}
*/