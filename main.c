#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "sample_pcm.h"
#include "get_features.h"

int sample_flag = 0;
int get_sample_offset = 0;

/*Thread function : Get PCM data*/
void *get_sample_audio(void *arg){

	get_sample_pcm(&sample_flag);
}

/*Thread function : Compare the raw audio and sample audio*/
void *process_audio(void *arg){
	char *raw_audio_path = "./voice-8000hz-16bit-mono.pcm";
	char *sample_audio_path = "./sample_test.pcm";
	int fd;
	FTUS raw_features;
	FTUS sample_features;
	get_audio_features(raw_audio_path, &raw_features, 0, 0);
	while(1){

		if(sample_flag == 1){

			get_audio_features(sample_audio_path, &sample_features, &get_sample_offset, &sample_flag);
        	sample_flag = 0;
			printf("result : %d \n", compare_raw_sample(&raw_features, &sample_features));
		}
	}	
}

int main(int argc,char *argv[])
{

	pthread_t ntid_get_sample, ntid_process_audio;
	int net = -1;

	net = pthread_create(&ntid_get_sample, NULL, get_sample_audio, NULL);
	
	if(net < 0){
		printf("Can't create thread: %s!\r\n", strerror(net));
	}
	net = pthread_create(&ntid_process_audio, NULL, process_audio, NULL);

	if(net < 0){
		printf("Can't create thread: %s!\r\n", strerror(net));
	}

	while(1) ;
	return 0;
}
