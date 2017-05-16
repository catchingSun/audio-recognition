main : main.o get_features.o sample_pcm.o
	gcc main.o get_features.o sample_pcm.o -o main -lpthread -lasound
sample_pcm.o : sample_pcm.c sample_pcm.h
	gcc -c sample_pcm.c -o sample_pcm.o
get_features.o : get_features.c get_features.h
	gcc -c get_features.c -o get_features.o
main.o : main.c get_features.h
	gcc -c main.c -o main.o
clean : 
	rm -r *.o main
