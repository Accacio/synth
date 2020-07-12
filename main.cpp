
#include <iostream>
#include <fcntl.h>
#include <cmath>
#include <time.h>
#include <unistd.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_main.h>
#include "mixer.hpp"
#include "instrument.hpp"
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

// TODO verify fourier
double square(double time, double freq){
    double timbre = std::sin(time * 1 * freq * 2 * M_PI )>0? 1: -1;
    return timbre*0.8;
}

double saw(double time, double freq){

    double timbre =
       (2.0/M_PI)*(freq*M_PI* fmod(time, 1/freq))- M_PI/2;
    return timbre;
}
double sine(double time, double freq){

    double timbre =
        std::sin(time * 1 * freq * 2 * M_PI );
    return timbre;
}

// ADSR_Envelope envelope;
// double gen_sound(double time, double freq){

//     // double timbre = square(time);
//     double timbre = sine(time, freq);
//     return envelope.getAmp(time)*timbre;
// }

double noise(double time, double freq){

    double timbre =
        2*((double) rand()/ (double) RAND_MAX)-1;
    return timbre;
}

class Bell : public Instrument {
    public:
        Bell() {
            m_Volume = 1.0;
        }

        double gen_sound(double time, double freq){
            // double timbre = sine(time, freq);
            double timbre = 0;
            double sound = 0;
            double env = 0;
            bool active ;

            // std::cout << m_notes.size() << " " << std::endl;
            if(!m_notes.empty()){
                for (int i = 0; i < m_notes.size();i++){
                    active = true;
                    env = m_envelope.getAmp(time, m_notes[i].timeOn, m_notes[i].timeOff,m_notes[i].active);
                    // if(i==0) std::cout << env << std::endl;
                    sound =(double)  sine(time-m_notes[i].timeOn, scale(m_notes[i].id))/m_notes.size();
                    timbre += sound*env;
                    // if (!active && (env < 0.1)) m_notes[i].active = false;
                    // std::cout << i <<" note:" << m_notes[i].id << " timeOn: " << m_notes[i].timeOn  << " timeOff: "<< m_notes[i].timeOff << " active: " << active << std::endl;
                }

                std::vector<Note>::iterator n = m_notes.begin();
                while (n != m_notes.end())
                    if (!n->active)
                        n = m_notes.erase(n);
                    else
                        ++n;

                // for (int i = m_notes.size()-1; i > 0;i--){
                //     if (!m_notes[i].active) m_notes.erase(m_notes.begin()+i-1);
                // }
                // if (!m_notes[0].active) m_notes.erase(m_notes.begin());
            }

                // +saw(time, 0.5*freq)
                // +sine(time, pow(2,(float) 3/12)*freq)
                // +sine(time, pow(2,(float) 7/12)*freq)
                // +sine(time, pow(2,(float) 7/12)*freq);
                ;
            return m_Volume* timbre;
        }
};
class Saw: public Instrument {
    public:
        Saw() {
            m_Volume = 1.0;
        }

        double gen_sound(double time, double freq){
            // double timbre = sine(time, freq);
            bool active = true;
            double timbre = saw(time, 2*freq)
                // +saw(time, 0.5*freq)
                // +sine(time, pow(2,(float) 3/12)*freq)
                // +sine(time, pow(2,(float) 7/12)*freq)
                // +sine(time, pow(2,(float) 7/12)*freq);
                ;
            return m_Volume*m_envelope.getAmp(time,0,0,active) * timbre;
        }
};


