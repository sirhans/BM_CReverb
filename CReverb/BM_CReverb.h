//
//  BM_CReverb.h
//  CReverb
//
//  Created by Hans on 7/2/16.
//  Copyright © 2016 Hans Anderson. All rights reserved.
//

#ifndef BM_CReverb_h
#define BM_CReverb_h

#include <Accelerate/Accelerate.h>

// default settings
#define BM_CREVERB_WETMIX 0.15 // dryMix = sqrt(1 - wetMix^2)
#define BM_CREVERB_NUMDELAYUNITS 4 // each unit contains 4 delays
#define BM_CREVERB_PREDELAY 0.007 // (in seconds)
#define BM_CREVERB_ROOMSIZE 0.100 // (seconds of sound travel time)
#define BM_CREVERB_DEFAULTSAMPLERATE 44100.0
#define BM_CREVERB_HIGHSHELFFC 8000.0 // above this frequency decay is faster
#define BM_CREVERB_HFDECAYMULTIPLIER 6.0 // HF decay is this many times faster
#define BM_CREVERB_HFSLOWDECAYMULTIPLIER 12.0 // used for sustain effect
#define BM_CREVERB_RT60 1.2 // overall decay time
#define BM_CREVERB_HIGHPASS_FC 30.0 // highpass filter on wet out
#define BM_CREVERB_LOWPASS_FC 6000.0 // lowpass filter on wet out
#define BM_CREVERB_CROSSSTEREOMIX 0.4 // mixing betwee L and R wet outputs
#define BM_CREVERB_SLOWDECAYRT60 8.0 // RT60 time when hold pedal is down

#ifdef __cplusplus
extern "C" {
#endif
    
    // the CReverb struct
    struct BM_CReverb {
        float *delayLines, *feedbackBuffers, *mixingBuffers, *fb0, *fb1, *fb2, *fb3, *mb0, *mb1, *mb2, *mb3, *z1, *a1, *b0, *b1, *a1Slow, *b0Slow, *b1Slow, *delayTimes, *decayGainAttenuation, *slowDecayGainAttenuation, *leftOutputTemp, *delayOutputSigns, *dryL, *dryR;
        size_t *bufferLengths, *bufferStartIndices, *bufferEndIndices, *rwIndices;
        float minDelay_seconds, maxDelay_seconds, sampleRate, wetGain, dryGain, inputAttenuation, matrixAttenuation, straightStereoMix, crossStereoMix, hfDecayMultiplier, hfSlowDecayMultiplier, highShelfFC, rt60, slowDecayRT60, highpassFC, lowpassFC;
        size_t delayUnits, newNumDelayUnits, numDelays, halfNumDelays, fourthNumDelays, threeFourthsNumDelays, samplesTillNextWrap, totalSamples;
        float* twoChannelFilterData [2];
        vDSP_biquadm_Setup stereoFilterSetup;
        double mainFilterCoefficients[5*2*2], *fcChLSec0, *fcChRSec0, *fcChLSec1, *fcChRSec1;
        bool slowDecay, settingsQueuedForUpdate, autoSustain;
    };
    
    
    
    /*
     * publicly usable functions
     */
    // initialisation and cleanup
    void BM_CReverbInit(struct BM_CReverb* rv);
    void BM_CReverbFree(struct BM_CReverb* rv);
    
    // main audio processing function
    void BM_CReverbProcessBuffer(struct BM_CReverb* rv, const float* inputL, const float* inputR, float* outputL, float* outputR, size_t numSamples);
    
    // settings that can be safely changed during reverb operation
    void BM_CReverbSetHighPassFC(struct BM_CReverb* rv, float fc);
    void BM_CReverbSetLowPassFC(struct BM_CReverb* rv, float fc);
    void BM_CReverbSetWetGain(struct BM_CReverb* rv, float wetGain);
    void BM_CReverbSetCrossStereoMix(struct BM_CReverb* rv, float crossMix);
    void BM_CReverbSetHFDecayMultiplier(struct BM_CReverb* rv, float multiplier);
    void BM_CReverbSetHFDecayFC(struct BM_CReverb* rv, float fc);
    void BM_CReverbSetRT60DecayTime(struct BM_CReverb* rv, float rt60);
    void BM_CReverbSetSlowDecayState(struct BM_CReverb* rv, bool slowDecay);
    void BM_CReverbSetAutoSustain(struct BM_CReverb* rv, bool autoSustain);
    
    // settings for which changes will queue until the end of the next buffer
    void BM_CReverbSetNumDelayUnits(struct BM_CReverb* rv, size_t delayUnits);
    void BM_CReverbSetPreDelay(struct BM_CReverb* rv, float preDelay_seconds);
    void BM_CReverbSetRoomSize(struct BM_CReverb* rv, float roomSize_seconds);
    void BM_CReverbSetSampleRate(struct BM_CReverb* rv, float sampleRate);
    
    
#ifdef __cplusplus
}
#endif

#endif /* BM_CReverb_h */
