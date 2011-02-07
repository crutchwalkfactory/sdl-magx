//Writed by Ant-ON, 2011

#ifndef ALL_AUDIO
#define ALL_AUDIO

typedef struct
{  
    unsigned char d1;     // = 0
    unsigned int  d2;     // = 0x4080803
    unsigned int  d3;     // = 0
    unsigned int  chanel; // 0 - mono, 1 -stereo
    short int     vol;    // = 0-100
    short int     d4;     // = 0
    unsigned int  freq;   // 8000, 11025, 12000, 16000, 22050, 24000, 32000, 44100, 48000 Hz
    unsigned int  bps;    // 1 or 2
    unsigned int  d[42];  // = 0
} AAL_PARAM;

typedef struct
{
    void*   buffer;
    int     length; 
} AAL_BUF;

extern "C"
{
	int AAL_open( AAL_PARAM* ); //return id
	void AAL_close( int id, int=0 );
	int AAL_write( int id, AAL_BUF*, int* );//return writed byte. What for last argument?	

	void AAL_barge( int id );
	void AAL_sync( int id );

	int AAL_get_buffer_size(int id, unsigned int* size);

	int AAL_in_call_capture( int id );// return true, if in call capture

	//1-100
	#define AAL_ATRIBUTE_VOL 1
	//1-7
	#define AAL_ATRIBUTE_BAS 10
	void AAL_set_attribute(int id, int atrib, void* value);

	int AAL_get_bytes_unrendered(int id, unsigned int* count);

	//
	void AAL_is_device_available();
	void AAL_read();
	void AAL_refresh_mic_device();
	void AAL_set_attributes();
	void AAL_set_path_rule();
	void AAL_VRDuringRecord();
	void AAL_VRSetHWIGain();
};

extern "C"
{
	void audio_mixer_calculate_mode();
	void audio_mixer_close();
	void audio_mixer_ioctl();
	void audio_mixer_open();
};
#endif
