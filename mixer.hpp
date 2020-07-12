
#ifndef __MIXER_H_
#define __MIXER_H_
#include <SDL2/SDL_audio.h>
#include <math.h>
#include <iostream>
#include <vector>
#include "instrument.hpp"

class Mixer{
        public:
                Mixer(unsigned int = 44100, unsigned int = 1, SDL_AudioFormat= AUDIO_S16SYS, unsigned int = 4096);
                ~Mixer();
                double (*userFunction)(double,double);
                double freq;
                std::vector<Instrument*> m_instruments;
                void addInstrument(Instrument *instrument);
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
  
};


#endif // __MIXER_H_
