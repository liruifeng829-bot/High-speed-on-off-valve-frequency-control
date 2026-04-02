/*
 * VVA_pressure_dynamic_basefreq_sin_private.h
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

#ifndef RTW_HEADER_VVA_pressure_dynamic_basefreq_sin_private_h_
#define RTW_HEADER_VVA_pressure_dynamic_basefreq_sin_private_h_
#include "rtwtypes.h"
#include "builtin_typeid_types.h"
#include "multiword_types.h"
#include "zero_crossing_types.h"
#include "VVA_pressure_dynamic_basefreq_sin.h"
#include "VVA_pressure_dynamic_basefreq_sin_types.h"
#include "rtw_continuous.h"
#include "rtw_solver.h"

/* Private macros used by the generated code to access rtModel */
#ifndef rtmIsMajorTimeStep
#define rtmIsMajorTimeStep(rtm)        (((rtm)->Timing.simTimeStep) == MAJOR_TIME_STEP)
#endif

#ifndef rtmIsMinorTimeStep
#define rtmIsMinorTimeStep(rtm)        (((rtm)->Timing.simTimeStep) == MINOR_TIME_STEP)
#endif

#ifndef rtmSetTFinal
#define rtmSetTFinal(rtm, val)         ((rtm)->Timing.tFinal = (val))
#endif

#ifndef rtmSetTPtr
#define rtmSetTPtr(rtm, val)           ((rtm)->Timing.t = (val))
#endif

real_T look_SplNBinSZcd(uint32_T numDims, const real_T* u, const
  rt_LUTSplineWork * const SWork);
void rt_Spline2Derivd(const real_T *x, const real_T *y, uint32_T n, real_T *u,
                      real_T *y2);
real_T intrp_NSplcd(uint32_T numDims, const rt_LUTSplineWork * const splWork,
                    uint32_T extrapMethod);
extern uint32_T plook_binx(real_T u, const real_T bp[], uint32_T maxIndex,
  real_T *fraction);
extern uint32_T binsearch_u32d(real_T u, const real_T bp[], uint32_T startIndex,
  uint32_T maxIndex);
extern void VVA_pressure_dynamic_bas_u_Init(DW_u_VVA_pressure_dynamic_bas_T
  *localDW);
extern void VVA_pressure_dynamic_basefreq_u(real_T rtu_current, real_T rtu_d,
  B_u_VVA_pressure_dynamic_base_T *localB, DW_u_VVA_pressure_dynamic_bas_T
  *localDW);

/* private model entry point functions */
extern void VVA_pressure_dynamic_basefreq_sin_derivatives(void);

#endif             /* RTW_HEADER_VVA_pressure_dynamic_basefreq_sin_private_h_ */
