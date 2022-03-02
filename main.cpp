#include <JuceHeader.h>
#include <iostream>
#include <memory>

using namespace juce;

class AudioApp : public JUCEApplication
{

private:
    AudioDeviceManager dm;
    MixerAudioSource source;
    AudioSourcePlayer player;
    AudioFormatManager fm;

    

public:
    AudioApp()
    {
        //DeviceManager routine
        dm.initialiseWithDefaultDevices(0, 1);
        dm.addAudioCallback(&player);


        File* file1 =new File(File::getSpecialLocation(File::userDesktopDirectory).getChildFile("music1.mp3"));
        File* file2 =new  File(File::getSpecialLocation(File::userDesktopDirectory).getChildFile("music2.mp3"));


        //FormatManager routine
        fm.registerFormat(new MP3AudioFormat(), true);


        //AudioFormatReaderSource routine
        AudioFormatReaderSource* source1 = new AudioFormatReaderSource (fm.createReaderFor(*file1), true);
        AudioFormatReaderSource* source2 = new AudioFormatReaderSource (fm.createReaderFor(*file2), true);


        //MixerAudioSource routine
        source.addInputSource(source1, true);
        source.addInputSource(source2, true);


        //Player Sound
        player.setSource(&source);


    }

    ~AudioApp()
    {
        dm.removeAudioCallback(&player);
        source.releaseResources();
    }





    const String getApplicationName() override { return "Anything"; }


    const String getApplicationVersion() override { return "1.0"; }


    
    void initialise(const String& commandLineParameters) override {}

    void shutdown() override {}




};

START_JUCE_APPLICATION(AudioApp)
