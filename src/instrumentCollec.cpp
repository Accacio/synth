
#include "instrumentCollec.hpp"

Monochord::Monochord() {
    m_Volume = 1.0;
    stringElements=50;
    length=0.65087;
    stringAmp = new double[stringElements+1];
    stringAmpNew = new double[stringElements+1];
    stringAmpOld = new double[stringElements+1];
    for(int i=0;i<stringElements;i++){
        stringAmp[i] = 0.0;
        stringAmpNew[i] = 0.0;
        stringAmpOld[i] = 0.0;
    }
    stringAmpNew[0] = 0.0;
    stringAmp[0] = 0.0;
    stringAmp[stringElements] = 0.0;
    tension=71.172;
    // tension=69.;
    density=0.0034323;

    pick=0.5;
    pickDist=1.0;
    pickUp=0.5;

}

Monochord::~Monochord() {
    delete stringAmp;
    delete stringAmpNew;
};

double Monochord::gen_sound(double time, double freq){
            // double timbre = sine(time, freq);
            double timbre = 0;
            double sound = 0;
            double env = 0;
            bool active ;
            double dt=(time-lastTime);
            double fs=1/dt;

            dx=(fingerLength/stringElements);
            // std::cout << fs << std::endl;
            // std::cout << m_notes.size() << " " << std::endl;
            double c =std::sqrt(tension/density);
            // std::cout << 1/(2*length)*c << std::endl;
            double nu = dt/dx*c;
            if(!m_notes.empty()){
                // for (int i = 0; i < m_notes.size();i++){
                    active = true;
                    // calculate amplitude

                    int pickElem = (int) std::floor(stringElements*pick)-1;
                    for (int x = 1; x < stringElements; ++x){
                        stringAmpNew[x] = 2*stringAmp[x]
                            - stringAmpOld[x]
                            + (nu)*(nu) * (stringAmp[x-1]-2*stringAmp[x]+stringAmp[x+1]);
                    }
                    for (int x = 1; x < stringElements; x++){
                        stringAmpOld[x] = stringAmp[x];
                        stringAmp[x] = stringAmpNew[x];
                        // std::cout << stringAmp[x] << " ";
                    }
                    // std::cout << std::endl;
                    env = m_envelope.getAmp(time, m_notes[0].timeOn, m_notes[0].timeOff,m_notes[0].active);
                    // if(i==0) std::cout << env << std::endl;
                    sound = stringAmp[pickElem];
                    // std::cout << stringAmp[298]<< " ";
                    // sound = (double)  saw(time-m_notes[i].timeOn, scale(m_notes[i].id))/12;
                    // timbre += sound*env;
                    timbre += sound;
                // }

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

void Monochord::add_note(Note note){
    // Instrument::add_note(note);
    if(!m_notes.empty()){
        for (int i = 0;i<m_notes.size();++i){
            if (m_notes[i].id == note.id) {
                // n.timeOn = mixer.m_instruments[0]->m_notes[i].timeOn;
                // n.note_off(mixer.getTime());
                // std::cout << "note off "<< i <<std::endl;
                m_notes[i].note_on(note.timeOn);
            } else {
               m_notes[i].id = note.id;
            }
        }
    }else{
        m_notes.push_back(note);

        double proport = 1/(pow(2,(float) (note.id-69)/12));
        // fingerLength =  proport<=1 ?  proport*length : length;
        fingerLength =  proport*length;
        std::cout << proport << std::endl;
        int pickUpElem = (int) std::floor(stringElements*pickUp)-1;
        int pickElem = (int) std::floor(stringElements*pick)-1;
        for (int x = 0; x < stringElements; x++){
            if (x < pickElem) {
                stringAmp[x] = (pickDist/pickElem)*x ;
                stringAmpOld[x] = (pickDist/pickElem)*x ;
                // std::cout << stringAmp[x] << " ";
            }
            else {
                stringAmp[x] = pickDist-(pickDist/(stringElements-pickElem-1))*(x-pickElem);
                stringAmpOld[x] = pickDist-(pickDist/(stringElements-pickElem-1))*(x-pickElem);
                // std::cout << stringAmp[x] << " ";
            }
        }
    }

    // if((time-m_notes[i].timeOn)<0.1) {

        // std::cout << time << " Pick elem " << pickElem << std::endl;
        // std::cout << std::endl;
    // }
}
// std::cout << "stub";


void Monochord::remove_note(Note note){
    Instrument::remove_note(note);
// std::cout << "stub";
}
