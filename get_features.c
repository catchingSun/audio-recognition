#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "sample_pcm.h"
#include "get_features.h"

void get_audio_features(char *audio_path,  FTUS *features, int *get_sample_offset, int *sample_flag){
	int i = 0;
	FILE *audio_fp, *feature_fp;
	int file_site;
	int fd;
	
	char *feature_path = "./feature";
	char sample[SAMPLE_SIZE];
	int count = 0;
	int file_size;
	
	//Open audio file
	audio_fp = fopen(audio_path, "rb");
	feature_fp = fopen(feature_path, "w");
	
	if(audio_fp == NULL){
		printf("Open %s file error", audio_path);
		return;
	}
	fseek(audio_fp,0,SEEK_END);
	file_size = ftell(audio_fp);
	fseek(audio_fp,0,SEEK_SET);
	
	memset( features->features_array, 0, FEATURES_SIZE);
	if(feature_fp == NULL){
		printf("Open %s file error", feature_path);
		return;
	}

	//Read sample audio file every 100 bytes and sum them as raw features
	if(strcmp(audio_path, "./sample_test.pcm") == 0){
		
		fseek(audio_fp, *get_sample_offset, SEEK_SET);
		while(!feof(audio_fp) && (count <= (SAMPLE_FILE_SIZE / SAMPLE_SIZE))){

			fread(sample, 1, SAMPLE_SIZE, audio_fp);
			for(i = 0; i < SAMPLE_SIZE; i++){
				features->features_array[count] += sample[i];
			}
			fprintf(feature_fp, "%f\t", features->features_array[count]);
			count ++;
		}
		*get_sample_offset += SAMPLE_FILE_SIZE;
		int so_size = *get_sample_offset;
		//printf("get_sample_offset : %d %d %d\n", so_size, file_size, count);

		if(so_size >= 1024 * 8){
			*sample_flag = -1;
    		*get_sample_offset = 0;
    		
		}
	}
	//Read raw audio file every 100 bytes and sum them as raw features
	else{
		while(!feof(audio_fp)){

			fread(sample, 1, SAMPLE_SIZE, audio_fp);
			for(i = 0; i < SAMPLE_SIZE; i++){
				features->features_array[count] += sample[i];
			}	
			fprintf(feature_fp, "%f\t", features->features_array[count]);
			count ++;
		}
	}
		
	features->len = count - 1;
	//printf("%s file_size: %d\n", audio_path,features->len);
	
	fclose(audio_fp);
	fclose(feature_fp);

}


int _compare_raw_sample( FTUS *raw_features, FTUS *sample_features){

	int *raw_win_start, *raw_win_end, *sample_win_start, *sample_win_end;
	double compare_result[1000000];
	
	int i = 0, j = 0, k = 0, m = 0;
	int raw_len = raw_features->len;
	int sample_len = sample_features->len;
	int count = 0;
	//printf("raw_len: %d\nsample_len: %d\n", raw_len, sample_len);
	
	char *feature_path = "./compare";
	FILE *fp;
	fp = fopen(feature_path, "w");

	/*Get the compare result*/
	for(; i < sample_len; i++){
		
		j = 0;
		if(k + raw_len <= sample_len){
			for(k = i; j < raw_len; j++, k++){
				if((raw_features->features_array[j] == 0) && (sample_features->features_array[k] != 0)){
					compare_result[count] = raw_features->features_array[j] / sample_features->features_array[k];
					
				}else if((sample_features->features_array[k] == 0) && (raw_features->features_array[j] != 0)){
					compare_result[count] = sample_features->features_array[k] / raw_features->features_array[j];

				}else if((raw_features->features_array[j] == 0) && (sample_features->features_array[k] == 0)){
					compare_result[count] = 0;

				}else{
					compare_result[count] = raw_features->features_array[j] / sample_features->features_array[k];

				}
				fprintf(fp, "%f\t", compare_result[count]);
				count ++;
			}
		}else if(k + raw_len >= sample_len){
			
			for(m = k + raw_len - sample_len - 1; j < raw_len, k < sample_len; j++, k++){
				
				if((raw_features->features_array[j] == 0) && (sample_features->features_array[k] != 0)){
					compare_result[count] = raw_features->features_array[j] / sample_features->features_array[k];
					
				}else if((sample_features->features_array[k] == 0) && (raw_features->features_array[j] != 0)){
					compare_result[count] = sample_features->features_array[k] / raw_features->features_array[j];

				}else if((raw_features->features_array[j] == 0) && (sample_features->features_array[k] == 0)){
					compare_result[count] = 0;

				}else{
					compare_result[count] = raw_features->features_array[j] / sample_features->features_array[k];

				}
				fprintf(fp, "%f\t", compare_result[count]);
				count ++;
			}
		}
		if(count % 10 == 0){
			fprintf(fp, "\n");
		}
	}
	//printf("count : %d\n", count);
	fclose(fp);
	int find_result = find_the_result(compare_result, count);
	//printf("find_the_result :%d \n", find_result);
	return find_result;
}

int compare_raw_sample( FTUS *raw_features, FTUS *sample_features){
	int find_result ;
	if(sample_features->len >= raw_features->len){
		find_result = _compare_raw_sample(raw_features, sample_features);
	}else{
		find_result = _compare_raw_sample(sample_features, raw_features);
	}
	return find_result;
}

int find_the_result(double *compare_result, int len){
	int i = 0, j = 0;
	
	double raw_features_sum;
	for(; i < len; i++){
	
		int count = 0;
		for(j = i; (j - i) < 40; j++){
			if(compare_result[j] > 1){
				
				compare_result[j] = compare_result[j] - 1;
			}else{
				compare_result[j]  = 1 - compare_result[j];
			}
			if(compare_result[j] >= 0.17){
				count++;
			}
		}
		if(count <= 10){
			//printf("==========================================================================\n");
			return 1;
		}

		i = i + j - 1;
	}
	
	return 0;
}
