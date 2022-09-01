#-Wall : pour afficher tous les warnning
#-g : affiche les options de deboguage proposer
saturnd : saturnd.o 
	gcc -Wall -g -o saturnd saturnd.o

saturnd.o : src/saturnd.c
	gcc -Wall -g -I include -c src/saturnd.c -o saturnd.o

cassini : cassini.o timing-text-io.o sent_request.o 
	gcc -Wall -g -o cassini cassini.o timing-text-io.o sent_request.o

cassini.o : src/cassini.c
	gcc -Wall -g -I include -c src/cassini.c -o cassini.o

timing-text-io.o : src/timing-text-io.c
	gcc -Wall -g -I include -c src/timing-text-io.c -o timing-text-io.o

sent_request.o : src/sent_request.c
	gcc -Wall -g -I include -c src/sent_request.c -o sent_request.o



distclean : 
	rm -rf *.o cassini saturnd