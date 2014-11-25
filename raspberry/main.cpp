/********************************
 * Tónvélarkóði
 * ===============================
 * Höfundar:
 * Halldór Eldjárn (hae28@hi.is)
 *  &
 * Kristján Eldjárn (keh4@hi.is)
 ********************************/

#include <unistd.h>

#include <iostream>
#include <functional>
#include <vector>

#include "Tonic.h"
#include "RtAudio.h"

#include "BassDrum.h"
#include "Snare.h"
#include "HiHat.h"
#include "Funky.h"

#include "PatternLoader.h"

#define RASPI false

#if RASPI
#include <wiringPi.h>
#include <wiringSerial.h>
#endif

using namespace Tonic;

const unsigned int nChannels = 2;
const float MAX_VALUE = 241.0;

const int NUM_TRACKS = 4;
const int GLOBAL_MAX_STEPS = 16;

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

    bool noteAtIndex(int noteIndex)
    {
        return track[noteIndex];
    }
    
    void printTrack()
    {
        std::cout << "SequencerTrack >> (" << std::to_string(this->currentStep) << "/" << std::to_string(this->maxSteps) << "): ";
        
        for (int i = 0; i < this->maxSteps; i++)
        {
            std::cout << std::to_string(track[i]) << " ";
        }
        std::cout << std::endl;
    }
};

class DrumMachine
{
    Synth synth;
    std::vector<SequencerTrack> tracks;
    std::vector<Synth> trackSynths;
    int activeTrackIndex;
public:
    bool shouldRefreshUserInterface;
    int outgoingInfo[32];

    DrumMachine(std::vector<Synth> trackSynths)
    {
	for(int i = 0; i < 32; i++)
        {
            this->outgoingInfo[i] = 0;
        }
	
        this->shouldRefreshUserInterface = false; 
        this->activeTrackIndex = 0;
        this->synth = synth;
        this->trackSynths = trackSynths;
        for(int i = 0; i < NUM_TRACKS; i++)
        {
            this->tracks.push_back(*new SequencerTrack(GLOBAL_MAX_STEPS));
        }
    }
    
    void tick()
    {
        for(int i = 0; i < NUM_TRACKS; i++)
        {
            if(this->tracks[i].tick())
            {
                this->trackSynths[i].setParameter("trigger", 1);
            }
        }
    }
    
    void setActiveChannel(int param)
    {
        int newActiveTrack = (int)(((float)param / (MAX_VALUE + 1)) * NUM_TRACKS);
        
        if(newActiveTrack != this->activeTrackIndex)
        {
            this->activeTrackIndex = newActiveTrack;
            this->shouldRefreshUserInterface = true;
            
            for(int i = 0; i < 16; i++)
            {
                this->outgoingInfo[2*i] = 100 + i;
                this->outgoingInfo[2*i+1] = this->tracks[activeTrackIndex].noteAtIndex(i) ? 255 : 0;
            }
            printf("Active track is: %i\n", this->activeTrackIndex);
            std::cout << "Outgoing info to arduino is: " << this->outgoingInfo << std::endl;
        }
    }
    
    void setNoteForTrack(int trackIndex, int noteIndex, bool state)
    {
        if(trackIndex >= tracks.size() || trackIndex < 0)
        {
            std::cout << "Trying to set state of track out of bounds. Ignoring." << std::endl;
        }
        else
        {
            std::cout << "Setting note " << noteIndex << " to " << state << " on track " << trackIndex << std::endl;
            tracks[trackIndex].setNote(noteIndex, state);
        }
    }
    
    void loadKit(int kitIndex)
    {
        PatternLoader p;
        StringMatrix kitData = p.readKit(kitIndex);
        for(int i = 0; i < kitData.size(); i++)
        {
            for(int j = 0; j < kitData[i].size(); j++)
            {
                this->trackSynths[i].setParameter("param" + to_string(j), stof(kitData[i][j])/238.0);
            }
        }
    }
    
