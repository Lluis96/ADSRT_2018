// programa per provar recollir dades durant 1 dia
//Per compilar: gcc informe.c -lsqlite3 -lrt -lpthread -o informe

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
#include <string.h>

#include <email.h> //llibreria per fer servir la funció enviarmail
#include <funcions.h> 


#define BAUDRATE B115200  //IMPORTANTE QUE SEA 115200                                                
//#define MODEMDEVICE "/dev/ttyS0"        //Conexió IGEP - Arduino
#define MODEMDEVICE "/dev/ttyACM0"         //Conexió directa PC(Linux) - Arduino                                   
#define _POSIX_SOURCE 1 /* POSIX compliant source */        


sqlite3 *db;
char *zErrMsg = 0;
int rc;
char *sql;
char *query = NULL;

sqlite3_stmt *stmt;

// Defineix punter a una estructura tm
    struct tm * p_data;
    char fecha[80];
    char fecha_daybefore[80];
    char fecha_daybefore_dos[80];
    char fecha_day[80];
    char dia [10];
    char dia_before [10];
	//-------------start variables ordres de sql--------------------------------------------
	char sqlTmax[80],sqlTmin[80],sqlTavg[80],sqlcounttimefan[80],sqlalarmestable[80];
	
	//char *sqldatainici= "SELECT (DATA) FROM TEMPERATURA order by DATA asc limit 1;";	//char *sql4= "SELECT (Data_i_Hora) WHERE Id=1 FROM historic5;";
	//char *sqldatafinal= "SELECT (DATA) FROM TEMPERATURA order by DATA desc limit 1;"; //////////////////
	//char *sqltemp= "SELECT * FROM TEMPERATURA;";
	
    int op;
    int j;
    
	char variable[80]; //es fa servir callback2
	char dataalarma[30][30]; //es fa servir callback
	char ton_fan[30][30]; //es fa servir callback

	char *De="1393272@campus.euss.org";
	char *To="1393272@campus.euss.org";
    
    // variables on guardar les consultes sql
	char Tmax[80],Tmin[80],Tavg[80],startdata[80], finishdata[80],tempsontotal[80],tempsavgtotal[80],numcopsfan[80],llistaalarmes[1000]; 

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
	sprintf(fecha_daybefore_dos,"%s 00:00:00",fecha_daybefore);
	
	printf ("fecha:\t\t%s\n",fecha_day);
	printf ("fecha before:\t%s\n",fecha_daybefore_dos);
	
    /*--------------------------------------------------------------------------------------------*/
	
    
    //sprintf(sqlTmin,"SELECT MIN (TEMPERATURA) FROM TEMPERATURA WHERE DATA BETWEEN  '%s' AND '%s';",fecha_daybefore_dos,fecha_day);
   // printf ("sqlTmin:\t%s\n",sqlTmin);
 
    
    sprintf(sqlTavg,"SELECT AVG (TEMPERATURA) FROM TEMPERATURA WHERE DATA BETWEEN  '%s' AND '%s';",fecha_daybefore_dos,fecha_day);
   // printf ("sqlTmin:\t%s\n",sqlTavg);
    
    
   
   // printf ("sqlcounttimefan:\t%s\n",sqlcounttimefan); // S'HA DE MIRAR SI FUNCIONA CORRECTAMENT
    
  //  printf("sqldatainici:\t%s\n",sqldatainici);
  //  printf("sqldatafinal:\t%s\n",sqldatafinal);
  //  printf("sqltotaltimefan:\t%s\n",sqltotaltimefan);
  //  printf("sqltemp:\t%s\n",sqltemp);
  
}

//-------------Start funcio callbacksql ------------------------------------------------
// callback que es fa servir quan el resultat de la busqueda son varis valor -> taula alarmes

static int callbacksql(void *NotUsed, int argc, char **argv, char **azColName) {
   int i;
   for(i = 0; i<argc; i++) {
     // printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
      if (i==0){
	  sprintf(dataalarma[j],"%s\n", argv[i] ? argv[i] : "NULL");
	  printf("dataalarma %s \n",dataalarma[j]);
	  }
      else if (i==1){
		sprintf(ton_fan[j],"%s\n", argv[i] ? argv[i] : "NULL");
		printf("ton fan %s \n",ton_fan[j]);
	  }
   }
    j++;
   printf("\n");
   return 0;
}




