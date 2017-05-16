#include <alsa/asoundlib.h>
#include <pthread.h>

#include "sample_pcm.h"


int get_sample_pcm(int *sample_flag){
	int rc;
	int sample_length = DEFAULT_SAMPLE_LENGTH;
	int dir;
	int sample_rate = DEFAULT_SAMPLE_RATE;
	unsigned int buffer_time, period_time;
	int size;
	char *buffer;
	int loops;
	int fd;
	int recv_count = 0;
	snd_pcm_t *handle;
	snd_pcm_hw_params_t *params;
	snd_pcm_format_t snd_format;
	snd_pcm_uframes_t frames;

	rc = snd_pcm_open(&handle, "default",
		SND_PCM_STREAM_CAPTURE, 0);
	if(rc < 0){
		fprintf(stderr, "Unable to open pcm device: %s/n", snd_strerror(rc));
		exit(1);
	}

	/* Allocate a hardware parameters object. */
	snd_pcm_hw_params_alloca(&params);

	/* Fill it in with default values. */
	snd_pcm_hw_params_any(handle, params);

	/* Interleaved mode */
	snd_pcm_hw_params_set_access(handle, params,
		SND_PCM_ACCESS_RW_INTERLEAVED);

	switch(sample_length){
	case 16:  
       	snd_format = SND_PCM_FORMAT_S16_LE;  
       	break;  
    case 8:  
        snd_format = SND_PCM_FORMAT_U8;  
       	break;  
    default:  
        snd_format = SND_PCM_FORMAT_UNKNOWN;  
       	break;   
	}

	/*Set sample length little-endian format */
	snd_pcm_hw_params_set_format(handle, params,
		snd_format);
	/*Set channels  */
	snd_pcm_hw_params_set_channels(handle, params, DEFAULT_CHANNELS);

	/*Set sample rate */
	snd_pcm_hw_params_set_rate_near(handle, params, &sample_rate, &dir);

    /* Set period size to 32 frames. */
	frames = 32;
	snd_pcm_hw_params_set_period_size_near(handle, params, &frames, &dir);

    rc = snd_pcm_hw_params(handle, params);
	if (rc < 0) {
		fprintf(stderr, "unable to set hw parameters: %s/n", snd_strerror(rc));
		exit(1);
	}

	/* Use a buffer large enough to hold one period */
	snd_pcm_hw_params_get_period_size(params, &frames, &dir);
	size = frames * DEFAULT_CHANNELS * DEFAULT_SAMPLE_LENGTH / 8; 
	buffer = (char *) malloc(size);

	/* We want to loop for 5 seconds */
	snd_pcm_hw_params_get_period_time(params, &sample_rate, &dir);
	loops = 500000 / sample_rate;
	if ((fd = open("./sample_test.pcm", O_WRONLY | O_CREAT | O_TRUNC, 0644)) == -1) {  
        fprintf(stderr, "Error open: [./sample.pcm]/n");  
        return -1;  
    } 
    int file_size = 0;
    while (1) {
		
		int flag = *sample_flag;
		if(flag == -1){

	    	lseek(fd, 0, SEEK_SET);
	    }
		
		if(flag == 0){
			rc = snd_pcm_readi(handle, buffer, frames);
			if (rc == -EPIPE) {
				/* EPIPE means overrun */
				fprintf(stderr, "overrun occurred/n");
				snd_pcm_prepare(handle);
			} else if (rc < 0) {
				fprintf(stderr, "error from read: %s/n", snd_strerror(rc));
			} else if (rc != (int)frames) {
				fprintf(stderr, "short read, read %d frames/n", rc);
			}

			/*Write the pcm to sample file*/
	   	 	if (write(fd, buffer, size) != size) {
	   	 		printf("write :%d", size);
	       		fprintf(stderr, "Error SNDWAV_Record[write]\n");  
	       		exit(-1);  
	    	}

	    	recv_count += size;

	    	if(recv_count >= SAMPLE_FILE_SIZE){
	
	    		*sample_flag = 1;
	    		recv_count = 0;
	    	}
		}
	}

	snd_pcm_drain(handle);
	snd_pcm_close(handle);
	free(buffer);
	
}

