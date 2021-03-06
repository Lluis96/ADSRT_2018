/***************************************************************************
                          main.c  -  client
                             -------------------
    begin                : lun feb  4 16:00:04 CET 2002
    copyright            : (C) 2002 by A. Moreno
    email                : amoreno@euss.es
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>



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

#include <email.h>


#define REQUEST_MSG_SIZE	1024
#define REPLY_MSG_SIZE		500
#define SERVER_PORT_NUM		25




/*! comunicacio important a B115200  */
#define BAUDRATE B115200  //IMPORTANTE QUE SEA 115200
/*! activar fila dev/ttySO quan es vulgui comunicar amb Raspberry  */
//#define MODEMDEVICE "/dev/ttyS0"        //Conexió IGEP - Arduino
/*! activar fila dev/ttyACM0 quan es vulgui comunicar amb Raspberry  */
#define MODEMDEVICE "/dev/ttyACM0"         //Conexió directa PC(Linux) - Arduino
#define _POSIX_SOURCE 1 /* POSIX compliant source */

//-------------start declaració variables de sql--------------------------------------------

sqlite3 *db;
char *zErrMsg = 0;
int rc;
char *sql;
char *query = NULL;

sqlite3_stmt *stmt;

//-------------Final declaració variables de sql--------------------------------------------

//-------------start declaració ordres de sql--------------------------------------------
char *sqlTmax= "SELECT MAX (TEMPERATURA) FROM TEMPERATURA;";
char *sqlTmin= "SELECT MIN (TEMPERATURA) FROM TEMPERATURA;";
char *sqlTavg= "SELECT AVG (TEMPERATURA) FROM TEMPERATURA;";
char *sqldatainici= "SELECT (DATA) FROM TEMPERATURA order by DATA asc limit 1;";	//char *sql4= "SELECT (Data_i_Hora) WHERE Id=1 FROM historic5;";
char *sqldatafinal= "SELECT (DATA) FROM TEMPERATURA order by DATA desc limit 1;"; //////////////////
char *sqltotaltimefan= "SELECT SUM(TEMPS_ON) FROM ALARMES ;";

//triar entre taula alarmes o temperatura
//char *sqlavgtimefan= "SELECT avg (TEMPS_ON) FROM ALARMES;"; 
char *sqlavgtimefan= "SELECT avg (VENT) FROM TEMPERATURA;";


char *sqlcounttimefan= "SELECT COUNT (VENT) FROM TEMPERATURA WHERE VENT > 0;";
char *sqlalarmestable= "SELECT * FROM ALARMES;";
char *sqltemp= "SELECT * FROM TEMPERATURA;";
//-------------Final declaració ordres de sql-------------------------------------------
char variable[30];
char dataalarma[30];
char ton_fan[30];

char informefinal[1000];
//-------------Start funcio callbacksql ------------------------------------------------
// callback que es fa servir quan el resultat de la busqueda son varis valor -> taula alarmes
static int callbacksql(void *NotUsed, int argc, char **argv, char **azColName) {
   int i;
   for(i = 0; i<argc; i++) {
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
      if (i==1){
	  sprintf(dataalarma,"%s\n",azColName[i], argv[i] ? argv[i] : "NULL");
	  }
      else if (i==2){
		sprintf(ton_fan,"%s\n",azColName[i], argv[i] ? argv[i] : "NULL");  
	  }
      
	
   }
   printf("\n");
   return 0;
}
// callback que es fa servir quan el resultat de la busqueda es un valor 
static int callbacksql2(void *NotUsed, int argc, char **argv, char **azColName) {
   int i;
   //char variable[30];
   for(i = 0; i<argc; i++) {
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
      sprintf(variable, "%s\n", argv[i] ? argv[i] : "NULL");
   }
   
   return 0;
}
//-------------Final funcio callbacksql ------------------------------------------------

//-------------Start creació del informe -----------------------------------------------
//char montarinforme (char *datahorainici, char *datahorafinal, int *tempmax, int *tempmin, int *tempavg, int *totaltime, int *timeavg, int *venttimes){

void informe (char startdata[30],char finishdata[30],char Tmax[30], char Tmin[30],char Tavg[30],char tempsontotal[30],char tempsavgtotal[30],char numcopsfan[30])
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
		A continuació es mostra una llista de d'alarmes:\n%s\n.\n",startdata,finishdata,Tmax,Tmin,Tavg,tempsontotal,tempsavgtotal,numcopsfan);
		
		printf("%s\n",informefinal);
}
//}
//-------------Final creació del informe -----------------------------------------------

