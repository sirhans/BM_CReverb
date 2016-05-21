//
//  BMCrossPlatformVDSP.h
//
//  This is an incomplete and unoptimised replacement for a few of the
//  functions in Apple's vDSP library of vectorised functions.  The
//  purpose of this code is to allow code that depends on certain vDSP
//  functions to compile and run on non-Apple operating systems.
//
//  In XCode with agressive compiler optimization settings, this code
//  is roughly 2x slower than the Apple vDSP functions applied to our
//  BMCReverb code.
//
//  Created by Hans on 23/2/16.
//  Copyright © 2016 Hans. All rights reserved.
//

#ifndef BMCrossPlatformVDSP_h
#define BMCrossPlatformVDSP_h

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <assert.h>


#ifdef __APPLE__
// copied from vDSP.h
// this code removes annoying xcode compiler warnings about
// required nullability specifers on pointers
#if !defined __has_feature
#define __has_feature(f)    0
#endif
#if __has_feature(assume_nonnull)
   _Pragma("clang assume_nonnull begin")
#else
#define __nullable
#define __nonnull
#endif
#endif       


/*************************************************************************
 * The code in this file implements the functions in Apple's
 * vDSP library.  See the Apple reference page for documentation.
 * https://developer.apple.com/library/mac/documentation/Accelerate/Reference/vDSPRef/
 *
 ************************************************************************/



/**************************************
 *  biquadm implementation functions  *
 **************************************/

typedef struct singleBiquadVariables {
    float b0,b1,b2,a1,a2,zb1,zb2,za1,za2;
}singleBiquadVariables;




typedef struct vDSP_biquadm_SetupStruct {
    singleBiquadVariables* filters;
    size_t numChannels, numLevels;
}*vDSP_biquadm_Setup;



/*
 * Here we re-declare the same setup as above just so that
 * we have the size of it to use in the sizeof function when
 * we allocate memory for it.
 */
typedef struct dummySetupStruct {
    singleBiquadVariables* filters;
    size_t numChannels, numLevels;
}dummySetupStructSize;





static __inline void vDSP_biquadm_DestroySetup(vDSP_biquadm_Setup setup){
    free(setup->filters);
    setup->filters = NULL;
    free(setup);
    setup = NULL;
    return;
}





/*
 *  You must call this function before using the filter
 */
static __inline vDSP_biquadm_Setup vDSP_biquadm_CreateSetup(const double* coefficients,
                                                            size_t numChannels,
                                                            size_t numLevels)
{
    vDSP_biquadm_Setup setup = malloc(sizeof(dummySetupStructSize));
    setup->filters = malloc(numChannels*numLevels*sizeof(singleBiquadVariables));
    
    for (size_t i=0; i < numLevels; i++){
        for (size_t j=0; j < numChannels; j++){
            setup->filters[i*numChannels + j].b0 = coefficients[i*numChannels*5+j*5+0];
            setup->filters[i*numChannels + j].b1 = coefficients[i*numChannels*5+j*5+1];
            setup->filters[i*numChannels + j].b2 = coefficients[i*numChannels*5+j*5+2];
            setup->filters[i*numChannels + j].a1 = coefficients[i*numChannels*5+j*5+3];
            setup->filters[i*numChannels + j].a2 = coefficients[i*numChannels*5+j*5+4];
            setup->filters[i*numChannels + j].za1 = 0;
            setup->filters[i*numChannels + j].za2 = 0;
            setup->filters[i*numChannels + j].zb1 = 0;
            setup->filters[i*numChannels + j].zb2 = 0;
        }
    }
    
    setup->numChannels = numChannels;
    setup->numLevels = numLevels;

    return setup;
}






static __inline void vDSP_biquadm(vDSP_biquadm_Setup setup,
                                  const float** input,
                                  size_t inputStride,
                                  float**  output,
                                  size_t outputStride,
                                  size_t count)
{
        assert (inputStride * outputStride == 1) ;
        for(size_t i=0; i< count; i++)
        {
            for(size_t j=0; j< setup->numChannels; j++)
                 {
                     for(size_t k=0; k < setup->numLevels; k++)
                         {
                             // y[n] = x[n]*b0 + zb1*b1 + zb2*b2 - za1*a1 - za2*a2
                             output[0][i] = input[0][i] * setup->filters[j+k*(setup->numChannels)].b0 +
                             setup->filters[j+k*(setup->numChannels)].zb1 *setup->filters[j+k*(setup->numChannels)].b1 +
                             setup->filters[j+k*(setup->numChannels)].zb2 *setup->filters[j+k*(setup->numChannels)].b2  -
                             setup->filters[j+k*(setup->numChannels)].za1 * setup->filters[j+k*(setup->numChannels)].a1 -
                             setup->filters[j+k*(setup->numChannels)].za2 *setup->filters[j+k*(setup->numChannels)].a2;
                             
                             // zb2 = zb1
                             setup->filters[j+k*(setup->numChannels)].zb2 = setup->filters[j+k*(setup->numChannels)].zb1;
                             
                             // zb1 = x[n]
                             setup->filters[j+k*(setup->numChannels)].zb1 = input[0][i];
                             
                             // za2 = za1
                             setup->filters[j+k*(setup->numChannels)].za2 = setup->filters[j+k*(setup->numChannels)].za1;
                             
                             // za1 = y[n]
                             setup->filters[j+k*(setup->numChannels)].za1 = output[0][i];
                            }
            
                    }
        }
    
        return;
}







