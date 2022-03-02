#define _USE_MATH_DEFINES
#include <memory>
#include <iostream>
#include <fstream>
#include <cmath>
#include <string>
#define conv(N) static_cast<short>(N)

//Global-static Variables
static constexpr int SampleRate = 44100;
static constexpr int BitDepth = 32;
static const int MaxAmplitude = std::pow(2,BitDepth - 1) - 1;

/**********************************************************************
 Generator of Sine wave; A * sin(2Pi * Freq/SampleRate) + Phase       *
 **********************************************************************/
class SineOscillator
{
public:
  SineOscillator(float amp, float freq) : amplitude{amp}, frequency{freq},
   offset{static_cast<float>(2 * M_PI * frequency/SampleRate)}, angle{0}  {}

   /*********************************
    Main function in SineOscillator *
    *********************************/
   float process()
   {
     auto sample = amplitude * std::sin(angle);
     angle +=offset;
     return sample;
   }


private:
  float amplitude, frequency, angle;
  const float offset;

};


void writeToFile(std::ofstream& stream ,int value, size_t size = 2)
{
  stream.write(reinterpret_cast<const char*>(&value), size);

}



int main(int argc, char const *argv[])
{
  std::cout << "Name of file: ";
  std::string name {};

  std::getline(std::cin,name);
  std::ofstream wavFile;

  wavFile.open(name + ".wav", std::ios::binary | std::ios::out);

/*********************
  Header of WAV file *
**********************/
  wavFile << "RIFF" ;  //RIFF structure
  wavFile << "----" ; //Size of all wav file
  wavFile << "WAVE" ; //RIFF type

/****************
  Header of FMT *
*****************/
  wavFile << "fmt " ;
  writeToFile(wavFile, 16, 4);
  writeToFile(wavFile, conv(1));
  writeToFile(wavFile, conv(1));
  writeToFile(wavFile, SampleRate, 4);
  writeToFile(wavFile, SampleRate * BitDepth / 8, 4);
  writeToFile(wavFile, conv(BitDepth / 8));
  writeToFile(wavFile, conv(BitDepth));


/*****************
  Header of Data *
******************/
  wavFile << "data";
  wavFile << "----";


  long long prePosition = wavFile.tellp();

  //20 - 20000

  std::cout << "Amplitude (-1.0;+1.0): ";
  std::string amp {};

  std::getline(std::cin,amp);
  std::cout << "Frequency : ";
  std::string freq {};

  std::getline(std::cin,freq);

  SineOscillator sineOscillator {std::stof(amp), std::stof(freq)};


  //Duration of wave
  const int duration = 60;




  //Sampling Process
  for(int i = 0; i < SampleRate * duration; ++i)
  {
    float sample = sineOscillator.process();

    //Quantization
    int sampleInt = sample * MaxAmplitude;

    wavFile.write(reinterpret_cast<const char *>(&sampleInt), sizeof(sampleInt));
  }

  long long postPosition = wavFile.tellp();


  /*
    Filling the size of chunks
  */

  wavFile.seekp(4,std::ios::beg);
  writeToFile(wavFile, postPosition - 8, 4);
  wavFile.seekp(prePosition - 4);
  writeToFile(wavFile, postPosition - prePosition, 4);


  wavFile.close();


  return 0;
}
