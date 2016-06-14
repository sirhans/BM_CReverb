//
//  main.c
//  CReverb
//
//  Created by Hans on 7/2/16.
//  Copyright © 2016 Hans. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "BMCReverb.h"


#define TESTBUFFERLENGTH 128

int main(int argc, const char * argv[]) {
    
    // open a file for writing
    FILE* audioFile;
    audioFile = fopen("./rvImpulse.csv", "w+");
    
    system("pwd\n");
    
    
    // initialize the reverb
    struct BMCReverb rv;
    BMCReverbInit(&rv);
    
    
    // set a long decay time
    BMCReverbSetRT60DecayTime(&rv, 1.30);
    // and a 100% wet reverb sound
    BMCReverbSetWetGain(&rv, 1.0);
    // set high frequency decay multiplier
    //BM_CReverbSetHFDecayMultiplier(&rv, 6.0);
    // don't mix l and r channels
    BMCReverbSetCrossStereoMix(&rv, 0.5f);
    // increase the size of the network
    //BM_CReverbSetNumDelayUnits(&rv, 16);
    // adjust the highpass filter frequency
    BMCReverbSetHighPassFC(&rv, 250.0f);
    

    float testBufferInL [TESTBUFFERLENGTH];
    float testBufferInR [TESTBUFFERLENGTH];
    float testBufferOutL [TESTBUFFERLENGTH];
    float testBufferOutR [TESTBUFFERLENGTH];
    
    
    // create the initial impulse followed by zeros
    testBufferInL[0] = 1.0f;
    testBufferInR[0] = 1.0f;
    memset(testBufferInL+1, 0, sizeof(float)*(TESTBUFFERLENGTH-1));
    memset(testBufferInR+1, 0, sizeof(float)*(TESTBUFFERLENGTH-1));
    
    
    // process the first frame twice (the first time to trigger an update)
    BMCReverbProcessBuffer(&rv, testBufferInL, testBufferInR, testBufferOutL, testBufferOutR, TESTBUFFERLENGTH);
    BMCReverbProcessBuffer(&rv, testBufferInL, testBufferInR, testBufferOutL, testBufferOutR, TESTBUFFERLENGTH);
    
    // print out the entire frame in .csv format
    for (size_t i=0; i<TESTBUFFERLENGTH; i++) {
        fprintf(audioFile, "%f,%f\n",testBufferOutL[i],testBufferOutR[i]);
    }
    
    // set the input buffers to all zeros (only the first value was non-zero)
    testBufferInL[0] = testBufferInR[0] = 0.0;
    
    
    // start a timer
    clock_t begin, end;
    double time_spent;
    begin = clock();
    
    
    // process and print more frames
    size_t numFramesToPrint = 44100/128;
    while (numFramesToPrint-- != 0) {
        // turn the hold pedal on
        //if (numFramesToPrint < 1400) BMCReverbSetSlowDecayState(&rv, true);
        // turn it back off
        //if (numFramesToPrint < 500) BMCReverbSetSlowDecayState(&rv, false);
        BMCReverbProcessBuffer(&rv, testBufferInL, testBufferInR, testBufferOutL, testBufferOutR, TESTBUFFERLENGTH);
        for (size_t i = 0; i<TESTBUFFERLENGTH; i++)
            fprintf(audioFile, "%f,%f\n", testBufferOutL[i], testBufferOutR[i]);
    }
    
    
    // print the time taken to process reverb
    end = clock();
    time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("time: %f\n", time_spent);
    
    
    fclose(audioFile);
    return 0;
}
