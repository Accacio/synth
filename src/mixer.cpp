
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
    SDL_PauseAudio(0);
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
        stream[i]=0;
        if (!mixer->m_instruments.empty()){
            int instruments = mixer->m_instruments.size();
            for (int inst = 0; inst < instruments;inst++){
                stream[i] += (double) clip(mixer->m_instruments[inst]->gen_sound(mixer->getTime(),mixer->freq),1.0)*AMPLITUDE/instruments;
            }

        } else {
            stream[i]=0;
        }
        // std::cout << mixer->m_instruments[0]->gen_sound(mixer->getTime(),mixer->freq/FREQUENCY),1.0;
        // std:: cout << std::endl;
        // stream[i] = clip(mixer->userFunction(mixer->getTime(),mixer->freq/FREQUENCY),1.0)*AMPLITUDE;
        mixer->incTime();
    }
}

void Mixer::addInstrument(Instrument *instrument){
    m_instruments.push_back(instrument);
}
void Mixer::removeInstrument(Instrument *instrument){
    for (int i = 0; i<m_instruments.size();i++ ){
        if (m_instruments[i]==instrument)
            m_instruments.erase(m_instruments.begin()+i);
    }
}
void Mixer::removeAllInstruments(){
    m_instruments.clear();
}

Instrument* Mixer::getInstrumentByIndex(int index){
  return m_instruments[index];
}
