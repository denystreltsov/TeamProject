#include <sndfile.h>
#include <portaudio.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>


#define CHANNEL_COUNT 2
#define SAMPLE_RATE 44100
#define BUFFER_SIZE 128
#define PA_ERROR_CHECK(pa) if(pa != paNoError) {printf("%s\n",Pa_GetErrorText(pa)); return 1;}
#define SND_ERROR_CHECK(snd) if(snd == NULL) {printf("%s\n",sf_strerror(snd)); return 1;}
 

typedef float SAMPLE_TYPE;
typedef struct AudioManager AudioManager;
typedef enum {
    PLAY = 0,
    PAUSE = 1
} STATE_AUDIO;

int AudioInit();
int AudioExit();

void LoadAudio(const char*);
int LoadTwoAudio(const char*, const char*);
int PlayAudio();
void StopAudio();
void TerminateAudio();


const char* AudioHostApiGet();

int CallBack(
    const void *input, void *output,
    unsigned long bufferSize,
    const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags,
    void *userData );



struct AudioManager
{

    float* buffer;
    PaStream* mainStream;
    PaStreamParameters outputStream;
    PaError errorCheck;
    STATE_AUDIO stateAudio;
    size_t bufferCountLeft;

};

static AudioManager audioManager;




int AudioInit()
{
    audioManager.errorCheck = Pa_Initialize();
    PA_ERROR_CHECK(audioManager.errorCheck)
    return 0;
}

int AudioExit()
{
    free(audioManager.buffer);
    audioManager.errorCheck = Pa_Terminate();
    PA_ERROR_CHECK(audioManager.errorCheck)
    return 0;
}



void LoadAudio(const char*)
{
    SF_INFO sfInfo1;
    sfInfo1.format = 0;

    SNDFILE* sndFile1 = sf_open(path1,SFM_READ, &sfInfo1);
    SND_ERROR_CHECK(sndFile1)

    size_t firstDuration = sfInfo1.frames * sfInfo1.channels;

    size_t bufferSize = firstDuration;

    audioManager.bufferCountLeft = bufferSize / CHANNEL_COUNT;

    audioManager.buffer = (float*) malloc(bufferSize * sizeof(float));

    sf_read_float(sndFile1,audioManager.buffer, firstDuration);
    SND_ERROR_CHECK(sndFile1)


    int err1 = sf_close(sndFile1);
    if(err1 != 0 )
    {
        printf("Error with close file:%d", err1);
    }

    return 0;
}




int LoadTwoAudio(const char* path1, const char* path2)
{
    SF_INFO sfInfo1, sfInfo2;
    sfInfo1.format = 0;
    sfInfo2.format = 0;

    SNDFILE* sndFile1 = sf_open(path1,SFM_READ, &sfInfo1);
    SND_ERROR_CHECK(sndFile1)

    SNDFILE* sndFile2 = sf_open(path2,SFM_READ, &sfInfo2);
    SND_ERROR_CHECK(sndFile2)


    size_t firstDuration = sfInfo1.frames * sfInfo1.channels;
    size_t secondDuration = sfInfo2.frames * sfInfo2.channels;

    size_t bufferSize = firstDuration + secondDuration;

    audioManager.bufferCountLeft = bufferSize / CHANNEL_COUNT;

    audioManager.buffer = (float*) malloc(bufferSize * sizeof(float));

    sf_read_float(sndFile1,audioManager.buffer, firstDuration);
    SND_ERROR_CHECK(sndFile1)
    sf_read_float(sndFile2, audioManager.buffer + firstDuration + 1, secondDuration);
    SND_ERROR_CHECK(sndFile2)


    int err1 = sf_close(sndFile1);
    if(err1 != 0 )
    {
        printf("Error with close file:%d", err1);
    }

    int err2 = sf_close(sndFile2);
    if(err2 != 0 )
    {
        printf("Error with close file:%d", err2);
    }

    return 0;

}

int PlayAudio()
{
    audioManager.mainStream = NULL;
    
    audioManager.outputStream.device = Pa_GetDefaultOutputDevice();
    audioManager.outputStream.channelCount = CHANNEL_COUNT;
    audioManager.outputStream.sampleFormat = paFloat32;
    audioManager.outputStream.suggestedLatency = Pa_GetDeviceInfo(Pa_GetDefaultOutputDevice())->defaultLowOutputLatency;
    audioManager.outputStream.hostApiSpecificStreamInfo = NULL;

    audioManager.errorCheck = Pa_OpenStream(&audioManager.mainStream,NULL,&audioManager.outputStream, SAMPLE_RATE, BUFFER_SIZE, paNoFlag, CallBack, NULL);
    PA_ERROR_CHECK(audioManager.errorCheck)


    audioManager.errorCheck = Pa_StartStream(audioManager.mainStream);
    PA_ERROR_CHECK(audioManager.errorCheck)
    audioManager.stateAudio = PLAY;

    while(Pa_IsStreamActive(audioManager.mainStream))
        continue;

    audioManager.errorCheck = Pa_CloseStream(audioManager.mainStream);
    PA_ERROR_CHECK(audioManager.errorCheck)


    return 0;

}    






const char* AudioHostApiGet()
{
    return Pa_GetHostApiInfo(Pa_GetDefaultHostApi())->name;
}






int main(int argc, char const *argv[])
{
    AudioInit();

    printf("%s\n",AudioHostApiGet());

    LoadTwoAudio("/home/deni/Desktop/music3.flac","/home/deni/Desktop/music3.flac");

    PlayAudio();

    AudioExit();

    return 0;
}



int CallBack(
    const void *input, void *output,
    unsigned long bufferSize,
    const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags,
    void *userData )
    {
        SAMPLE_TYPE* outBuff = (SAMPLE_TYPE*) output;

        unsigned long sizeToFill = (audioManager.bufferCountLeft <= bufferSize) ? 
            audioManager.bufferCountLeft : bufferSize;


        for(int i = 0; i < sizeToFill; ++i)
        {
            *outBuff++ =  *audioManager.buffer++;
            *outBuff++ =  *audioManager.buffer++;
        }

        audioManager.bufferCountLeft -= bufferSize;
        if(audioManager.bufferCountLeft <= 0)
        {
            return paComplete;
        }

        else if(audioManager.stateAudio == PLAY)
            return paContinue;
        
        else if(audioManager.stateAudio == PAUSE)
            return paComplete;
        

        return 0;
    }


