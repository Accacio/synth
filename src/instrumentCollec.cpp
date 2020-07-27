
#include "instrumentCollec.hpp"

Guitar::Guitar() {
    m_Volume = 1.0;
    stringElements=100;
    length=0.65087;
    dx=(length/stringElements-1);
    stringAmpNew = new double[stringElements];
    stringAmp = new double[stringElements];
    stringAmpOld = new double[stringElements];
    stringAmp[0] = 0.0;
    stringAmp[stringElements] = 0.0;
    tension=58.272;
    density=0.0013856;
}

double Guitar::gen_sound(double time, double freq){
            // double timbre = sine(time, freq);
            double timbre = 0;
            double sound = 0;
            double env = 0;
            bool active ;
            double dt=(time-lastTime);
            double fs=1/dt;
            // std::cout << fs << std::endl;
            double prop=0.5;
            double pickDist=1;
            int pickElem = (int) std::floor(stringElements*prop)-1;
            // std::cout << m_notes.size() << " " << std::endl;
            if(!m_notes.empty()){
                for (int i = 0; i < m_notes.size();i++){
                    active = true;
                    // calculate amplitude in string
                    if((time-m_notes[i].timeOn)<0.1) {

                            std::cout << time << " Pick elem " << pickElem << std::endl;
                        for (int x = 0; x < stringElements; x++){
                            if (x <= pickElem) {
                                stringAmp[x] = (pickDist/pickElem)*x ;
                                stringAmpOld[x] = (pickDist/pickElem)*x ;
                                // std::cout << stringAmpOld[x] << " ";
                            }
                            else {
                                stringAmp[x] = pickDist-(pickDist/(stringElements-pickElem-1))*(x-pickElem);
                                stringAmpOld[x] = pickDist-(pickDist/(stringElements-pickElem-1))*(x-pickElem);
                                // std::cout << stringAmpOld[x] << " ";
                            }
                        }
                        // std::cout << std::endl;
                    }                   

                    for (int x = 1; x < stringElements-1; ++x){
                        stringAmpNew[x] = 2*stringAmp[x]
                            - stringAmpOld[x]
                            + tension/density * dt*dt/(dx*dx) * (stringAmp[x-1]-2*stringAmp[x]+stringAmp[x+1]);
                    }
                    for (int x = 1; x < stringElements-1; x++){
                        stringAmpOld[x] = stringAmp[x];
                        stringAmp[x] = stringAmpNew[x];
                        // std::cout << stringAmp[x] << " ";
                    }
                    // std::cout << std::endl;
                    env = m_envelope.getAmp(time, m_notes[i].timeOn, m_notes[i].timeOff,m_notes[i].active);
                    // if(i==0) std::cout << env << std::endl;
                    // sound = stringAmp[pickElem];
                    // std::cout << stringAmp[298]<< " ";
                    sound = (double)  saw(time-m_notes[i].timeOn, scale(m_notes[i].id))/12;
                    // timbre += sound*env;
                    timbre += sound;
                }

                std::vector<Note>::iterator n = m_notes.begin();
                while (n != m_notes.end())
                    if (!n->active)
                        n = m_notes.erase(n);
                    else
                        ++n;

            }
            lastTime = time;
            return m_Volume* timbre;
        }
