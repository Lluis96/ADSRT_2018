all: compilar 

compilar: testmail.o libemail.so
	gcc -o main informe.o -lsqlite3 -L/home/alumne/Escriptori/ADSRT_2018/FITA\ 2 -lemail 

email.o: email.c
	gcc  -fPIC -c -o email.o email.c

libemail.so: email.o
	gcc -shared -fPIC -o libemail.so email.o

testmail.o: informe.c
	gcc -c -o informe.o -lsqlite3 informe.c -I/home/alumne/Escriptori/ADSRT_2018/FITA\ 2

exec: compilar
	LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/alumne/Escriptori/ADSRT_2018/FITA\ 2 ./main
	
clean: 
	
	rm -rf *o main
