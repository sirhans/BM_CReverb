//
//  BMCrossPlatformVDSP.h
//
//  This is an incomplete and unoptimised replacement for a few of the
//  functions in Apple's vDSP library of vectorised functions.  The
//  purpose of this code is to allow code that depends on certain vDSP
//  functions to compile and run on non-Apple operating systems.
//
//  Created by Hans on 23/2/16.
//  Copyright © 2016 Hans. All rights reserved.
//

#ifndef BMCrossPlatformVDSP_h
#define BMCrossPlatformVDSP_h

#include <stdio.h>
#include <Stdbool.h>
#include <math.h>

typedef struct vDSP_biquadm_Setup {
    float* buffers;
} vDSP_biquadm_Setup;

void vDSP_biquadm_DestroySetup(vDSP_biquadm_Setup setup);

vDSP_biquadm_Setup vDSP_biquadm_CreateSetup(const double* coefficients, size_t numChannels, size_t numLevels);

void vDSP_biquadm(vDSP_biquadm_Setup setup, const float* _Nonnull * _Nonnull input, size_t inputStride, float* _Nonnull * _Nonnull output, size_t outputStride, size_t count);

// square and sum elements of A into B.
//
// A is an array
// Astride is the stride for A
// result is a pointer to a floating point value for output
// count is the number of elements in A to process
void vDSP_svesq(const float* A, size_t Astride, float* result, size_t count);


// scalar multiply, scalar multiply, and add
//
// result[i] = b*A[i] + d*C[i];
void vDSP_vsmsma(const float* A, size_t Astride, const float* b, const float* C, size_t Cstride, const float* d, float* result, size_t resultStride, size_t count);


// fill destination with value
void vDSP_vfill(float* value, float* destination, size_t destinationStride, size_t count);

// create an arithmetic sequence
// destination[i] = i*interval + startVal;
void vDSP_vramp(const float* startVal, const float* interval, float* destination, size_t destinationStride, size_t count);

// vector, scalar addition
// C[i] = A[i] + b;
void vDSP_vsadd(const float* A, size_t Astride, const float* b, float* C, size_t Cstride, size_t count);

// vector, vector multiplication
// C[i] = A[i] * B[i];
void vDSP_vmul(const float* A, size_t Astride, const float* B, size_t Bstride,  float* result, size_t Cstride, size_t count);

// vector multiply and add
// D[i] = A[i]*B[i] + C[i];
void vDSP_vma(const float* A, size_t Astride, const float* B, size_t Bstride, const float* C, size_t Cstride, float* D, size_t Dstride, size_t count);

// vector clear
// A[i]=0
void vDSP_vclr(float* A, size_t Astride, size_t count);


// vector multiply, vector multiply, and add
// E[i] = A[i]*B[i] + C[i]*D[i]
void vDSP_vmma(const float* A, size_t Astride, const float* B, size_t Bstride, const float* C, size_t Cstride, const float* D, size_t Dstride, float* E, size_t Estride, size_t count);

// vector sum
// *result = total(A)
void vDSP_sve(const float* A, size_t Astride, float* result, size_t count);


// batch copy from vector of array indices
// B[i] = A[AIDX[i]];
void vDSP_vgathr(const float* A, const size_t* AIDX, size_t AIDXstride, float* B, size_t Bstride, size_t count);


// vector,vector add
// C[i] = A[i] + B[i];
void vDSP_vadd(const float* A, size_t Astride, const float* B, size_t Bstride,  float* C, size_t Cstride, size_t count);


// vector,vector subtract
// C[i] = A[i] - B[i];
void vDSP_vsub(const float* A, size_t Astride, const float* B, size_t Bstride,  float* C, size_t Cstride, size_t count);


// vector, scalar multiply
// C[i] = A[i] * b
void vDSP_vsmul(const float* A, size_t Astride, const float* b, float* C, size_t Cstride, size_t count);

#endif /* BMCrossPlatformVDSP_h */
