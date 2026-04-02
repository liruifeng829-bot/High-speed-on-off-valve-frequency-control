/*
 * VVA_pressure_dynamic_basefreq_sin.c
 *
 * Code generation for model "VVA_pressure_dynamic_basefreq_sin".
 *
 * Model version              : 12.16
 * Simulink Coder version : 23.2 (R2023b) 01-Aug-2023
 * C source code generated on : Sun Jan  4 15:43:32 2026
 *
 * Target selection: grt.tlc
 * Note: GRT includes extra infrastructure and instrumentation for prototyping
 * Embedded hardware selection: Intel->x86-64 (Windows64)
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#include "VVA_pressure_dynamic_basefreq_sin.h"
#include "rtwtypes.h"
#include "VVA_pressure_dynamic_basefreq_sin_private.h"
#include <string.h>
#include <math.h>
#include "rt_nonfinite.h"

/* Block signals (default storage) */
B_VVA_pressure_dynamic_basefr_T VVA_pressure_dynamic_basefreq_B;

/* Continuous states */
X_VVA_pressure_dynamic_basefr_T VVA_pressure_dynamic_basefreq_X;

/* Disabled State Vector */
XDis_VVA_pressure_dynamic_bas_T VVA_pressure_dynamic_basef_XDis;

/* Block states (default storage) */
DW_VVA_pressure_dynamic_basef_T VVA_pressure_dynamic_basefre_DW;

/* Previous zero-crossings (trigger) states */
PrevZCX_VVA_pressure_dynamic__T VVA_pressure_dynamic_ba_PrevZCX;

/* Real-time model */
static RT_MODEL_VVA_pressure_dynamic_T VVA_pressure_dynamic_basefre_M_;
RT_MODEL_VVA_pressure_dynamic_T *const VVA_pressure_dynamic_basefre_M =
  &VVA_pressure_dynamic_basefre_M_;

/* Forward declaration for local functions */
static real_T VVA_pressure_dynamic_basef_rand(DW_u_VVA_pressure_dynamic_bas_T
  *localDW);
static void rate_scheduler(void);

/* n-D Spline interpolation function */
real_T look_SplNBinSZcd(uint32_T numDims, const real_T* u, const
  rt_LUTSplineWork * const SWork)
{
  /*
   *   n-D column-major table lookup operating on real_T with:
   *       - Spline interpolation
   *       - Spline extrapolation
   *       - Binary breakpoint search
   *       - Index search starts at the same place each time
   */
  rt_LUTnWork * const TWork_look = SWork->m_TWork;
  real_T* const fraction = (real_T*) TWork_look->m_bpLambda;
  uint32_T* const bpIdx = TWork_look->m_bpIndex;
  const uint32_T* const maxIndex = TWork_look->m_maxIndex;
  uint32_T k;
  for (k = 0U; k < numDims; k++) {
    const real_T* const bpData = ((const real_T * const *)
      TWork_look->m_bpDataSet)[k];
    bpIdx[k] = plook_binx(u[k], bpData, maxIndex[k], &fraction[k]);
  }

  return(intrp_NSplcd(numDims, SWork, 3U));
}

/*
 * Second derivative initialization function for spline
 * for last dimension.
 */
void rt_Spline2Derivd(const real_T *x, const real_T *y, uint32_T n, real_T *u,
                      real_T *y2)
{
  real_T p, qn, sig, un;
  uint32_T n1, i, k;
  n1 = n - 1U;
  y2[0U] = 0.0;
  u[0U] = 0.0;
  for (i = 1U; i < n1; i++) {
    real_T dxm1 = x[i] - x[i - 1U];
    real_T dxp1 = x[i + 1U] - x[i];
    real_T dxpm = dxp1 + dxm1;
    sig = dxm1 / dxpm;
    p = (sig * y2[i - 1U]) + 2.0;
    y2[i] = (sig - 1.0) / p;
    u[i] = ((y[i + 1U] - y[i]) / dxp1) - ((y[i] - y[i - 1U]) / dxm1);
    u[i] = (((6.0 * u[i]) / dxpm) - (sig * u[i - 1U])) / p;
  }

  qn = 0.0;
  un = 0.0;
  y2[n1] = (un - (qn * u[n1 - 1U])) / ((qn * y2[n1 - 1U]) + 1.0);
  for (k = n1; k > 0U; k--) {
    y2[k-1U] = (y2[k-1U] * y2[k]) + u[k-1U];
  }

  return;
}

/* n-D natural spline calculation function */
real_T intrp_NSplcd(uint32_T numDims, const rt_LUTSplineWork * const splWork,
                    uint32_T extrapMethod)
{
  uint32_T il;
  uint32_T iu, k, i;
  real_T h, s, p, smsq, pmsq;

  /* intermediate results work areas "this" and "next" */
  const rt_LUTnWork *TWork_interp = (const rt_LUTnWork *)splWork->m_TWork;
  const real_T *fraction = (real_T *) TWork_interp->m_bpLambda;
  const real_T *yp = (real_T *) TWork_interp->m_tableData;
  real_T *yyA = (real_T *) splWork->m_yyA;
  real_T *yyB = (real_T *) splWork->m_yyB;
  real_T *yy2 = (real_T *) splWork->m_yy2;
  real_T *up = (real_T *) splWork->m_up;
  real_T *y2 = (real_T *) splWork->m_y2;
  uint8_T* reCalc = splWork->m_reCalc;
  real_T *dp = (real_T *) splWork->m_preBp0AndTable;
  const real_T **bpDataSet = (const real_T **) TWork_interp->m_bpDataSet;
  const real_T *xp = bpDataSet[0U];
  real_T *yy = yyA;
  uint32_T bufBank = 0U;
  uint32_T len = TWork_interp->m_maxIndex[0U] + 1U;

  /* compare bp0 and table to see whether they get changed */
  {
    /* compare the bp0 data */
    if (memcmp(dp, xp,
               len * sizeof(real_T)) != 0) {
      *reCalc = 1;
      (void) memcpy(dp, xp,
                    len * sizeof(real_T));
    }

    /* compare the table data */
    dp = &(dp[len]);
    if (memcmp(dp, yp,
               len * splWork->m_numYWorkElts[0U] * sizeof(real_T)) != 0) {
      *reCalc = 1;
      (void) memcpy(dp, yp,
                    len * splWork->m_numYWorkElts[0U] * sizeof(real_T));
    }
  }

  if (*reCalc == 1) {
    /* If table and bps are tunable calculate 1st dim 2nd deriv */
    /* Generate first dimension's second derivatives */
    for (i = 0U; i < splWork->m_numYWorkElts[0U]; i++) {
      rt_Spline2Derivd(xp, yp, len, up, y2);
      yp = &yp[len];
      y2 = &y2[len];
    }

    /* Set pointers back to beginning */
    yp = (const real_T *) TWork_interp->m_tableData;
    y2 = (real_T *) splWork->m_y2;
  }

  *reCalc = 0;

  /* Generate at-point splines in each dimension */
  for (k = 0U; k < numDims; k++ ) {
    /* this dimension's input setup */
    xp = bpDataSet[k];
    len = TWork_interp->m_maxIndex[k] + 1U;
    il = TWork_interp->m_bpIndex[k];
    iu = il + 1U;
    h = xp[iu] - xp[il];
    p = fraction[k];
    s = 1.0 - p;
    pmsq = p * ((p*p) - 1.0);
    smsq = s * ((s*s) - 1.0);

    /*
     * Calculate spline curves for input in this
     * dimension at each value of the higher
     * other dimensions\' points in the table.
     */
    if ((p > 1.0) && (extrapMethod == 2U) ) {
      real_T slope;
      for (i = 0U; i < splWork->m_numYWorkElts[k]; i++) {
        slope = (yp[iu] - yp[il]) + ((y2[il]*h*h)*(1.0/6.0));
        yy[i] = yp[iu] + (slope * (p-1.0));
        yp = &yp[len];
        y2 = &y2[len];
      }
    } else if ((p < 0.0) && (extrapMethod == 2U) ) {
      real_T slope;
      for (i = 0U; i < splWork->m_numYWorkElts[k]; i++) {
        slope = (yp[iu] - yp[il]) - ((y2[iu]*h*h)*(1.0/6.0));
        yy[i] = yp[il] + (slope * p);
        yp = &yp[len];
        y2 = &y2[len];
      }
    } else {
      for (i = 0U; i < splWork->m_numYWorkElts[k]; i++) {
        yy[i] = yp[il] + p * (yp[iu] - yp[il]) +
          ((smsq * y2[il] + pmsq * y2[iu])*h*h)*(1.0/6.0);
        yp = &yp[len];
        y2 = &y2[len];
      }
    }

    /* set pointers to new result and calculate second derivatives */
    yp = yy;
    y2 = yy2;
    if (splWork->m_numYWorkElts[k+1U] > 0U ) {
      uint32_T nextLen = TWork_interp->m_maxIndex[k+1U] + 1U;
      const real_T *nextXp = bpDataSet[k+1U];
      for (i = 0U; i < splWork->m_numYWorkElts[k+1U]; i++) {
        rt_Spline2Derivd(nextXp, yp, nextLen, up, y2);
        yp = &yp[nextLen];
        y2 = &y2[nextLen];
      }
    }

    /*
     * Set work vectors yp, y2 and yy for next iteration;
     * the yy just calculated becomes the yp in the
     * next iteration, y2 was just calculated for these
     * new points and the yy buffer is swapped to the space
     * for storing the next iteration\'s results.
     */
    yp = yy;
    y2 = yy2;

    /*
     * Swap buffers for next dimension and
     * toggle bufBank for next iteration.
     */
    if (bufBank == 0U) {
      yy = yyA;
      bufBank = 1U;
    } else {
      yy = yyB;
      bufBank = 0U;
    }
  }

  return( yp[0U] );
}

uint32_T plook_binx(real_T u, const real_T bp[], uint32_T maxIndex, real_T
                    *fraction)
{
  uint32_T bpIndex;

  /* Prelookup - Index and Fraction
     Index Search method: 'binary'
     Extrapolation method: 'Linear'
     Use previous index: 'off'
     Use last breakpoint for index at or above upper limit: 'off'
     Remove protection against out-of-range input in generated code: 'off'
   */
  if (u <= bp[0U]) {
    bpIndex = 0U;
    *fraction = (u - bp[0U]) / (bp[1U] - bp[0U]);
  } else if (u < bp[maxIndex]) {
    bpIndex = binsearch_u32d(u, bp, maxIndex >> 1U, maxIndex);
    *fraction = (u - bp[bpIndex]) / (bp[bpIndex + 1U] - bp[bpIndex]);
  } else {
    bpIndex = maxIndex - 1U;
    *fraction = (u - bp[maxIndex - 1U]) / (bp[maxIndex] - bp[maxIndex - 1U]);
  }

  return bpIndex;
}

uint32_T binsearch_u32d(real_T u, const real_T bp[], uint32_T startIndex,
  uint32_T maxIndex)
{
  uint32_T bpIdx;
  uint32_T bpIndex;
  uint32_T iRght;

  /* Binary Search */
  bpIdx = startIndex;
  bpIndex = 0U;
  iRght = maxIndex;
  while (iRght - bpIndex > 1U) {
    if (u < bp[bpIdx]) {
      iRght = bpIdx;
    } else {
      bpIndex = bpIdx;
    }

    bpIdx = (iRght + bpIndex) >> 1U;
  }

  return bpIndex;
}

/*
 *         This function updates active task flag for each subrate.
 *         The function is called at model base rate, hence the
 *         generated code self-manages all its subrates.
 */
static void rate_scheduler(void)
{
  /* Compute which subrates run during the next base time step.  Subrates
   * are an integer multiple of the base rate counter.  Therefore, the subtask
   * counter is reset when it reaches its limit (zero means run).
   */
  (VVA_pressure_dynamic_basefre_M->Timing.TaskCounters.TID[2])++;
  if ((VVA_pressure_dynamic_basefre_M->Timing.TaskCounters.TID[2]) > 1) {/* Sample time: [0.001s, 0.0s] */
    VVA_pressure_dynamic_basefre_M->Timing.TaskCounters.TID[2] = 0;
  }

  (VVA_pressure_dynamic_basefre_M->Timing.TaskCounters.TID[3])++;
  if ((VVA_pressure_dynamic_basefre_M->Timing.TaskCounters.TID[3]) > 5) {/* Sample time: [0.003s, 0.0s] */
    VVA_pressure_dynamic_basefre_M->Timing.TaskCounters.TID[3] = 0;
  }

  (VVA_pressure_dynamic_basefre_M->Timing.TaskCounters.TID[4])++;
  if ((VVA_pressure_dynamic_basefre_M->Timing.TaskCounters.TID[4]) > 1999) {/* Sample time: [1.0s, 0.0s] */
    VVA_pressure_dynamic_basefre_M->Timing.TaskCounters.TID[4] = 0;
  }
}

/*
 * This function updates continuous states using the ODE3 fixed-step
 * solver algorithm
 */
static void rt_ertODEUpdateContinuousStates(RTWSolverInfo *si )
{
  /* Solver Matrices */
  static const real_T rt_ODE3_A[3] = {
    1.0/2.0, 3.0/4.0, 1.0
  };

  static const real_T rt_ODE3_B[3][3] = {
    { 1.0/2.0, 0.0, 0.0 },

    { 0.0, 3.0/4.0, 0.0 },

    { 2.0/9.0, 1.0/3.0, 4.0/9.0 }
  };

  time_T t = rtsiGetT(si);
  time_T tnew = rtsiGetSolverStopTime(si);
  time_T h = rtsiGetStepSize(si);
  real_T *x = rtsiGetContStates(si);
  ODE3_IntgData *id = (ODE3_IntgData *)rtsiGetSolverData(si);
  real_T *y = id->y;
  real_T *f0 = id->f[0];
  real_T *f1 = id->f[1];
  real_T *f2 = id->f[2];
  real_T hB[3];
  int_T i;
  int_T nXc = 3;
  rtsiSetSimTimeStep(si,MINOR_TIME_STEP);

  /* Save the state values at time t in y, we'll use x as ynew. */
  (void) memcpy(y, x,
                (uint_T)nXc*sizeof(real_T));

  /* Assumes that rtsiSetT and ModelOutputs are up-to-date */
  /* f0 = f(t,y) */
  rtsiSetdX(si, f0);
  VVA_pressure_dynamic_basefreq_sin_derivatives();

  /* f(:,2) = feval(odefile, t + hA(1), y + f*hB(:,1), args(:)(*)); */
  hB[0] = h * rt_ODE3_B[0][0];
  for (i = 0; i < nXc; i++) {
    x[i] = y[i] + (f0[i]*hB[0]);
  }

  rtsiSetT(si, t + h*rt_ODE3_A[0]);
  rtsiSetdX(si, f1);
  VVA_pressure_dynamic_basefreq_sin_step();
  VVA_pressure_dynamic_basefreq_sin_derivatives();

  /* f(:,3) = feval(odefile, t + hA(2), y + f*hB(:,2), args(:)(*)); */
  for (i = 0; i <= 1; i++) {
    hB[i] = h * rt_ODE3_B[1][i];
  }

  for (i = 0; i < nXc; i++) {
    x[i] = y[i] + (f0[i]*hB[0] + f1[i]*hB[1]);
  }

  rtsiSetT(si, t + h*rt_ODE3_A[1]);
  rtsiSetdX(si, f2);
  VVA_pressure_dynamic_basefreq_sin_step();
  VVA_pressure_dynamic_basefreq_sin_derivatives();

  /* tnew = t + hA(3);
     ynew = y + f*hB(:,3); */
  for (i = 0; i <= 2; i++) {
    hB[i] = h * rt_ODE3_B[2][i];
  }

  for (i = 0; i < nXc; i++) {
    x[i] = y[i] + (f0[i]*hB[0] + f1[i]*hB[1] + f2[i]*hB[2]);
  }

  rtsiSetT(si, tnew);
  rtsiSetSimTimeStep(si,MAJOR_TIME_STEP);
}

