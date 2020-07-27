#ifndef __INSTRUMENT_H_
#define __INSTRUMENT_H_

#include <time.h>
#include <stdint.h>
#include <iostream>
#include <vector>
#include "aux.hpp"


class ADSR_Envelope {
    public:
        ADSR_Envelope(double startAmp = 0, double attackTime = 0.1, double decayTime = 0.01, double sustainAmp = 0.8, double releaseTime = 0.2);
        double getAmp(double, double, double,bool&);
        ~ADSR_Envelope(){};
        void note_on(uint32_t time_on) {
            play_note=true;
            m_TimeOn = time_on;
            // std::cout<< " set time on";
        };

        double gen_sound(double time, double freq);
        void setA(double newA) {
            m_AttackTime = lerp(minAttackTime, maxAttackTime, newA);
        };
        double getA() {
            return ilerp(minAttackTime,maxAttackTime, m_AttackTime);
        };
        void setAAbsolute(double newA) {
            std::cout << "Set Attack";
            m_AttackTime = newA;
        };
        double getAAbsolute() {
            return m_AttackTime;
        };

        void setD(double newD) {
            m_DecayTime = lerp(minDecayTime,maxDecayTime,newD);
        };
        double getD() {
            return ilerp(minDecayTime,maxDecayTime, m_DecayTime);
        };
        void setDAbsolute(double newD) {
            m_DecayTime = newD;
        };
        double getDAbsolute() {
            return m_DecayTime;
        };

        void setS(double newS) {
            m_SustainAmp = lerp(minSustainAmp,maxSustainAmp,newS);
        };
        double getS() {
            return ilerp(minSustainAmp,maxSustainAmp,m_SustainAmp);
        };
        void setSAbsolute(double newS) {
            m_SustainAmp = newS;
        };
        double getSAbsolute() {
            return m_SustainAmp;
        };

        void setR(double newR) {
            m_ReleaseTime = lerp(minReleaseTime,maxReleaseTime,newR);
        };
        double getR() {
            return ilerp(minReleaseTime,maxReleaseTime, m_ReleaseTime);
        };
        void setRAbsolute(double newR) {
            m_ReleaseTime = newR;
        };
        double getRAbsolute() {
            return m_ReleaseTime;
        };

        void note_off(uint32_t time_off) {
            play_note=false;
            m_TimeOff = time_off;
            // std::cout<< " set time off";
        };
    private:
        bool play_note=false;
        double m_StartAmp;
        double m_AttackTime;
        double m_DecayTime;
        double m_SustainAmp;
        double m_ReleaseTime;
        uint32_t m_TimeOn=0;
        uint32_t m_TimeOff=0;


        double minAttackTime=0.01;
        double minDecayTime=0.01;
        double minSustainAmp=0.1;
        double minReleaseTime=0.01;

        double maxAttackTime=2;
        double maxDecayTime=2;
        double maxSustainAmp=1;
        double maxReleaseTime=2;

};

struct Note{
    int id;
    uint32_t timeOn;
    uint32_t timeOff;
    bool active;

    Note()
    {
        id = 0;
        timeOn = 0;
        timeOff = 0;
        active = true;
    }
    void note_on(uint32_t time_on) {
        // play_note=false;
        timeOn = time_on;
        timeOff = time_on+2000;
        // std::cout<< " set time off";
    };
    void note_off(uint32_t time_off) {
        // play_note=false;
        timeOff = time_off;
        // std::cout<< " set time off";
    };
};

class Instrument {
    public:
        double m_Volume;
        // Instrument(){};
        // ~Instrument(){};
        virtual double gen_sound(double time, double freq){ return 0;};
        // void note_on(uint32_t timeOn) {m_envelope.note_on(timeOn);};
        // void note_off(uint32_t timeOff){m_envelope.note_off(timeOff);};
        ADSR_Envelope m_envelope;
        std::vector<Note> m_notes;
        double scale(int noteId) {return pow(2,(float) (noteId-69)/12)*440;}
};


#endif // __INSTRUMENT_H_
