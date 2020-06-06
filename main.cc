#include <iostream>
#include <cmath>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_main.h>
#include "mixer.hpp"
// #include "beaper.h"

unsigned int sampleFrequency = 0;
unsigned int audioBufferSize = 0;
unsigned int outputAudioBufferSize = 0;

const int AMPLITUDE = 28000;
const int FREQUENCY = 44100;

double freq = 0;


class ADSR_Envelope {
    public:
        ADSR_Envelope(double startAmp = 1, double attackTime = 0.5, double decayTime = 0.8, double sustainAmp = 0.8, double releaseTime = 1.3);
        double getAmp(double);
        ~ADSR_Envelope(){};
        void note_on(Uint32 time_on) {
            play_note=true;
            m_TimeOn = time_on;
            // std::cout<< " set time on";
        };
        void note_off(Uint32 time_off) {
            play_note=false;
            m_TimeOff = time_off;
            std::cout<< " set time off";
        };
    private:
        bool play_note=false;
        double m_StartAmp;
        double m_AttackTime;
        double m_DecayTime;
        double m_SustainAmp;
        double m_ReleaseTime;
        Uint32 m_TimeOn=0;
        Uint32 m_TimeOff=0;
};

double ADSR_Envelope::getAmp(double _time){
    double lifeTime = (_time - m_TimeOn)/FREQUENCY;
    double time = _time/FREQUENCY;
    double TimeOff = m_TimeOff/FREQUENCY;
    double amp=m_StartAmp;
    // std::cout << "time " << _time ;
    // std::cout << " timeon " << m_TimeOn ;
    // std::cout << " lifetime " << lifeTime ;
    // std::cout << " play " << play_note;
    if (play_note){
      // std::cout << "note on" << std::endl;
    if (lifeTime<=m_AttackTime){
        amp = (lifeTime/m_AttackTime)*m_StartAmp;
    }
    if (lifeTime>m_AttackTime && lifeTime<=(m_AttackTime+m_DecayTime)){
        amp = ((lifeTime-m_AttackTime)/m_DecayTime)*(m_SustainAmp-m_StartAmp)+m_StartAmp;
    }
    if (lifeTime>(m_AttackTime+m_DecayTime)){
        amp = m_SustainAmp;
    }
    }
    else {
      // std::cout << "note off" << std::endl;
    // if (lifeTime<=m_AttackTime){
    //     amp = (lifeTime/m_AttackTime)*m_StartAmp;
    // }
    // if (lifeTime>m_AttackTime && lifeTime<=(m_AttackTime+m_DecayTime)){
    //     amp = ((lifeTime-m_AttackTime)/m_DecayTime)*(m_SustainAmp-m_StartAmp)+m_StartAmp;
    // }
    if (lifeTime>(m_AttackTime+m_DecayTime)){
        amp = ((time-TimeOff)/m_ReleaseTime) * (0.0 - m_SustainAmp) + m_SustainAmp;
    }
    }
    if (amp<0.001) amp = 0;
    return amp;
}

ADSR_Envelope::ADSR_Envelope(double startAmp, double attackTime, double decayTime, double sustainAmp, double releaseTime){
        m_StartAmp=startAmp;
        m_AttackTime=attackTime;
        m_DecayTime=decayTime;
        m_SustainAmp=sustainAmp;
        m_ReleaseTime=releaseTime;
}

ADSR_Envelope envelope;

double gen_sound(double time){
    return envelope.getAmp(time)*AMPLITUDE * std::sin(time * freq*2 * M_PI / FREQUENCY);
}

int main(int argc, char *argv[]) {
    SDL_Window* window = NULL;

    if( SDL_Init(SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_VIDEO) < 0 ) {
        std::cout << "Unable to init SDL: " << SDL_GetError() << std::endl;
        return 1;
    }
    // Mixer mixer = Mixer(22050,1,AUDIO_S16,512);
    Mixer mixer = Mixer();
    envelope = ADSR_Envelope();
    // envelope.getAmp(time(0));
    mixer.userFunction = gen_sound;

    window = SDL_CreateWindow("SDL audio test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_SHOWN);
    if (window == NULL) return 1;
    SDL_Surface *screensurface = SDL_GetWindowSurface(window);
    SDL_FillRect(screensurface, NULL, SDL_MapRGB(screensurface->format, 0xFF,0xFF,0xFF));
    SDL_UpdateWindowSurface(window);
    bool running = true;
    SDL_Event event;
    while(running)
    {
        while(SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_KEYDOWN:
                    if (event.key.keysym.sym && ! event.key.repeat){
                            envelope.note_on(mixer.getTime());
                    switch (event.key.keysym.sym ) {
                        case SDLK_ESCAPE: running=false; break;
                        case SDLK_w: freq=261.63; break;
                        case SDLK_s: freq=277.18; break;
                        case SDLK_x: freq=293.66; break;
                        case SDLK_d: freq=311.13; break;
                        case SDLK_c: freq=329.63; break;
                        case SDLK_v: freq=349.23; break;
                        case SDLK_g: freq=369.99; break;
                        case SDLK_b: freq=392.00; break;
                        case SDLK_h: freq=415.30; break;
                        case SDLK_n: freq=440.00; break;
                        case SDLK_j: freq=466.16; break;
                        case SDLK_COMMA: freq=493.88; break;
                        case SDLK_SEMICOLON: freq=523.25; break;
                        default: break;
                    }
                    }
                    break;
                case SDL_KEYUP:
                    envelope.note_off(mixer.getTime());
                    break;
                case SDL_QUIT:
                    running=false;
                    break;
            }
        }
    }



    SDL_DestroyWindow(window);
    SDL_Quit();
   return 0;
}
