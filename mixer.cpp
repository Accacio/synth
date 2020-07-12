#include "mixer.hpp"



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


double Mixer::clip(double Amp, double AmpMax){
    return fmax(fmin(Amp, AmpMax),-AmpMax);
}

int Mixer::channels = 0;

void Mixer::fillStream(void *_mixer, Uint8 *_stream, int _len) {
    Mixer* mixer = (Mixer*) _mixer;
    Sint16 *stream = (Sint16*) _stream;
    for (int i = 0; i < _len/2; i++) {
        stream[i] = clip(mixer->userFunction(mixer->getTime(),mixer->freq),1.0)*AMPLITUDE;
        mixer->incTime();
    }
}

void Mixer::addInstrument(Instrument *instrument){
    m_instruments.push_back(instrument);
}
