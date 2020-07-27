
#ifndef __INSTRUMENTCOLLEC_H_
#define __INSTRUMENTCOLLEC_H_

#include "instrument.hpp"
#include "aux.hpp"
#include "math.h"

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
                    sound =(double)  sine(time-m_notes[i].timeOn, scale(m_notes[i].id))/12;
                    timbre += sound*env;
                }

                std::vector<Note>::iterator n = m_notes.begin();
                while (n != m_notes.end())
                    if (!n->active)
                        n = m_notes.erase(n);
                    else
                        ++n;

            }
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
                    sound = (double)  saw(time-m_notes[i].timeOn, scale(m_notes[i].id))/12;
                    timbre += sound*env;
                }

                std::vector<Note>::iterator n = m_notes.begin();
                while (n != m_notes.end())
                    if (!n->active)
                        n = m_notes.erase(n);
                    else
                        ++n;

            }
            return m_Volume* timbre;
        }
};

class Guitar: public Instrument {
    public:
        Guitar();
        ~Guitar(){
            delete stringAmpNew;
            delete stringAmp;
            delete stringAmpOld;
        }

        double length;
        double tension;
        double density;
        int stringElements;
        double lastTime;
        double dx;

        double * stringAmpNew; //y(x,t)
        double * stringAmp; //y(x,t-1)
        double * stringAmpOld; //y(x,t-2)
        double gen_sound(double time, double freq);
                                             //
};


#endif // __INSTRUMENTCOLLEC_H_
