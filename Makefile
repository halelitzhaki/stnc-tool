default: all
all: stnc
stnc: stnc.o client.o server.o
	gcc -o stnc stnc.o client.o server.o
stnc.o: stnc.c client.h server.h
	gcc -c stnc.c 
client.o: client.h client.c
	gcc -c client.c
server.o: server.h server.c
	gcc -c server.c

clean: 
	rm *.o stnc