/* Function for MATLAB Function: '<S26>/ 电磁阀滞后' */
static real_T VVA_pressure_dynamic_basef_rand(DW_u_VVA_pressure_dynamic_bas_T
  *localDW)
{
  real_T r;
  int32_T k;
  int32_T kk;
  uint32_T u[2];
  if (localDW->method == 4U) {
    uint32_T mti;
    uint32_T y;
    k = (int32_T)(localDW->state / 127773U);
    mti = (localDW->state - (uint32_T)k * 127773U) * 16807U;
    y = 2836U * (uint32_T)k;
    if (mti < y) {
      mti = ~(y - mti) & 2147483647U;
    } else {
      mti -= y;
    }

    r = (real_T)mti * 4.6566128752457969E-10;
    localDW->state = mti;
  } else if (localDW->method == 5U) {
    uint32_T mti;
    uint32_T y;
    mti = 69069U * localDW->state_g[0] + 1234567U;
    y = localDW->state_g[1] << 13 ^ localDW->state_g[1];
    y ^= y >> 17;
    y ^= y << 5;
    localDW->state_g[0] = mti;
    localDW->state_g[1] = y;
    r = (real_T)(mti + y) * 2.328306436538696E-10;
  } else {
    /* ========================= COPYRIGHT NOTICE ============================ */
    /*  This is a uniform (0,1) pseudorandom number generator based on:        */
    /*                                                                         */
    /*  A C-program for MT19937, with initialization improved 2002/1/26.       */
    /*  Coded by Takuji Nishimura and Makoto Matsumoto.                        */
    /*                                                                         */
    /*  Copyright (C) 1997 - 2002, Makoto Matsumoto and Takuji Nishimura,      */
    /*  All rights reserved.                                                   */
    /*                                                                         */
    /*  Redistribution and use in source and binary forms, with or without     */
    /*  modification, are permitted provided that the following conditions     */
    /*  are met:                                                               */
    /*                                                                         */
    /*    1. Redistributions of source code must retain the above copyright    */
    /*       notice, this list of conditions and the following disclaimer.     */
    /*                                                                         */
    /*    2. Redistributions in binary form must reproduce the above copyright */
    /*       notice, this list of conditions and the following disclaimer      */
    /*       in the documentation and/or other materials provided with the     */
    /*       distribution.                                                     */
    /*                                                                         */
    /*    3. The names of its contributors may not be used to endorse or       */
    /*       promote products derived from this software without specific      */
    /*       prior written permission.                                         */
    /*                                                                         */
    /*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS    */
    /*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT      */
    /*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR  */
    /*  A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT  */
    /*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,  */
    /*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT       */
    /*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,  */
    /*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY  */
    /*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT    */
    /*  (INCLUDING  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE */
    /*  OF THIS  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  */
    /*                                                                         */
    /* =============================   END   ================================= */
    int32_T exitg1;
    do {
      uint32_T mti;
      exitg1 = 0;
      for (k = 0; k < 2; k++) {
        uint32_T y;
        mti = localDW->state_h[624] + 1U;
        if (localDW->state_h[624] + 1U >= 625U) {
          for (kk = 0; kk < 227; kk++) {
            mti = (localDW->state_h[kk + 1] & 2147483647U) | (localDW->
              state_h[kk] & 2147483648U);
            if ((mti & 1U) == 0U) {
              mti >>= 1U;
            } else {
              mti = mti >> 1U ^ 2567483615U;
            }

            localDW->state_h[kk] = localDW->state_h[kk + 397] ^ mti;
          }

          for (kk = 0; kk < 396; kk++) {
            mti = (localDW->state_h[kk + 227] & 2147483648U) | (localDW->
              state_h[kk + 228] & 2147483647U);
            if ((mti & 1U) == 0U) {
              mti >>= 1U;
            } else {
              mti = mti >> 1U ^ 2567483615U;
            }

            localDW->state_h[kk + 227] = localDW->state_h[kk] ^ mti;
          }

          mti = (localDW->state_h[623] & 2147483648U) | (localDW->state_h[0] &
            2147483647U);
          if ((mti & 1U) == 0U) {
            mti >>= 1U;
          } else {
            mti = mti >> 1U ^ 2567483615U;
          }

          localDW->state_h[623] = localDW->state_h[396] ^ mti;
          mti = 1U;
        }

        y = localDW->state_h[(int32_T)mti - 1];
        localDW->state_h[624] = mti;
        y ^= y >> 11U;
        y ^= y << 7U & 2636928640U;
        y ^= y << 15U & 4022730752U;
        u[k] = y >> 18U ^ y;
      }

      r = ((real_T)(u[0] >> 5U) * 6.7108864E+7 + (real_T)(u[1] >> 6U)) *
        1.1102230246251565E-16;
      if (r == 0.0) {
        boolean_T b_isvalid;
        if ((localDW->state_h[624] >= 1U) && (localDW->state_h[624] < 625U)) {
          boolean_T exitg2;
          b_isvalid = false;
          k = 1;
          exitg2 = false;
          while ((!exitg2) && (k < 625)) {
            if (localDW->state_h[k - 1] == 0U) {
              k++;
            } else {
              b_isvalid = true;
              exitg2 = true;
            }
          }
        } else {
          b_isvalid = false;
        }

        if (!b_isvalid) {
          mti = 5489U;
          localDW->state_h[0] = 5489U;
          for (k = 0; k < 623; k++) {
            mti = ((mti >> 30U ^ mti) * 1812433253U + (uint32_T)k) + 1U;
            localDW->state_h[k + 1] = mti;
          }

          localDW->state_h[624] = 624U;
        }
      } else {
        exitg1 = 1;
      }
    } while (exitg1 == 0);
  }

  return r;
}

/*
 * System initialize for atomic system:
 *    '<S26>/ 电磁阀滞后'
 *    '<S26>/ 电磁阀滞后2'
 */
void VVA_pressure_dynamic_bas_u_Init(DW_u_VVA_pressure_dynamic_bas_T *localDW)
{
  static const uint32_T tmp[625] = { 5489U, 1301868182U, 2938499221U,
    2950281878U, 1875628136U, 751856242U, 944701696U, 2243192071U, 694061057U,
    219885934U, 2066767472U, 3182869408U, 485472502U, 2336857883U, 1071588843U,
    3418470598U, 951210697U, 3693558366U, 2923482051U, 1793174584U, 2982310801U,
    1586906132U, 1951078751U, 1808158765U, 1733897588U, 431328322U, 4202539044U,
    530658942U, 1714810322U, 3025256284U, 3342585396U, 1937033938U, 2640572511U,
    1654299090U, 3692403553U, 4233871309U, 3497650794U, 862629010U, 2943236032U,
    2426458545U, 1603307207U, 1133453895U, 3099196360U, 2208657629U, 2747653927U,
    931059398U, 761573964U, 3157853227U, 785880413U, 730313442U, 124945756U,
    2937117055U, 3295982469U, 1724353043U, 3021675344U, 3884886417U, 4010150098U,
    4056961966U, 699635835U, 2681338818U, 1339167484U, 720757518U, 2800161476U,
    2376097373U, 1532957371U, 3902664099U, 1238982754U, 3725394514U, 3449176889U,
    3570962471U, 4287636090U, 4087307012U, 3603343627U, 202242161U, 2995682783U,
    1620962684U, 3704723357U, 371613603U, 2814834333U, 2111005706U, 624778151U,
    2094172212U, 4284947003U, 1211977835U, 991917094U, 1570449747U, 2962370480U,
    1259410321U, 170182696U, 146300961U, 2836829791U, 619452428U, 2723670296U,
    1881399711U, 1161269684U, 1675188680U, 4132175277U, 780088327U, 3409462821U,
    1036518241U, 1834958505U, 3048448173U, 161811569U, 618488316U, 44795092U,
    3918322701U, 1924681712U, 3239478144U, 383254043U, 4042306580U, 2146983041U,
    3992780527U, 3518029708U, 3545545436U, 3901231469U, 1896136409U, 2028528556U,
    2339662006U, 501326714U, 2060962201U, 2502746480U, 561575027U, 581893337U,
    3393774360U, 1778912547U, 3626131687U, 2175155826U, 319853231U, 986875531U,
    819755096U, 2915734330U, 2688355739U, 3482074849U, 2736559U, 2296975761U,
    1029741190U, 2876812646U, 690154749U, 579200347U, 4027461746U, 1285330465U,
    2701024045U, 4117700889U, 759495121U, 3332270341U, 2313004527U, 2277067795U,
    4131855432U, 2722057515U, 1264804546U, 3848622725U, 2211267957U, 4100593547U,
    959123777U, 2130745407U, 3194437393U, 486673947U, 1377371204U, 17472727U,
    352317554U, 3955548058U, 159652094U, 1232063192U, 3835177280U, 49423123U,
    3083993636U, 733092U, 2120519771U, 2573409834U, 1112952433U, 3239502554U,
    761045320U, 1087580692U, 2540165110U, 641058802U, 1792435497U, 2261799288U,
    1579184083U, 627146892U, 2165744623U, 2200142389U, 2167590760U, 2381418376U,
    1793358889U, 3081659520U, 1663384067U, 2009658756U, 2689600308U, 739136266U,
    2304581039U, 3529067263U, 591360555U, 525209271U, 3131882996U, 294230224U,
    2076220115U, 3113580446U, 1245621585U, 1386885462U, 3203270426U, 123512128U,
    12350217U, 354956375U, 4282398238U, 3356876605U, 3888857667U, 157639694U,
    2616064085U, 1563068963U, 2762125883U, 4045394511U, 4180452559U, 3294769488U,
    1684529556U, 1002945951U, 3181438866U, 22506664U, 691783457U, 2685221343U,
    171579916U, 3878728600U, 2475806724U, 2030324028U, 3331164912U, 1708711359U,
    1970023127U, 2859691344U, 2588476477U, 2748146879U, 136111222U, 2967685492U,
    909517429U, 2835297809U, 3206906216U, 3186870716U, 341264097U, 2542035121U,
    3353277068U, 548223577U, 3170936588U, 1678403446U, 297435620U, 2337555430U,
    466603495U, 1132321815U, 1208589219U, 696392160U, 894244439U, 2562678859U,
    470224582U, 3306867480U, 201364898U, 2075966438U, 1767227936U, 2929737987U,
    3674877796U, 2654196643U, 3692734598U, 3528895099U, 2796780123U, 3048728353U,
    842329300U, 191554730U, 2922459673U, 3489020079U, 3979110629U, 1022523848U,
    2202932467U, 3583655201U, 3565113719U, 587085778U, 4176046313U, 3013713762U,
    950944241U, 396426791U, 3784844662U, 3477431613U, 3594592395U, 2782043838U,
    3392093507U, 3106564952U, 2829419931U, 1358665591U, 2206918825U, 3170783123U,
    31522386U, 2988194168U, 1782249537U, 1105080928U, 843500134U, 1225290080U,
    1521001832U, 3605886097U, 2802786495U, 2728923319U, 3996284304U, 903417639U,
    1171249804U, 1020374987U, 2824535874U, 423621996U, 1988534473U, 2493544470U,
    1008604435U, 1756003503U, 1488867287U, 1386808992U, 732088248U, 1780630732U,
    2482101014U, 976561178U, 1543448953U, 2602866064U, 2021139923U, 1952599828U,
    2360242564U, 2117959962U, 2753061860U, 2388623612U, 4138193781U, 2962920654U,
    2284970429U, 766920861U, 3457264692U, 2879611383U, 815055854U, 2332929068U,
    1254853997U, 3740375268U, 3799380844U, 4091048725U, 2006331129U, 1982546212U,
    686850534U, 1907447564U, 2682801776U, 2780821066U, 998290361U, 1342433871U,
    4195430425U, 607905174U, 3902331779U, 2454067926U, 1708133115U, 1170874362U,
    2008609376U, 3260320415U, 2211196135U, 433538229U, 2728786374U, 2189520818U,
    262554063U, 1182318347U, 3710237267U, 1221022450U, 715966018U, 2417068910U,
    2591870721U, 2870691989U, 3418190842U, 4238214053U, 1540704231U, 1575580968U,
    2095917976U, 4078310857U, 2313532447U, 2110690783U, 4056346629U, 4061784526U,
    1123218514U, 551538993U, 597148360U, 4120175196U, 3581618160U, 3181170517U,
    422862282U, 3227524138U, 1713114790U, 662317149U, 1230418732U, 928171837U,
    1324564878U, 1928816105U, 1786535431U, 2878099422U, 3290185549U, 539474248U,
    1657512683U, 552370646U, 1671741683U, 3655312128U, 1552739510U, 2605208763U,
    1441755014U, 181878989U, 3124053868U, 1447103986U, 3183906156U, 1728556020U,
    3502241336U, 3055466967U, 1013272474U, 818402132U, 1715099063U, 2900113506U,
    397254517U, 4194863039U, 1009068739U, 232864647U, 2540223708U, 2608288560U,
    2415367765U, 478404847U, 3455100648U, 3182600021U, 2115988978U, 434269567U,
    4117179324U, 3461774077U, 887256537U, 3545801025U, 286388911U, 3451742129U,
    1981164769U, 786667016U, 3310123729U, 3097811076U, 2224235657U, 2959658883U,
    3370969234U, 2514770915U, 3345656436U, 2677010851U, 2206236470U, 271648054U,
    2342188545U, 4292848611U, 3646533909U, 3754009956U, 3803931226U, 4160647125U,
    1477814055U, 4043852216U, 1876372354U, 3133294443U, 3871104810U, 3177020907U,
    2074304428U, 3479393793U, 759562891U, 164128153U, 1839069216U, 2114162633U,
    3989947309U, 3611054956U, 1333547922U, 835429831U, 494987340U, 171987910U,
    1252001001U, 370809172U, 3508925425U, 2535703112U, 1276855041U, 1922855120U,
    835673414U, 3030664304U, 613287117U, 171219893U, 3423096126U, 3376881639U,
    2287770315U, 1658692645U, 1262815245U, 3957234326U, 1168096164U, 2968737525U,
    2655813712U, 2132313144U, 3976047964U, 326516571U, 353088456U, 3679188938U,
    3205649712U, 2654036126U, 1249024881U, 880166166U, 691800469U, 2229503665U,
    1673458056U, 4032208375U, 1851778863U, 2563757330U, 376742205U, 1794655231U,
    340247333U, 1505873033U, 396524441U, 879666767U, 3335579166U, 3260764261U,
    3335999539U, 506221798U, 4214658741U, 975887814U, 2080536343U, 3360539560U,
    571586418U, 138896374U, 4234352651U, 2737620262U, 3928362291U, 1516365296U,
    38056726U, 3599462320U, 3585007266U, 3850961033U, 471667319U, 1536883193U,
    2310166751U, 1861637689U, 2530999841U, 4139843801U, 2710569485U, 827578615U,
    2012334720U, 2907369459U, 3029312804U, 2820112398U, 1965028045U, 35518606U,
    2478379033U, 643747771U, 1924139484U, 4123405127U, 3811735531U, 3429660832U,
    3285177704U, 1948416081U, 1311525291U, 1183517742U, 1739192232U, 3979815115U,
    2567840007U, 4116821529U, 213304419U, 4125718577U, 1473064925U, 2442436592U,
    1893310111U, 4195361916U, 3747569474U, 828465101U, 2991227658U, 750582866U,
    1205170309U, 1409813056U, 678418130U, 1171531016U, 3821236156U, 354504587U,
    4202874632U, 3882511497U, 1893248677U, 1903078632U, 26340130U, 2069166240U,
    3657122492U, 3725758099U, 831344905U, 811453383U, 3447711422U, 2434543565U,
    4166886888U, 3358210805U, 4142984013U, 2988152326U, 3527824853U, 982082992U,
    2809155763U, 190157081U, 3340214818U, 2365432395U, 2548636180U, 2894533366U,
    3474657421U, 2372634704U, 2845748389U, 43024175U, 2774226648U, 1987702864U,
    3186502468U, 453610222U, 4204736567U, 1392892630U, 2471323686U, 2470534280U,
    3541393095U, 4269885866U, 3909911300U, 759132955U, 1482612480U, 667715263U,
    1795580598U, 2337923983U, 3390586366U, 581426223U, 1515718634U, 476374295U,
    705213300U, 363062054U, 2084697697U, 2407503428U, 2292957699U, 2426213835U,
    2199989172U, 1987356470U, 4026755612U, 2147252133U, 270400031U, 1367820199U,
    2369854699U, 2844269403U, 79981964U, 624U };

  memcpy(&localDW->state_h[0], &tmp[0], 625U * sizeof(uint32_T));
  localDW->method = 7U;
  localDW->state = 1144108930U;
  localDW->state_g[0] = 362436069U;
  localDW->state_g[1] = 521288629U;
}

