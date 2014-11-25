//
//  HiHat.h
//  Synthesizer
//
//  Created by Halldór Eldjárn on 11/11/14.
//  Copyright (c) 2014 Edu. All rights reserved.
//

#ifndef Synthesizer_Drum_h
#define Synthesizer_Drum_h

#include "Tonic.h"

typedef enum : int {
    AMP_VOLUME,
    AMP_ATTACK,
    AMP_DECAY
} DrumParameter;

using namespace Tonic;

class Drum : public Synth
{
public:
    ControlParameter trigger = addParameter("trigger", 0);
    std::vector<ControlParameter> parameters;
    
    Drum()
    {
        this->init();
    }
    
    void init()
    {
        for(int i = 0; i < 8; i++)
        {
            parameters.push_back(addParameter("param" + std::to_string(i), 0.5));
        }
    }
};
#endif