    void loadPattern(int pattIndex)
    {
        PatternLoader p;
        StringMatrix pattData = p.readPattern(pattIndex);
        for(int i = 0; i < pattData.size(); i++)
        {
            for(int j = 0; j < pattData[i].size(); j++)
            {
                this->tracks[i].setNote(j, stoi(pattData[i][j]));
            }
        }
    }
    
    
    // Inject received parameters to the Tonic synth
    void changeValueForParameter(int param, int value)
    {
        TonicFloat normalizedValue = (float)value / MAX_VALUE;
        SequencerTrack activeTrack = this->tracks[this->activeTrackIndex];
        Synth activeSynth = this->trackSynths[this->activeTrackIndex];
        
        switch(param)
        {
                // Parameters 0-7 are the pots.
                // TODO: Connect ALL the pots!
            case 0:
            {
                activeSynth.setParameter("param0", normalizedValue);
                break;
            }
            case 1:
            {
                activeSynth.setParameter("param1", normalizedValue);
                break;
            }
                
            case 2:
            {
                activeSynth.setParameter("param2", normalizedValue);
                break;
            }
                
            case 3:
            {
                activeSynth.setParameter("param3", normalizedValue);
                break;
            }
                
            case 4:
            {
                activeSynth.setParameter("param4", normalizedValue);
                break;
            }
               /*
            case 5:
            {
                activeSynth.setParameter("param5", normalizedValue);
                break;
            }
                
            case 6:
            {
                activeSynth.setParameter("param6", normalizedValue);
                break;
            }*/
                
            case 5:
            {
                this->setActiveChannel(value); 
		break;
            }
                
                // Those are messages when toggling the Trellis button array
            case 100: case 101: case 102: case 103:
            case 104: case 105: case 106: case 107:
            case 108: case 109: case 110: case 111:
            case 112: case 113: case 114: case 115:
            {
                bool state;
                state = (value > 128) ? true : false;
                this->setNoteForTrack(activeTrackIndex, param - 100, state);
                activeTrack.printTrack();
                break;
            }
        }
    }

    
};

// Very simple serial communication
void listenForMessages( DrumMachine *drumMachine )
{
#if RASPI
    wiringPiSetup();
    
    int serialConnection = serialOpen("/dev/ttyACM0", 9600);
#endif

    while(1)
    {
#if RASPI
        if(drumMachine->shouldRefreshUserInterface)
        {
            for(int i = 0; i < 16*2; i++)
            {
                if(drumMachine->outgoingInfo[i] >= 0 && drumMachine->outgoingInfo[i] <= 255)
                {
                    serialPutchar(serialConnection, drumMachine->outgoingInfo[i]);
                    std::cout << "out byte: " << drumMachine->outgoingInfo[i] << std::endl;
                }
                else
                {
                    std::cout << "Error, trying to send shitty byte from outgoingInfo." << std::endl;
                }
            }
            drumMachine->shouldRefreshUserInterface = false;
        }

        int param = serialGetchar(serialConnection);
        if(param == -1) continue;
        int value = serialGetchar(serialConnection);
        if(value == -1) continue;
#endif
        
#if RASPI
        drumMachine->changeValueForParameter(param, value);
#else
        // Use this to fake some parameter changes when running on other systems than
        // the Raspi, and we don't have any controls.
        //drumMachine->changeValueForParameter(100 + rand() % 7, rand() * 238);
        sleep(1);
        std::cout << "Value changed" << std::endl;
#endif
        
    }
}
// Global Tonic Synth instance
static Synth mixer;

// The callback function we pass to the Audio
int renderCallback( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
                   double streamTime, RtAudioStreamStatus status, void *userData )
{
    mixer.fillBufferOfFloats((float*)outputBuffer, nBufferFrames, nChannels);
    return 0;
}

int main(int argc, const char * argv[])
{
    RtAudio dac;
    RtAudio::StreamParameters rtParams;
    rtParams.deviceId = dac.getDefaultOutputDevice();
    rtParams.nChannels = nChannels;
#if RASPI
    unsigned int sampleRate = 22000;
#else
    unsigned int sampleRate = 44100;
#endif
    unsigned int bufferFrames = 512; // 512 sample frames
    
    Tonic::setSampleRate(sampleRate);
    
    std::vector<Synth> synths;
    synths.push_back(*new BassDrum());
    synths.push_back(*new Snare());
    synths.push_back(*new HiHat());
    synths.push_back(*new Funky());
    
    // Test write pattern
    
    DrumMachine *drumMachine = new DrumMachine(synths);
    
    drumMachine->loadPattern(0);
    
    ControlMetro metro = ControlMetro().bpm(480);
    
    ControlCallback drumMachineTick = ControlCallback(&mixer, [&](ControlGeneratorOutput output){
        drumMachine->tick();
    }).input(metro);
    
    Generator mixedSignal;
    for(int i = 0; i < NUM_TRACKS; i++)
    {
        mixedSignal = mixedSignal + synths[i];
    }
    mixer.setOutputGen(mixedSignal);
    
    try
    {
        dac.openStream( &rtParams, NULL, RTAUDIO_FLOAT32, sampleRate, &bufferFrames, &renderCallback, NULL, NULL );
        dac.startStream();
        
        // Send a pointer to our global drumMachine instance
        // to the serial communications layer.
        listenForMessages( drumMachine );
        
        dac.stopStream();
    }
    catch ( RtError& e )
    {
        std::cout << '\n' << e.getMessage() << '\n' << std::endl;
        exit( 0 );
    }
    
    return 0;
}

