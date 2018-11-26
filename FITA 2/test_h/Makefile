all: compilar 

compilar: testmail.o libemail.so
	gcc -o main testmail.o -L/home/alumne/Escriptori/ADSRT_2018/FITA\ 2/test_h -lemail 

email.o: email.c
	gcc  -fPIC -c -o email.o email.c

libemail.so: email.o
	gcc -shared -fPIC -o libemail.so email.o

testmail.o: testmail.c
	gcc -c -o testmail.o testmail.c -I/home/alumne/Escriptori/ADSRT_2018/FITA\ 2/test_h

exec: compilar
	LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/alumne/Escriptori/ADSRT_2018/FITA\ 2/test_h ./main
	
clean: 
	
	rm -rf *o main
