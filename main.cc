
#include <iostream>
#include <fcntl.h>
#include <cmath>
#include <time.h>
#include <unistd.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_main.h>
#include "mixer.hpp"
#include <poll.h>

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl2.h"
// #include "beaper.h"


#define  MIDI_DEVICE  "/dev/snd/midiC2D0"

enum midiinput {
NOTE_ON = 144,
NOTE_OFF = 128,
CONTROL_CODE = 176,
AFTERTOUCH = 208,
};



unsigned int sampleFrequency = 0;
unsigned int audioBufferSize = 0;
unsigned int outputAudioBufferSize = 0;

const int FREQUENCY = 44100;



double min(double a, double b) {
    // printf("%f %f",a,b);
    return (a>b)?b:a;
}

double max(double a, double b) {
    // printf("%f %f",a,b);
    return (a<b)?b:a;
}

double lerp(double a, double b, double i){
    double newi= max(0,min(1,i));
    // TODO verificar
    // printf("%f %f %f",i,newi,-a*(newi-1)+b*newi);
    return -a*(newi-1) + b*newi;
}
double ilerp(double a, double b, double i){
    return (i-a)/(b-a);
}

class ADSR_Envelope {
    public:
        ADSR_Envelope(double startAmp = 0, double attackTime = 0.5, double decayTime = 0.8, double sustainAmp = 0.8, double releaseTime = 1.3);
        double getAmp(double);
        ~ADSR_Envelope(){};
        void note_on(Uint32 time_on) {
            play_note=true;
            m_TimeOn = time_on;
            // std::cout<< " set time on";
        };

        void setA(double newA) {
            m_AttackTime = lerp(minAttackTime, maxAttackTime, newA);
        };
        double getA() {
            return ilerp(minAttackTime,maxAttackTime, m_AttackTime);
        };

        void setD(double newD) {
            m_DecayTime = lerp(minDecayTime,maxDecayTime,newD);
        };
        double getD() {
            return ilerp(minDecayTime,maxDecayTime, m_DecayTime);
        };

        void setS(double newS) {
            m_SustainAmp = lerp(minSustainAmp,maxSustainAmp,newS);
        };
        double getS() {
            return ilerp(minSustainAmp,maxSustainAmp,m_SustainAmp);
        };

        void setR(double newR) {
            m_ReleaseTime = lerp(minReleaseTime,maxReleaseTime,newR);
        };
        double getR() {
            return ilerp(minReleaseTime,maxReleaseTime, m_ReleaseTime);
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


        double minAttackTime=0.0;
        double minDecayTime=0.0;
        double minSustainAmp=0.0;
        double minReleaseTime=0.0;

        double maxAttackTime=2;
        double maxDecayTime=2;
        double maxSustainAmp=1;
        double maxReleaseTime=2;

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
    if (lifeTime<=m_AttackTime){
        amp = (lifeTime/m_AttackTime)*m_StartAmp;
    }
    else if (lifeTime>m_AttackTime && lifeTime<=(m_AttackTime+m_DecayTime)){
        amp = ((lifeTime-m_AttackTime)/m_DecayTime)*(m_SustainAmp-m_StartAmp)+m_StartAmp;
    }
    else if (lifeTime>(m_AttackTime+m_DecayTime)){
        amp = m_SustainAmp;
    }

    if (!play_note){
      // std::cout << "note on" << std::endl;
      // std::cout << "note off" << std::endl;
    // if (lifeTime<=m_AttackTime){
    //     amp = (lifeTime/m_AttackTime)*m_StartAmp;
    // }
    // if (lifeTime>m_AttackTime && lifeTime<=(m_AttackTime+m_DecayTime)){
    //     amp = ((lifeTime-m_AttackTime)/m_DecayTime)*(m_SustainAmp-m_StartAmp)+m_StartAmp;
    // }
    // if (lifeTime>(m_AttackTime+m_DecayTime)){
        amp = ((time-TimeOff)/m_ReleaseTime) * (0.0 - amp) + amp;
    // }
    }
    if (amp<0.001) amp = 0;
    return amp;
}

ADSR_Envelope::ADSR_Envelope(double startAmp,
                             double attackTime,
                             double decayTime,
                             double sustainAmp,
                             double releaseTime){
        m_StartAmp=startAmp;
        m_AttackTime=attackTime;
        m_DecayTime=decayTime;
        m_SustainAmp=sustainAmp;
        m_ReleaseTime=releaseTime;
}

ADSR_Envelope envelope;

// TODO verify fourier
double square(double time, double freq){

    double timbre =
        std::sin(time * 1 * freq * 2 * M_PI / FREQUENCY) +
        std::sin(time * 3 * freq * 2 * M_PI / FREQUENCY)/2 +
        std::sin(time * 5 * freq * 2 * M_PI / FREQUENCY)/3 +
        std::sin(time * 7 * freq * 2 * M_PI / FREQUENCY)/4 +
        std::sin(time * 9 * freq * 2 * M_PI / FREQUENCY)/5;
    return timbre / (1+1/2+1/3+1/4+1/5);
}
double sine(double time, double freq){

    double timbre =
        std::sin(time * 1 * freq * 2 * M_PI / FREQUENCY);
    return timbre;
}

double gen_sound(double time, double freq){

    // double timbre = square(time);
    double timbre = sine(time, freq);
    return envelope.getAmp(time)*timbre;
}

int main(int argc, char *argv[]) {
    double freq = 0;
    SDL_Window* window = NULL;
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);

