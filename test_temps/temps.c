// programa per provar recollir dades durant 1 dia
//Per compilar: gcc temps.c -lsqlite3 -lrt -lpthread -o temps


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

#define BAUDRATE B115200  //IMPORTANTE QUE SEA 115200                                                
//#define MODEMDEVICE "/dev/ttyS0"        //Conexió IGEP - Arduino
#define MODEMDEVICE "/dev/ttyACM0"         //Conexió directa PC(Linux) - Arduino                                   
#define _POSIX_SOURCE 1 /* POSIX compliant source */        

typedef void (timer_callback) (union sigval);
/* Funció set_timer
 * 
 * Crear un timer
 * 
 * Paràmetres:
 * timer_id: punter a una estructura de tipus timer_t
 * delay: retard disparament timer (segons)
 * interval: periode disparament timer  (segons)
 * func: funció que s'executarà al disparar el timer
 * data: informació que es passarà a la funció func
 * 
 * */
/**
 * @brief Funció per configurar el timer                
 */ 
int set_timer(timer_t * timer_id, float delay, float interval, timer_callback * func, void * data) 
{
    int status =0;
    struct itimerspec ts;
    struct sigevent se;

    se.sigev_notify = SIGEV_THREAD;
    se.sigev_value.sival_ptr = data;
    se.sigev_notify_function = func;
    se.sigev_notify_attributes = NULL;

    status = timer_create(CLOCK_REALTIME, &se, timer_id);

    ts.it_value.tv_sec = abs(delay);
    ts.it_value.tv_nsec = (delay-abs(delay)) * 1e09;
    ts.it_interval.tv_sec = abs(interval);
    ts.it_interval.tv_nsec = (interval-abs(interval)) * 1e09;

    status = timer_settime(*timer_id, 0, &ts, 0);
    return 0;
}

void fecha_informe (){
// Defineix punter a una estructura tm
    struct tm * p_data;
    char fecha[80];
    char fecha_daybefore[80];
    char fecha_day[80];
    char dia [10];
    char dia_before [10];
	//-------------start variables ordres de sql--------------------------------------------
	char sqlTmax[80],sqlTmin[80],sqlTavg[80],sqlavgtimefan[80],sqlcounttimefan[80];
	char *sqldatainici= "SELECT (DATA) FROM TEMPERATURA order by DATA asc limit 1;";	//char *sql4= "SELECT (Data_i_Hora) WHERE Id=1 FROM historic5;";
	char *sqldatafinal= "SELECT (DATA) FROM TEMPERATURA order by DATA desc limit 1;"; //////////////////
	char *sqltotaltimefan= "SELECT SUM(TEMPS_ON) FROM ALARMES ;";
	char *sqlalarmestable= "SELECT * FROM ALARMES;";
	char *sqltemp= "SELECT * FROM TEMPERATURA;";
    int op;
    
    //Definim una variable de tipus time_t
	time_t temps;
	//Capturem el temps amb la funcio time(time_t *t);
	temps = time(NULL);
	    
    /*---------------------------------------------------------------------------------------------*/
	//Funcion localtime() per traduir segons UTC a la hora:minuts:segons de la hora local
	//struct tm *localtime(const time_t *timep);
	
    p_data = localtime( &temps );
	
	strftime(fecha, 80,"%d/%m/%Y",p_data);
	
	strcpy(fecha_daybefore, fecha);
	
	strftime(dia, 10,"%d",p_data);
	
	op = (dia[0]-48)*10 + dia[1]-48;
	op = op--;
	
	if (op<10){
		sprintf (dia_before,"0%d", op);
		//printf ("dia_sql: %s\n",dia_before);
	}
	else {
		sprintf (dia_before,"%d", op);
		//printf ("dia_sql: %s\n",dia_before);
	}
	
	fecha_daybefore[0]=dia_before[0];
	fecha_daybefore[1]=dia_before[1];
	
	sprintf(fecha_day,"%s 23:59:59",fecha_daybefore);
	sprintf(fecha_daybefore,"%s 00:00:00",fecha_daybefore);
	
	printf ("fecha:\t\t%s\n",fecha_day);
	printf ("fecha before:\t%s\n",fecha_daybefore);
	
    /*--------------------------------------------------------------------------------------------*/
   
    sprintf(sqlTmax,"SELECT MAX (TEMPERATURA) FROM TEMPERATURA WHERE DATA BETWEEN  '%s' AND '%s';",fecha_daybefore,fecha_day);
    printf ("sqlTmax:\t%s\n",sqlTmax);
    
    sprintf(sqlTmin,"SELECT MIN (TEMPERATURA) FROM TEMPERATURA WHERE DATA BETWEEN  '%s' AND '%s';",fecha_daybefore,fecha_day);
    printf ("sqlTmin:\t%s\n",sqlTmin);
    
    sprintf(sqlTavg,"SELECT AVG (TEMPERATURA) FROM TEMPERATURA WHERE DATA BETWEEN  '%s' AND '%s';",fecha_daybefore,fecha_day);
    printf ("sqlTmin:\t%s\n",sqlTavg);
    
    sprintf(sqlavgtimefan,"SELECT AVG (VENT) FROM TEMPERATURA WHERE DATA BETWEEN  '%s' AND '%s';",fecha_daybefore,fecha_day);
    printf ("sqlavgtimefan:\t%s\n",sqlavgtimefan);
   
    sprintf(sqlcounttimefan,"SELECT COUNT (VENT) FROM TEMPERATURA WHERE DATA BETWEEN '%s' AND '%s' AND VENT > 0;",fecha_daybefore,fecha_day);
    printf ("sqlcounttimefan:\t%s\n",sqlcounttimefan); // S'HA DE MIRAR SI FUNCIONA CORRECTAMENT
    
    printf("sqldatainici:\t%s\n",sqldatainici);
    printf("sqldatafinal:\t%s\n",sqldatafinal);
    printf("sqltotaltimefan:\t%s\n",sqltotaltimefan);
    printf("sqlalarmestable:\t%s\n",sqlalarmestable);
    printf("sqltemp:\t%s\n",sqltemp);
    
}

int main(int argc, char ** argv)
{
	fecha_informe();

	}
