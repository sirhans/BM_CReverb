//
//  BMCrossPlatformVDSP.c
//  CReverb
//
//  Created by Hans on 23/2/16.
//  Copyright © 2016 Hans. All rights reserved.
//

#include "BMCrossPlatformVDSP.h"
#include <string.h>


void vDSP_svesq(const float* A, size_t Astride, float* result, size_t count){
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


void vDSP_vsmsma(const float* A, size_t Astride, const float* b, const float* C, size_t Cstride, const float* d, float* result, size_t resultStride, size_t count){
    
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

void vDSP_vfill(float* value, float* destination, size_t destinationStride, size_t count){
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


void vDSP_vramp(const float* startVal, const float* interval, float* destination, size_t destinationStride, size_t count){
    size_t i=0;
    float floatI = 0.0;
    while (count-- > 0) {
        destination[i] = floatI*(*interval) + (*startVal);
        i+=destinationStride;
        floatI++;
    }
}


void vDSP_vsadd(const float* A, size_t Astride, const float* b, float* C, size_t Cstride, size_t count){
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

void vDSP_vmul(const float* A, size_t Astride, const float* B, size_t Bstride,  float* C, size_t Cstride, size_t count){
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


void vDSP_vma(const float* A, size_t Astride, const float* B, size_t Bstride, const float* C, size_t Cstride, float* D, size_t Dstride, size_t count){
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

void vDSP_vmma(const float* A, size_t Astride, const float* B, size_t Bstride, const float* C, size_t Cstride, const float* D, size_t Dstride, float* E, size_t Estride, size_t count){
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



void vDSP_sve(const float* A, size_t Astride, float* result, size_t count){
    *result = 0;
    
    if (Astride==1) {
        for (size_t i=0; i<count; i++)
            *result += A[i];
    } else {
        for (size_t i=0; i<count*Astride; i+=Astride)
            *result += A[i];
    }
}


void vDSP_vclr(float* A, size_t Astride, size_t count){
    // if stride is 1
    if(Astride==1) memset(A,0,sizeof(float)*count);
    
    // if stride is not 1
    else {
        for (size_t i=0; i<count*Astride; i+=Astride)
            A[i]=0.0f;
    }
}


void vDSP_vgathr(const float* A, const size_t* AIDX, size_t AIDXstride, float* B, size_t Bstride, size_t count){
    // if strides are 1
    if(AIDXstride*Bstride==1)
        for (size_t i=0; i<count; i++)
            B[i] = A[AIDX[i]];
    
    // if any stride is not 1
    else {
        size_t ai=0, bi=0;
        while (count-- > 0) {
            B[bi] = A[AIDX[ai]];
            bi += Bstride;
            ai += AIDXstride;
        }
    }
}


void vDSP_vsub(const float* A, size_t Astride, const float* B, size_t Bstride,  float* C, size_t Cstride, size_t count){
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


void vDSP_vadd(const float* A, size_t Astride, const float* B, size_t Bstride,  float* C, size_t Cstride, size_t count){
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

void vDSP_vsmul(const float* A, size_t Astride, const float* b, float* C, size_t Cstride, size_t count){
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