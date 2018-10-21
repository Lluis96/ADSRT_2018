/**
  @Author Lluis Farnes
 */

/*!
 * (c) EUSS 2013
 *
 * Author    Lluis Farnes
   * Copyright (C) 2017 Lluis Farnes
   *e-mail: 1393274@campus.euss.org+
   * 
 * Exemple d'utilització dels timers de la biblioteca librt
 * Crea dos timers que es disparen cada segon de forma alternada
 * Cada cop que es disparen imprimeixen per pantalla un missatge
 * 
 * Per compilar: gcc main.c -lrt -lpthread -o main
 */
 
#include <pthread.h>
#include <signal.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>

/* VARIABLES GLOBALS*/
	/**
	@brief Variable conta els minuts que esta ences el ventilador
 */
int alarma=0;
	/**
 *@brief Variable que mostra l'estat del ventilador (0:apagat,1:posem funcionament, 2: continuem en funcionament)
 */
int vent=0;
	/**
 * @brief Variable amb la temperatura de consigna
 */
int Temperatura_regulacio=25;
	/**
 * @brief Variable amb la temperatura actual
 */
int Temperatura_actual;
	/**
 * @brief Variable funciona de flag per donar alarma: el ventilador porta 5 min consecutius funcionant
 */
int x;
/*------------------------*/

void regulacio_Temp (int T,int Treg){
	
	
	/*CONTROL VARIABLE TAULA TEMPERATURA + ESTAT VENTILADOR*/
	//T=temp. actual ha d'estar ja processada 
	//T=temperatura de regulacio
	//vent= Estat del ventilador (0:apagat,1:posem funcionament, 2: continuem en funcionament)
	//alarma= Flag per saltar alarma 
	
	if (T>Treg && vent==0) { // comprovem si s'ha d'encendre el ventilador		
							//Si el ventilador no estava ences posem variable ventilador a 1 
		printf("Encendre ventilador\n");/*<---------ENVIAR ORDRE ARDUINO ENCENDRE VENT*/
		
		vent=1;
		alarma=0; //resetejem contador minuts ventilador
		}
	else if(T>Treg && vent==1){
		vent=2;
		alarma++;	
		}
	else if (T>Treg && vent==2){
		alarma++;
		}
	else if (T<Treg && (vent==2||vent==1)){
		printf("Apagar ventilador\n");/*<---------ENVIAR ORDRE ARDUINO APAGAR VENT*/
		vent=0;
	}
	else {vent==0;}

	printf("Taula temperatura\n"); /*<---------ESCRIURE TAULA TEMPERATURES SQL*/
	printf("Temp: %d\n",T);
	printf("Estat vent: %d\n",vent);
/*--------------------------------------------------------------------------------------------*/

/*CONTROL VARIABLE TAULA ALARMES */

	if (alarma == 5){
		printf("!! Salta alarma -> Taula alarmes\n"); /*<---------ESCRIURE TAULA ALARMES SQL*/
		alarma=0;
	}

/*--------------------------------------------------------------------------------------------*/
	
}

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

void callback(union sigval si)
{
    char * msg = (char *) si.sival_ptr;
	printf("Executa rutina\n");
	x=1;
	printf("X: %d\n",x);
    printf("%s\n",msg);
}

int main(int argc, char ** argv)
{

    timer_t rutina;
 
    set_timer(&rutina, 1, 10, callback,(void *) "rutina"  );
    //getchar();
	while(1){
	if (x==1){
		printf("Escriu temperatura actual: ");
		scanf("%d/n",&Temperatura_actual);
		regulacio_Temp (Temperatura_actual,Temperatura_regulacio);/*<---------ES COMPARA T AMB TREG I POSA BASE DADES*/
		x=0;
	}else{}
	}
	
		
    return 0;
}
