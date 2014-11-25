//
//  Funky.h
//  Synthesizer
//
//  Created by Halldór Eldjárn on 11/11/14.
//  Copyright (c) 2014 Edu. All rights reserved.
//

#ifndef Synthesizer_Funky_h
#define Synthesizer_Funky_h

#include "Tonic.h"
#include "Drum.h"

using namespace Tonic;

class Funky : public Drum
{
public:
    Funky()
    {
        Generator saw = SawtoothWave().freq(130 + parameters[0] * 500 + parameters[1] * SineWave().freq(0.1 + 1000 * parameters[2]));
        Generator sine = SineWave().freq(340 + 300 * SawtoothWave().freq(0.1 + 2000 * parameters[3]));
        
        ADSR env = ADSR(0.001, 0.2, 0.0, 0.0).decay(0.001 + parameters[4] * 0.5).trigger(trigger);
        
        setOutputGen((saw + sine) * env);
    }
};
#endif
