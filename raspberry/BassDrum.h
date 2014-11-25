//
//  BassDrumNew.cpp
//  Synthesizer
//
//  Created by Halldór Eldjárn on 06/08/14.
//  Copyright (c) 2014 Edu. All rights reserved.
//

#ifndef __Synthesizer__BassDrum__
#define __Synthesizer__BassDrum__

#include "Tonic.h"
#include "Drum.h"

using namespace Tonic;

class BassDrum : public Drum
{
public:
    
    BassDrum()
    {
        ADSR envelope = ADSR(0.01, 0.30, 0.0, 0.15)
        .decay(0.01 + parameters[2] * 0.5)
        .trigger(trigger)
        .doesSustain(false)
        .legato(false);
        
        ADSR pitchEnvelope = ADSR(0.01, 0.06, 0, 0.1)
        .trigger(trigger)
        .decay(0.001 + parameters[1] * 0.2)
        .doesSustain(false)
        .legato(false);
        
        ADSR filterEnvelope = ADSR(0.01, 0.22, 0, 0.1)
        .trigger(trigger)
        .doesSustain(false)
        .legato(false);
        
        ADSR noiseEnvelope = ADSR(0.0, 0.01, 0.0, 0.0)
        .trigger(trigger)
        .doesSustain(true)
        .legato(false);
        
        ADSR noiseFilterEnvelope = ADSR(0.0, 0.007, 0, 0.0)
        .decay(0.001 + parameters[4] * 0.2)
        .trigger(trigger)
        .doesSustain(false)
        .legato(false);
        
        Generator sawtooth = (SineWave().freq(  40 + parameters[0]*1000 + 100 * pitchEnvelope )
                              + (0.1 * SawtoothWave().freq( 200 ) * noiseEnvelope));
        //>> LPF24().cutoff(10000 * filterEnvelope + 100);
        sawtooth = sawtooth * envelope;
        
        Generator noise = Noise();// >> HPF12().cutoff(8000 * noiseFilterEnvelope + 1000);
        noise = noise * noiseEnvelope;
        
        Generator bassDrum = (((sawtooth + noise * parameters[3] * 0.7) /*>> LPF24().cutoff(2000)*/)) * 0.25;
        
        setOutputGen(bassDrum);
    }
};

#endif