/**********************************
 *  Vector mathematics functions  *
 **********************************/


// square and sum elements of A into B.
//
// A is an array
// Astride is the stride for A
// result is a pointer to a floating point value for output
// count is the number of elements in A to process
static __inline void vDSP_svesq(const float* A, size_t Astride, float* result, size_t count){
    *result = 0;
    
    // if stride is 1
    if (Astride==1) {
        for (size_t i=0; i<count; i++)
            *result += A[i]*A[i];
    }
    
    // if stride is not 1
    else {
        for (size_t i=0; i<count*Astride; i+=Astride)
            *result += A[i]*A[i];
    }
}




// scalar multiply, scalar multiply, and add
//
// result[i] = b*A[i] + d*C[i];
static __inline void vDSP_vsmsma(const float* A, size_t Astride, const float* b, const float* C, size_t Cstride, const float* d, float* result, size_t resultStride, size_t count){
    
    // if all strides are 1
    if(Astride*Cstride*resultStride == 1)
        for (size_t i=0; i<count; i++)
            result[i] = A[i]*(*b) + C[i]*(*d);
    
    
    // if some strides are not 1
    else
    {
        size_t ax, cx, resultx;
        ax=cx=resultx=0;
        
        while (count-- > 0) {
            result[resultx] = A[ax]*(*b) + C[cx]*(*d);
            ax+=Astride;
            cx+=Cstride;
            resultx+=resultStride;
        }
    }
}




// fill destination with value
static __inline void vDSP_vfill(float* value, float* destination, size_t destinationStride, size_t count){
    // if stride is 1
    if (destinationStride == 1)
        for (size_t i=0; i<count; i++)
            destination[i]=*value;
    
    // if stride != 1
    else {
        size_t i = 0;
        while (count-- > 0) {
            destination[i]=*value;
            i+=destinationStride;
        }
    }
}




// create an arithmetic sequence
// destination[i] = i*interval + startVal;
static __inline void vDSP_vramp(const float* startVal, const float* interval, float* destination, size_t destinationStride, size_t count){
    size_t i=0;
    float floatI = 0.0;
    while (count-- > 0) {
        destination[i] = floatI*(*interval) + (*startVal);
        i+=destinationStride;
        floatI++;
    }
}




// vector, scalar addition
// C[i] = A[i] + b;
static __inline void vDSP_vsadd(const float* A, size_t Astride, const float* b, float* C, size_t Cstride, size_t count){
    // if all strides are 1
    if (Astride*Cstride == 1)
        for (size_t i=0; i<count; i++)
            C[i]=A[i] + (*b);
    
    // if some stride is not 1
    else {
        size_t ai = 0, ci=0;
        while (count-- >0) {
            C[ci]=A[ai] + (*b);
            ci+=Cstride;
            ai+=Astride;
        }
    }
}





// vector, vector multiplication
// C[i] = A[i] * B[i];
static __inline void vDSP_vmul(const float* A, size_t Astride, const float* B, size_t Bstride, float* C, size_t Cstride, size_t count){
    // if all strides are 1
    if(Astride*Bstride*Cstride == 1)
        for (size_t i=0; i<count; i++)
            C[i] = A[i]*B[i];
    
    // if some strides are not 1
    else {
        while (count-- > 0) {
            size_t ai=0, bi=0, ci=0;
            C[ci] = A[ai]*B[bi];
            ai+= Astride;
            bi+= Bstride;
            ci+= Cstride;
        }
    }
}



// vector multiply and add
// D[i] = A[i]*B[i] + C[i];
static __inline void vDSP_vma(const float* A, size_t Astride, const float* B, size_t Bstride, const float* C, size_t Cstride, float* D, size_t Dstride, size_t count){
    // if all strides are 1
    if(Astride*Bstride*Cstride*Dstride == 1)
        for (size_t i=0; i<count; i++)
            D[i] = A[i]*B[i] + C[i];
    
    // if some strides are not 1
    else {
        while (count-- > 0) {
            size_t ai=0, bi=0, ci=0, di=0;
            D[di] = A[ai]*B[bi] + C[ci];
            ai+= Astride;
            bi+= Bstride;
            ci+= Cstride;
            di+= Dstride;
        }
    }
}




