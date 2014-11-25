//
//  Snare.cpp
//  Synthesizer
//
//  Created by Halldór Eldjárn on 10/11/14.
//  Copyright (c) 2014 Edu. All rights reserved.
//

#ifndef __Synthesizer__Snare__
#define __Synthesizer__Snare__

#include "Tonic.h"
#include "Drum.h"

using namespace Tonic;

class Snare : public Drum
{
public:
    Snare()
    {
        ADSR snarePitchEnvelope = ADSR(0.0, 0.05, 0.0, 0.0)
        .trigger(trigger);
        
        Generator snare = (PinkNoise() + SineWave().freq(81 + 200 * snarePitchEnvelope)) \
        * ADSR(0.0001, 0.2, 0.0, 0.0)
        .exponential(true)
        .trigger(trigger);
        
        snare = snare * parameters[0];
        
        setOutputGen(snare);
    }
};

#endif
