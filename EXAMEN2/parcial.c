/***************************************************************************
                          parcial.c  
                             -------------------
    begin                : mar ene  9 16:00:04 CET 2002
    copyright            : (C) 2018 by Agusti Fontquerni
    email                : afontquerni@euss.es
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 
 //compilar : gcc parcial.c -lsqlite3 -o parcial 

#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <email.h> //llibreria per fer servir la funció enviarmail

/*! \mainpage Fita 1
 *
 * \section intro_sec Introduction
 *
 * This is the introduction EUSS.
 *
 * \section install_sec Installation
 *
 * \subsection step1 Step 1: Opening the box
 *
 * etc...
 */

/*
sqlite> .schema meteo

CREATE TABLE meteo(
  "Hora" TEXT,
  "TempMitjana" REAL,
  "TempMaxima" REAL,
  "Temp Minima" REAL,
  "Humitat" INTEGER,
  "Precipitacio" REAL,
  "VentVeloMitjana" REAL,
  "VentDireccioMitjana" INTEGER,
  "VentMaxim" REAL,
  "Pressio" REAL,
  "IrradianciaSolar" INTEGER
);

 */ 

#define MAXSTR 100

int g_verbose = 0;
char *g_bdname = "parcial.db";
char g_dbstr[MAXSTR];


char *De="1393274@campus.euss.org"; //Adreça electrònica del que envia el mail
char *To="1393274@campus.euss.org"; //Adreça electrònca del receptor
    

struct meteo {
	int hminima;  //Humitat Minima
	float tmaxima;  //Temperatura Maxima
	float tmitjana;  //Temperatura Mitjana
	float tminima;  //Temperatura Minima
	float velventmax;  //Velocitat maxima del vent
	int direccioventvelmax;  //Direccio del vent en la velocitat max
	int irradiaciomax;  //Irradiància Solar Màxima
	char hora[10];	//Hora de la irradiació Solar Max
	
} g_dades;

void sqlclean(void)
{
	memset( g_dbstr, 0, MAXSTR );

}

static int sqlcallback(void *NotUsed, int argc, char **argv, char
**azColName){
	int i;

	
	if(g_verbose) {
			printf("%s:%s\n", *azColName, argv[0]);
	}
	strcpy(g_dbstr, argv[0]);
	return 0;
}

int generar_informe(char *buffer) {

	strcpy(buffer, "Generant informe METEO\n" );
	strcat(buffer, "======================\n\n" );
	
	sprintf(buffer, 
		"%s SUBJECT:INFORME \n\
Temperatura Mitjana: %f\n\
Temperatura Maxima: %f\n\
Temperatura Minima: %f\n\
Velocitat màxima del vent: %f\n\
Direccio del vent en la velocitat maxima: %d\n\
Irradiància Solar Maxima: %d\n\
Hora Irradiància Solar Maxima:%s\n",
		buffer, 
		g_dades.tmitjana , 
		g_dades.tmaxima ,
		g_dades.tminima ,
		g_dades.velventmax ,
		g_dades.direccioventvelmax ,
		g_dades.irradiaciomax ,
		g_dades.hora
		);

	strcat(buffer, "======================\n\n\n.\n" );

	return 0;
}

