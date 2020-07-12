
#include "instrument.hpp"

double ADSR_Envelope::getAmp(double _time){
    // std::cout << "Getting Amp" << std::endl;
    // std::cout << "Time: " << _time << std::endl;
    // std::cout << "Time on: " << m_TimeOn << std::endl;
    // std::cout << "Time off: " << m_TimeOff << std::endl;
    // std::cout << "Playnote : " << play_note << std::endl;
    double lifeTime = (_time - m_TimeOn);
    double time = _time;
    double TimeOff = m_TimeOff;
    double amp=m_StartAmp;
    if (lifeTime<=m_AttackTime){
        amp = (lifeTime/m_AttackTime)*m_StartAmp;
    }
    else if (lifeTime>m_AttackTime && lifeTime<=(m_AttackTime+m_DecayTime)){
        amp = ((lifeTime-m_AttackTime)/m_DecayTime)*(m_SustainAmp-m_StartAmp)+m_StartAmp;
    }
    else if (lifeTime>(m_AttackTime+m_DecayTime)){
        amp = m_SustainAmp;
    }

    if (!play_note) amp = ((time-TimeOff)/m_ReleaseTime) * (0.0 - amp) + amp;
    if (amp<0.001) amp = 0;

    // std::cout << "Amp: " << amp << std::endl;
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

