#include <common.h>
#include <stdio.h>
#include <salfwug.h>

#if !defined(LWS_LOADER)
static char getchar(void){
    char c; 
    while(!RI){
        return(0xFF);
    }
    RI=0; 
    c=SBUF0; 
    return c; 
}
#endif
void loader_main()
{
    printf("loader start\n");

    loader_init();

    while(1)
    {
        event_dispatch();
    }
}

