all: compilar 

compilar: parcial.o libemail.so
	gcc -o parcial parcial.o -lsqlite3 -L/home/alumne/Escriptori/EXAMEN2 -lemail 

email.o: email.c
	gcc  -fPIC -c -o email.o email.c

libemail.so: email.o 
	gcc -shared -fPIC -o libemail.so email.o 

parcial.o: parcial.c
	gcc -c -o parcial.o -lsqlite3 parcial.c -I/home/alumne/Escriptori/EXAMEN2

exec: compilar
	LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/alumne/Escriptori/EXAMEN2 ./parcial -b parcial.db

clean: 
	rm -rf *o parcial