    unsigned char inpacket[4];
    int readPacketOk;
    int seqfd = open(MIDI_DEVICE, O_RDONLY);
    // printf("Error: cannot open %d\n", seqfd);
    if (seqfd == -1) {
        printf("Error: cannot open %s\n", MIDI_DEVICE);
        // exit(1);
    }
	struct pollfd pfd = { seqfd, POLLIN, 0 };


    if( SDL_Init(SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_VIDEO) < 0 ) {
        std::cout << "Unable to init SDL: " << SDL_GetError() << std::endl;
        return 1;
    }
    // Mixer mixer = Mixer(22050,1,AUDIO_S16,512);
    Mixer mixer = Mixer();
    envelope = ADSR_Envelope();
    // envelope.getAmp(time(0));
    mixer.userFunction = gen_sound;

        window = SDL_CreateWindow("Mysynth", 10, 10, 800, 600, SDL_WINDOW_SHOWN);
        if (window == NULL) return 1;
        SDL_GL_MakeCurrent(window, gl_context);
        SDL_GL_SetSwapInterval(1); // Enable vsync

        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();


        // Setup Platform/Renderer bindings
        ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
        ImGui_ImplOpenGL2_Init();

        SDL_Surface *screensurface = SDL_GetWindowSurface(window);
        SDL_FillRect(screensurface, NULL, SDL_MapRGB(screensurface->format, 0x69,0x95,0xED));
        SDL_UpdateWindowSurface(window);
    // }
    bool running = true;
    SDL_Event event;

    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    float A;
    float D;
    float S;
    float R;
    while(running)
    {
        ImGui_ImplOpenGL2_NewFrame();
        ImGui_ImplSDL2_NewFrame(window);
        ImGui::NewFrame();
        A = envelope.getA();
        D = envelope.getD();
        S = envelope.getS();
        R = envelope.getR();
        ImGui::Begin("Control");
        ImGui::SliderFloat("Attack", &A, 0, 1);
        ImGui::SliderFloat("Decay", &D, 0, 1);
        ImGui::SliderFloat("Sustain", &S, 0, 1);
        ImGui::SliderFloat("Release", &R, 0, 1);
        ImGui::End();
        envelope.setA(A);
        envelope.setD(D);
        envelope.setS(S);
        envelope.setR(R);

        if (seqfd > 0) {
            int ret = poll(&pfd,1,5);
            struct pollfd pfd = { seqfd, POLLIN, 0 };
            if (ret > 0){

                readPacketOk = read(seqfd, &inpacket, 1);

                //    // print the MIDI byte if this input packet contains one
                if (inpacket[0] == NOTE_ON) {
                    readPacketOk = read(seqfd, &inpacket, 3);
                    printf("Note on: %d , Velocity: %d, Channel: %d",inpacket[0], inpacket[1],inpacket[2]);
                    envelope.note_on(mixer.getTime());
                    int diff = (inpacket[0]-69);
                    freq  =  pow(2,(float) diff/12)*440;
                    printf("freq: %d %f, %f \n",diff,freq,(double)pow(2,diff));
                    // printf("received MIDI byte: %d %d %d %d \n",inpacket[0],inpacket[1], inpacket[2],inpacket[3]);
                }
                if (inpacket[0] == NOTE_OFF) {
                    readPacketOk = read(seqfd, &inpacket, 3);
                    printf("Note off: %d , Velocity: %d, Channel: %d\n",inpacket[0], inpacket[1],inpacket[2]);
                    envelope.note_off(mixer.getTime());
                    // printf("received MIDI byte: %d %d %d %d \n",inpacket[0],inpacket[1], inpacket[2],inpacket[3]);
                }
                if (inpacket[0] == CONTROL_CODE) {
                    readPacketOk = read(seqfd, &inpacket, 3);
                    printf("Code: %d , Angle: %f, Channel: %d\n",inpacket[0],(double) inpacket[1]/127*360,inpacket[2]);
                    // envelope.setD((double) inpacket[1]/127);
                    if (inpacket[0]==20) envelope.setA((double) inpacket[1]/127);
                    if (inpacket[0]==21) envelope.setD((double) inpacket[1]/127);
                    if (inpacket[0]==22) envelope.setS((double) inpacket[1]/127);
                    if (inpacket[0]==23) envelope.setR((double) inpacket[1]/127);
                    if (inpacket[0]==118) running=false;
                }
                if (inpacket[0] == AFTERTOUCH) {
                    readPacketOk = read(seqfd, &inpacket, 1);
                    printf("Aftertouch: %d \n",inpacket[0]);
                }
            }
        }






        while(SDL_PollEvent(&event)) {

            // ImGui_ImplSDL2_ProcessEvent(&event);
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
        mixer.freq = freq;

        ImGui::Render();
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        //glUseProgram(0); // You may want this if using this code in an OpenGL 3+ context where shaders may be bound
        ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }

    ImGui_ImplOpenGL2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();
   return 0;
}
