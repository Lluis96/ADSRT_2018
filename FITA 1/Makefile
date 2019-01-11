#Makefile Fita 1 - Author: Lluis Farnes  Revision:1 (21/10/18 )

all: compilar

compilar: 
		gcc captura.c -lsqlite3 -lrt -lpthread -o captura
		
exec: compilar
	./captura
		
clean: 
	rm -rf *o captura



