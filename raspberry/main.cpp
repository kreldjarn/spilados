// Dependencies: TonicAudio, RtAudio and wiringPi

#include <iostream>

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
	TonicFloat normalizedValue = (float)value / 255.0;
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
		// Push button
		case 2:
		{
			if(value > 128) synth.setParameter("trigger", 1);
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
    unsigned int bufferFrames = 512;

    Tonic::setSampleRate(sampleRate);

    // The Tonic patch is constructed here. Should refactor to an object inheriting from Tonic::Synth
    ControlMetro metro = ControlMetro().bpm(120);

    Generator pitchParameter = synth.addParameter("pitch", 0.5).smoothed().length(0.01);
    ControlParameter releaseParameter = synth.addParameter("release", 0.5);
    ControlParameter triggerParameter = synth.addParameter("trigger", 0);

    ADSR env = ADSR(0.0001, 0.3, 0.0, 0.0).decay(0.05 + (releaseParameter * 0.9)).doesSustain(false).trigger(metro);
    ADSR pitchEnv = ADSR(0.0001, 0.001, 0.0, 0.0).doesSustain(false).trigger(metro);

    Generator sine = SineWave().freq(10 + pitchParameter * 200 + pitchEnv * 1000);
    Generator output = sine * env;

    synth.setOutputGen(output * 0.2);

    // Connect the Tonic synth to the audio library and set the callback for
    // received messages over the serial bus.
    try
    {
        dac.openStream( &rtParams, NULL, RTAUDIO_FLOAT32, sampleRate, &bufferFrames, &renderCallback, NULL, NULL );
        dac.startStream();

        // Run the message loop on a seperate thread?
        // Currently it blocks the main thread.
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

