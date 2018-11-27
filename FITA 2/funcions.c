#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "funcions.h"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif 

#include <fcntl.h>                                                        
#include <termios.h>       
#include <sys/ioctl.h> 
#include <sys/time.h>
#include <time.h> 
#include <pthread.h>
#include <signal.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h> 
#include <sqlite3.h> 
#include <getopt.h>


void informe (char *informefinal, char startdata[30],char finishdata[30],char Tmax[30], char Tmin[30],char Tavg[30],char tempsontotal[30],char tempsavgtotal[30],char numcopsfan[30])
{
	

	sprintf(informefinal,"SUBJECT:INFORME SEGUIMENT TEMPERATURA\nINFORME SEGUIMENT TEMPERATURA\n\
		----------------------------\n\
		Data inici: %s\t\tData final: %s\n\
		Temperatura màxima (ºC): %s\n\
		Temperatura mínima(ºC): %s\n\
		Temperatura mitjana(ºC): %s \n\
		Temps total ventilador ON (s): %s \n\
		Temps mitja ventilador ON (s): %s\n\
		Número cops que ha funciuonat el ventilador: %s \n\
		----------------------------\n\
		Llistat\n%s\n.\n",startdata,finishdata,Tmax,Tmin,Tavg,tempsontotal,tempsavgtotal,numcopsfan);
		
		printf("%s\n",informefinal);
}
