
#ifndef __MIXER_H_
#define __MIXER_H_
#include <SDL2/SDL_audio.h>
#include <math.h>
#include <iostream>
#include <vector>
#include "instrument.hpp"

class Mixer{
        public:
                Mixer(unsigned int = 44100, unsigned int = 1, SDL_AudioFormat= AUDIO_S16SYS, unsigned int = 512);
                ~Mixer();
                double (*userFunction)(double,double);
                double freq;
                void addInstrument(Instrument *instrument);
                void removeInstrument(Instrument *instrument);
                void removeAllInstruments();
                Instrument* getInstrumentByIndex(int index);
                static const int AMPLITUDE = 28000;
                static const int FREQUENCY = 44100;
                double getTime(){return v/FREQUENCY;}
                void incTime(){v+=1;}
                void setTime(double time){v=time;}
                static int channels;
                static double clip(double Amp, double AmpMax);
                static void fillStream(void *_mixer, Uint8 *_stream, int _len);
        private:
                double v;
                std::vector<Instrument*> m_instruments;
  
};


#endif // __MIXER_H_
