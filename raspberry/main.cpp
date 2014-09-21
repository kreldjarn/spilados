//
//  main.cpp
//  TonicStandaloneDemo
//
//  Created by Nick Donaldson on 5/16/13.
//
//

// This is a super-simple demo showing a very basic command-line C++ program to play a Tonic synth

#include <iostream>
#include "Tonic.h"
#include "RtAudio.h"
#include <vector>

//#include <wiringPi.h>
//#include <wiringSerial.h>

using namespace Tonic;

const unsigned int nChannels = 2;

// Static smart pointer for our Synth

static Synth synth;

class SequencerTrack
{
    int currentStep;
    int maxSteps;
    bool track[64];
    
public:
    SequencerTrack(int maxSteps)
    {
        this->currentStep = 0;
        this->maxSteps = maxSteps;
        
        for(int i = 0; i < this->maxSteps; i++)
        {
            track[i] = 0;
        }
    }
    
    bool tick()
    {
        bool shouldTrigger = track[currentStep];
        this->currentStep = (this->currentStep + 1) % this->maxSteps;
        return shouldTrigger;
    }
    
    void setNote(int noteIndex, bool state)
    {
        if (noteIndex >= 0 && noteIndex < this->maxSteps)
        {
            this->track[noteIndex] = state;
        }
    }
    
    void printTrack()
    {
        std:cout << "SequencerTrack >> (" << to_string(this->currentStep) << "/" << to_string(this->maxSteps) << "): ";
        
        for (int i = 0; i < this->maxSteps; i++)
        {
            std::cout << to_string(track[i]) << " ";
        }
        std::cout << std::endl;
    }
};

class BassDrum : public Synth
{
    ControlParameter pitchParameter = addParameter("pitch", 0.5);
    const TonicFloat MIN_PITCH = 20;
    const TonicFloat PITCH_RANGE = 400;
    
    ControlParameter decayParameter = addParameter("decay", 0.5);
    const TonicFloat MIN_DECAY = 0.01;
    const TonicFloat DECAY_RANGE = 0.65;
    
    ControlParameter rampParameter = addParameter("ramp", 0.5);
    const TonicFloat MIN_RAMP_PITCH = 20;
    const TonicFloat RAMP_PITCH_RANGE = 1000;
    
    ControlParameter rampDecayParameter = addParameter("rampDecay", 0.5);
    const TonicFloat MIN_RAMP_DECAY = 0.001;
    const TonicFloat RAMP_DECAY_RANGE = 0.35;
    
    ControlParameter noiseLevelParameter = addParameter("noiseLevel", 0.5);
    const TonicFloat MIN_NOISE_LEVEL = 0;
    const TonicFloat NOISE_LEVEL_RANGE = 1.0;
    
    ControlParameter noiseDecayParameter = addParameter("noiseDecay", 0.5);
    const TonicFloat MIN_NOISE_DECAY = 0.01;
    const TonicFloat NOISE_DECAY_RANGE = 0.9;
    
    ControlParameter triggerParameter = addParameter("trigger", 0);
    
public:
    BassDrum()
    {
        ControlMetro metro = ControlMetro().bpm(120);
        
        ADSR ampEnv = ADSR(0.001, 0.8, 0.0, 0.0)
        .attack(0.001)
        .decay(MIN_DECAY + decayParameter * DECAY_RANGE)
        .sustain(0)
        .release(0)
        .doesSustain(true)
        .trigger(triggerParameter);
        
        ADSR pitchEnv = ADSR(0.001, 0.3, 0.0, 0.0)
        .attack(0.001)
        .decay(MIN_RAMP_DECAY + rampDecayParameter * RAMP_DECAY_RANGE)
        .doesSustain(false)
        .trigger(triggerParameter)
        .exponential(1);
        
        ADSR noiseEnv = ADSR(0.001, 0.3, 0.0, 0.0)
        .attack(0.001)
        .decay(MIN_NOISE_DECAY + noiseDecayParameter * NOISE_DECAY_RANGE)
        .doesSustain(false)
        .trigger(triggerParameter);
        
        Generator sineOsc = SineWave()
        .freq((MIN_PITCH + pitchParameter * PITCH_RANGE) + (MIN_RAMP_PITCH + pitchEnv * RAMP_PITCH_RANGE));
        
        Generator noiseOsc = PinkNoise() * noiseEnv * (MIN_NOISE_LEVEL + noiseLevelParameter * NOISE_DECAY_RANGE);
    
        setOutputGen((sineOsc + noiseOsc) * ampEnv);
    }
};

TONIC_REGISTER_SYNTH(BassDrum);

int renderCallback( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
        double streamTime, RtAudioStreamStatus status, void *userData )
{
    synth.fillBufferOfFloats((float*)outputBuffer, nBufferFrames, nChannels);
    return 0;
}

int main(int argc, const char * argv[])
{
    RtAudio dac;
    RtAudio::StreamParameters rtParams;
    rtParams.deviceId = dac.getDefaultOutputDevice();
    rtParams.nChannels = nChannels;
    unsigned int sampleRate = 44100;
    unsigned int bufferFrames = 512;
    Tonic::setSampleRate(sampleRate);

    ControlMetro metro = ControlMetro().bpm(480);
    
    SequencerTrack *track = new SequencerTrack(16);
    track->setNote(2, true);
    track->printTrack();
    
    BassDrum *bassDrum = new BassDrum();
    
    ControlCallback callback = ControlCallback(bassDrum, [=](ControlGeneratorOutput output){
        if(track->tick())
        {
            bassDrum->setParameter("trigger", 1);
        }
    }).input(metro);
    
    synth.setOutputGen(*bassDrum * 0.1);

    
    for(int i = 0; i < 32; i++)
    {
        std::cout << track->tick() << ", ";
    }
    std::cout << std::endl;
    
    // open rtaudio stream
    try {
        dac.openStream( &rtParams, NULL, RTAUDIO_FLOAT32, sampleRate, &bufferFrames, &renderCallback, NULL, NULL );
        
        dac.startStream();
        
        // hacky, yes, but let's just hang out for awhile until someone presses a key
        printf("\n\nPress Enter to stop\n\n");
        
        cin.get();
	    
        dac.stopStream();
    }
    catch ( RtError& e ) {
        std::cout << '\n' << e.getMessage() << '\n' << std::endl;
        exit( 0 );
    }
 
    return 0;
}

