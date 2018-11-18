// Per compilar el programa :  //gcc informe.c  -lsqlite3 -o informe

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
char *sqldatainici= "SELECT (data_i_hora) FROM historic5 order by Id asc limit 1;";	//char *sql4= "SELECT (Data_i_Hora) WHERE Id=1 FROM historic5;";
char *sqldatafinal= "SELECT (data_i_hora) FROM historic5 order by Id desc limit 1;"; //////////////////
char *sqltotaltimefan= "SELECT (T_ventilador) FROM Alarmes5 order by Id desc limit 1;";
char *sqlavgtimefan= "SELECT avg (T_ventilador) FROM Alarmes5;";
char *sqlmaxtimefan= "SELECT max (Id) FROM Alarmes5;";
char *sqlalarmestable= "SELECT * FROM ALARMES;";
//-------------Final declaració ordres de sql-------------------------------------------
char variable[30];

//-------------Start funcio callbacksql ------------------------------------------------

static int callbacksql(void *NotUsed, int argc, char **argv, char **azColName) {
   int i;
   for(i = 0; i<argc; i++) {
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
      printf("argv[i]:%s\n",argv[i]);
	
   }
   printf("\n");
   return 0;
}

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
	
//}
//-------------Final creació del informe -----------------------------------------------

int main(int argc, char ** argv)
{
	char Tmax[30],Tmin[30],Tavg[30]; // variables on guardar les consultes sql
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
   
   rc = sqlite3_exec(db, sqlalarmestable, callbacksql, 0, &zErrMsg);

	   if( rc != SQLITE_OK ){
	   fprintf(stderr, "SQL error: %s\n", zErrMsg);
		  sqlite3_free(zErrMsg);
	   } else {
		  fprintf(stdout, "Select datos correctamente\n");
	   }
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

   //---------------------------Final instruccions sql------------------------------------------------

 
   
}
