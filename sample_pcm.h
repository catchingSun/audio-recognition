#ifndef __SAMPLE_PCM_

#define DEFAULT_CHANNELS         (1)  
#define DEFAULT_SAMPLE_RATE      (8000)  
#define DEFAULT_SAMPLE_LENGTH    (16)  
#define SAMPLE_FILE_SIZE	     (8000)
#define SAMPLE_SIZE 100


/**
*function :
*	Set up the sound parameters and Read pcm data
*
*sample_flag:
*	Control the synchronization of threads
*/
int get_sample_pcm(int *sample_flag);
#endif