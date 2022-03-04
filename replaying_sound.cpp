#include <iostream>
#include <portaudio.h>
#include <cstdlib>

typedef int error_number;
typedef int device_index;
typedef char inputDevice_index;
typedef char outputDevice_index;

void error(error_number i)
{
    Pa_Terminate();
    std::cerr << "You fucked it up" << std::endl;
    std::cerr << Pa_GetErrorText(i) << std::endl;
    exit(EXIT_FAILURE);
}

void info_device(device_index i)
{
    const PaDeviceInfo* device = Pa_GetDeviceInfo(i);

    printf("The Name of %dth Device: %s\n",i, device->name);
    printf("The Default Sample Rate:%f\n", device->defaultSampleRate);
    printf("The High Input Latency:%f\n", device->defaultHighInputLatency);
    printf("The High Output Latency:%f\n", device->defaultHighOutputLatency);
    printf("The Low Input Latency:%f\n", device->defaultLowInputLatency);
    printf("The Low Output Latency:%f\n", device->defaultLowOutputLatency);
    printf("The Max Input Channels:%d\n", device->maxInputChannels);
    printf("The Max Output Channels:%d\n", device->maxOutputChannels);
    std::cout << "----------------------------------------------------------------" << std::endl;
    std::cout << "----------------------------------------------------------------" << std::endl;

}

int myOwnAudioCallBack(

        const void *input, void *output, unsigned long frameCount,
        const PaStreamCallbackTimeInfo *timeInfo,
        PaStreamCallbackFlags statusFlags, void *userData
        )
    {
        float* inp = (float*) input;
        float* oup = (float*) output;

        if(input != NULL) {
            for (int i = 0; i < frameCount; ++i) {

                //Too quiet
                if(*inp <0.1 )
                {
                    *oup++ = *inp;
                    *oup++ = *inp++;
                }

                *oup++ = *inp;
                *oup++ = *inp++;
            }
        }
        else{
            for(int i = 0 ; i < frameCount; ++i)
            {
                *oup++ = 0;
                *oup++ = 0;
            }
        }

        return paContinue;
    }




int main(int argc, char** argv)
{
    auto err = Pa_Initialize();

    if(err !=paNoError)
    {
        error(err);
    }
    auto number_devices = Pa_GetDeviceCount();

    if(number_devices == paNoDevice)
    {
        error(number_devices);
    }
    auto hostApi = Pa_GetHostApiInfo(Pa_GetDefaultHostApi());
    std::cout << "\nAPI Used: " << hostApi->name << std::endl;

    std::cout << "Default Input Device: " << Pa_GetDeviceInfo(hostApi->defaultInputDevice)->name << std::endl;
    std::cout << "Default Output Device: " << Pa_GetDeviceInfo(hostApi->defaultOutputDevice)->name << std::endl;
    std::cout << "----------------------------------------------------------------" << std::endl;
    std::cout << "----------------------------------------------------------------" << std::endl;
    std::cout << "\n------------------------The Device Info!------------------------\n" << std::endl;

    for(int i = 0; i < number_devices; ++i)
    {
        info_device(i);
    }

    std::cout << "\n";
    std::cout << "\n------------------------Choice Device!------------------------\n" << std::endl;

    for(int i = 0 ; i < number_devices; ++i)
    {
        printf("write down [%d] to choose: [%s] and InputChannel: %d and OutputChannel: %d\n",i,Pa_GetDeviceInfo(i)->name,
                Pa_GetDeviceInfo(i)->maxInputChannels,Pa_GetDeviceInfo(i)->maxOutputChannels );
    }

    inputDevice_index ch1[2];
    outputDevice_index ch2[2];

    std::cout << "Input Device:";
    std::cin.getline(ch1,2);

    std::cout << "Output Device:";
    std::cin.getline(ch2,2);

    PaStreamParameters inputStream, outputStream;


    const PaDeviceInfo* inputDevice = Pa_GetDeviceInfo(atoi(&ch1[0]));
    inputStream.device = atoi(&ch1[0]);
    inputStream.channelCount = inputDevice->maxInputChannels;
    inputStream.sampleFormat = paFloat32;
    inputStream.hostApiSpecificStreamInfo = NULL;

    const PaDeviceInfo* outputDevice = Pa_GetDeviceInfo(atoi(&ch2[0]));
    outputStream.device = atoi(&ch2[0]);
    outputStream.channelCount = outputDevice->maxOutputChannels;
    outputStream.sampleFormat = paFloat32;
    outputStream.hostApiSpecificStreamInfo = NULL;

    PaStream* stream;

    err = Pa_OpenStream(&stream, &inputStream, &outputStream , 44100, 128 , paNoFlag, myOwnAudioCallBack, NULL);

    if(err != paNoError)
    {
        error(err);
    }

    err = Pa_StartStream(stream);
    if(err != paNoError)
    {
        error(err);
    }


    std::cout << "\n\nPress Enter to exit:";
    std::cin.get();

    err = Pa_StopStream(stream);
    if(err != paNoError)
    {
        error(err);
    }


    Pa_Terminate();

    return 0;
}