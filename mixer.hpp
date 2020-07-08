#ifndef __MIXER_H_
#define __MIXER_H_
#include <SDL2/SDL_audio.h>

class Mixer{
    public:
        Mixer(unsigned int = 44100, unsigned int = 1, SDL_AudioFormat= AUDIO_S16SYS, unsigned int = 4096);
        ~Mixer();
        double (*userFunction)(double);
        double getTime(){return v;}
        void setTime(double time){v=time;}
        static void fillStream(void *_mixer, Uint8 *_stream, int _len);
    private:
        double v;
  
};


Mixer::Mixer(unsigned int nSampleRate , unsigned int nChannels , SDL_AudioFormat nBlocks , unsigned int nSamples )
{
    SDL_AudioSpec desired,obtained;
    desired.freq = nSampleRate;
    desired.format = AUDIO_S16SYS;
    desired.samples = nSamples;
    desired.callback = fillStream;
    desired.channels = nChannels;
    desired.userdata = this;

    if(SDL_OpenAudio(&desired, &obtained)<0){
        fprintf(stderr, "AudioMixer, Unable to open audio: %s\n", SDL_GetError());
        exit(1);
    }
    SDL_PauseAudio(0);

}
Mixer::~Mixer(){
   SDL_CloseAudio();
};

void Mixer::fillStream(void *_mixer, Uint8 *_stream, int _len) {
    Mixer* mixer = (Mixer*) _mixer;
    Sint16 *stream = (Sint16*) _stream;
    for (int i = 0; i < _len/2; i++) {
        stream[i] = mixer->userFunction(mixer->getTime());
        mixer->setTime(mixer->getTime()+1);
    }
}

#endif // __MIXER_H_
