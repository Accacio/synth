
#include <poll.h>
#include <fcntl.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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
#include "instrumentCollec.hpp"
#include <poll.h>
#include <algorithm>
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




int main(int argc, char *argv[]) {
    // double freq = 0;
    int noteId = 0;
    Instrument *bell = new Bell();
    Instrument *monochord = new Monochord();
    Instrument *saw= new Saw();

    SDL_Window* window = NULL;

    unsigned char inpacket[4];
    int readPacketOk;
    int seqfd = open(MIDI_DEVICE, O_RDONLY);
    // printf("Error: cannot open %d\n", seqfd);
    if (seqfd == -1) {
        printf("Error: cannot open %s\n", MIDI_DEVICE);
        // exit(1);
    }
    int nfds = 1;
	struct pollfd *pfd;
    pfd = (struct pollfd*) calloc(nfds,sizeof(struct pollfd));
    for (int i =0;i<nfds;i++){
        pfd[i].fd = seqfd;
        pfd[i].events = POLLIN;
       
    }

    if( SDL_Init(SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_VIDEO) < 0 ) {
        std::cout << "Unable to init SDL: " << SDL_GetError() << std::endl;
        return 1;
    }
    Mixer mixer = Mixer(44100,1,AUDIO_S16SYS,256);
    // Mixer mixer = Mixer();
    // envelope = ADSR_Envelope();
    // envelope.getAmp(time(0));
    mixer.addInstrument(monochord);
    // mixer.addInstrument(saw);
    // mixer.addInstrument(bell);
    // mixer.userFunction = gen_sound;

    // if (seqfd ==-1) {
    window = SDL_CreateWindow("Mysynth", 10, 10, 1280, 720, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL| SDL_WINDOW_ALLOW_HIGHDPI );
        SDL_GLContext gl_context = SDL_GL_CreateContext(window);
        SDL_GL_MakeCurrent(window, gl_context);
        SDL_GL_SetSwapInterval(1); // Enable vsync

        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

        // Setup Dear ImGui style
        // ImGui::StyleColorsDark();

        ImGuiStyle& style = ImGui::GetStyle();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        // Setup Platform/Renderer bindings
        ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
        ImGui_ImplOpenGL2_Init();

        SDL_UpdateWindowSurface(window);
    // }
    bool running = true;
    SDL_Event event;


    float A; float D; float S; float R;
    SDL_SetWindowBordered(window, SDL_FALSE);
   
    float step = (float) 1/60;
    bool quitting=false;
    float opac = 0;

    Instrument* instrument = mixer.getInstrumentByIndex(0);

    while(running)
    {
        if (!quitting & (opac<1)){
            SDL_SetWindowOpacity(window, opac);
            opac+=step;
        }
        if (quitting){
            if (opac > 0) {
                opac-=step;
                SDL_SetWindowOpacity(window, opac);
            }else{
                running=false;
            }
        }


        ImGui_ImplOpenGL2_NewFrame();
        ImGui_ImplSDL2_NewFrame(window);
        ImGui::NewFrame();
        A = instrument->m_envelope.getA();
        D = instrument->m_envelope.getD();
        S = instrument->m_envelope.getS();
        R = instrument->m_envelope.getR();
        ImGui::Begin("Control");
        ImGui::SliderFloat("Attack", &A, 0, 1);
        ImGui::SliderFloat("Decay", &D, 0, 1);
        ImGui::SliderFloat("Sustain", &S, 0, 1);
        ImGui::SliderFloat("Release", &R, 0, 1);
        ImGui::End();
        instrument->m_envelope.setA(A);
        instrument->m_envelope.setD(D);
        instrument->m_envelope.setS(S);
        instrument->m_envelope.setR(R);

        if (seqfd > 0) {
            int ret = poll(pfd,1,0);

            while (ret > 0 && running){
                readPacketOk = read(seqfd, &inpacket, 1);

                //    // print the MIDI byte if this input packet contains one
                if (inpacket[0] == NOTE_ON) {
                    Note n;
                    readPacketOk = read(seqfd, &inpacket, 2);
                    // printf("Note on: %d , Velocity: %d, Channel: %d",inpacket[0], inpacket[1],inpacket[2]);
                    n.id = inpacket[0];
                    n.note_on(mixer.getTime());
                    instrument->add_note(n);
                    // saw->m_notes.push_back(n);
                    // mixer.freq  =  bell->scale(inpacket[0]);
                    // printf("received MIDI byte: %d %d %d %d \n",inpacket[0],inpacket[1], inpacket[2],inpacket[3]);
                }
                if (inpacket[0] == NOTE_OFF) {
                    Note n;
                    readPacketOk = read(seqfd, &inpacket, 2);
                    // printf("Note off: %d , Velocity: %d, Channel: %d\n",inpacket[0], inpacket[1],inpacket[2]);
                    // envelope.note_off(mixer.getTime());
                    n.id = inpacket[0];
                    n.note_off(mixer.getTime());
                    instrument->remove_note(n);
                    // bell->m_envelope.note_off(mixer.getTime());
                    // saw->m_envelope.note_off(mixer.getTime());
                    // printf("received MIDI byte: %d %d %d %d \n",inpacket[0],inpacket[1], inpacket[2],inpacket[3]);
                }
                else if (inpacket[0] == CONTROL_CODE) {
                    readPacketOk = read(seqfd, &inpacket, 2);
                    // printf("Code: %d , Angle: %f, Channel: %d\n",inpacket[0],(double) inpacket[1]/127*360,inpacket[2]);
                    if (inpacket[0]==20){
                        instrument->m_envelope.setA((double) inpacket[1]/127);
                        // saw->m_envelope.setA((double) inpacket[1]/127);
                    }
                    if (inpacket[0]==21){
                        instrument->m_envelope.setD((double) inpacket[1]/127);
                        // saw->m_envelope.setD((double) inpacket[1]/127);
                    }
                    if (inpacket[0]==22){
                        instrument->m_envelope.setS((double) inpacket[1]/127);
                        // saw->m_envelope.setS((double) inpacket[1]/127);
                    }
                    if (inpacket[0]==23){
                        instrument->m_envelope.setR((double) inpacket[1]/127);
                        // saw->m_envelope.setR((double) inpacket[1]/127);
                    }
                    if (inpacket[0]==118) running=false;
                    if (inpacket[0]==119) instrument->m_notes.erase(instrument->m_notes.begin(),instrument->m_notes.end());
                }
                if (inpacket[0] == AFTERTOUCH) {
                    readPacketOk = read(seqfd, &inpacket, 1);
                    // printf("Aftertouch: %d \n",inpacket[0]);
                }
                ret-=1;
            }
         

        }






        while(SDL_PollEvent(&event)) {
            // ImGui_ImplSDL2_ProcessEvent(&event);
            switch (event.type) {
                case SDL_KEYDOWN:
                    if (event.key.keysym.sym && ! event.key.repeat){
                        bell->m_envelope.note_on(mixer.getTime());
                        switch (event.key.keysym.sym ) {
                            case SDLK_ESCAPE:
                                quitting=true;
                                break;
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
                    quitting=true;
                    break;
            }
        }


        ImGui::Render();
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
            SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
        }
        // ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
        // glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        // glClear(GL_COLOR_BUFFER_BIT);
        ImVec4 clear_color = ImVec4(0.42f, 0.58f, 0.92f, 1.00f);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }
    bell->gen_sound(mixer.getTime(), 2230);
    ImGui_ImplOpenGL2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(gl_context);
    SDL_AudioQuit();
    SDL_DestroyWindow(window);
    SDL_Quit();
    mixer.removeAllInstruments();
    delete bell;
    delete monochord;
    delete saw;
   return 0;
}