int main(int argc, char *argv[]) {
    double freq = 0;
    int noteId = 0;
    Instrument *bell = new Bell();
    Instrument *saw= new Saw();

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
    // envelope = ADSR_Envelope();
    // envelope.getAmp(time(0));
    mixer.addInstrument(bell);
    mixer.addInstrument(saw);
    // mixer.userFunction = gen_sound;

    // if (seqfd ==-1) {
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
        A = bell->m_envelope.getA();
        D = bell->m_envelope.getD();
        S = bell->m_envelope.getS();
        R = bell->m_envelope.getR();
        ImGui::Begin("Control");
        ImGui::SliderFloat("Attack", &A, 0, 1);
        ImGui::SliderFloat("Decay", &D, 0, 1);
        ImGui::SliderFloat("Sustain", &S, 0, 1);
        ImGui::SliderFloat("Release", &R, 0, 1);
        ImGui::End();
        bell->m_envelope.setA(A);
        bell->m_envelope.setD(D);
        bell->m_envelope.setS(S);
        bell->m_envelope.setR(R);

        if (seqfd > 0) {
            int ret = poll(&pfd,1,0);

            if (ret > 0){
            std::cout << "events " << ret << std::endl;
                readPacketOk = read(seqfd, &inpacket, 1);

                //    // print the MIDI byte if this input packet contains one
                if (inpacket[0] == NOTE_ON) {
                    Note n;
                    readPacketOk = read(seqfd, &inpacket, 3);
                    // printf("Note on: %d , Velocity: %d, Channel: %d",inpacket[0], inpacket[1],inpacket[2]);
                    // envelope.note_on(mixer.getTime());
                    // bell->m_envelope.note_on(mixer.getTime());
                    n.note_on(mixer.getTime());
                    // n.timeOff = mixer.getTime()+2;
                    // saw->m_envelope.note_on(mixer.getTime());
                    n.id = inpacket[0];
                    bell->m_notes.push_back(n);
                    saw->m_notes.push_back(n);
                    // mixer.freq  =  bell->scale(inpacket[0]);
                    // printf("freq: %d %f, %f \n",diff,freq,(double)pow(2,diff));
                    // printf("received MIDI byte: %d %d %d %d \n",inpacket[0],inpacket[1], inpacket[2],inpacket[3]);
                }
                if (inpacket[0] == NOTE_OFF) {
                    Note n;
                    readPacketOk = read(seqfd, &inpacket, 3);
                    // printf("Note off: %d , Velocity: %d, Channel: %d\n",inpacket[0], inpacket[1],inpacket[2]);
                    // envelope.note_off(mixer.getTime());
                    n.id = inpacket[0];
                    if(!bell->m_notes.empty()){
                        for (int i = 0;i<bell->m_notes.size();++i){
                            if (bell->m_notes[i].id == n.id) {
                                // n.timeOn = bell->m_notes[i].timeOn;
                                // n.note_off(mixer.getTime());
                                // std::cout << "note off "<< i <<std::endl;
                            bell->m_notes[i].note_off(mixer.getTime());  
                            } 
                        }
                    }
                    // bell->m_envelope.note_off(mixer.getTime());
                    // saw->m_envelope.note_off(mixer.getTime());
                    printf("received MIDI byte: %d %d %d %d \n",inpacket[0],inpacket[1], inpacket[2],inpacket[3]);
                }
                    else if (inpacket[0] == CONTROL_CODE) {
                    readPacketOk = read(seqfd, &inpacket, 3);
                    // printf("Code: %d , Angle: %f, Channel: %d\n",inpacket[0],(double) inpacket[1]/127*360,inpacket[2]);
                    if (inpacket[0]==20){
                        bell->m_envelope.setA((double) inpacket[1]/127);
                        saw->m_envelope.setA((double) inpacket[1]/127);
                    }
                    if (inpacket[0]==21){
                        bell->m_envelope.setD((double) inpacket[1]/127);
                        saw->m_envelope.setD((double) inpacket[1]/127);
                    }
                    if (inpacket[0]==22){
                        bell->m_envelope.setS((double) inpacket[1]/127);
                        saw->m_envelope.setS((double) inpacket[1]/127);
                    }
                    if (inpacket[0]==23){
                        bell->m_envelope.setR((double) inpacket[1]/127);
                        saw->m_envelope.setR((double) inpacket[1]/127);
                    }
                    if (inpacket[0]==118) running=false;
                    if (inpacket[0]==119) bell->m_notes.erase(bell->m_notes.begin(),bell->m_notes.end());
                }
                if (inpacket[0] == AFTERTOUCH) {
                    readPacketOk = read(seqfd, &inpacket, 1);
                    // printf("Aftertouch: %d \n",inpacket[0]);
                }
            }
        }






        while(SDL_PollEvent(&event)) {
            // ImGui_ImplSDL2_ProcessEvent(&event);
            switch (event.type) {
                case SDL_KEYDOWN:
                    if (event.key.keysym.sym && ! event.key.repeat){
                        bell->m_envelope.note_on(mixer.getTime());
                        switch (event.key.keysym.sym ) {
                            case SDLK_ESCAPE: running=false; break;
                            case SDLK_w: noteId=60;
                                mixer.freq = bell->scale(noteId);
                                break;
                            case SDLK_s: noteId=61;
                                mixer.freq = bell->scale(noteId);
                                break;
                            case SDLK_x: noteId=62;
                                mixer.freq = bell->scale(noteId);
                                break;
                            case SDLK_d: noteId=63;
                                mixer.freq = bell->scale(noteId);
                                break;
                            case SDLK_c: noteId=64;
                                mixer.freq = bell->scale(noteId);
                                break;
                            case SDLK_v: noteId=65;
                                mixer.freq = bell->scale(noteId);
                                break;
                            case SDLK_g: noteId=66;
                                mixer.freq = bell->scale(noteId);
                                break;
                            case SDLK_b: noteId=67;
                                mixer.freq = bell->scale(noteId);
                                break;
                            case SDLK_h: noteId=68;
                                mixer.freq = bell->scale(noteId);
                                break;
                            case SDLK_n: noteId=69;
                                mixer.freq = bell->scale(noteId);
                                break;
                            case SDLK_j: noteId=70;
                                mixer.freq = bell->scale(noteId);
                                break;
                            case SDLK_COMMA: noteId=71;
                                mixer.freq = bell->scale(noteId);
                                break;
                            case SDLK_SEMICOLON: noteId=72;
                                mixer.freq = bell->scale(noteId);
                                break;
                            default:
                                break;

                        }
                    }
                    break;
                case SDL_KEYUP:
                    bell->m_envelope.note_off(mixer.getTime());
                    break;
                case SDL_QUIT:
                    running=false;
                    break;
            }
        }


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