/*
 * Output and update for atomic system:
 *    '<S26>/ 电磁阀滞后'
 *    '<S26>/ 电磁阀滞后2'
 */
void VVA_pressure_dynamic_basefreq_u(real_T rtu_current, real_T rtu_d,
  B_u_VVA_pressure_dynamic_base_T *localB, DW_u_VVA_pressure_dynamic_bas_T
  *localDW)
{
  if (rtu_current == 0.0) {
    if (VVA_pressure_dynamic_basef_rand(localDW) <= rtu_d) {
      localB->next = 1.0;
    } else {
      localB->next = 0.0;
    }
  } else if (VVA_pressure_dynamic_basef_rand(localDW) <= 1.0 - rtu_d) {
    localB->next = 0.0;
  } else {
    localB->next = 1.0;
  }
}

/* Model step function */
void VVA_pressure_dynamic_basefreq_sin_step(void)
{
  /* local block i/o variables */
  real_T rtb_PulseGenerator;
  uint8_T rtb_FixPtSwitch;
  if (rtmIsMajorTimeStep(VVA_pressure_dynamic_basefre_M)) {
    /* set solver stop time */
    if (!(VVA_pressure_dynamic_basefre_M->Timing.clockTick0+1)) {
      rtsiSetSolverStopTime(&VVA_pressure_dynamic_basefre_M->solverInfo,
                            ((VVA_pressure_dynamic_basefre_M->Timing.clockTickH0
        + 1) * VVA_pressure_dynamic_basefre_M->Timing.stepSize0 * 4294967296.0));
    } else {
      rtsiSetSolverStopTime(&VVA_pressure_dynamic_basefre_M->solverInfo,
                            ((VVA_pressure_dynamic_basefre_M->Timing.clockTick0
        + 1) * VVA_pressure_dynamic_basefre_M->Timing.stepSize0 +
        VVA_pressure_dynamic_basefre_M->Timing.clockTickH0 *
        VVA_pressure_dynamic_basefre_M->Timing.stepSize0 * 4294967296.0));
    }
  }                                    /* end MajorTimeStep */

  /* Update absolute time of base rate at minor time step */
  if (rtmIsMinorTimeStep(VVA_pressure_dynamic_basefre_M)) {
    VVA_pressure_dynamic_basefre_M->Timing.t[0] = rtsiGetT
      (&VVA_pressure_dynamic_basefre_M->solverInfo);
  }

  {
    real_T outputMFCache[505];
    real_T rtb_aggregatedOutputs[101];
    real_T rtb_antecedentOutputs[49];
    real_T inputMFCache[14];
    real_T A;
    real_T D;
    real_T E;
    real_T d_min;
    real_T rtb_Derivative1;
    real_T rtb_Saturation;
    real_T rtb_TmpSignalConversionAtSFun_0;
    real_T rtb_TmpSignalConversionAtSFun_1;
    real_T rtb_dm;
    real_T *lastU;
    int32_T f_cmd;
    int32_T inputID;
    uint8_T rtb_Output;
    boolean_T cross;
    static const int8_T b[98] = { 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 3, 3,
      3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6,
      6, 7, 7, 7, 7, 7, 7, 7, 1, 2, 3, 4, 5, 6, 7, 1, 2, 3, 4, 5, 6, 7, 1, 2, 3,
      4, 5, 6, 7, 1, 2, 3, 4, 5, 6, 7, 1, 2, 3, 4, 5, 6, 7, 1, 2, 3, 4, 5, 6, 7,
      1, 2, 3, 4, 5, 6, 7 };

    static const int8_T b_0[49] = { 4, 4, 5, 5, 5, 4, 4, 2, 3, 3, 4, 3, 3, 2, 1,
      2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 1, 2, 3, 3, 4, 3,
      3, 2, 4, 4, 5, 5, 5, 4, 4 };

    /* Integrator: '<Root>/设置压力起始值' */
    VVA_pressure_dynamic_basefreq_B.u = VVA_pressure_dynamic_basefreq_X._CSTATE;
    if (rtmIsMajorTimeStep(VVA_pressure_dynamic_basefre_M) &&
        VVA_pressure_dynamic_basefre_M->Timing.TaskCounters.TID[4] == 0) {
      /* UnitDelay: '<S33>/Output' */
      rtb_Output = VVA_pressure_dynamic_basefre_DW.Output_DSTATE;

      /* MultiPortSwitch: '<S18>/Output' incorporates:
       *  Constant: '<S18>/Vector'
       *  UnitDelay: '<S33>/Output'
       */
      VVA_pressure_dynamic_basefreq_B.Output =
        VVA_pressure_dynamic_basefreq_P.RepeatingSequenceStair2_OutValu[VVA_pressure_dynamic_basefre_DW.Output_DSTATE];
    }

    /* Clock: '<Root>/Clock5' incorporates:
     *  Clock: '<S26>/Clock'
     *  Clock: '<S26>/Clock2'
     *  Derivative: '<Root>/Derivative'
     *  Derivative: '<Root>/Derivative1'
     *  Derivative: '<Root>/Derivative2'
     */
    rtb_dm = VVA_pressure_dynamic_basefre_M->Timing.t[0];

    /* ManualSwitch: '<Root>/Manual Switch1' incorporates:
     *  ManualSwitch: '<Root>/Manual Switch6'
     *  Step: '<Root>/Step'
     */
    if (VVA_pressure_dynamic_basefreq_P.ManualSwitch1_CurrentSetting == 1) {
      /* ManualSwitch: '<Root>/Manual Switch' incorporates:
       *  ManualSwitch: '<Root>/Manual Switch14'
       */
      if (VVA_pressure_dynamic_basefreq_P.ManualSwitch_CurrentSetting == 1) {
        /* ManualSwitch: '<Root>/Manual Switch1' */
        VVA_pressure_dynamic_basefreq_B.ManualSwitch1 =
          VVA_pressure_dynamic_basefreq_B.Output;
      } else if (VVA_pressure_dynamic_basefreq_P.ManualSwitch14_CurrentSetting ==
                 1) {
        /* ManualSwitch: '<Root>/Manual Switch3' incorporates:
         *  ManualSwitch: '<Root>/Manual Switch14'
         */
        if (VVA_pressure_dynamic_basefreq_P.ManualSwitch3_CurrentSetting == 1) {
          /* ManualSwitch: '<Root>/Manual Switch1' incorporates:
           *  Sin: '<Root>/Sine Wave2'
           */
          VVA_pressure_dynamic_basefreq_B.ManualSwitch1 = sin
            (VVA_pressure_dynamic_basefreq_P.SineWave2_Freq *
             VVA_pressure_dynamic_basefre_M->Timing.t[0] +
             VVA_pressure_dynamic_basefreq_P.SineWave2_Phase) *
            VVA_pressure_dynamic_basefreq_P.SineWave2_Amp +
            VVA_pressure_dynamic_basefreq_P.SineWave2_Bias;
        } else {
          /* ManualSwitch: '<Root>/Manual Switch1' incorporates:
           *  Sin: '<Root>/Sine Wave1'
           */
          VVA_pressure_dynamic_basefreq_B.ManualSwitch1 = sin
            (VVA_pressure_dynamic_basefreq_P.SineWave1_Freq *
             VVA_pressure_dynamic_basefre_M->Timing.t[0] +
             VVA_pressure_dynamic_basefreq_P.SineWave1_Phase) *
            VVA_pressure_dynamic_basefreq_P.SineWave1_Amp +
            VVA_pressure_dynamic_basefreq_P.SineWave1_Bias;
        }

        /* End of ManualSwitch: '<Root>/Manual Switch3' */
      } else {
        /* ManualSwitch: '<Root>/Manual Switch1' incorporates:
         *  Clock: '<Root>/Clock5'
         *  MATLAB Function: '<Root>/MATLAB Function7'
         *  ManualSwitch: '<Root>/Manual Switch14'
         */
        VVA_pressure_dynamic_basefreq_B.ManualSwitch1 = fmin(fmax(((sin
          (25.132741228718345 * rtb_dm + 1.0) * 0.3 + sin(6.2831853071795862 *
          rtb_dm) * 0.6) + sin(75.398223686155035 * rtb_dm - 0.7) * 0.1) *
          100000.0 + 400000.0, 100000.0), 420000.0);
      }

      /* End of ManualSwitch: '<Root>/Manual Switch' */
    } else if (VVA_pressure_dynamic_basefreq_P.ManualSwitch6_CurrentSetting == 1)
    {
      /* ManualSwitch: '<Root>/Manual Switch6' incorporates:
       *  Constant: '<Root>/Constant1'
       *  ManualSwitch: '<Root>/Manual Switch1'
       */
      VVA_pressure_dynamic_basefreq_B.ManualSwitch1 =
        VVA_pressure_dynamic_basefreq_P.Constant1_Value;
    } else if (VVA_pressure_dynamic_basefre_M->Timing.t[0] <
               VVA_pressure_dynamic_basefreq_P.Step_Time) {
      /* Step: '<Root>/Step' incorporates:
       *  ManualSwitch: '<Root>/Manual Switch1'
       */
      VVA_pressure_dynamic_basefreq_B.ManualSwitch1 =
        VVA_pressure_dynamic_basefreq_P.Step_Y0;
    } else {
      /* ManualSwitch: '<Root>/Manual Switch1' incorporates:
       *  Step: '<Root>/Step'
       */
      VVA_pressure_dynamic_basefreq_B.ManualSwitch1 =
        VVA_pressure_dynamic_basefreq_P.Step_YFinal;
    }

    /* End of ManualSwitch: '<Root>/Manual Switch1' */

    /* Derivative: '<Root>/Derivative1' */
    if ((VVA_pressure_dynamic_basefre_DW.TimeStampA >= rtb_dm) &&
        (VVA_pressure_dynamic_basefre_DW.TimeStampB >= rtb_dm)) {
      rtb_Derivative1 = 0.0;
    } else {
      rtb_TmpSignalConversionAtSFun_0 =
        VVA_pressure_dynamic_basefre_DW.TimeStampA;
      lastU = &VVA_pressure_dynamic_basefre_DW.LastUAtTimeA;
      if (VVA_pressure_dynamic_basefre_DW.TimeStampA <
          VVA_pressure_dynamic_basefre_DW.TimeStampB) {
        if (VVA_pressure_dynamic_basefre_DW.TimeStampB < rtb_dm) {
          rtb_TmpSignalConversionAtSFun_0 =
            VVA_pressure_dynamic_basefre_DW.TimeStampB;
          lastU = &VVA_pressure_dynamic_basefre_DW.LastUAtTimeB;
        }
      } else if (VVA_pressure_dynamic_basefre_DW.TimeStampA >= rtb_dm) {
        rtb_TmpSignalConversionAtSFun_0 =
          VVA_pressure_dynamic_basefre_DW.TimeStampB;
        lastU = &VVA_pressure_dynamic_basefre_DW.LastUAtTimeB;
      }

      rtb_Derivative1 = (VVA_pressure_dynamic_basefreq_B.u - *lastU) / (rtb_dm -
        rtb_TmpSignalConversionAtSFun_0);
    }

    /* Derivative: '<Root>/Derivative' */
    if ((VVA_pressure_dynamic_basefre_DW.TimeStampA_h >= rtb_dm) &&
        (VVA_pressure_dynamic_basefre_DW.TimeStampB_a >= rtb_dm)) {
      rtb_TmpSignalConversionAtSFun_0 = 0.0;
    } else {
      rtb_TmpSignalConversionAtSFun_0 =
        VVA_pressure_dynamic_basefre_DW.TimeStampA_h;
      lastU = &VVA_pressure_dynamic_basefre_DW.LastUAtTimeA_o;
      if (VVA_pressure_dynamic_basefre_DW.TimeStampA_h <
          VVA_pressure_dynamic_basefre_DW.TimeStampB_a) {
        if (VVA_pressure_dynamic_basefre_DW.TimeStampB_a < rtb_dm) {
          rtb_TmpSignalConversionAtSFun_0 =
            VVA_pressure_dynamic_basefre_DW.TimeStampB_a;
          lastU = &VVA_pressure_dynamic_basefre_DW.LastUAtTimeB_n;
        }
      } else if (VVA_pressure_dynamic_basefre_DW.TimeStampA_h >= rtb_dm) {
        rtb_TmpSignalConversionAtSFun_0 =
          VVA_pressure_dynamic_basefre_DW.TimeStampB_a;
        lastU = &VVA_pressure_dynamic_basefre_DW.LastUAtTimeB_n;
      }

      rtb_TmpSignalConversionAtSFun_0 =
        (VVA_pressure_dynamic_basefreq_B.ManualSwitch1 - *lastU) / (rtb_dm -
        rtb_TmpSignalConversionAtSFun_0);
    }

    /* MATLAB Function: '<Root>/e' */
    VVA_pressure_dynamic_basefreq_B.e =
      VVA_pressure_dynamic_basefreq_B.ManualSwitch1 -
      VVA_pressure_dynamic_basefreq_B.u;
    VVA_pressure_dynamic_basefreq_B.de = rtb_TmpSignalConversionAtSFun_0 -
      rtb_Derivative1;

    /* Gain: '<Root>/Gain' */
    rtb_TmpSignalConversionAtSFun_0 = VVA_pressure_dynamic_basefreq_P.Gain_Gain *
      VVA_pressure_dynamic_basefreq_B.e;

    /* Saturate: '<Root>/Saturation6' */
    if (rtb_TmpSignalConversionAtSFun_0 >
        VVA_pressure_dynamic_basefreq_P.Saturation6_UpperSat) {
      /* Outputs for Atomic SubSystem: '<Root>/Fuzzy Logic  Controller1' */
      /* SignalConversion generated from: '<S29>/ SFunction ' incorporates:
       *  MATLAB Function: '<S13>/Evaluate Rule Antecedents'
       */
      rtb_TmpSignalConversionAtSFun_0 =
        VVA_pressure_dynamic_basefreq_P.Saturation6_UpperSat;

      /* End of Outputs for SubSystem: '<Root>/Fuzzy Logic  Controller1' */
    } else if (rtb_TmpSignalConversionAtSFun_0 <
               VVA_pressure_dynamic_basefreq_P.Saturation6_LowerSat) {
      /* Outputs for Atomic SubSystem: '<Root>/Fuzzy Logic  Controller1' */
      /* SignalConversion generated from: '<S29>/ SFunction ' incorporates:
       *  MATLAB Function: '<S13>/Evaluate Rule Antecedents'
       */
      rtb_TmpSignalConversionAtSFun_0 =
        VVA_pressure_dynamic_basefreq_P.Saturation6_LowerSat;

      /* End of Outputs for SubSystem: '<Root>/Fuzzy Logic  Controller1' */
    }

    /* End of Saturate: '<Root>/Saturation6' */

    /* Gain: '<Root>/Gain1' */
    rtb_TmpSignalConversionAtSFun_1 = VVA_pressure_dynamic_basefreq_P.Gain1_Gain
      * VVA_pressure_dynamic_basefreq_B.de;

    /* Saturate: '<Root>/Saturation7' */
    if (rtb_TmpSignalConversionAtSFun_1 >
        VVA_pressure_dynamic_basefreq_P.Saturation7_UpperSat) {
      /* Outputs for Atomic SubSystem: '<Root>/Fuzzy Logic  Controller1' */
      /* SignalConversion generated from: '<S29>/ SFunction ' incorporates:
       *  MATLAB Function: '<S13>/Evaluate Rule Antecedents'
       */
      rtb_TmpSignalConversionAtSFun_1 =
        VVA_pressure_dynamic_basefreq_P.Saturation7_UpperSat;

      /* End of Outputs for SubSystem: '<Root>/Fuzzy Logic  Controller1' */
    } else if (rtb_TmpSignalConversionAtSFun_1 <
               VVA_pressure_dynamic_basefreq_P.Saturation7_LowerSat) {
      /* Outputs for Atomic SubSystem: '<Root>/Fuzzy Logic  Controller1' */
      /* SignalConversion generated from: '<S29>/ SFunction ' incorporates:
       *  MATLAB Function: '<S13>/Evaluate Rule Antecedents'
       */
      rtb_TmpSignalConversionAtSFun_1 =
        VVA_pressure_dynamic_basefreq_P.Saturation7_LowerSat;

      /* End of Outputs for SubSystem: '<Root>/Fuzzy Logic  Controller1' */
    }

    /* End of Saturate: '<Root>/Saturation7' */

    /* Outputs for Atomic SubSystem: '<Root>/Fuzzy Logic  Controller1' */
    /* MATLAB Function: '<S13>/Evaluate Rule Antecedents' */
    rtb_Derivative1 = 0.0;
    inputMFCache[0] = 0.0;
    if (rtb_TmpSignalConversionAtSFun_0 < -2.4) {
      inputMFCache[0] = 1.0;
    } else if (rtb_TmpSignalConversionAtSFun_0 < -1.8) {
      inputMFCache[0] = (-1.8 - rtb_TmpSignalConversionAtSFun_0) *
        1.666666666666667;
    }

    inputMFCache[1] = exp(-((rtb_TmpSignalConversionAtSFun_0 - -1.28) *
      (rtb_TmpSignalConversionAtSFun_0 - -1.28)) / 0.4608);
    inputMFCache[2] = exp(-((rtb_TmpSignalConversionAtSFun_0 - -0.48) *
      (rtb_TmpSignalConversionAtSFun_0 - -0.48)) / 0.23120000000000004);
    inputMFCache[3] = exp(-(rtb_TmpSignalConversionAtSFun_0 *
      rtb_TmpSignalConversionAtSFun_0) / 0.0512);
    inputMFCache[4] = exp(-((rtb_TmpSignalConversionAtSFun_0 - 0.496) *
      (rtb_TmpSignalConversionAtSFun_0 - 0.496)) / 0.23120000000000004);
    inputMFCache[5] = exp(-((rtb_TmpSignalConversionAtSFun_0 - 1.28) *
      (rtb_TmpSignalConversionAtSFun_0 - 1.28)) / 0.4608);
    inputMFCache[6] = 0.0;
    if (rtb_TmpSignalConversionAtSFun_0 >= 2.4) {
      inputMFCache[6] = 1.0;
    } else if (rtb_TmpSignalConversionAtSFun_0 >= 1.68) {
      inputMFCache[6] = (rtb_TmpSignalConversionAtSFun_0 - 1.68) *
        1.3888888888888888;
    }

    inputMFCache[7] = 0.0;
    if (rtb_TmpSignalConversionAtSFun_1 < -21.0) {
      inputMFCache[7] = 1.0;
    } else if (rtb_TmpSignalConversionAtSFun_1 < -14.7) {
      inputMFCache[7] = (-14.7 - rtb_TmpSignalConversionAtSFun_1) *
        0.15873015873015872;
    }

    inputMFCache[8] = exp(-((rtb_TmpSignalConversionAtSFun_1 - -11.2) *
      (rtb_TmpSignalConversionAtSFun_1 - -11.2)) / 41.404999999999994);
    inputMFCache[9] = exp(-((rtb_TmpSignalConversionAtSFun_1 - -4.054) *
      (rtb_TmpSignalConversionAtSFun_1 - -4.054)) / 19.845);
    inputMFCache[10] = exp(-(rtb_TmpSignalConversionAtSFun_1 *
      rtb_TmpSignalConversionAtSFun_1) / 4.96125);
    inputMFCache[11] = exp(-((rtb_TmpSignalConversionAtSFun_1 - 4.2) *
      (rtb_TmpSignalConversionAtSFun_1 - 4.2)) / 15.679999999999998);
    inputMFCache[12] = exp(-((rtb_TmpSignalConversionAtSFun_1 - 11.2) *
      (rtb_TmpSignalConversionAtSFun_1 - 11.2)) / 41.404999999999994);
    inputMFCache[13] = 0.0;
    if (rtb_TmpSignalConversionAtSFun_1 >= 21.0) {
      inputMFCache[13] = 1.0;
    } else if (rtb_TmpSignalConversionAtSFun_1 >= 14.7) {
      inputMFCache[13] = (rtb_TmpSignalConversionAtSFun_1 - 14.7) *
        0.15873015873015872;
    }

    for (f_cmd = 0; f_cmd < 49; f_cmd++) {
      rtb_TmpSignalConversionAtSFun_0 = 1.0;
      for (inputID = 0; inputID < 2; inputID++) {
        rtb_TmpSignalConversionAtSFun_1 = inputMFCache[(b[49 * inputID + f_cmd]
          + 7 * inputID) - 1];
        if ((rtb_TmpSignalConversionAtSFun_0 > rtb_TmpSignalConversionAtSFun_1) ||
            (rtIsNaN(rtb_TmpSignalConversionAtSFun_0) && (!rtIsNaN
              (rtb_TmpSignalConversionAtSFun_1)))) {
          rtb_TmpSignalConversionAtSFun_0 = rtb_TmpSignalConversionAtSFun_1;
        }
      }

      rtb_antecedentOutputs[f_cmd] = rtb_TmpSignalConversionAtSFun_0;
      rtb_Derivative1 += rtb_TmpSignalConversionAtSFun_0;
    }

    /* MATLAB Function: '<S13>/Evaluate Rule Consequents' incorporates:
     *  Constant: '<S13>/Output Sample Points'
     */
    for (f_cmd = 0; f_cmd < 101; f_cmd++) {
      rtb_aggregatedOutputs[f_cmd] = 0.0;
      rtb_TmpSignalConversionAtSFun_1 =
        VVA_pressure_dynamic_basefreq_P.OutputSamplePoints_Value[f_cmd];
      outputMFCache[5 * f_cmd] = exp(-((rtb_TmpSignalConversionAtSFun_1 -
        0.004175) * (rtb_TmpSignalConversionAtSFun_1 - 0.004175)) / 0.022472);
      outputMFCache[5 * f_cmd + 1] = exp(-((rtb_TmpSignalConversionAtSFun_1 -
        0.25) * (rtb_TmpSignalConversionAtSFun_1 - 0.25)) / 0.02251442);
      outputMFCache[5 * f_cmd + 2] = exp(-((rtb_TmpSignalConversionAtSFun_1 -
        0.5) * (rtb_TmpSignalConversionAtSFun_1 - 0.5)) / 0.02251442);
      outputMFCache[5 * f_cmd + 3] = exp(-((rtb_TmpSignalConversionAtSFun_1 -
        0.75) * (rtb_TmpSignalConversionAtSFun_1 - 0.75)) / 0.02251442);
      outputMFCache[5 * f_cmd + 4] = exp(-((rtb_TmpSignalConversionAtSFun_1 -
        1.0) * (rtb_TmpSignalConversionAtSFun_1 - 1.0)) / 0.02251442);
    }

    for (f_cmd = 0; f_cmd < 49; f_cmd++) {
      rtb_TmpSignalConversionAtSFun_1 = rtb_antecedentOutputs[f_cmd];
      for (inputID = 0; inputID < 101; inputID++) {
        rtb_TmpSignalConversionAtSFun_0 = outputMFCache[(5 * inputID + b_0[f_cmd])
          - 1];
        if ((rtb_TmpSignalConversionAtSFun_0 > rtb_TmpSignalConversionAtSFun_1) ||
            (rtIsNaN(rtb_TmpSignalConversionAtSFun_0) && (!rtIsNaN
              (rtb_TmpSignalConversionAtSFun_1)))) {
          rtb_TmpSignalConversionAtSFun_0 = rtb_TmpSignalConversionAtSFun_1;
        }

        if (rtb_aggregatedOutputs[inputID] < rtb_TmpSignalConversionAtSFun_0) {
          rtb_aggregatedOutputs[inputID] = rtb_TmpSignalConversionAtSFun_0;
        }
      }
    }

    /* End of MATLAB Function: '<S13>/Evaluate Rule Consequents' */

    /* MATLAB Function: '<S13>/Defuzzify Outputs' incorporates:
     *  Constant: '<S13>/Output Sample Points'
     *  MATLAB Function: '<S13>/Evaluate Rule Antecedents'
     */
    if (rtb_Derivative1 == 0.0) {
      rtb_Derivative1 = 0.5;
    } else {
      rtb_Derivative1 = 0.0;
      rtb_TmpSignalConversionAtSFun_0 = 0.0;
      for (f_cmd = 0; f_cmd < 101; f_cmd++) {
        rtb_TmpSignalConversionAtSFun_0 += rtb_aggregatedOutputs[f_cmd];
      }

      if (rtb_TmpSignalConversionAtSFun_0 == 0.0) {
        rtb_Derivative1 =
          (VVA_pressure_dynamic_basefreq_P.OutputSamplePoints_Value[0] +
           VVA_pressure_dynamic_basefreq_P.OutputSamplePoints_Value[100]) / 2.0;
      } else {
        for (f_cmd = 0; f_cmd < 101; f_cmd++) {
          rtb_Derivative1 +=
            VVA_pressure_dynamic_basefreq_P.OutputSamplePoints_Value[f_cmd] *
            rtb_aggregatedOutputs[f_cmd];
        }

        rtb_Derivative1 *= 1.0 / rtb_TmpSignalConversionAtSFun_0;
      }
    }

    /* End of MATLAB Function: '<S13>/Defuzzify Outputs' */
    /* End of Outputs for SubSystem: '<Root>/Fuzzy Logic  Controller1' */

    /* Saturate: '<Root>/Saturation4' */
    if (rtb_Derivative1 > VVA_pressure_dynamic_basefreq_P.Saturation4_UpperSat)
    {
      /* Saturate: '<Root>/Saturation4' */
      VVA_pressure_dynamic_basefreq_B.u_n =
        VVA_pressure_dynamic_basefreq_P.Saturation4_UpperSat;
    } else if (rtb_Derivative1 <
               VVA_pressure_dynamic_basefreq_P.Saturation4_LowerSat) {
      /* Saturate: '<Root>/Saturation4' */
      VVA_pressure_dynamic_basefreq_B.u_n =
        VVA_pressure_dynamic_basefreq_P.Saturation4_LowerSat;
    } else {
      /* Saturate: '<Root>/Saturation4' */
      VVA_pressure_dynamic_basefreq_B.u_n = rtb_Derivative1;
    }

    /* End of Saturate: '<Root>/Saturation4' */
    if (rtmIsMajorTimeStep(VVA_pressure_dynamic_basefre_M) &&
        VVA_pressure_dynamic_basefre_M->Timing.TaskCounters.TID[1] == 0) {
      /* Saturate: '<Root>/Saturation2' incorporates:
       *  Constant: '<Root>/freq_mode'
       */
      if (VVA_pressure_dynamic_basefreq_P.freq_mode_Value >
          VVA_pressure_dynamic_basefreq_P.Saturation2_UpperSat) {
        rtb_Saturation = VVA_pressure_dynamic_basefreq_P.Saturation2_UpperSat;
      } else if (VVA_pressure_dynamic_basefreq_P.freq_mode_Value <
                 VVA_pressure_dynamic_basefreq_P.Saturation2_LowerSat) {
        rtb_Saturation = VVA_pressure_dynamic_basefreq_P.Saturation2_LowerSat;
      } else {
        rtb_Saturation = VVA_pressure_dynamic_basefreq_P.freq_mode_Value;
      }

      /* End of Saturate: '<Root>/Saturation2' */

      /* Saturate: '<S12>/Saturation' */
      if (rtb_Saturation > VVA_pressure_dynamic_basefreq_P.Saturation_UpperSat)
      {
        rtb_Saturation = VVA_pressure_dynamic_basefreq_P.Saturation_UpperSat;
      } else if (rtb_Saturation <
                 VVA_pressure_dynamic_basefreq_P.Saturation_LowerSat) {
        rtb_Saturation = VVA_pressure_dynamic_basefreq_P.Saturation_LowerSat;
      }

      /* End of Saturate: '<S12>/Saturation' */
    }

    /* Derivative: '<Root>/Derivative2' */
    if ((VVA_pressure_dynamic_basefre_DW.TimeStampA_p >= rtb_dm) &&
        (VVA_pressure_dynamic_basefre_DW.TimeStampB_l >= rtb_dm)) {
      rtb_Derivative1 = 0.0;
    } else {
      rtb_TmpSignalConversionAtSFun_0 =
        VVA_pressure_dynamic_basefre_DW.TimeStampA_p;
      lastU = &VVA_pressure_dynamic_basefre_DW.LastUAtTimeA_b;
      if (VVA_pressure_dynamic_basefre_DW.TimeStampA_p <
          VVA_pressure_dynamic_basefre_DW.TimeStampB_l) {
        if (VVA_pressure_dynamic_basefre_DW.TimeStampB_l < rtb_dm) {
          rtb_TmpSignalConversionAtSFun_0 =
            VVA_pressure_dynamic_basefre_DW.TimeStampB_l;
          lastU = &VVA_pressure_dynamic_basefre_DW.LastUAtTimeB_l;
        }
      } else if (VVA_pressure_dynamic_basefre_DW.TimeStampA_p >= rtb_dm) {
        rtb_TmpSignalConversionAtSFun_0 =
          VVA_pressure_dynamic_basefre_DW.TimeStampB_l;
        lastU = &VVA_pressure_dynamic_basefre_DW.LastUAtTimeB_l;
      }

      rtb_Derivative1 = (VVA_pressure_dynamic_basefreq_B.de - *lastU) / (rtb_dm
        - rtb_TmpSignalConversionAtSFun_0);
    }

    /* MATLAB Function: '<Root>/MATLAB Function' incorporates:
     *  Constant: '<Root>/Constant15'
     *  Constant: '<Root>/Constant16'
     *  Constant: '<Root>/Constant17'
     *  Gain: '<Root>/Gain7'
     *  Gain: '<Root>/Gain8'
     *  Gain: '<Root>/Gain9'
     */
    VVA_pressure_dynamic_basefreq_B.eta =
      (VVA_pressure_dynamic_basefreq_P.Gain7_Gain *
       VVA_pressure_dynamic_basefreq_B.e *
       VVA_pressure_dynamic_basefreq_P.Constant15_Value +
       VVA_pressure_dynamic_basefreq_P.Gain8_Gain *
       VVA_pressure_dynamic_basefreq_B.de *
       VVA_pressure_dynamic_basefreq_P.Constant16_Value) +
      VVA_pressure_dynamic_basefreq_P.Gain9_Gain * rtb_Derivative1 *
      VVA_pressure_dynamic_basefreq_P.Constant17_Value;
    VVA_pressure_dynamic_basefreq_B.eta = fmax(fmin
      (VVA_pressure_dynamic_basefreq_B.eta, 0.5), -0.5);

    /* Saturate: '<Root>/Saturation1' */
    if (VVA_pressure_dynamic_basefreq_B.u_n >
        VVA_pressure_dynamic_basefreq_P.Saturation1_UpperSat) {
      /* Saturate: '<Root>/Saturation1' */
      VVA_pressure_dynamic_basefreq_B.Saturation1 =
        VVA_pressure_dynamic_basefreq_P.Saturation1_UpperSat;
    } else if (VVA_pressure_dynamic_basefreq_B.u_n <
               VVA_pressure_dynamic_basefreq_P.Saturation1_LowerSat) {
      /* Saturate: '<Root>/Saturation1' */
      VVA_pressure_dynamic_basefreq_B.Saturation1 =
        VVA_pressure_dynamic_basefreq_P.Saturation1_LowerSat;
    } else {
      /* Saturate: '<Root>/Saturation1' */
      VVA_pressure_dynamic_basefreq_B.Saturation1 =
        VVA_pressure_dynamic_basefreq_B.u_n;
    }

    /* End of Saturate: '<Root>/Saturation1' */
    if (rtmIsMajorTimeStep(VVA_pressure_dynamic_basefre_M) &&
        VVA_pressure_dynamic_basefre_M->Timing.TaskCounters.TID[1] == 0) {
      /* MATLAB Function: '<Root>/频率' incorporates:
       *  Constant: '<Root>/Constant11'
       *  Constant: '<Root>/Constant14'
       */
      A = fabs(VVA_pressure_dynamic_basefreq_B.eta);
      if (!VVA_pressure_dynamic_basefre_DW.eta_prev_not_empty) {
        VVA_pressure_dynamic_basefre_DW.eta_prev =
          VVA_pressure_dynamic_basefreq_B.eta;
        VVA_pressure_dynamic_basefre_DW.eta_prev_not_empty = true;
      }

      rtb_Derivative1 = VVA_pressure_dynamic_basefreq_B.eta -
        VVA_pressure_dynamic_basefre_DW.eta_prev;
      cross = (VVA_pressure_dynamic_basefreq_B.eta *
               VVA_pressure_dynamic_basefre_DW.eta_prev < 0.0);
      VVA_pressure_dynamic_basefre_DW.eta_prev =
        VVA_pressure_dynamic_basefreq_B.eta;
      if ((A < VVA_pressure_dynamic_basefreq_P.Constant11_Value) && (cross ||
           (fabs(rtb_Derivative1) > 0.02))) {
        cross = true;
      } else {
        cross = false;
      }

      if (VVA_pressure_dynamic_basefre_DW.burst_cnt > 0) {
        VVA_pressure_dynamic_basefre_DW.burst_cnt--;
      } else if (cross) {
        VVA_pressure_dynamic_basefre_DW.burst_cnt = 200;
      }

      if (A >= VVA_pressure_dynamic_basefreq_P.Constant14_Value) {
        f_cmd = 100;
      } else if (A >= VVA_pressure_dynamic_basefreq_P.Constant11_Value) {
        f_cmd = 300;
      } else {
        f_cmd = 100;
      }

      if (VVA_pressure_dynamic_basefre_DW.burst_cnt > 0) {
        f_cmd = 500;
      }

      if (f_cmd < VVA_pressure_dynamic_basefre_DW.f_old) {
        VVA_pressure_dynamic_basefre_DW.f_old = f_cmd;
      } else {
        VVA_pressure_dynamic_basefre_DW.f_old = 0.85 *
          VVA_pressure_dynamic_basefre_DW.f_old + 0.15000000000000002 * (real_T)
          f_cmd;
      }

      A = VVA_pressure_dynamic_basefre_DW.f_old;

      /* End of MATLAB Function: '<Root>/频率' */
    }

    /* Gain: '<Root>/Gain11' */
    VVA_pressure_dynamic_basefreq_B.Gain11 =
      VVA_pressure_dynamic_basefreq_P.Gain11_Gain *
      VVA_pressure_dynamic_basefreq_B.e;

    /* Gain: '<Root>/Gain12' */
    VVA_pressure_dynamic_basefreq_B.Gain12 =
      VVA_pressure_dynamic_basefreq_P.Gain12_Gain *
      VVA_pressure_dynamic_basefreq_B.de;
    if (rtmIsMajorTimeStep(VVA_pressure_dynamic_basefre_M) &&
        VVA_pressure_dynamic_basefre_M->Timing.TaskCounters.TID[1] == 0) {
      /* MATLAB Function: '<Root>/DutyComp_D' incorporates:
       *  Constant: '<Root>/Constant'
       *  Constant: '<Root>/Constant32'
       *  Gain: '<Root>/Gain15'
       *  Gain: '<Root>/Gain16'
       *  UnitDelay: '<Root>/Unit Delay'
       */
      rtb_Derivative1 = fmax(1.0 / fmax
        (VVA_pressure_dynamic_basefre_DW.UnitDelay_DSTATE, 1.0), 1.0E-12);
      d_min = fmin(fmax(VVA_pressure_dynamic_basefreq_P.Gain15_Gain *
                        VVA_pressure_dynamic_basefreq_P.Constant_Value /
                        rtb_Derivative1, 0.0), 1.0);
      rtb_Derivative1 = fmin(fmax(1.0 -
        VVA_pressure_dynamic_basefreq_P.Gain16_Gain *
        VVA_pressure_dynamic_basefreq_P.Constant32_Value / rtb_Derivative1, 0.0),
        1.0);
      if (d_min > rtb_Derivative1) {
        d_min = 0.5;
      } else {
        d_min = fmin(fmax(fmin(fmax(VVA_pressure_dynamic_basefreq_B.Saturation1,
          0.0), 1.0), d_min), rtb_Derivative1);
      }

      /* End of MATLAB Function: '<Root>/DutyComp_D' */

      /* MATLAB Function: '<Root>/FreqPolicy' */
      if (!VVA_pressure_dynamic_basefre_DW.de_f_not_empty) {
        VVA_pressure_dynamic_basefre_DW.de_f =
          VVA_pressure_dynamic_basefreq_B.Gain12;
        VVA_pressure_dynamic_basefre_DW.de_f_not_empty = true;
      }

      VVA_pressure_dynamic_basefre_DW.de_f = 0.99004983374916811 *
        VVA_pressure_dynamic_basefre_DW.de_f + 0.0099501662508318933 *
        VVA_pressure_dynamic_basefreq_B.Gain12;
      E = fabs(VVA_pressure_dynamic_basefreq_B.Gain11);
      D = fabs(VVA_pressure_dynamic_basefre_DW.de_f);
      if ((E < 0.02) && (D < 0.005)) {
        if (VVA_pressure_dynamic_basefre_DW.hold_cnt_p < 50) {
          VVA_pressure_dynamic_basefre_DW.hold_cnt_p++;
        }
      } else {
        VVA_pressure_dynamic_basefre_DW.hold_cnt_p = 0;
      }

      if (E > 0.45) {
        rtb_Derivative1 = 90.0;
      } else if (E > 0.18) {
        rtb_Derivative1 = (E - 0.18) / 0.27 * -110.0 + 200.0;
      } else {
        rtb_Derivative1 = 200.0;
      }

      if ((E > 0.18) && (D < 0.002)) {
        if (VVA_pressure_dynamic_basefre_DW.stall_cnt < 300) {
          VVA_pressure_dynamic_basefre_DW.stall_cnt++;
        }
      } else {
        VVA_pressure_dynamic_basefre_DW.stall_cnt = 0;
      }

      if (VVA_pressure_dynamic_basefre_DW.hold_cnt_p >= 50) {
        VVA_pressure_dynamic_basefre_DW.burst_cnt_n = 0;
        E = 120.0;
      } else {
        if (((E < 0.06) && (VVA_pressure_dynamic_basefreq_B.Gain11 *
                            VVA_pressure_dynamic_basefre_DW.de_f < 0.0) && (D >
              0.03)) || (VVA_pressure_dynamic_basefre_DW.stall_cnt >= 300)) {
          VVA_pressure_dynamic_basefre_DW.burst_cnt_n = 300;
        } else if (VVA_pressure_dynamic_basefre_DW.burst_cnt_n > 0) {
          VVA_pressure_dynamic_basefre_DW.burst_cnt_n--;
        }

        if (VVA_pressure_dynamic_basefre_DW.burst_cnt_n > 0) {
          E = 450.0;
        } else {
          E = rtb_Derivative1;
        }
      }

      /* End of MATLAB Function: '<Root>/FreqPolicy' */

      /* MATLAB Function: '<Root>/BaseFreqOpt' incorporates:
       *  Constant: '<Root>/Constant13'
       */
      if ((d_min <= 1.0E-6) || (d_min >= 0.999999)) {
        D = 60.0;
      } else {
        D = fmin(d_min / 0.0008, (1.0 - d_min) / 0.0004) * 0.94;
      }

      D = fmin(D, fmax(VVA_pressure_dynamic_basefreq_P.Constant13_Value, 1.0));
      rtb_Derivative1 = fmin(E, D);
      if (!VVA_pressure_dynamic_basefre_DW.f_prev_not_empty) {
        VVA_pressure_dynamic_basefre_DW.f_prev = rtb_Derivative1;
        VVA_pressure_dynamic_basefre_DW.f_prev_not_empty = true;
      }

      VVA_pressure_dynamic_basefre_DW.S += fmax
        (VVA_pressure_dynamic_basefre_DW.f_prev - 160.0, 0.0) * 0.0001;
      VVA_pressure_dynamic_basefre_DW.S = fmax(0.0,
        VVA_pressure_dynamic_basefre_DW.S - 0.008);
      if (VVA_pressure_dynamic_basefre_DW.S > 40.0) {
        rtb_Derivative1 = fmin(rtb_Derivative1, 160.0);
      }

      VVA_pressure_dynamic_basefreq_B.f_out = fmax(fmin((0.97 *
        VVA_pressure_dynamic_basefre_DW.f_prev + 0.030000000000000027 *
        rtb_Derivative1) - VVA_pressure_dynamic_basefre_DW.f_prev, 0.3), -2.0) +
        VVA_pressure_dynamic_basefre_DW.f_prev;
      VVA_pressure_dynamic_basefreq_B.f_out = fmin(fmax
        (VVA_pressure_dynamic_basefreq_B.f_out, fmin(60.0, D)), D);
      VVA_pressure_dynamic_basefre_DW.f_prev =
        VVA_pressure_dynamic_basefreq_B.f_out;

      /* MultiPortSwitch: '<S12>/Multiport Switch' incorporates:
       *  Constant: '<Root>/Constant12'
       */
      switch ((int32_T)rtb_Saturation) {
       case 1:
        D = VVA_pressure_dynamic_basefreq_P.Constant12_Value;
        break;

       case 2:
        D = A;
        break;

       default:
        D = VVA_pressure_dynamic_basefreq_B.f_out;
        break;
      }

      /* End of MultiPortSwitch: '<S12>/Multiport Switch' */
      /* MATLAB Function: '<Root>/varFreqPWM' incorporates:
       *  MATLAB Function: '<Root>/BaseFreqOpt'
       */
      VVA_pressure_dynamic_basefre_DW.phi += fmin(fmax(D, 1.0), 100000.0) *
        0.0001;
      VVA_pressure_dynamic_basefre_DW.phi -= floor
        (VVA_pressure_dynamic_basefre_DW.phi);
      VVA_pressure_dynamic_basefreq_B.u_a = (VVA_pressure_dynamic_basefre_DW.phi
        < d_min);
    }

    if (rtmIsMajorTimeStep(VVA_pressure_dynamic_basefre_M) &&
        VVA_pressure_dynamic_basefre_M->Timing.TaskCounters.TID[2] == 0) {
      /* MATLAB Function: '<Root>/模式选择' incorporates:
       *  Constant: '<Root>/Constant10'
       */
      if (VVA_pressure_dynamic_basefre_DW.hold_cnt > 0) {
        VVA_pressure_dynamic_basefre_DW.hold_cnt--;
      }

      switch (VVA_pressure_dynamic_basefre_DW.a_prev) {
       case 1:
        if (VVA_pressure_dynamic_basefreq_B.e < 0.6 *
            VVA_pressure_dynamic_basefreq_P.Constant10_Value) {
          f_cmd = 2;
        } else {
          f_cmd = 1;
        }
        break;

       case 3:
        if (VVA_pressure_dynamic_basefreq_B.e > -(0.6 *
             VVA_pressure_dynamic_basefreq_P.Constant10_Value)) {
          f_cmd = 2;
        } else {
          f_cmd = 3;
        }
        break;

       default:
        if (VVA_pressure_dynamic_basefreq_B.e >
            VVA_pressure_dynamic_basefreq_P.Constant10_Value) {
          f_cmd = 1;
        } else if (VVA_pressure_dynamic_basefreq_B.e <
                   -VVA_pressure_dynamic_basefreq_P.Constant10_Value) {
          f_cmd = 3;
        } else {
          f_cmd = 2;
        }
        break;
      }

      if ((VVA_pressure_dynamic_basefre_DW.hold_cnt > 0) && (!(fabs
            (VVA_pressure_dynamic_basefreq_B.e) > 3.0 *
            VVA_pressure_dynamic_basefreq_P.Constant10_Value))) {
        f_cmd = VVA_pressure_dynamic_basefre_DW.a_prev;
      } else {
        if (f_cmd != VVA_pressure_dynamic_basefre_DW.a_prev) {
          VVA_pressure_dynamic_basefre_DW.hold_cnt = 30;
        }

        VVA_pressure_dynamic_basefre_DW.a_prev = f_cmd;
      }

      /* End of MATLAB Function: '<Root>/模式选择' */

      /* MATLAB Function: '<Root>/2阀模式' */
      switch (f_cmd) {
       case 1:
        VVA_pressure_dynamic_basefreq_B.U1 = VVA_pressure_dynamic_basefreq_B.u_a;
        VVA_pressure_dynamic_basefreq_B.U2 = 0.0;
        break;

       case 2:
        VVA_pressure_dynamic_basefreq_B.U1 = 0.0;
        VVA_pressure_dynamic_basefreq_B.U2 = 0.0;
        break;

       case 3:
        VVA_pressure_dynamic_basefreq_B.U1 = 0.0;
        VVA_pressure_dynamic_basefreq_B.U2 = VVA_pressure_dynamic_basefreq_B.u_a;
        break;

       default:
        VVA_pressure_dynamic_basefreq_B.U1 = 0.0;
        VVA_pressure_dynamic_basefreq_B.U2 = 0.0;
        break;
      }

      /* End of MATLAB Function: '<Root>/2阀模式' */
    }

    if (rtmIsMajorTimeStep(VVA_pressure_dynamic_basefre_M) &&
        VVA_pressure_dynamic_basefre_M->Timing.TaskCounters.TID[1] == 0) {
    }

    /* Switch: '<S26>/Switch' */
    if (VVA_pressure_dynamic_basefreq_B.e >=
        VVA_pressure_dynamic_basefreq_P.Switch_Threshold) {
      /* Switch: '<S26>/Switch' incorporates:
       *  Constant: '<S26>/Constant6'
       */
      VVA_pressure_dynamic_basefreq_B.Switch[0] =
        VVA_pressure_dynamic_basefreq_B.u_n;
      VVA_pressure_dynamic_basefreq_B.Switch[1] =
        VVA_pressure_dynamic_basefreq_P.Constant6_Value;
    } else {
      /* Switch: '<S26>/Switch' incorporates:
       *  Constant: '<S26>/Constant2'
       */
      VVA_pressure_dynamic_basefreq_B.Switch[0] =
        VVA_pressure_dynamic_basefreq_P.Constant2_Value;
      VVA_pressure_dynamic_basefreq_B.Switch[1] =
        VVA_pressure_dynamic_basefreq_B.u_n;
    }

    /* End of Switch: '<S26>/Switch' */
    if (rtmIsMajorTimeStep(VVA_pressure_dynamic_basefre_M) &&
        VVA_pressure_dynamic_basefre_M->Timing.TaskCounters.TID[1] == 0) {
      /* MATLAB Function: '<S26>/ 电磁阀滞后' */
      VVA_pressure_dynamic_basefreq_u(VVA_pressure_dynamic_basefreq_B.U1,
        VVA_pressure_dynamic_basefreq_B.Switch[0],
        &VVA_pressure_dynamic_basefreq_B.sf__d,
        &VVA_pressure_dynamic_basefre_DW.sf__d);

      /* MATLAB Function: '<Root>/parameter_set' */
      VVA_pressure_dynamic_basefreq_B.para[0] = 800000.0;
      VVA_pressure_dynamic_basefreq_B.para[1] = 101325.0;
      VVA_pressure_dynamic_basefreq_B.para[2] = 293.15;
      VVA_pressure_dynamic_basefreq_B.para[3] = 1.4;
    }

    /* MATLAB Function: '<S17>/MATLAB Function2' */
    rtb_Saturation = VVA_pressure_dynamic_basefreq_B.u /
      VVA_pressure_dynamic_basefreq_B.para[0];

    /* Lookup_n-D: '<S17>/1-D Lookup Table2' */
    /*
     * About '<S17>/1-D Lookup Table2':
     *       Table size:  51 x 7
     *    Interpolation:  Spline
     *    Extrapolation:  Spline
     *   Breakpt Search:  Binary
     *    Breakpt Cache:  OFF
     */
    {
      real_T rt_LUTuVect[2];
      rt_LUTuVect[0] = rtb_Saturation;
      rt_LUTuVect[1] = VVA_pressure_dynamic_basefreq_B.para[0];
      VVA_pressure_dynamic_basefreq_B.uDLookupTable2 = look_SplNBinSZcd(2U,
        rt_LUTuVect, (rt_LUTSplineWork*)&VVA_pressure_dynamic_basefre_DW.SWork[0]);
    }

    /* MATLAB Function: '<S26>/考虑滞后的流量' */
    f_cmd = 0;
    if ((VVA_pressure_dynamic_basefreq_B.U1 == 0.0) &&
        (VVA_pressure_dynamic_basefreq_B.sf__d.next == 1.0)) {
      f_cmd = 1;
    }

    if ((VVA_pressure_dynamic_basefreq_B.U1 == 1.0) &&
        (VVA_pressure_dynamic_basefreq_B.sf__d.next == 0.0)) {
      f_cmd = 2;
    }

    switch (f_cmd) {
     case 0:
      VVA_pressure_dynamic_basefreq_B.q1 =
        VVA_pressure_dynamic_basefreq_B.uDLookupTable2;
      break;

     case 1:
      rtb_Saturation = rtb_dm - rtb_dm;
      if (rtb_Saturation <= 0.00055) {
        VVA_pressure_dynamic_basefreq_B.q1 =
          VVA_pressure_dynamic_basefreq_B.uDLookupTable2 / 0.00055 *
          rtb_Saturation;
      } else {
        VVA_pressure_dynamic_basefreq_B.q1 =
          VVA_pressure_dynamic_basefreq_B.uDLookupTable2;
      }
      break;

     default:
      rtb_Saturation = rtb_dm - rtb_dm;
      if (rtb_Saturation <= 0.00055) {
        VVA_pressure_dynamic_basefreq_B.q1 =
          VVA_pressure_dynamic_basefreq_B.uDLookupTable2 / 0.00055 * (0.00055 -
          rtb_Saturation);
      } else {
        VVA_pressure_dynamic_basefreq_B.q1 =
          VVA_pressure_dynamic_basefreq_B.uDLookupTable2;
      }
      break;
    }

    /* End of MATLAB Function: '<S26>/考虑滞后的流量' */
    if (rtmIsMajorTimeStep(VVA_pressure_dynamic_basefre_M) &&
        VVA_pressure_dynamic_basefre_M->Timing.TaskCounters.TID[1] == 0) {
    }

    /* Gain: '<Root>/Gain5' */
    VVA_pressure_dynamic_basefreq_B.Gain5 =
      VVA_pressure_dynamic_basefreq_P.Gain5_Gain *
      VVA_pressure_dynamic_basefreq_B.ManualSwitch1;

    /* Gain: '<Root>/Gain6' */
    VVA_pressure_dynamic_basefreq_B.Gain6 =
      VVA_pressure_dynamic_basefreq_P.Gain6_Gain *
      VVA_pressure_dynamic_basefreq_B.u;
    if (rtmIsMajorTimeStep(VVA_pressure_dynamic_basefre_M) &&
        VVA_pressure_dynamic_basefre_M->Timing.TaskCounters.TID[1] == 0) {
    }

    /* Integrator: '<Root>/Integrator' */
    VVA_pressure_dynamic_basefreq_B.Integrator =
      VVA_pressure_dynamic_basefreq_X.Integrator_CSTATE;
    if (rtmIsMajorTimeStep(VVA_pressure_dynamic_basefre_M) &&
        VVA_pressure_dynamic_basefre_M->Timing.TaskCounters.TID[1] == 0) {
    }

    /* Integrator: '<Root>/Integrator3' */
    VVA_pressure_dynamic_basefreq_B.Integrator3 =
      VVA_pressure_dynamic_basefreq_X.Integrator3_CSTATE;
    if (rtmIsMajorTimeStep(VVA_pressure_dynamic_basefre_M) &&
        VVA_pressure_dynamic_basefre_M->Timing.TaskCounters.TID[1] == 0) {
    }

    /* MATLAB Function: '<Root>/VL' */
    VVA_pressure_dynamic_basefreq_B.VL =
      VVA_pressure_dynamic_basefreq_B.Integrator3 * 1000.0 /
      (VVA_pressure_dynamic_basefreq_B.u / VVA_pressure_dynamic_basefreq_B.para
       [1] * 1.293);
    if (rtmIsMajorTimeStep(VVA_pressure_dynamic_basefre_M) &&
        VVA_pressure_dynamic_basefre_M->Timing.TaskCounters.TID[1] == 0) {
    }

    /* Gain: '<Root>/Gain17' */
    VVA_pressure_dynamic_basefreq_B.Gain17 =
      VVA_pressure_dynamic_basefreq_P.Gain17_Gain *
      VVA_pressure_dynamic_basefreq_B.Integrator;
    if (rtmIsMajorTimeStep(VVA_pressure_dynamic_basefre_M) &&
        VVA_pressure_dynamic_basefre_M->Timing.TaskCounters.TID[1] == 0) {
    }

    /* Gain: '<Root>/Gain10' */
    VVA_pressure_dynamic_basefreq_B.Gain10 =
      VVA_pressure_dynamic_basefreq_P.Gain10_Gain *
      VVA_pressure_dynamic_basefreq_B.e;
    if (rtmIsMajorTimeStep(VVA_pressure_dynamic_basefre_M) &&
        VVA_pressure_dynamic_basefre_M->Timing.TaskCounters.TID[1] == 0) {
      /* MATLAB Function: '<S26>/ 电磁阀滞后2' */
      VVA_pressure_dynamic_basefreq_u(VVA_pressure_dynamic_basefreq_B.U2,
        VVA_pressure_dynamic_basefreq_B.Switch[1],
        &VVA_pressure_dynamic_basefreq_B.sf_2_m,
        &VVA_pressure_dynamic_basefre_DW.sf_2_m);
    }

    /* MATLAB Function: '<S17>/MATLAB Function3' */
    rtb_Saturation = VVA_pressure_dynamic_basefreq_B.para[1] /
      VVA_pressure_dynamic_basefreq_B.u;

    /* Lookup_n-D: '<S17>/2-D Lookup Table3' */
    /*
     * About '<S17>/2-D Lookup Table3':
     *       Table size:  51 x 7
     *    Interpolation:  Spline
     *    Extrapolation:  Spline
     *   Breakpt Search:  Binary
     *    Breakpt Cache:  OFF
     */
    {
      real_T rt_LUTuVect[2];
      rt_LUTuVect[0] = rtb_Saturation;
      rt_LUTuVect[1] = VVA_pressure_dynamic_basefreq_B.u;
      VVA_pressure_dynamic_basefreq_B.uDLookupTable3 = look_SplNBinSZcd(2U,
        rt_LUTuVect, (rt_LUTSplineWork*)
        &VVA_pressure_dynamic_basefre_DW.SWork_h[0]);
    }

    /* MATLAB Function: '<S26>/考虑滞后的流量2' */
    f_cmd = 0;
    if ((VVA_pressure_dynamic_basefreq_B.U2 == 0.0) &&
        (VVA_pressure_dynamic_basefreq_B.sf_2_m.next == 1.0)) {
      f_cmd = 1;
    }

    if ((VVA_pressure_dynamic_basefreq_B.U2 == 1.0) &&
        (VVA_pressure_dynamic_basefreq_B.sf_2_m.next == 0.0)) {
      f_cmd = 2;
    }

    switch (f_cmd) {
     case 0:
      VVA_pressure_dynamic_basefreq_B.q2 =
        VVA_pressure_dynamic_basefreq_B.uDLookupTable3;
      break;

     case 1:
      rtb_Saturation = rtb_dm - rtb_dm;
      if (rtb_Saturation <= 0.00055) {
        VVA_pressure_dynamic_basefreq_B.q2 =
          VVA_pressure_dynamic_basefreq_B.uDLookupTable3 / 0.00055 *
          rtb_Saturation;
      } else {
        VVA_pressure_dynamic_basefreq_B.q2 =
          VVA_pressure_dynamic_basefreq_B.uDLookupTable3;
      }
      break;

     default:
      rtb_Saturation = rtb_dm - rtb_dm;
      if (rtb_Saturation <= 0.00055) {
        VVA_pressure_dynamic_basefreq_B.q2 =
          VVA_pressure_dynamic_basefreq_B.uDLookupTable3 / 0.00055 * (0.00055 -
          rtb_Saturation);
      } else {
        VVA_pressure_dynamic_basefreq_B.q2 =
          VVA_pressure_dynamic_basefreq_B.uDLookupTable3;
      }
      break;
    }

    /* End of MATLAB Function: '<S26>/考虑滞后的流量2' */

    /* MATLAB Function: '<Root>/MATLAB Function1' */
    rtb_dm = VVA_pressure_dynamic_basefreq_B.U1 *
      VVA_pressure_dynamic_basefreq_B.q1 - VVA_pressure_dynamic_basefreq_B.U2 *
      VVA_pressure_dynamic_basefreq_B.q2;

    /* Abs: '<Root>/Abs' */
    VVA_pressure_dynamic_basefreq_B.Abs = fabs(rtb_dm);

    /* Switch: '<Root>/Switch2' */
    if (VVA_pressure_dynamic_basefreq_B.e >=
        VVA_pressure_dynamic_basefreq_P.Switch2_Threshold) {
      /* Switch: '<Root>/Switch2' incorporates:
       *  Constant: '<Root>/Constant4'
       */
      VVA_pressure_dynamic_basefreq_B.Switch2[0] =
        VVA_pressure_dynamic_basefreq_B.u_n;
      VVA_pressure_dynamic_basefreq_B.Switch2[1] =
        VVA_pressure_dynamic_basefreq_P.Constant4_Value;
    } else {
      /* Switch: '<Root>/Switch2' incorporates:
       *  Constant: '<Root>/Constant5'
       *  Constant: '<Root>/Constant8'
       */
      VVA_pressure_dynamic_basefreq_B.Switch2[0] =
        VVA_pressure_dynamic_basefreq_P.Constant5_Value;
      VVA_pressure_dynamic_basefreq_B.Switch2[1] =
        VVA_pressure_dynamic_basefreq_P.Constant8_Value;
    }

    /* End of Switch: '<Root>/Switch2' */

    /* Switch: '<Root>/Switch3' */
    if (VVA_pressure_dynamic_basefreq_B.e >=
        VVA_pressure_dynamic_basefreq_P.Switch3_Threshold) {
      /* Switch: '<Root>/Switch3' */
      VVA_pressure_dynamic_basefreq_B.Switch3[0] =
        VVA_pressure_dynamic_basefreq_B.Switch2[0];
      VVA_pressure_dynamic_basefreq_B.Switch3[1] =
        VVA_pressure_dynamic_basefreq_B.Switch2[1];
    } else {
      /* Switch: '<Root>/Switch3' incorporates:
       *  Constant: '<Root>/Constant2'
       */
      VVA_pressure_dynamic_basefreq_B.Switch3[0] =
        VVA_pressure_dynamic_basefreq_P.Constant2_Value_c;
      VVA_pressure_dynamic_basefreq_B.Switch3[1] =
        VVA_pressure_dynamic_basefreq_B.u_n;
    }

    /* End of Switch: '<Root>/Switch3' */
    if (rtmIsMajorTimeStep(VVA_pressure_dynamic_basefre_M) &&
        VVA_pressure_dynamic_basefre_M->Timing.TaskCounters.TID[1] == 0) {
      /* Memory: '<Root>/Memory2' */
      VVA_pressure_dynamic_basefreq_B.Memory2 =
        VVA_pressure_dynamic_basefre_DW.Memory2_PreviousInput;
    }

    if (rtmIsMajorTimeStep(VVA_pressure_dynamic_basefre_M) &&
        VVA_pressure_dynamic_basefre_M->Timing.TaskCounters.TID[4] == 0) {
      /* Sum: '<S34>/FixPt Sum1' incorporates:
       *  Constant: '<S34>/FixPt Constant'
       */
      rtb_Output = (uint8_T)((uint32_T)rtb_Output +
        VVA_pressure_dynamic_basefreq_P.FixPtConstant_Value);

      /* Switch: '<S35>/FixPt Switch' */
      if (rtb_Output > VVA_pressure_dynamic_basefreq_P.LimitedCounter_uplimit) {
        /* Switch: '<S35>/FixPt Switch' incorporates:
         *  Constant: '<S35>/Constant'
         */
        rtb_FixPtSwitch = VVA_pressure_dynamic_basefreq_P.Constant_Value_e;
      } else {
        /* Switch: '<S35>/FixPt Switch' */
        rtb_FixPtSwitch = rtb_Output;
      }

      /* End of Switch: '<S35>/FixPt Switch' */
    }

    /* MATLAB Function: '<Root>/dPL_dPR' */
    VVA_pressure_dynamic_basefreq_B.dP = VVA_pressure_dynamic_basefreq_B.para[3]
      * 0.287 * VVA_pressure_dynamic_basefreq_B.para[2] * rtb_dm / 0.000157;
    if (rtmIsMajorTimeStep(VVA_pressure_dynamic_basefre_M) &&
        VVA_pressure_dynamic_basefre_M->Timing.TaskCounters.TID[1] == 0) {
      /* DiscretePulseGenerator: '<Root>/Pulse Generator' */
      rtb_PulseGenerator = (VVA_pressure_dynamic_basefre_DW.clockTickCounter <
                            VVA_pressure_dynamic_basefreq_P.PulseGenerator_Duty)
        && (VVA_pressure_dynamic_basefre_DW.clockTickCounter >= 0) ?
        VVA_pressure_dynamic_basefreq_P.PulseGenerator_Amp : 0.0;

      /* DiscretePulseGenerator: '<Root>/Pulse Generator' */
      if (VVA_pressure_dynamic_basefre_DW.clockTickCounter >=
          VVA_pressure_dynamic_basefreq_P.PulseGenerator_Period - 1.0) {
        VVA_pressure_dynamic_basefre_DW.clockTickCounter = 0;
      } else {
        VVA_pressure_dynamic_basefre_DW.clockTickCounter++;
      }
    }
  }

  if (rtmIsMajorTimeStep(VVA_pressure_dynamic_basefre_M)) {
    /* Matfile logging */
    rt_UpdateTXYLogVars(VVA_pressure_dynamic_basefre_M->rtwLogInfo,
                        (VVA_pressure_dynamic_basefre_M->Timing.t));
  }                                    /* end MajorTimeStep */

  if (rtmIsMajorTimeStep(VVA_pressure_dynamic_basefre_M)) {
    real_T *lastU;
    if (rtmIsMajorTimeStep(VVA_pressure_dynamic_basefre_M) &&
        VVA_pressure_dynamic_basefre_M->Timing.TaskCounters.TID[4] == 0) {
      /* Update for UnitDelay: '<S33>/Output' */
      VVA_pressure_dynamic_basefre_DW.Output_DSTATE = rtb_FixPtSwitch;
    }

    /* Update for Derivative: '<Root>/Derivative1' */
    if (VVA_pressure_dynamic_basefre_DW.TimeStampA == (rtInf)) {
      VVA_pressure_dynamic_basefre_DW.TimeStampA =
        VVA_pressure_dynamic_basefre_M->Timing.t[0];
      lastU = &VVA_pressure_dynamic_basefre_DW.LastUAtTimeA;
    } else if (VVA_pressure_dynamic_basefre_DW.TimeStampB == (rtInf)) {
      VVA_pressure_dynamic_basefre_DW.TimeStampB =
        VVA_pressure_dynamic_basefre_M->Timing.t[0];
      lastU = &VVA_pressure_dynamic_basefre_DW.LastUAtTimeB;
    } else if (VVA_pressure_dynamic_basefre_DW.TimeStampA <
               VVA_pressure_dynamic_basefre_DW.TimeStampB) {
      VVA_pressure_dynamic_basefre_DW.TimeStampA =
        VVA_pressure_dynamic_basefre_M->Timing.t[0];
      lastU = &VVA_pressure_dynamic_basefre_DW.LastUAtTimeA;
    } else {
      VVA_pressure_dynamic_basefre_DW.TimeStampB =
        VVA_pressure_dynamic_basefre_M->Timing.t[0];
      lastU = &VVA_pressure_dynamic_basefre_DW.LastUAtTimeB;
    }

    *lastU = VVA_pressure_dynamic_basefreq_B.u;

    /* End of Update for Derivative: '<Root>/Derivative1' */

    /* Update for Derivative: '<Root>/Derivative' */
    if (VVA_pressure_dynamic_basefre_DW.TimeStampA_h == (rtInf)) {
      VVA_pressure_dynamic_basefre_DW.TimeStampA_h =
        VVA_pressure_dynamic_basefre_M->Timing.t[0];
      lastU = &VVA_pressure_dynamic_basefre_DW.LastUAtTimeA_o;
    } else if (VVA_pressure_dynamic_basefre_DW.TimeStampB_a == (rtInf)) {
      VVA_pressure_dynamic_basefre_DW.TimeStampB_a =
        VVA_pressure_dynamic_basefre_M->Timing.t[0];
      lastU = &VVA_pressure_dynamic_basefre_DW.LastUAtTimeB_n;
    } else if (VVA_pressure_dynamic_basefre_DW.TimeStampA_h <
               VVA_pressure_dynamic_basefre_DW.TimeStampB_a) {
      VVA_pressure_dynamic_basefre_DW.TimeStampA_h =
        VVA_pressure_dynamic_basefre_M->Timing.t[0];
      lastU = &VVA_pressure_dynamic_basefre_DW.LastUAtTimeA_o;
    } else {
      VVA_pressure_dynamic_basefre_DW.TimeStampB_a =
        VVA_pressure_dynamic_basefre_M->Timing.t[0];
      lastU = &VVA_pressure_dynamic_basefre_DW.LastUAtTimeB_n;
    }

    *lastU = VVA_pressure_dynamic_basefreq_B.ManualSwitch1;

    /* End of Update for Derivative: '<Root>/Derivative' */

    /* Update for Derivative: '<Root>/Derivative2' */
    if (VVA_pressure_dynamic_basefre_DW.TimeStampA_p == (rtInf)) {
      VVA_pressure_dynamic_basefre_DW.TimeStampA_p =
        VVA_pressure_dynamic_basefre_M->Timing.t[0];
      lastU = &VVA_pressure_dynamic_basefre_DW.LastUAtTimeA_b;
    } else if (VVA_pressure_dynamic_basefre_DW.TimeStampB_l == (rtInf)) {
      VVA_pressure_dynamic_basefre_DW.TimeStampB_l =
        VVA_pressure_dynamic_basefre_M->Timing.t[0];
      lastU = &VVA_pressure_dynamic_basefre_DW.LastUAtTimeB_l;
    } else if (VVA_pressure_dynamic_basefre_DW.TimeStampA_p <
               VVA_pressure_dynamic_basefre_DW.TimeStampB_l) {
      VVA_pressure_dynamic_basefre_DW.TimeStampA_p =
        VVA_pressure_dynamic_basefre_M->Timing.t[0];
      lastU = &VVA_pressure_dynamic_basefre_DW.LastUAtTimeA_b;
    } else {
      VVA_pressure_dynamic_basefre_DW.TimeStampB_l =
        VVA_pressure_dynamic_basefre_M->Timing.t[0];
      lastU = &VVA_pressure_dynamic_basefre_DW.LastUAtTimeB_l;
    }

    *lastU = VVA_pressure_dynamic_basefreq_B.de;

    /* End of Update for Derivative: '<Root>/Derivative2' */
    if (rtmIsMajorTimeStep(VVA_pressure_dynamic_basefre_M) &&
        VVA_pressure_dynamic_basefre_M->Timing.TaskCounters.TID[1] == 0) {
      /* Update for UnitDelay: '<Root>/Unit Delay' */
      VVA_pressure_dynamic_basefre_DW.UnitDelay_DSTATE =
        VVA_pressure_dynamic_basefreq_B.f_out;

      /* Update for Memory: '<Root>/Memory2' */
      VVA_pressure_dynamic_basefre_DW.Memory2_PreviousInput =
        VVA_pressure_dynamic_basefreq_B.dP;
    }
  }                                    /* end MajorTimeStep */

  if (rtmIsMajorTimeStep(VVA_pressure_dynamic_basefre_M)) {
    /* signal main to stop simulation */
    {                                  /* Sample time: [0.0s, 0.0s] */
      if ((rtmGetTFinal(VVA_pressure_dynamic_basefre_M)!=-1) &&
          !((rtmGetTFinal(VVA_pressure_dynamic_basefre_M)-
             (((VVA_pressure_dynamic_basefre_M->Timing.clockTick1+
                VVA_pressure_dynamic_basefre_M->Timing.clockTickH1* 4294967296.0))
              * 0.0005)) > (((VVA_pressure_dynamic_basefre_M->Timing.clockTick1+
                              VVA_pressure_dynamic_basefre_M->Timing.clockTickH1*
              4294967296.0)) * 0.0005) * (DBL_EPSILON))) {
        rtmSetErrorStatus(VVA_pressure_dynamic_basefre_M, "Simulation finished");
      }
    }

    rt_ertODEUpdateContinuousStates(&VVA_pressure_dynamic_basefre_M->solverInfo);

    /* Update absolute time for base rate */
    /* The "clockTick0" counts the number of times the code of this task has
     * been executed. The absolute time is the multiplication of "clockTick0"
     * and "Timing.stepSize0". Size of "clockTick0" ensures timer will not
     * overflow during the application lifespan selected.
     * Timer of this task consists of two 32 bit unsigned integers.
     * The two integers represent the low bits Timing.clockTick0 and the high bits
     * Timing.clockTickH0. When the low bit overflows to 0, the high bits increment.
     */
    if (!(++VVA_pressure_dynamic_basefre_M->Timing.clockTick0)) {
      ++VVA_pressure_dynamic_basefre_M->Timing.clockTickH0;
    }

    VVA_pressure_dynamic_basefre_M->Timing.t[0] = rtsiGetSolverStopTime
      (&VVA_pressure_dynamic_basefre_M->solverInfo);

    {
      /* Update absolute timer for sample time: [0.0005s, 0.0s] */
      /* The "clockTick1" counts the number of times the code of this task has
       * been executed. The resolution of this integer timer is 0.0005, which is the step size
       * of the task. Size of "clockTick1" ensures timer will not overflow during the
       * application lifespan selected.
       * Timer of this task consists of two 32 bit unsigned integers.
       * The two integers represent the low bits Timing.clockTick1 and the high bits
       * Timing.clockTickH1. When the low bit overflows to 0, the high bits increment.
       */
      VVA_pressure_dynamic_basefre_M->Timing.clockTick1++;
      if (!VVA_pressure_dynamic_basefre_M->Timing.clockTick1) {
        VVA_pressure_dynamic_basefre_M->Timing.clockTickH1++;
      }
    }

    rate_scheduler();
  }                                    /* end MajorTimeStep */
}