int llegir_dades(void) {

	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
	char *sql;

	rc = sqlite3_open(g_bdname, &db);
	if (rc) {
		fprintf(stderr, "Can't open database: %s\n",
		sqlite3_errmsg(db));
		sqlite3_close(db);
		return 1;
	}

	// -- Temperatura mitjana
	sqlclean();
	sql = "select AVG(TempMitjana) from meteo;";
	rc = sqlite3_exec(db, sql, sqlcallback, 0, &zErrMsg);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return 2;
	}
	g_dades.tmitjana = atof(g_dbstr);
	
	// -- Temperatura Maxima
	sqlclean();
	sql = "select MAX(TempMaxima) from meteo;";
	rc = sqlite3_exec(db, sql, sqlcallback, 0, &zErrMsg);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return 2;
	}
	g_dades.tmaxima = atof(g_dbstr);
	
	// -- Temperatura minima
	sqlclean();
	sql = "select MIN(TempMinima) from meteo;";
	rc = sqlite3_exec(db, sql, sqlcallback, 0, &zErrMsg);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return 2;
	}
	g_dades.tminima = atof(g_dbstr);
	
	// -- Velocitat maxima del vent
	sqlclean();
	sql = "select MAX(VentMaxim) from meteo;";
	rc = sqlite3_exec(db, sql, sqlcallback, 0, &zErrMsg);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return 2;
	}
	g_dades.velventmax = atof(g_dbstr);
	
	// -- Direccio del vent en la velocitat max 
	sqlclean();
	sql = "select VentDireccioMitjana from meteo WHERE  VentMaxim = (SELECT MAX(VentMaxim) FROM meteo);";
	rc = sqlite3_exec(db, sql, sqlcallback, 0, &zErrMsg);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return 2;
	}
	g_dades.direccioventvelmax = atof(g_dbstr);
	
	// -- Irradiació Solar maxima
	sqlclean();
	sql = "select MAX(IrradianciaSolar) from meteo;";
	rc = sqlite3_exec(db, sql, sqlcallback, 0, &zErrMsg);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return 2;
	}
	g_dades.irradiaciomax = atof(g_dbstr);
	
	// -- Hora irradiació solar maxima
	sqlclean();
	sql = "select Hora from meteo WHERE  IrradianciaSolar = (SELECT MAX(IrradianciaSolar) FROM meteo);";
	rc = sqlite3_exec(db, sql, sqlcallback, 0, &zErrMsg);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return 2;
	}
	g_dades.hora[0]=g_dbstr[0];
	g_dades.hora[1]=g_dbstr[1];
	g_dades.hora[2]=g_dbstr[2];
	g_dades.hora[3]=g_dbstr[3];
	g_dades.hora[4]=g_dbstr[4];

	sqlite3_close(db);
	return 0;
}


void print_usage(void) {
    printf("Usage: ./parcial --bdname FITXER [arguments]\n");
	printf("\n\t-b or --bdname [name] \tPath and database name\n");
    printf("\n   arguments:");
    printf("\n\t-h or --help");
    printf("\n\t-v or --verbose");
    printf("\n\n");
}

int input_parameters(int argc, char *argv[]) {
    int opt= 0;

    //Specifying the expected options
    //The two options l and b expect numbers as argument
    static struct option long_options[] = {
	{"bdname",	required_argument, 0,  'b' },
	{"help",	no_argument, 	0,  'h' },
	{"verbose",	no_argument, 	0,  'v' },
	{0,           0,                 0,  0   }
    };

    int long_index =0;
    while ((opt = getopt_long(argc, argv,"b:hv", 
		long_options, &long_index )) != -1) {
		switch (opt) {
			case 'b' : 
				g_bdname = optarg;
				break;
			default:
			case 'h':
				print_usage();
				return 1;
			case 'v' : 
				g_verbose = 1;
				break;
		}
    }

    return 0;
}


int main(int argc, char **argv)
{
	char buffer[10000];
	int result = 0;
	

	printf("Inici exercici parcial:\n\n");
	memset( &g_dades, 0, sizeof(struct meteo) );
	
	if(argc <2)	{
		print_usage();
		return 1;
	}
	
	if ( input_parameters(argc, argv) )
		return 0;

	if(g_verbose) printf("\n llegir_dades\n");
	llegir_dades();

	if(g_verbose) printf("\n generar_informe\n");
	generar_informe( buffer );
	enviar_mail(De, To, buffer); // funcio que envia el missatge llistaalarmes per e-mail 
	printf( "%s", buffer );

	return 0;
}
		