// callback que es fa servir quan el resultat de la busqueda es un valor 
static int callbacksql2(void *NotUsed, int argc, char **argv, char **azColName) {
   int i;
   for(i = 0; i<argc; i++) {
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
      sprintf(variable, "%s\n", argv[i] ? argv[i] : "NULL");
   }
   
   return 0;
}



int main(int argc, char ** argv)
{
	
	int c;
	while ((c = getopt(argc, argv, "d:t:h")) != -1) {
		switch (c) {
		case 'd':
			De=optarg;	
			printf("El correu remitent es: %s\n",De);
			
			break;
		case 't':
			To=optarg;
			printf("El correu destí es: %s\n",To);
			break;
		case '?':
			printf("Opció desconeguda prem '-h' per veure l'ajuda.\n");
			break;
		default:
			abort();
		}
	}
	
	char basedatos[50]="database.db";
	/* Open database */
	rc = sqlite3_open(basedatos, &db);
	
	if( rc ) {
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
      return(0);
   } else {
      fprintf(stdout, "Opened database successfully\n");
   }
   
	fecha_informe();
	
	//// 1- Llista alarmes
	//printf ("1-Llista alarmes: \n"); 
	//sprintf(sqlalarmestable,"SELECT * FROM ALARMES WHERE DATA BETWEEN  '%s' AND '%s';",fecha_daybefore,fecha_day);
   // printf ("sqlalarmestable:\t%s\n",sqlalarmestable); // S'HA DE MIRAR SI FUNCIONA CORRECTAMENT
	//printf ("SENTENCIA SQL: %s \n",sqlalarmestable); 
	//rc = sqlite3_exec(db, sqlalarmestable, callbacksql, 0, &zErrMsg);

	   //if( rc != SQLITE_OK ){
	   //fprintf(stderr, "SQL error: %s\n", zErrMsg);
		  //sqlite3_free(zErrMsg);
	   //} else {
		  //fprintf(stdout, "Select datos correctamente\n");
	   //}
	   
	   //printf ("data alarma: %s\n",dataalarma);
	   //printf ("ton_fan: %s\n",ton_fan);
	  
	 // 2- Busquem la temperatura maxima de la base de dades  
	 
	//printf ("2- Busquem la temperatura maxima de la base de dades: \n"); 
    sprintf(sqlTmax,"SELECT MAX (TEMPERATURA) FROM TEMPERATURA WHERE DATA BETWEEN  '%s' AND '%s';",fecha_daybefore_dos,fecha_day);
    //printf ("sqlTmax:\t%s\n",sqlTmax);
    
    rc = sqlite3_exec(db, sqlTmax, callbacksql2, 0, &zErrMsg);
	   if( rc != SQLITE_OK ){
	   fprintf(stderr, "SQL error: %s\n", zErrMsg);
		  sqlite3_free(zErrMsg);
	   } else {
		  fprintf(stdout, "Select datos correctamente\n");
	   }
	 strcpy(Tmax,variable);
	 //printf ("Temperatura maxima: %s\n",Tmax);
	
	
	// 3- Busquem la temperatura min de la base de dades  
	//printf (" 3- Busquem la temperatura min de la base de dades  : \n"); 
    sprintf(sqlTmin,"SELECT MIN (TEMPERATURA) FROM TEMPERATURA WHERE DATA BETWEEN  '%s' AND '%s';",fecha_daybefore_dos,fecha_day);
    //printf ("sqlTmax:\t%s\n",sqlTmin);
    
	 rc = sqlite3_exec(db, sqlTmin, callbacksql2, 0, &zErrMsg);
	   if( rc != SQLITE_OK ){
	   fprintf(stderr, "SQL error: %s\n", zErrMsg);
		  sqlite3_free(zErrMsg);
	   } else {
		  fprintf(stdout, "Select datos correctamente\n");
	   }
	 strcpy(Tmin,variable);
	 printf ("Temperatura minima: %s\n",Tmin);
	 
	 // 4- Busquem la temperatura avg de la base de dades  
	 rc = sqlite3_exec(db, sqlTavg, callbacksql2, 0, &zErrMsg);
	   if( rc != SQLITE_OK ){
	   fprintf(stderr, "SQL error: %s\n", zErrMsg);
		  sqlite3_free(zErrMsg);
	   } else {
		  fprintf(stdout, "Select datos correctamente\n");
	   }
	 strcpy(Tavg,variable);
	 //printf ("Temperatura mediana: %s\n",Tavg);
	 
	 // 5- Busquem el temps que el ventilador ha estat funcionant   
	 sprintf(sqlcounttimefan,"SELECT COUNT(*) FROM TEMPERATURA WHERE VENT>0 AND DATA BETWEEN '%s' AND '%s' ;",fecha_daybefore_dos,fecha_day);
	 rc = sqlite3_exec(db, sqlcounttimefan, callbacksql2, 0, &zErrMsg);
	   if( rc != SQLITE_OK ){
	   fprintf(stderr, "SQL error: %s\n", zErrMsg);
		  sqlite3_free(zErrMsg);
	   } else {
		  fprintf(stdout, "Select datos correctamente\n");
	   }
	 strcpy(numcopsfan,variable);
	 strcpy(tempsontotal,variable);
	 
	 int numcopsfanint;
	
	 numcopsfanint = atoi (numcopsfan);
	 printf("Minutos totales encendido: %d \n\n",numcopsfanint);
	 float mediafan = numcopsfanint/1440.0;
	 printf ("Temps mitja de funcionant el ventilador (s): %f\n",mediafan);

	j=0; 
	//6- Llista alarmes 
	sprintf(sqlalarmestable,"SELECT * FROM ALARMES WHERE DATA BETWEEN '%s' AND '%s' ;",fecha_daybefore_dos,fecha_day);
	rc = sqlite3_exec(db, sqlalarmestable, callbacksql, 0, &zErrMsg);
	   if( rc != SQLITE_OK ){
	   fprintf(stderr, "SQL error: %s\n", zErrMsg);
		  sqlite3_free(zErrMsg);
	   } else {
		  fprintf(stdout, "Select datos correctamente\n");
	   }
	  
	// Generació del informe
	sprintf(llistaalarmes,"SUBJECT:INFORME SEGUIMENT TEMPERATURA\nINFORME SEGUIMENT TEMPERATURA\n");
	sprintf(llistaalarmes,"%s\nData/hora inici %s\n",llistaalarmes,fecha_daybefore_dos);
	sprintf(llistaalarmes,"%s\nData/hora final %s\n",llistaalarmes,fecha_day);
	sprintf (llistaalarmes,"%s\nTemperatura maxima: %s",llistaalarmes,Tmax);
	sprintf (llistaalarmes,"%s\nTemperatura minima: %s",llistaalarmes,Tmin);
	sprintf (llistaalarmes,"%s\nTemperatura mediana: %s",llistaalarmes,Tavg);
	sprintf (llistaalarmes,"%s\nTemps total que ha estat funcionant el ventilador (s): %s",llistaalarmes,tempsontotal);
	sprintf (llistaalarmes,"%s\nTemps mitja de funcionant el ventilador (s): %.2f\n",llistaalarmes,mediafan);
	sprintf (llistaalarmes,"%s\nNumero de cops  que ha estat funcionant el ventilador (s): %s\n",llistaalarmes,numcopsfan);
	sprintf (llistaalarmes,"%s\nLLista d'alarmes:\n",llistaalarmes);
	
	
	int i;
	
	
	sprintf (llistaalarmes,"%s\n %s %s",llistaalarmes,dataalarma[0],ton_fan[0]);
	
	for(i = 1; i<j; i++) {
	sprintf (llistaalarmes,"%s %s %s",llistaalarmes,dataalarma[i],ton_fan[i]);
	
	}
	sprintf (llistaalarmes,"%s\n.\n",llistaalarmes);
	printf("%s\n",llistaalarmes);
	
	enviar_mail(De, To, llistaalarmes);
	
	return 0;
	
	}