// vector clear
// A[i]=0
static __inline void vDSP_vclr(float* A, size_t Astride, size_t count){
    // if stride is 1
    if(Astride==1) memset(A,0,sizeof(float)*count);
    
    // if stride is not 1
    else {
        for (size_t i=0; i<count*Astride; i+=Astride)
            A[i]=0.0f;
    }
}




// vector multiply, vector multiply, and add
// E[i] = A[i]*B[i] + C[i]*D[i]
static __inline void vDSP_vmma(const float* A, size_t Astride, const float* B, size_t Bstride, const float* C, size_t Cstride, const float* D, size_t Dstride, float* E, size_t Estride, size_t count){
    // if all strides are 1
    if(Astride*Bstride*Cstride*Dstride*Estride == 1)
        for (size_t i=0; i<count; i++)
            E[i] = A[i]*B[i] + C[i]*D[i];
    
    
    // if some strides are not 1
    else {
        while (count-- > 0) {
            size_t ai=0, bi=0, ci=0, di=0, ei=0;
            E[ei] = A[ai]*B[bi] + C[ci]*D[di];
            ai+= Astride;
            bi+= Bstride;
            ci+= Cstride;
            di+= Dstride;
            ei+= Estride;
        }
    }
}




// vector sum
// *result = total(A)
static __inline void vDSP_sve(const float* A, size_t Astride, float* result, size_t count){
    *result = 0;
    
    if (Astride==1) {
        for (size_t i=0; i<count; i++)
            *result += A[i];
    } else {
        for (size_t i=0; i<count*Astride; i+=Astride)
            *result += A[i];
    }
}




// batch copy from vector of array indices
// B[i] = A[AIDX[i]-1];
static __inline void vDSP_vgathr(const float* A, const size_t* AIDX, size_t AIDXstride, float* B, size_t Bstride, size_t count){
    // the apple definition of this function indexes the array starting at 1, not 0
    const float* Azero = A - 1;
    
    // if strides are 1
    if(AIDXstride*Bstride==1)
        for (size_t i=0; i<count; i++)
            B[i] = Azero[AIDX[i]];
    
    // if any stride is not 1
    else {
        size_t ai=0, bi=0;
        while (count-- > 0) {
            B[bi] = Azero[AIDX[ai]];
            bi += Bstride;
            ai += AIDXstride;
        }
    }
}




// vector,vector add
// C[i] = A[i] + B[i];
static __inline void vDSP_vadd(const float* A, size_t Astride, const float* B, size_t Bstride, float* C, size_t Cstride, size_t count){
    // if all strides are 1
    if (Astride*Bstride*Cstride==1)
        for (size_t i=0; i<count; i++)
            C[i] = A[i] + B[i];
    
    // if some strides are not 1
    else {
        size_t ai=0, bi=0, ci=0;
        while (count-- > 0) {
            C[ci] = A[ai] + B[bi];
            ci += Cstride;
            bi += Bstride;
            ai += Astride;
        }
    }
}




// vector,vector subtract
// C[i] = A[i] - B[i];
static __inline void vDSP_vsub(const float* A, size_t Astride, const float* B, size_t Bstride, float* C, size_t Cstride, size_t count){
    // if all strides are 1
    if (Astride*Bstride*Cstride==1)
        for (size_t i=0; i<count; i++)
            C[i] = A[i] - B[i];
    
    // if some strides are not 1
    else {
        size_t ai=0, bi=0, ci=0;
        while (count-- > 0) {
            C[ci] = A[ai] - B[bi];
            ci += Cstride;
            bi += Bstride;
            ai += Astride;
        }
    }
}




// vector, scalar multiply
// C[i] = A[i] * b
static __inline void vDSP_vsmul(const float* A, size_t Astride, const float* b, float* C, size_t Cstride, size_t count){
    // if all strides are 1
    if (Astride*Cstride == 1)
        for (size_t i=0; i<count; i++)
            C[i]=A[i] * (*b);
    
    // if some stride is not 1
    else {
        size_t ai = 0, ci=0;
        while (count-- >0) {
            C[ci]=A[ai] * (*b);
            ci+=Cstride;
            ai+=Astride;
        }
    }
}



#ifdef __APPLE__
#if __has_feature(assume_nonnull)
_Pragma("clang assume_nonnull end")
#endif
#endif

#endif /* BMCrossPlatformVDSP_h */
