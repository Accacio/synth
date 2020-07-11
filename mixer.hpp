
#ifndef __MIXER_H_
#define __MIXER_H_
#include <SDL2/SDL_audio.h>
#include <math.h>
#include <iostream>




class Mixer{
    public:
        Mixer(unsigned int = 44100, unsigned int = 1, SDL_AudioFormat= AUDIO_S16SYS, unsigned int = 4096);
        ~Mixer();
        double (*userFunction)(double,double);
        double getTime(){return v;}
        void setTime(double time){v=time;}
        double freq;
        static const int AMPLITUDE = 28000;
        static const int FREQUENCY = 44100;
        static double clip(double Amp, double AmpMax);
        static void fillStream(void *_mixer, Uint8 *_stream, int _len);
    private:
        double v;
  
};


#endif // __MIXER_H_
