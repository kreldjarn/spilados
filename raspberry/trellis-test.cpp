//
//  main.cpp
//  TonicStandaloneDemo
//
//  Created by Nick Donaldson on 5/16/13.
//
//

// This is a super-simple demo showing a very basic command-line C++ program to play a Tonic synth

#include <iostream>
#include <functional>

#include "Tonic.h"
#include "RtAudio.h"

#include <wiringPi.h>
#include <wiringSerial.h>

using namespace Tonic;

const unsigned int nChannels = 2;

static Synth synth;

// Very simple serial communication
void listenForMessages( void (*callback)(int, int) )
{
        wiringPiSetup();

        int serialConnection = serialOpen("/dev/ttyACM0", 9600);

        while(1)
        {
                int param = serialGetchar(serialConnection);
                if(param == -1) continue;

                int value = serialGetchar(serialConnection);
                if(value == -1) continue;

                callback(param, value);
        }
}

// Inject received parameters to the Tonic synth
void changeValueForParameter(int param, int value)
{
	printf("%i, %i\n", param, value);

	TonicFloat normalizedValue = (float)value / 238.0;
	switch(param)
	{
		// Pot 1
		case 0:
		{
			synth.setParameter("pitch", normalizedValue); 
			break;
		}
		// Pot 2
		case 1:
		{
			synth.setParameter("release", normalizedValue);
			break;
		}

		case 2:
		{
			synth.setParameter("hihatDecay", normalizedValue);
			break;
		}
		
		case 3:
		{
			synth.setParameter("hihatCutoff", normalizedValue);
			break;
		}

		case 4:
		{
			synth.setParameter("hihatQ", normalizedValue);
			break;
		}

        // Push buttons

		case 100:
		{
			int state;
			
			if(value > 128) { state = 1; }
			else { state = 0; }
			synth.setParameter("s0", state);
			break;
		}

        case 101:
        {
                int state;
                
                if(value > 128) { state = 1; }
                else { state = 0; }
                synth.setParameter("s1", state);
                break;
        }

        case 102:
        {
                int state;
                
                if(value > 128) { state = 1; }
                else { state = 0; }
                synth.setParameter("s2", state);
                break;
        }

        case 103:
        {
                int state;
                
                if(value > 128) { state = 1; }
                else { state = 0; }
                synth.setParameter("s3", state);
                break;
        }

        case 104:
        {
                int state;
                
                if(value > 128) { state = 1; }
                else { state = 0; }
                synth.setParameter("s4", state);
                break;
        }

        case 105:
        {
                int state;
                
                if(value > 128) { state = 1; }
                else { state = 0; }
                synth.setParameter("s5", state);
                break;
        }

        case 106:
        {
                int state;
                
                if(value > 128) { state = 1; }
                else { state = 0; }
                synth.setParameter("s6", state);
                break;
        }

        case 107:
        {
                int state;
                
                if(value > 128) { state = 1; }
                else { state = 0; }
                synth.setParameter("s7", state);
                break;
        }

        case 108:
        {
                int state;
                
                if(value > 128) { state = 1; }
                else { state = 0; }
                synth.setParameter("s8", state);
                break;
        }

        case 109:
        {
                int state;
                
                if(value > 128) { state = 1; }
                else { state = 0; }
                synth.setParameter("s9", state);
                break;
        }

        case 110:
        {
                int state;
                
                if(value > 128) { state = 1; }
                else { state = 0; }
                synth.setParameter("s10", state);
                break;
        }

        case 111:
        {
                int state;
                
                if(value > 128) { state = 1; }
                else { state = 0; }
                synth.setParameter("s11", state);
                break;
        }

        case 112:
        {
                int state;
                
                if(value > 128) { state = 1; }
                else { state = 0; }
                synth.setParameter("s12", state);
                break;
        }

        case 113:
        {
                int state;
                
                if(value > 128) { state = 1; }
                else { state = 0; }
                synth.setParameter("s13", state);
                break;
        }

        case 114:
        {
                int state;
                
                if(value > 128) { state = 1; }
                else { state = 0; }
                synth.setParameter("s14", state);
                break;
        }

        case 115:
        {
                int state;
                
                if(value > 128) { state = 1; }
                else { state = 0; }
                synth.setParameter("s15", state);
                break;
        }
	}
}

// Audio stuff
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
    unsigned int bufferFrames = 512; // 512 sample frames

    Tonic::setSampleRate(sampleRate);

    ControlMetro metro = ControlMetro().bpm(480);

    Generator pitchParameter = synth.addParameter("pitch", 0.5).smoothed().length(0.01);
    ControlParameter releaseParameter = synth.addParameter("release", 0.5);
    ControlParameter triggerParameter = synth.addParameter("trigger", 0);
    ControlParameter hihatDecayParameter = synth.addParameter("hihatDecay", 0.3);
    ControlParameter hihatCutoffParameter = synth.addParameter("hihatCutoff", 0.0);
    ControlParameter hihatQParameter = synth.addParameter("hihatQ", 0.0);

    ControlStepper step = ControlStepper().end(16).trigger(metro);
    ControlSwitcher notes = ControlSwitcher().inputIndex(step);
    
    for(int i = 0; i < 16; i++)
    { 
        notes.addInput(synth.addParameter("s" + to_string(i), 0).min(0).max(1));
    }

    Generator sineLfoHihat = SineWave().freq(2.5);

    

    ControlGenerator hihatPattern = ControlTriggerFilter().trigger(metro).sequence("1|1010|1010|0101|0101|1010|1010|1001|001");
    ADSR envHihat = ADSR(0.0001, 0.1, 0.0, 0.0).attack(0.0001 + hihatCutoffParameter * 0.4).decay(0.05 + (hihatDecayParameter * 0.35)).trigger(hihatPattern);

    ADSR env = ADSR(0.0001, 0.3, 0.0, 0.0).decay(0.05 + (releaseParameter * 0.9)).doesSustain(false).trigger(notes);
    ADSR pitchEnv = ADSR(0.0001, 0.001, 0.0, 0.0).doesSustain(false).trigger(notes);

    Generator hihat = (PinkNoise() * envHihat);

    Generator sine = SineWave().freq(10 + pitchParameter * 200 + pitchEnv * 1000);
    Generator output = sine * env;

    synth.setOutputGen(output * 0.3 + hihat * 0.2 * hihatQParameter);

    try
    {
        dac.openStream( &rtParams, NULL, RTAUDIO_FLOAT32, sampleRate, &bufferFrames, &renderCallback, NULL, NULL );
        dac.startStream();

	listenForMessages( &changeValueForParameter );

        dac.stopStream();
    }
    catch ( RtError& e )
    {
        std::cout << '\n' << e.getMessage() << '\n' << std::endl;
        exit( 0 );
    }
 
    return 0;
}

