
#define FEATURES_SIZE 4000
#define SAMPLE_SIZE 100

struct Features{
	double features_array[FEATURES_SIZE];
	int len;
};

typedef struct Features FTUS;

/**
*function : 
*	Get audio file's features
*
*audio_path : 
*	Audio file's path
*features : 
*	Used to save the audio features.
*get_sample_offset: 
*	Change the file pointer position when read sample audio file
*sample_flag:
*	Control the synchronization of threads
*/

void get_audio_features(char *audio_path,  FTUS *features, int *get_sample_offset, int *sample_flag);

/**
*function : 
*	Compare the raw'features and sample's
*
*raw_features : 
*	Raw audio's features
*sample_features : 
*	Sample audio's features
*/
int _compare_raw_sample( FTUS *raw_features, FTUS *sample_features);

int compare_raw_sample( FTUS *raw_features, FTUS *sample_features);

/**
*function : 
*	recognition the raw audio faetures
*
*compare_result:
*	Compare the result of raw'features and sample's
*len :
*	Compare_result' length
*/
int find_the_result(double *compare_result, int len);