int main(int argc, char ** argv)
{
	char Tmax[30],Tmin[30],Tavg[30],startdata[30], finishdata[30],tempsontotal[30],tempsavgtotal[30],numcopsfan[30]; // variables on guardar les consultes sql
	char basedatos[50]="database.db";
	
	//-------------------start getopt per passar parametres per la línea de comandes------------------------------
	//---------------------------Final getopt---------------------------------------------------------
	//---------------------------Start open database--------------------------------------------------
		/* Open database */
   rc = sqlite3_open(basedatos, &db);

   if( rc ) {
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
      return(0);
   } else {
      fprintf(stdout, "Opened database successfully\n");
   }
   //---------------------------Final open database---------------------------------------------------
   
   //---------------------------Start instruccions sql------------------------------------------------
	
    // Llista alarmes 
	rc = sqlite3_exec(db, sqlalarmestable, callbacksql, 0, &zErrMsg);

	   if( rc != SQLITE_OK ){
	   fprintf(stderr, "SQL error: %s\n", zErrMsg);
		  sqlite3_free(zErrMsg);
	   } else {
		  fprintf(stdout, "Select datos correctamente\n");
	   }
	   
	   printf ("%s\n",dataalarma);
	   printf ("%s\n",ton_fan);
	   
	 // busquem la temperatura maxima de la base de dades  
	 rc = sqlite3_exec(db, sqlTmax, callbacksql2, 0, &zErrMsg);
	   if( rc != SQLITE_OK ){
	   fprintf(stderr, "SQL error: %s\n", zErrMsg);
		  sqlite3_free(zErrMsg);
	   } else {
		  fprintf(stdout, "Select datos correctamente\n");
	   }
	 strcpy(Tmax,variable);
	 printf ("Tmax: %s\n",Tmax);
	 
	  // busquem la temperatura min de la base de dades  
	 rc = sqlite3_exec(db, sqlTmin, callbacksql2, 0, &zErrMsg);
	   if( rc != SQLITE_OK ){
	   fprintf(stderr, "SQL error: %s\n", zErrMsg);
		  sqlite3_free(zErrMsg);
	   } else {
		  fprintf(stdout, "Select datos correctamente\n");
	   }
	 strcpy(Tmin,variable);
	 printf ("Tmin: %s\n",Tmin);
	  
	   // busquem la temperatura min de la base de dades  
	 rc = sqlite3_exec(db, sqlTavg, callbacksql2, 0, &zErrMsg);
	   if( rc != SQLITE_OK ){
	   fprintf(stderr, "SQL error: %s\n", zErrMsg);
		  sqlite3_free(zErrMsg);
	   } else {
		  fprintf(stdout, "Select datos correctamente\n");
	   }
	 strcpy(Tavg,variable);
	 printf ("Tavg: %s\n",Tavg); 

   // busquem la hora inci a la base de dades  
	 rc = sqlite3_exec(db, sqldatainici, callbacksql2, 0, &zErrMsg);
	   if( rc != SQLITE_OK ){
	   fprintf(stderr, "SQL error: %s\n", zErrMsg);
		  sqlite3_free(zErrMsg);
	   } else {
		  fprintf(stdout, "Select datos correctamente\n");
	   }
	 strcpy(startdata,variable);
	 printf ("data inici: %s\n",startdata); 
	 
	 
   // busquem la hora final a la base de dades  
	 rc = sqlite3_exec(db, sqldatafinal, callbacksql2, 0, &zErrMsg);
	   if( rc != SQLITE_OK ){
	   fprintf(stderr, "SQL error: %s\n", zErrMsg);
		  sqlite3_free(zErrMsg);
	   } else {
		  fprintf(stdout, "Select datos correctamente\n");
	   }
	 strcpy(finishdata,variable);
	 printf ("data final: %s\n",finishdata); 
	 
	 // busquem temps total funcionant del ventilador a la base de dades  
	 rc = sqlite3_exec(db, sqltotaltimefan, callbacksql2, 0, &zErrMsg);
	   if( rc != SQLITE_OK ){
	   fprintf(stderr, "SQL error: %s\n", zErrMsg);
		  sqlite3_free(zErrMsg);
	   } else {
		  fprintf(stdout, "Select datos correctamente\n");
	   }
	 strcpy(tempsontotal,variable);
	 printf ("Temps total ventilador ON: %s\n",tempsontotal); 
	 
	 // busquem temps mitja funcionant del ventilador a la base de dades  
	 rc = sqlite3_exec(db, sqlavgtimefan, callbacksql2, 0, &zErrMsg);
	   if( rc != SQLITE_OK ){
	   fprintf(stderr, "SQL error: %s\n", zErrMsg);
		  sqlite3_free(zErrMsg);
	   } else {
		  fprintf(stdout, "Select datos correctamente\n");
	   }
	 strcpy(tempsavgtotal,variable);
	 printf ("Temps mitja ventilador ON: %s\n",tempsavgtotal); 
	 
	 // busquem numero de cops funcionant del ventilador a la base de dades  
	 rc = sqlite3_exec(db, sqlcounttimefan, callbacksql2, 0, &zErrMsg);
	   if( rc != SQLITE_OK ){
	   fprintf(stderr, "SQL error: %s\n", zErrMsg);
		  sqlite3_free(zErrMsg);
	   } else {
		  fprintf(stdout, "Select datos correctamente\n");
	   }
	 strcpy(numcopsfan,variable);
	 printf ("Numero de cops funcionant ventilador ON: %s\n",numcopsfan);
   //---------------------------Final instruccions sql------------------------------------------------
  
   informe (startdata,finishdata,Tmax, Tmin,Tavg,tempsontotal,tempsavgtotal,numcopsfan);
  
	char De[]="1393272@campus.euss.org";
	char To[]="1393272@campus.euss.org";
	
	enviar_mail(De, To, informefinal);
	
   return 0;
   
}
