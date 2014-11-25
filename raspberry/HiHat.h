//
//  HiHat.h
//  Synthesizer
//
//  Created by Halldór Eldjárn on 11/11/14.
//  Copyright (c) 2014 Edu. All rights reserved.
//

#ifndef Synthesizer_HiHat_h
#define Synthesizer_HiHat_h

#include "Tonic.h"
#include "Drum.h"

using namespace Tonic;

class HiHat : public Drum
{
public:
    ControlParameter open = addParameter("open", 0);

    HiHat()
    {
        ControlGenerator hihatDecay = this->parameters[0];

        ADSR hihatEnv = ADSR(0.0001, 0.02, 0.0, 0.0).decay(0.002 + hihatDecay * 0.2 + 0.3 * open).trigger(trigger);

        Generator hiHat = PinkNoise() * hihatEnv;/* >> HPF12().cutoff(10000 * this->parameters[1] + 4000);*/
        
        setOutputGen(hiHat);
    }
};
#endif