/* Derivatives for root system: '<Root>' */
void VVA_pressure_dynamic_basefreq_sin_derivatives(void)
{
  XDot_VVA_pressure_dynamic_bas_T *_rtXdot;
  _rtXdot = ((XDot_VVA_pressure_dynamic_bas_T *)
             VVA_pressure_dynamic_basefre_M->derivs);

  /* Derivatives for Integrator: '<Root>/设置压力起始值' */
  _rtXdot->_CSTATE = VVA_pressure_dynamic_basefreq_B.Memory2;

  /* Derivatives for Integrator: '<Root>/Integrator' */
  _rtXdot->Integrator_CSTATE = VVA_pressure_dynamic_basefreq_B.Abs;

  /* Derivatives for Integrator: '<Root>/Integrator3' */
  _rtXdot->Integrator3_CSTATE = VVA_pressure_dynamic_basefreq_B.uDLookupTable2;
}

/* Model initialize function */
void VVA_pressure_dynamic_basefreq_sin_initialize(void)
{
  /* Registration code */

  /* initialize non-finites */
  rt_InitInfAndNaN(sizeof(real_T));

  /* initialize real-time model */
  (void) memset((void *)VVA_pressure_dynamic_basefre_M, 0,
                sizeof(RT_MODEL_VVA_pressure_dynamic_T));

  {
    /* Setup solver object */
    rtsiSetSimTimeStepPtr(&VVA_pressure_dynamic_basefre_M->solverInfo,
                          &VVA_pressure_dynamic_basefre_M->Timing.simTimeStep);
    rtsiSetTPtr(&VVA_pressure_dynamic_basefre_M->solverInfo, &rtmGetTPtr
                (VVA_pressure_dynamic_basefre_M));
    rtsiSetStepSizePtr(&VVA_pressure_dynamic_basefre_M->solverInfo,
                       &VVA_pressure_dynamic_basefre_M->Timing.stepSize0);
    rtsiSetdXPtr(&VVA_pressure_dynamic_basefre_M->solverInfo,
                 &VVA_pressure_dynamic_basefre_M->derivs);
    rtsiSetContStatesPtr(&VVA_pressure_dynamic_basefre_M->solverInfo, (real_T **)
                         &VVA_pressure_dynamic_basefre_M->contStates);
    rtsiSetNumContStatesPtr(&VVA_pressure_dynamic_basefre_M->solverInfo,
      &VVA_pressure_dynamic_basefre_M->Sizes.numContStates);
    rtsiSetNumPeriodicContStatesPtr(&VVA_pressure_dynamic_basefre_M->solverInfo,
      &VVA_pressure_dynamic_basefre_M->Sizes.numPeriodicContStates);
    rtsiSetPeriodicContStateIndicesPtr
      (&VVA_pressure_dynamic_basefre_M->solverInfo,
       &VVA_pressure_dynamic_basefre_M->periodicContStateIndices);
    rtsiSetPeriodicContStateRangesPtr
      (&VVA_pressure_dynamic_basefre_M->solverInfo,
       &VVA_pressure_dynamic_basefre_M->periodicContStateRanges);
    rtsiSetContStateDisabledPtr(&VVA_pressure_dynamic_basefre_M->solverInfo,
      (boolean_T**) &VVA_pressure_dynamic_basefre_M->contStateDisabled);
    rtsiSetErrorStatusPtr(&VVA_pressure_dynamic_basefre_M->solverInfo,
                          (&rtmGetErrorStatus(VVA_pressure_dynamic_basefre_M)));
    rtsiSetRTModelPtr(&VVA_pressure_dynamic_basefre_M->solverInfo,
                      VVA_pressure_dynamic_basefre_M);
  }

  rtsiSetSimTimeStep(&VVA_pressure_dynamic_basefre_M->solverInfo,
                     MAJOR_TIME_STEP);
  VVA_pressure_dynamic_basefre_M->intgData.y =
    VVA_pressure_dynamic_basefre_M->odeY;
  VVA_pressure_dynamic_basefre_M->intgData.f[0] =
    VVA_pressure_dynamic_basefre_M->odeF[0];
  VVA_pressure_dynamic_basefre_M->intgData.f[1] =
    VVA_pressure_dynamic_basefre_M->odeF[1];
  VVA_pressure_dynamic_basefre_M->intgData.f[2] =
    VVA_pressure_dynamic_basefre_M->odeF[2];
  VVA_pressure_dynamic_basefre_M->contStates = ((X_VVA_pressure_dynamic_basefr_T
    *) &VVA_pressure_dynamic_basefreq_X);
  VVA_pressure_dynamic_basefre_M->contStateDisabled =
    ((XDis_VVA_pressure_dynamic_bas_T *) &VVA_pressure_dynamic_basef_XDis);
  VVA_pressure_dynamic_basefre_M->Timing.tStart = (0.0);
  rtsiSetSolverData(&VVA_pressure_dynamic_basefre_M->solverInfo, (void *)
                    &VVA_pressure_dynamic_basefre_M->intgData);
  rtsiSetIsMinorTimeStepWithModeChange
    (&VVA_pressure_dynamic_basefre_M->solverInfo, false);
  rtsiSetSolverName(&VVA_pressure_dynamic_basefre_M->solverInfo,"ode3");
  rtmSetTPtr(VVA_pressure_dynamic_basefre_M,
             &VVA_pressure_dynamic_basefre_M->Timing.tArray[0]);
  rtmSetTFinal(VVA_pressure_dynamic_basefre_M, 6.0);
  VVA_pressure_dynamic_basefre_M->Timing.stepSize0 = 0.0005;

  /* Setup for data logging */
  {
    static RTWLogInfo rt_DataLoggingInfo;
    rt_DataLoggingInfo.loggingInterval = (NULL);
    VVA_pressure_dynamic_basefre_M->rtwLogInfo = &rt_DataLoggingInfo;
  }

  /* Setup for data logging */
  {
    rtliSetLogXSignalInfo(VVA_pressure_dynamic_basefre_M->rtwLogInfo, (NULL));
    rtliSetLogXSignalPtrs(VVA_pressure_dynamic_basefre_M->rtwLogInfo, (NULL));
    rtliSetLogT(VVA_pressure_dynamic_basefre_M->rtwLogInfo, "tout");
    rtliSetLogX(VVA_pressure_dynamic_basefre_M->rtwLogInfo, "");
    rtliSetLogXFinal(VVA_pressure_dynamic_basefre_M->rtwLogInfo, "");
    rtliSetLogVarNameModifier(VVA_pressure_dynamic_basefre_M->rtwLogInfo, "rt_");
    rtliSetLogFormat(VVA_pressure_dynamic_basefre_M->rtwLogInfo, 4);
    rtliSetLogMaxRows(VVA_pressure_dynamic_basefre_M->rtwLogInfo, 0);
    rtliSetLogDecimation(VVA_pressure_dynamic_basefre_M->rtwLogInfo, 1);
    rtliSetLogY(VVA_pressure_dynamic_basefre_M->rtwLogInfo, "");
    rtliSetLogYSignalInfo(VVA_pressure_dynamic_basefre_M->rtwLogInfo, (NULL));
    rtliSetLogYSignalPtrs(VVA_pressure_dynamic_basefre_M->rtwLogInfo, (NULL));
  }

  /* block I/O */
  (void) memset(((void *) &VVA_pressure_dynamic_basefreq_B), 0,
                sizeof(B_VVA_pressure_dynamic_basefr_T));

  /* states (continuous) */
  {
    (void) memset((void *)&VVA_pressure_dynamic_basefreq_X, 0,
                  sizeof(X_VVA_pressure_dynamic_basefr_T));
  }

  /* disabled states */
  {
    (void) memset((void *)&VVA_pressure_dynamic_basef_XDis, 0,
                  sizeof(XDis_VVA_pressure_dynamic_bas_T));
  }

  /* states (dwork) */
  (void) memset((void *)&VVA_pressure_dynamic_basefre_DW, 0,
                sizeof(DW_VVA_pressure_dynamic_basef_T));

  /* Matfile logging */
  rt_StartDataLoggingWithStartTime(VVA_pressure_dynamic_basefre_M->rtwLogInfo,
    0.0, rtmGetTFinal(VVA_pressure_dynamic_basefre_M),
    VVA_pressure_dynamic_basefre_M->Timing.stepSize0, (&rtmGetErrorStatus
    (VVA_pressure_dynamic_basefre_M)));

  /* Start for Lookup_n-D: '<S17>/1-D Lookup Table2' */
  {
    rt_LUTnWork *TWork_start = (rt_LUTnWork *)
      &VVA_pressure_dynamic_basefre_DW.TWork[0];
    void **bpDataSet = (void **) &VVA_pressure_dynamic_basefre_DW.m_bpDataSet[0];
    TWork_start->m_dimSizes = (const uint32_T *)
      VVA_pressure_dynamic_basefreq_P.uDLookupTable2_dimSizes;
    TWork_start->m_tableData = (void *) VVA_pressure_dynamic_basefreq_P.z;
    TWork_start->m_bpDataSet = bpDataSet;
    TWork_start->m_bpIndex = &VVA_pressure_dynamic_basefre_DW.m_bpIndex[0];
    TWork_start->m_bpLambda = &VVA_pressure_dynamic_basefre_DW.m_bpLambda[0];
    TWork_start->m_maxIndex = (const uint32_T *)
      VVA_pressure_dynamic_basefreq_P.uDLookupTable2_maxIndex;
    bpDataSet[0] = (void *) VVA_pressure_dynamic_basefreq_P.y;
    bpDataSet[1] = (void *) VVA_pressure_dynamic_basefreq_P.x;
  }

  {
    const real_T **bpDataSet;
    const real_T *xp, *yp;
    real_T *dp;
    uint32_T len;
    const rt_LUTnWork *TWork_interp;
    rt_LUTSplineWork *rt_SplWk = (rt_LUTSplineWork*)
      &VVA_pressure_dynamic_basefre_DW.SWork[0];
    rt_SplWk->m_TWork = (rt_LUTnWork*)&VVA_pressure_dynamic_basefre_DW.TWork[0];
    rt_SplWk->m_yyA = &VVA_pressure_dynamic_basefre_DW.m_yyA[0];
    rt_SplWk->m_yyB = &VVA_pressure_dynamic_basefre_DW.m_yyB[0];
    rt_SplWk->m_yy2 = &VVA_pressure_dynamic_basefre_DW.m_yy2[0];
    rt_SplWk->m_up = &VVA_pressure_dynamic_basefre_DW.m_up[0];
    rt_SplWk->m_y2 = &VVA_pressure_dynamic_basefre_DW.m_y2[0];
    rt_SplWk->m_numYWorkElts =
      VVA_pressure_dynamic_basefreq_P.uDLookupTable2_numYWorkElts;
    rt_SplWk->m_reCalc =
      &VVA_pressure_dynamic_basefre_DW.reCalcSecDerivFirstDimCoeffs;
    rt_SplWk->m_preBp0AndTable =
      &VVA_pressure_dynamic_basefre_DW.prevBp0AndTableData[0];
    *rt_SplWk->m_reCalc = 1;

    /* cache table data and first breakpoint data */
    TWork_interp = (const rt_LUTnWork *)rt_SplWk->m_TWork;
    bpDataSet = (const real_T **) TWork_interp->m_bpDataSet;
    xp = bpDataSet[0U];
    len = TWork_interp->m_maxIndex[0U] + 1U;
    dp = (real_T *) rt_SplWk->m_preBp0AndTable;
    yp = (real_T *) TWork_interp->m_tableData;
    (void) memcpy(dp, xp,
                  len * sizeof(real_T));
    dp = &(dp[len]);

    /* save the table data */
    (void) memcpy(dp, yp,
                  len * rt_SplWk->m_numYWorkElts[0U] * sizeof(real_T));
  }

  /* Start for Lookup_n-D: '<S17>/2-D Lookup Table3' */
  {
    rt_LUTnWork *TWork_start = (rt_LUTnWork *)
      &VVA_pressure_dynamic_basefre_DW.TWork_f[0];
    void **bpDataSet = (void **) &VVA_pressure_dynamic_basefre_DW.m_bpDataSet_j
      [0];
    TWork_start->m_dimSizes = (const uint32_T *)
      VVA_pressure_dynamic_basefreq_P.uDLookupTable3_dimSizes;
    TWork_start->m_tableData = (void *) VVA_pressure_dynamic_basefreq_P.z;
    TWork_start->m_bpDataSet = bpDataSet;
    TWork_start->m_bpIndex = &VVA_pressure_dynamic_basefre_DW.m_bpIndex_a[0];
    TWork_start->m_bpLambda = &VVA_pressure_dynamic_basefre_DW.m_bpLambda_o[0];
    TWork_start->m_maxIndex = (const uint32_T *)
      VVA_pressure_dynamic_basefreq_P.uDLookupTable3_maxIndex;
    bpDataSet[0] = (void *) VVA_pressure_dynamic_basefreq_P.y;
    bpDataSet[1] = (void *) VVA_pressure_dynamic_basefreq_P.x;
  }

  {
    const real_T **bpDataSet;
    const real_T *xp, *yp;
    real_T *dp;
    uint32_T len;
    const rt_LUTnWork *TWork_interp;
    rt_LUTSplineWork *rt_SplWk = (rt_LUTSplineWork*)
      &VVA_pressure_dynamic_basefre_DW.SWork_h[0];
    rt_SplWk->m_TWork = (rt_LUTnWork*)&VVA_pressure_dynamic_basefre_DW.TWork_f[0];
    rt_SplWk->m_yyA = &VVA_pressure_dynamic_basefre_DW.m_yyA_n[0];
    rt_SplWk->m_yyB = &VVA_pressure_dynamic_basefre_DW.m_yyB_o[0];
    rt_SplWk->m_yy2 = &VVA_pressure_dynamic_basefre_DW.m_yy2_l[0];
    rt_SplWk->m_up = &VVA_pressure_dynamic_basefre_DW.m_up_g[0];
    rt_SplWk->m_y2 = &VVA_pressure_dynamic_basefre_DW.m_y2_i[0];
    rt_SplWk->m_numYWorkElts =
      VVA_pressure_dynamic_basefreq_P.uDLookupTable3_numYWorkElts;
    rt_SplWk->m_reCalc =
      &VVA_pressure_dynamic_basefre_DW.reCalcSecDerivFirstDimCoeffs_a;
    rt_SplWk->m_preBp0AndTable =
      &VVA_pressure_dynamic_basefre_DW.prevBp0AndTableData_k[0];
    *rt_SplWk->m_reCalc = 1;

    /* cache table data and first breakpoint data */
    TWork_interp = (const rt_LUTnWork *)rt_SplWk->m_TWork;
    bpDataSet = (const real_T **) TWork_interp->m_bpDataSet;
    xp = bpDataSet[0U];
    len = TWork_interp->m_maxIndex[0U] + 1U;
    dp = (real_T *) rt_SplWk->m_preBp0AndTable;
    yp = (real_T *) TWork_interp->m_tableData;
    (void) memcpy(dp, xp,
                  len * sizeof(real_T));
    dp = &(dp[len]);

    /* save the table data */
    (void) memcpy(dp, yp,
                  len * rt_SplWk->m_numYWorkElts[0U] * sizeof(real_T));
  }

  /* Start for DiscretePulseGenerator: '<Root>/Pulse Generator' */
  VVA_pressure_dynamic_basefre_DW.clockTickCounter = 0;
  VVA_pressure_dynamic_ba_PrevZCX.EnabledandTriggeredSubsystem8.EnabledandTriggeredSubsystem1_T
    = UNINITIALIZED_ZCSIG;
  VVA_pressure_dynamic_ba_PrevZCX.EnabledandTriggeredSubsystem7.EnabledandTriggeredSubsystem1_T
    = UNINITIALIZED_ZCSIG;
  VVA_pressure_dynamic_ba_PrevZCX.EnabledandTriggeredSubsystem5.EnabledandTriggeredSubsystem1_T
    = UNINITIALIZED_ZCSIG;
  VVA_pressure_dynamic_ba_PrevZCX.EnabledandTriggeredSubsystem4.EnabledandTriggeredSubsystem1_T
    = UNINITIALIZED_ZCSIG;
  VVA_pressure_dynamic_ba_PrevZCX.EnabledandTriggeredSubsystem3.EnabledandTriggeredSubsystem1_T
    = UNINITIALIZED_ZCSIG;
  VVA_pressure_dynamic_ba_PrevZCX.EnabledandTriggeredSubsystem2.EnabledandTriggeredSubsystem1_T
    = UNINITIALIZED_ZCSIG;
  VVA_pressure_dynamic_ba_PrevZCX.EnabledandTriggeredSubsystem1.EnabledandTriggeredSubsystem1_T
    = UNINITIALIZED_ZCSIG;

  /* InitializeConditions for Integrator: '<Root>/设置压力起始值' */
  VVA_pressure_dynamic_basefreq_X._CSTATE = VVA_pressure_dynamic_basefreq_P._IC;

  /* InitializeConditions for UnitDelay: '<S33>/Output' */
  VVA_pressure_dynamic_basefre_DW.Output_DSTATE =
    VVA_pressure_dynamic_basefreq_P.Output_InitialCondition;

  /* InitializeConditions for Derivative: '<Root>/Derivative1' */
  VVA_pressure_dynamic_basefre_DW.TimeStampA = (rtInf);
  VVA_pressure_dynamic_basefre_DW.TimeStampB = (rtInf);

  /* InitializeConditions for Derivative: '<Root>/Derivative' */
  VVA_pressure_dynamic_basefre_DW.TimeStampA_h = (rtInf);
  VVA_pressure_dynamic_basefre_DW.TimeStampB_a = (rtInf);

  /* InitializeConditions for Derivative: '<Root>/Derivative2' */
  VVA_pressure_dynamic_basefre_DW.TimeStampA_p = (rtInf);
  VVA_pressure_dynamic_basefre_DW.TimeStampB_l = (rtInf);

  /* InitializeConditions for UnitDelay: '<Root>/Unit Delay' */
  VVA_pressure_dynamic_basefre_DW.UnitDelay_DSTATE =
    VVA_pressure_dynamic_basefreq_P.UnitDelay_InitialCondition;

  /* InitializeConditions for Integrator: '<Root>/Integrator' */
  VVA_pressure_dynamic_basefreq_X.Integrator_CSTATE =
    VVA_pressure_dynamic_basefreq_P.Integrator_IC;

  /* InitializeConditions for Integrator: '<Root>/Integrator3' */
  VVA_pressure_dynamic_basefreq_X.Integrator3_CSTATE =
    VVA_pressure_dynamic_basefreq_P.Integrator3_IC;

  /* InitializeConditions for Memory: '<Root>/Memory2' */
  VVA_pressure_dynamic_basefre_DW.Memory2_PreviousInput =
    VVA_pressure_dynamic_basefreq_P.Memory2_InitialCondition;

  /* SystemInitialize for MATLAB Function: '<Root>/频率' */
  VVA_pressure_dynamic_basefre_DW.eta_prev_not_empty = false;
  VVA_pressure_dynamic_basefre_DW.burst_cnt = 0;
  VVA_pressure_dynamic_basefre_DW.f_old = 100.0;

  /* SystemInitialize for MATLAB Function: '<Root>/FreqPolicy' */
  VVA_pressure_dynamic_basefre_DW.de_f_not_empty = false;
  VVA_pressure_dynamic_basefre_DW.hold_cnt_p = 0;
  VVA_pressure_dynamic_basefre_DW.burst_cnt_n = 0;
  VVA_pressure_dynamic_basefre_DW.stall_cnt = 0;

  /* SystemInitialize for MATLAB Function: '<Root>/BaseFreqOpt' */
  VVA_pressure_dynamic_basefre_DW.f_prev_not_empty = false;
  VVA_pressure_dynamic_basefre_DW.S = 0.0;

  /* SystemInitialize for MATLAB Function: '<Root>/varFreqPWM' */
  VVA_pressure_dynamic_basefre_DW.phi = 0.0;

  /* SystemInitialize for MATLAB Function: '<Root>/模式选择' */
  VVA_pressure_dynamic_basefre_DW.a_prev = 2;
  VVA_pressure_dynamic_basefre_DW.hold_cnt = 0;

  /* SystemInitialize for MATLAB Function: '<S26>/ 电磁阀滞后' */
  VVA_pressure_dynamic_bas_u_Init(&VVA_pressure_dynamic_basefre_DW.sf__d);

  /* SystemInitialize for MATLAB Function: '<S26>/ 电磁阀滞后2' */
  VVA_pressure_dynamic_bas_u_Init(&VVA_pressure_dynamic_basefre_DW.sf_2_m);
}

/* Model terminate function */
void VVA_pressure_dynamic_basefreq_sin_terminate(void)
{
  /* (no terminate code required) */
}
