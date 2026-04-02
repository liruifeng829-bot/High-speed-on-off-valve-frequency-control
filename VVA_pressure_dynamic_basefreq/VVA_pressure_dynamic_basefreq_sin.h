/*
 * VVA_pressure_dynamic_basefreq_sin.h
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

#ifndef RTW_HEADER_VVA_pressure_dynamic_basefreq_sin_h_
#define RTW_HEADER_VVA_pressure_dynamic_basefreq_sin_h_
#ifndef VVA_pressure_dynamic_basefreq_sin_COMMON_INCLUDES_
#define VVA_pressure_dynamic_basefreq_sin_COMMON_INCLUDES_
#include "rtwtypes.h"
#include "rtw_continuous.h"
#include "rtw_solver.h"
#include "rt_logging.h"
#endif                  /* VVA_pressure_dynamic_basefreq_sin_COMMON_INCLUDES_ */

#include "VVA_pressure_dynamic_basefreq_sin_types.h"
#include "rt_nonfinite.h"
#include "rtsplntypes.h"
#include <string.h>
#include "rtGetInf.h"
#include <float.h>
#include <stddef.h>
#include "zero_crossing_types.h"

/* Macros for accessing real-time model data structure */
#ifndef rtmGetContStateDisabled
#define rtmGetContStateDisabled(rtm)   ((rtm)->contStateDisabled)
#endif

#ifndef rtmSetContStateDisabled
#define rtmSetContStateDisabled(rtm, val) ((rtm)->contStateDisabled = (val))
#endif

#ifndef rtmGetContStates
#define rtmGetContStates(rtm)          ((rtm)->contStates)
#endif

#ifndef rtmSetContStates
#define rtmSetContStates(rtm, val)     ((rtm)->contStates = (val))
#endif

#ifndef rtmGetContTimeOutputInconsistentWithStateAtMajorStepFlag
#define rtmGetContTimeOutputInconsistentWithStateAtMajorStepFlag(rtm) ((rtm)->CTOutputIncnstWithState)
#endif

#ifndef rtmSetContTimeOutputInconsistentWithStateAtMajorStepFlag
#define rtmSetContTimeOutputInconsistentWithStateAtMajorStepFlag(rtm, val) ((rtm)->CTOutputIncnstWithState = (val))
#endif

#ifndef rtmGetDerivCacheNeedsReset
#define rtmGetDerivCacheNeedsReset(rtm) ((rtm)->derivCacheNeedsReset)
#endif

#ifndef rtmSetDerivCacheNeedsReset
#define rtmSetDerivCacheNeedsReset(rtm, val) ((rtm)->derivCacheNeedsReset = (val))
#endif

#ifndef rtmGetFinalTime
#define rtmGetFinalTime(rtm)           ((rtm)->Timing.tFinal)
#endif

#ifndef rtmGetIntgData
#define rtmGetIntgData(rtm)            ((rtm)->intgData)
#endif

#ifndef rtmSetIntgData
#define rtmSetIntgData(rtm, val)       ((rtm)->intgData = (val))
#endif

#ifndef rtmGetOdeF
#define rtmGetOdeF(rtm)                ((rtm)->odeF)
#endif

#ifndef rtmSetOdeF
#define rtmSetOdeF(rtm, val)           ((rtm)->odeF = (val))
#endif

#ifndef rtmGetOdeY
#define rtmGetOdeY(rtm)                ((rtm)->odeY)
#endif

#ifndef rtmSetOdeY
#define rtmSetOdeY(rtm, val)           ((rtm)->odeY = (val))
#endif

#ifndef rtmGetPeriodicContStateIndices
#define rtmGetPeriodicContStateIndices(rtm) ((rtm)->periodicContStateIndices)
#endif

#ifndef rtmSetPeriodicContStateIndices
#define rtmSetPeriodicContStateIndices(rtm, val) ((rtm)->periodicContStateIndices = (val))
#endif

#ifndef rtmGetPeriodicContStateRanges
#define rtmGetPeriodicContStateRanges(rtm) ((rtm)->periodicContStateRanges)
#endif

#ifndef rtmSetPeriodicContStateRanges
#define rtmSetPeriodicContStateRanges(rtm, val) ((rtm)->periodicContStateRanges = (val))
#endif

#ifndef rtmGetRTWLogInfo
#define rtmGetRTWLogInfo(rtm)          ((rtm)->rtwLogInfo)
#endif

#ifndef rtmGetZCCacheNeedsReset
#define rtmGetZCCacheNeedsReset(rtm)   ((rtm)->zCCacheNeedsReset)
#endif

#ifndef rtmSetZCCacheNeedsReset
#define rtmSetZCCacheNeedsReset(rtm, val) ((rtm)->zCCacheNeedsReset = (val))
#endif

#ifndef rtmGetdX
#define rtmGetdX(rtm)                  ((rtm)->derivs)
#endif

#ifndef rtmSetdX
#define rtmSetdX(rtm, val)             ((rtm)->derivs = (val))
#endif

#ifndef rtmGetErrorStatus
#define rtmGetErrorStatus(rtm)         ((rtm)->errorStatus)
#endif

#ifndef rtmSetErrorStatus
#define rtmSetErrorStatus(rtm, val)    ((rtm)->errorStatus = (val))
#endif

#ifndef rtmGetStopRequested
#define rtmGetStopRequested(rtm)       ((rtm)->Timing.stopRequestedFlag)
#endif

#ifndef rtmSetStopRequested
#define rtmSetStopRequested(rtm, val)  ((rtm)->Timing.stopRequestedFlag = (val))
#endif

#ifndef rtmGetStopRequestedPtr
#define rtmGetStopRequestedPtr(rtm)    (&((rtm)->Timing.stopRequestedFlag))
#endif

#ifndef rtmGetT
#define rtmGetT(rtm)                   (rtmGetTPtr((rtm))[0])
#endif

#ifndef rtmGetTFinal
#define rtmGetTFinal(rtm)              ((rtm)->Timing.tFinal)
#endif

#ifndef rtmGetTPtr
#define rtmGetTPtr(rtm)                ((rtm)->Timing.t)
#endif

#ifndef rtmGetTStart
#define rtmGetTStart(rtm)              ((rtm)->Timing.tStart)
#endif

#define VVA_pressure_dynamic_basefreq_sin_M (VVA_pressure_dynamic_basefre_M)

/* Zero-crossing (trigger) state for system '<Root>/Enabled and Triggered Subsystem1' */
typedef struct {
  ZCSigState EnabledandTriggeredSubsystem1_T;
                                 /* '<Root>/Enabled and Triggered Subsystem1' */
} ZCE_EnabledandTriggeredSubsys_T;

/* Block signals for system '<S26>/ 电磁阀滞后' */
typedef struct {
  real_T next;                         /* '<S26>/ 电磁阀滞后' */
} B_u_VVA_pressure_dynamic_base_T;

/* Block states (default storage) for system '<S26>/ 电磁阀滞后' */
typedef struct {
  uint32_T method;                     /* '<S26>/ 电磁阀滞后' */
  uint32_T state;                      /* '<S26>/ 电磁阀滞后' */
  uint32_T state_g[2];                 /* '<S26>/ 电磁阀滞后' */
  uint32_T state_h[625];               /* '<S26>/ 电磁阀滞后' */
} DW_u_VVA_pressure_dynamic_bas_T;

/* Block signals (default storage) */
typedef struct {
  real_T u;                            /* '<Root>/设置压力起始值' */
  real_T Output;                       /* '<S18>/Output' */
  real_T ManualSwitch1;                /* '<Root>/Manual Switch1' */
  real_T u_n;                          /* '<Root>/Saturation4' */
  real_T Saturation1;                  /* '<Root>/Saturation1' */
  real_T Gain11;                       /* '<Root>/Gain11' */
  real_T Gain12;                       /* '<Root>/Gain12' */
  real_T Switch[2];                    /* '<S26>/Switch' */
  real_T uDLookupTable2;               /* '<S17>/1-D Lookup Table2' */
  real_T Gain5;                        /* '<Root>/Gain5' */
  real_T Gain6;                        /* '<Root>/Gain6' */
  real_T Integrator;                   /* '<Root>/Integrator' */
  real_T Integrator3;                  /* '<Root>/Integrator3' */
  real_T Gain17;                       /* '<Root>/Gain17' */
  real_T Gain10;                       /* '<Root>/Gain10' */
  real_T uDLookupTable3;               /* '<S17>/2-D Lookup Table3' */
  real_T Abs;                          /* '<Root>/Abs' */
  real_T Switch2[2];                   /* '<Root>/Switch2' */
  real_T Switch3[2];                   /* '<Root>/Switch3' */
  real_T Memory2;                      /* '<Root>/Memory2' */
  real_T q2;                           /* '<S26>/考虑滞后的流量2' */
  real_T q1;                           /* '<S26>/考虑滞后的流量' */
  real_T u_a;                          /* '<Root>/varFreqPWM' */
  real_T para[4];                      /* '<Root>/parameter_set' */
  real_T e;                            /* '<Root>/e' */
  real_T de;                           /* '<Root>/e' */
  real_T dP;                           /* '<Root>/dPL_dPR' */
  real_T VL;                           /* '<Root>/VL' */
  real_T eta;                          /* '<Root>/MATLAB Function' */
  real_T f_out;                        /* '<Root>/BaseFreqOpt' */
  real_T U1;                           /* '<Root>/2阀模式' */
  real_T U2;                           /* '<Root>/2阀模式' */
  B_u_VVA_pressure_dynamic_base_T sf_2_m;/* '<S26>/ 电磁阀滞后2' */
  B_u_VVA_pressure_dynamic_base_T sf__d;/* '<S26>/ 电磁阀滞后' */
} B_VVA_pressure_dynamic_basefr_T;

/* Block states (default storage) for system '<Root>' */
typedef struct {
  real_T UnitDelay_DSTATE;             /* '<Root>/Unit Delay' */
  real_T TimeStampA;                   /* '<Root>/Derivative1' */
  real_T LastUAtTimeA;                 /* '<Root>/Derivative1' */
  real_T TimeStampB;                   /* '<Root>/Derivative1' */
  real_T LastUAtTimeB;                 /* '<Root>/Derivative1' */
  real_T TimeStampA_h;                 /* '<Root>/Derivative' */
  real_T LastUAtTimeA_o;               /* '<Root>/Derivative' */
  real_T TimeStampB_a;                 /* '<Root>/Derivative' */
  real_T LastUAtTimeB_n;               /* '<Root>/Derivative' */
  real_T TimeStampA_p;                 /* '<Root>/Derivative2' */
  real_T LastUAtTimeA_b;               /* '<Root>/Derivative2' */
  real_T TimeStampB_l;                 /* '<Root>/Derivative2' */
  real_T LastUAtTimeB_l;               /* '<Root>/Derivative2' */
  real_T m_bpLambda[2];                /* '<S17>/1-D Lookup Table2' */
  real_T m_yyA[7];                     /* '<S17>/1-D Lookup Table2' */
  real_T m_yyB[7];                     /* '<S17>/1-D Lookup Table2' */
  real_T m_yy2[7];                     /* '<S17>/1-D Lookup Table2' */
  real_T m_up[51];                     /* '<S17>/1-D Lookup Table2' */
  real_T m_y2[357];                    /* '<S17>/1-D Lookup Table2' */
  real_T prevBp0AndTableData[408];     /* '<S17>/1-D Lookup Table2' */
  real_T m_bpLambda_o[2];              /* '<S17>/2-D Lookup Table3' */
  real_T m_yyA_n[7];                   /* '<S17>/2-D Lookup Table3' */
  real_T m_yyB_o[7];                   /* '<S17>/2-D Lookup Table3' */
  real_T m_yy2_l[7];                   /* '<S17>/2-D Lookup Table3' */
  real_T m_up_g[51];                   /* '<S17>/2-D Lookup Table3' */
  real_T m_y2_i[357];                  /* '<S17>/2-D Lookup Table3' */
  real_T prevBp0AndTableData_k[408];   /* '<S17>/2-D Lookup Table3' */
  real_T Memory2_PreviousInput;        /* '<Root>/Memory2' */
  real_T eta_prev;                     /* '<Root>/频率' */
  real_T f_old;                        /* '<Root>/频率' */
  real_T phi;                          /* '<Root>/varFreqPWM' */
  real_T de_f;                         /* '<Root>/FreqPolicy' */
  real_T S;                            /* '<Root>/BaseFreqOpt' */
  real_T f_prev;                       /* '<Root>/BaseFreqOpt' */
  struct {
    void *LoggedData;
  } PWM_PWORK;                         /* '<Root>/PWM占空比' */

  struct {
    void *LoggedData[2];
  } U1U2_PWORK;                        /* '<Root>/U1U2 ' */

  struct {
    void *LoggedData;
  } action_PWORK;                      /* '<Root>/action' */

  struct {
    void *LoggedData;
  } d_PWORK;                           /* '<Root>/d' */

  struct {
    void *LoggedData;
  } f_PWORK;                           /* '<Root>/f' */

  struct {
    void *LoggedData;
  } frequency_PWORK;                   /* '<Root>/frequency' */

  void* m_bpDataSet[2];                /* '<S17>/1-D Lookup Table2' */
  void* TWork[6];                      /* '<S17>/1-D Lookup Table2' */
  void* SWork[9];                      /* '<S17>/1-D Lookup Table2' */
  struct {
    void *LoggedData[2];
  } _PWORK;                            /* '<Root>/压力' */

  struct {
    void *LoggedData;
  } _PWORK_k;                          /* '<Root>/耗气量' */

  struct {
    void *LoggedData;
  } u_PWORK;                           /* '<Root>/耗气量3' */

  struct {
    void *LoggedData;
  } _PWORK_i;                          /* '<Root>/耗气量正弦' */

  struct {
    void *LoggedData;
  } _PWORK_e;                          /* '<Root>/误差' */

  void* m_bpDataSet_j[2];              /* '<S17>/2-D Lookup Table3' */
  void* TWork_f[6];                    /* '<S17>/2-D Lookup Table3' */
  void* SWork_h[9];                    /* '<S17>/2-D Lookup Table3' */
  int32_T clockTickCounter;            /* '<Root>/Pulse Generator' */
  int32_T burst_cnt;                   /* '<Root>/频率' */
  int32_T a_prev;                      /* '<Root>/模式选择' */
  int32_T hold_cnt;                    /* '<Root>/模式选择' */
  int32_T hold_cnt_p;                  /* '<Root>/FreqPolicy' */
  int32_T burst_cnt_n;                 /* '<Root>/FreqPolicy' */
  int32_T stall_cnt;                   /* '<Root>/FreqPolicy' */
  uint32_T m_bpIndex[2];               /* '<S17>/1-D Lookup Table2' */
  uint32_T m_bpIndex_a[2];             /* '<S17>/2-D Lookup Table3' */
  uint8_T Output_DSTATE;               /* '<S33>/Output' */
  uint8_T reCalcSecDerivFirstDimCoeffs;/* '<S17>/1-D Lookup Table2' */
  uint8_T reCalcSecDerivFirstDimCoeffs_a;/* '<S17>/2-D Lookup Table3' */
  boolean_T eta_prev_not_empty;        /* '<Root>/频率' */
  boolean_T de_f_not_empty;            /* '<Root>/FreqPolicy' */
  boolean_T f_prev_not_empty;          /* '<Root>/BaseFreqOpt' */
  DW_u_VVA_pressure_dynamic_bas_T sf_2_m;/* '<S26>/ 电磁阀滞后2' */
  DW_u_VVA_pressure_dynamic_bas_T sf__d;/* '<S26>/ 电磁阀滞后' */
} DW_VVA_pressure_dynamic_basef_T;

/* Continuous states (default storage) */
typedef struct {
  real_T _CSTATE;                      /* '<Root>/设置压力起始值' */
  real_T Integrator_CSTATE;            /* '<Root>/Integrator' */
  real_T Integrator3_CSTATE;           /* '<Root>/Integrator3' */
} X_VVA_pressure_dynamic_basefr_T;

/* State derivatives (default storage) */
typedef struct {
  real_T _CSTATE;                      /* '<Root>/设置压力起始值' */
  real_T Integrator_CSTATE;            /* '<Root>/Integrator' */
  real_T Integrator3_CSTATE;           /* '<Root>/Integrator3' */
} XDot_VVA_pressure_dynamic_bas_T;

/* State disabled  */
typedef struct {
  boolean_T _CSTATE;                   /* '<Root>/设置压力起始值' */
  boolean_T Integrator_CSTATE;         /* '<Root>/Integrator' */
  boolean_T Integrator3_CSTATE;        /* '<Root>/Integrator3' */
} XDis_VVA_pressure_dynamic_bas_T;

/* Zero-crossing (trigger) state */
typedef struct {
  ZCE_EnabledandTriggeredSubsys_T EnabledandTriggeredSubsystem8;
                                 /* '<Root>/Enabled and Triggered Subsystem8' */
  ZCE_EnabledandTriggeredSubsys_T EnabledandTriggeredSubsystem7;
                                 /* '<Root>/Enabled and Triggered Subsystem7' */
  ZCE_EnabledandTriggeredSubsys_T EnabledandTriggeredSubsystem5;
                                 /* '<Root>/Enabled and Triggered Subsystem5' */
  ZCE_EnabledandTriggeredSubsys_T EnabledandTriggeredSubsystem4;
                                 /* '<Root>/Enabled and Triggered Subsystem4' */
  ZCE_EnabledandTriggeredSubsys_T EnabledandTriggeredSubsystem3;
                                 /* '<Root>/Enabled and Triggered Subsystem3' */
  ZCE_EnabledandTriggeredSubsys_T EnabledandTriggeredSubsystem2;
                                 /* '<Root>/Enabled and Triggered Subsystem2' */
  ZCE_EnabledandTriggeredSubsys_T EnabledandTriggeredSubsystem1;
                                 /* '<Root>/Enabled and Triggered Subsystem1' */
} PrevZCX_VVA_pressure_dynamic__T;

#ifndef ODE3_INTG
#define ODE3_INTG

/* ODE3 Integration Data */
typedef struct {
  real_T *y;                           /* output */
  real_T *f[3];                        /* derivatives */
} ODE3_IntgData;

#endif

/* Parameters for system: '<Root>/Enabled and Triggered Subsystem1' */
struct P_EnabledandTriggeredSubsyste_T_ {
  real_T Out1_Y0;                      /* Computed Parameter: Out1_Y0
                                        * Referenced by: '<S4>/Out1'
                                        */
};

/* Parameters (default storage) */
struct P_VVA_pressure_dynamic_basefr_T_ {
  real_T x[7];                         /* Variable: x
                                        * Referenced by:
                                        *   '<S17>/1-D Lookup Table2'
                                        *   '<S17>/2-D Lookup Table3'
                                        */
  real_T y[51];                        /* Variable: y
                                        * Referenced by:
                                        *   '<S17>/1-D Lookup Table2'
                                        *   '<S17>/2-D Lookup Table3'
                                        */
  real_T z[357];                       /* Variable: z
                                        * Referenced by:
                                        *   '<S17>/1-D Lookup Table2'
                                        *   '<S17>/2-D Lookup Table3'
                                        */
  real_T RepeatingSequenceStair2_OutValu[8];
                              /* Mask Parameter: RepeatingSequenceStair2_OutValu
                               * Referenced by: '<S18>/Vector'
                               */
  uint8_T LimitedCounter_uplimit;      /* Mask Parameter: LimitedCounter_uplimit
                                        * Referenced by: '<S35>/FixPt Switch'
                                        */
  real_T Constant12_Value;             /* Expression: 500
                                        * Referenced by: '<Root>/Constant12'
                                        */
  real_T OutputSamplePoints_Value[101];/* Expression: fis.outputSamplePoints
                                        * Referenced by: '<S13>/Output Sample Points'
                                        */
  real_T _IC;                          /* Expression: 1.01325e5
                                        * Referenced by: '<Root>/设置压力起始值'
                                        */
  real_T SineWave2_Amp;                /* Expression: 2.98675e5
                                        * Referenced by: '<Root>/Sine Wave2'
                                        */
  real_T SineWave2_Bias;               /* Expression: 4e5
                                        * Referenced by: '<Root>/Sine Wave2'
                                        */
  real_T SineWave2_Freq;               /* Expression: 2*pi*0.5
                                        * Referenced by: '<Root>/Sine Wave2'
                                        */
  real_T SineWave2_Phase;              /* Expression: 0
                                        * Referenced by: '<Root>/Sine Wave2'
                                        */
  real_T SineWave1_Amp;                /* Expression: 2.98675e5
                                        * Referenced by: '<Root>/Sine Wave1'
                                        */
  real_T SineWave1_Bias;               /* Expression: 4e5
                                        * Referenced by: '<Root>/Sine Wave1'
                                        */
  real_T SineWave1_Freq;               /* Expression: 2*pi*0.25
                                        * Referenced by: '<Root>/Sine Wave1'
                                        */
  real_T SineWave1_Phase;              /* Expression: 0
                                        * Referenced by: '<Root>/Sine Wave1'
                                        */
  real_T Constant1_Value;              /* Expression: 4e5
                                        * Referenced by: '<Root>/Constant1'
                                        */
  real_T Step_Time;                    /* Expression: 1
                                        * Referenced by: '<Root>/Step'
                                        */
  real_T Step_Y0;                      /* Expression: 6e5
                                        * Referenced by: '<Root>/Step'
                                        */
  real_T Step_YFinal;                  /* Expression: 3e5
                                        * Referenced by: '<Root>/Step'
                                        */
  real_T Gain_Gain;                    /* Expression: 1
                                        * Referenced by: '<Root>/Gain'
                                        */
  real_T Saturation6_UpperSat;         /* Expression: 4
                                        * Referenced by: '<Root>/Saturation6'
                                        */
  real_T Saturation6_LowerSat;         /* Expression: -4
                                        * Referenced by: '<Root>/Saturation6'
                                        */
  real_T Gain1_Gain;                   /* Expression: 1e-5
                                        * Referenced by: '<Root>/Gain1'
                                        */
  real_T Saturation7_UpperSat;         /* Expression: 35
                                        * Referenced by: '<Root>/Saturation7'
                                        */
  real_T Saturation7_LowerSat;         /* Expression: -35
                                        * Referenced by: '<Root>/Saturation7'
                                        */
  real_T Saturation4_UpperSat;         /* Expression: 0.98
                                        * Referenced by: '<Root>/Saturation4'
                                        */
  real_T Saturation4_LowerSat;         /* Expression: 0.1
                                        * Referenced by: '<Root>/Saturation4'
                                        */
  real_T freq_mode_Value;              /* Expression: 3
                                        * Referenced by: '<Root>/freq_mode'
                                        */
  real_T Saturation2_UpperSat;         /* Expression: 3
                                        * Referenced by: '<Root>/Saturation2'
                                        */
  real_T Saturation2_LowerSat;         /* Expression: 1
                                        * Referenced by: '<Root>/Saturation2'
                                        */
  real_T Saturation_UpperSat;          /* Expression: 3
                                        * Referenced by: '<S12>/Saturation'
                                        */
  real_T Saturation_LowerSat;          /* Expression: 1
                                        * Referenced by: '<S12>/Saturation'
                                        */
  real_T Gain7_Gain;                   /* Expression: 1/4e5
                                        * Referenced by: '<Root>/Gain7'
                                        */
  real_T Gain8_Gain;                   /* Expression: 1/3e6
                                        * Referenced by: '<Root>/Gain8'
                                        */
  real_T Gain9_Gain;                   /* Expression: 1/5e7
                                        * Referenced by: '<Root>/Gain9'
                                        */
  real_T Constant15_Value;             /* Expression: 1
                                        * Referenced by: '<Root>/Constant15'
                                        */
  real_T Constant16_Value;             /* Expression: 0.2
                                        * Referenced by: '<Root>/Constant16'
                                        */
  real_T Constant17_Value;             /* Expression: 0.05
                                        * Referenced by: '<Root>/Constant17'
                                        */
  real_T Constant11_Value;             /* Expression: 0.06
                                        * Referenced by: '<Root>/Constant11'
                                        */
  real_T Constant14_Value;             /* Expression: 0.12
                                        * Referenced by: '<Root>/Constant14'
                                        */
  real_T Saturation1_UpperSat;         /* Expression: 1
                                        * Referenced by: '<Root>/Saturation1'
                                        */
  real_T Saturation1_LowerSat;         /* Expression: 0
                                        * Referenced by: '<Root>/Saturation1'
                                        */
  real_T UnitDelay_InitialCondition;   /* Expression: 0
                                        * Referenced by: '<Root>/Unit Delay'
                                        */
  real_T Constant_Value;               /* Expression: 0.8
                                        * Referenced by: '<Root>/Constant'
                                        */
  real_T Gain15_Gain;                  /* Expression: 1e-3
                                        * Referenced by: '<Root>/Gain15'
                                        */
  real_T Constant32_Value;             /* Expression: 0.4
                                        * Referenced by: '<Root>/Constant32'
                                        */
  real_T Gain16_Gain;                  /* Expression: 1e-3
                                        * Referenced by: '<Root>/Gain16'
                                        */
  real_T Constant13_Value;             /* Expression: 500
                                        * Referenced by: '<Root>/Constant13'
                                        */
  real_T Gain11_Gain;                  /* Expression: 1/4e5
                                        * Referenced by: '<Root>/Gain11'
                                        */
  real_T Gain12_Gain;                  /* Expression: 1/3e6
                                        * Referenced by: '<Root>/Gain12'
                                        */
  real_T Constant10_Value;             /* Expression: 1000
                                        * Referenced by: '<Root>/Constant10'
                                        */
  real_T Constant6_Value;              /* Expression: 0
                                        * Referenced by: '<S26>/Constant6'
                                        */
  real_T Constant2_Value;              /* Expression: 0
                                        * Referenced by: '<S26>/Constant2'
                                        */
  real_T Switch_Threshold;             /* Expression: 0
                                        * Referenced by: '<S26>/Switch'
                                        */
  real_T Gain5_Gain;                   /* Expression: 1e-5
                                        * Referenced by: '<Root>/Gain5'
                                        */
  real_T Gain6_Gain;                   /* Expression: 1e-5
                                        * Referenced by: '<Root>/Gain6'
                                        */
  real_T Integrator_IC;                /* Expression: 0
                                        * Referenced by: '<Root>/Integrator'
                                        */
  real_T Integrator3_IC;               /* Expression: 0
                                        * Referenced by: '<Root>/Integrator3'
                                        */
  real_T Gain17_Gain;                  /* Expression: 1/3
                                        * Referenced by: '<Root>/Gain17'
                                        */
  real_T Gain10_Gain;                  /* Expression: 1e-5
                                        * Referenced by: '<Root>/Gain10'
                                        */
  real_T Constant2_Value_c;            /* Expression: 0
                                        * Referenced by: '<Root>/Constant2'
                                        */
  real_T Constant4_Value;              /* Expression: 0
                                        * Referenced by: '<Root>/Constant4'
                                        */
  real_T Constant5_Value;              /* Expression: 0
                                        * Referenced by: '<Root>/Constant5'
                                        */
  real_T Constant8_Value;              /* Expression: 0
                                        * Referenced by: '<Root>/Constant8'
                                        */
  real_T Switch2_Threshold;            /* Expression: 100
                                        * Referenced by: '<Root>/Switch2'
                                        */
  real_T Switch3_Threshold;            /* Expression: -100
                                        * Referenced by: '<Root>/Switch3'
                                        */
  real_T Memory2_InitialCondition;     /* Expression: 10
                                        * Referenced by: '<Root>/Memory2'
                                        */
  real_T PulseGenerator_Amp;           /* Expression: 1
                                        * Referenced by: '<Root>/Pulse Generator'
                                        */
  real_T PulseGenerator_Period;     /* Computed Parameter: PulseGenerator_Period
                                     * Referenced by: '<Root>/Pulse Generator'
                                     */
  real_T PulseGenerator_Duty;         /* Computed Parameter: PulseGenerator_Duty
                                       * Referenced by: '<Root>/Pulse Generator'
                                       */
  real_T PulseGenerator_PhaseDelay;    /* Expression: 0
                                        * Referenced by: '<Root>/Pulse Generator'
                                        */
  uint32_T uDLookupTable2_maxIndex[2];
                                  /* Computed Parameter: uDLookupTable2_maxIndex
                                   * Referenced by: '<S17>/1-D Lookup Table2'
                                   */
  uint32_T uDLookupTable2_dimSizes[2];
                                  /* Computed Parameter: uDLookupTable2_dimSizes
                                   * Referenced by: '<S17>/1-D Lookup Table2'
                                   */
  uint32_T uDLookupTable2_numYWorkElts[3];
                              /* Computed Parameter: uDLookupTable2_numYWorkElts
                               * Referenced by: '<S17>/1-D Lookup Table2'
                               */
  uint32_T uDLookupTable3_maxIndex[2];
                                  /* Computed Parameter: uDLookupTable3_maxIndex
                                   * Referenced by: '<S17>/2-D Lookup Table3'
                                   */
  uint32_T uDLookupTable3_dimSizes[2];
                                  /* Computed Parameter: uDLookupTable3_dimSizes
                                   * Referenced by: '<S17>/2-D Lookup Table3'
                                   */
  uint32_T uDLookupTable3_numYWorkElts[3];
                              /* Computed Parameter: uDLookupTable3_numYWorkElts
                               * Referenced by: '<S17>/2-D Lookup Table3'
                               */
  uint8_T Constant_Value_e;            /* Computed Parameter: Constant_Value_e
                                        * Referenced by: '<S35>/Constant'
                                        */
  uint8_T ManualSwitch3_CurrentSetting;
                             /* Computed Parameter: ManualSwitch3_CurrentSetting
                              * Referenced by: '<Root>/Manual Switch3'
                              */
  uint8_T ManualSwitch14_CurrentSetting;
                            /* Computed Parameter: ManualSwitch14_CurrentSetting
                             * Referenced by: '<Root>/Manual Switch14'
                             */
  uint8_T ManualSwitch_CurrentSetting;
                              /* Computed Parameter: ManualSwitch_CurrentSetting
                               * Referenced by: '<Root>/Manual Switch'
                               */
  uint8_T ManualSwitch6_CurrentSetting;
                             /* Computed Parameter: ManualSwitch6_CurrentSetting
                              * Referenced by: '<Root>/Manual Switch6'
                              */
  uint8_T Output_InitialCondition;/* Computed Parameter: Output_InitialCondition
                                   * Referenced by: '<S33>/Output'
                                   */
  uint8_T ManualSwitch1_CurrentSetting;
                             /* Computed Parameter: ManualSwitch1_CurrentSetting
                              * Referenced by: '<Root>/Manual Switch1'
                              */
  uint8_T FixPtConstant_Value;        /* Computed Parameter: FixPtConstant_Value
                                       * Referenced by: '<S34>/FixPt Constant'
                                       */
  P_EnabledandTriggeredSubsyste_T EnabledandTriggeredSubsystem8;
                                 /* '<Root>/Enabled and Triggered Subsystem8' */
  P_EnabledandTriggeredSubsyste_T EnabledandTriggeredSubsystem7;
                                 /* '<Root>/Enabled and Triggered Subsystem7' */
  P_EnabledandTriggeredSubsyste_T EnabledandTriggeredSubsystem5;
                                 /* '<Root>/Enabled and Triggered Subsystem5' */
  P_EnabledandTriggeredSubsyste_T EnabledandTriggeredSubsystem4;
                                 /* '<Root>/Enabled and Triggered Subsystem4' */
  P_EnabledandTriggeredSubsyste_T EnabledandTriggeredSubsystem3;
                                 /* '<Root>/Enabled and Triggered Subsystem3' */
  P_EnabledandTriggeredSubsyste_T EnabledandTriggeredSubsystem2;
                                 /* '<Root>/Enabled and Triggered Subsystem2' */
  P_EnabledandTriggeredSubsyste_T EnabledandTriggeredSubsystem1;
                                 /* '<Root>/Enabled and Triggered Subsystem1' */
};

/* Real-time Model Data Structure */
struct tag_RTM_VVA_pressure_dynamic__T {
  const char_T *errorStatus;
  RTWLogInfo *rtwLogInfo;
  RTWSolverInfo solverInfo;
  X_VVA_pressure_dynamic_basefr_T *contStates;
  int_T *periodicContStateIndices;
  real_T *periodicContStateRanges;
  real_T *derivs;
  XDis_VVA_pressure_dynamic_bas_T *contStateDisabled;
  boolean_T zCCacheNeedsReset;
  boolean_T derivCacheNeedsReset;
  boolean_T CTOutputIncnstWithState;
  real_T odeY[3];
  real_T odeF[3][3];
  ODE3_IntgData intgData;

  /*
   * Sizes:
   * The following substructure contains sizes information
   * for many of the model attributes such as inputs, outputs,
   * dwork, sample times, etc.
   */
  struct {
    int_T numContStates;
    int_T numPeriodicContStates;
    int_T numSampTimes;
  } Sizes;

  /*
   * Timing:
   * The following substructure contains information regarding
   * the timing information for the model.
   */
  struct {
    uint32_T clockTick0;
    uint32_T clockTickH0;
    time_T stepSize0;
    uint32_T clockTick1;
    uint32_T clockTickH1;
    struct {
      uint16_T TID[5];
    } TaskCounters;

    time_T tStart;
    time_T tFinal;
    SimTimeStep simTimeStep;
    boolean_T stopRequestedFlag;
    time_T *t;
    time_T tArray[5];
  } Timing;
};

/* Block parameters (default storage) */
extern P_VVA_pressure_dynamic_basefr_T VVA_pressure_dynamic_basefreq_P;

/* Block signals (default storage) */
extern B_VVA_pressure_dynamic_basefr_T VVA_pressure_dynamic_basefreq_B;

/* Continuous states (default storage) */
extern X_VVA_pressure_dynamic_basefr_T VVA_pressure_dynamic_basefreq_X;

/* Disabled states (default storage) */
extern XDis_VVA_pressure_dynamic_bas_T VVA_pressure_dynamic_basef_XDis;

/* Block states (default storage) */
extern DW_VVA_pressure_dynamic_basef_T VVA_pressure_dynamic_basefre_DW;

/* Zero-crossing (trigger) state */
extern PrevZCX_VVA_pressure_dynamic__T VVA_pressure_dynamic_ba_PrevZCX;

/* Model entry point functions */
extern void VVA_pressure_dynamic_basefreq_sin_initialize(void);
extern void VVA_pressure_dynamic_basefreq_sin_step(void);
extern void VVA_pressure_dynamic_basefreq_sin_terminate(void);

/* Real-time Model object */
extern RT_MODEL_VVA_pressure_dynamic_T *const VVA_pressure_dynamic_basefre_M;

/*-
 * The generated code includes comments that allow you to trace directly
 * back to the appropriate location in the model.  The basic format
 * is <system>/block_name, where system is the system number (uniquely
 * assigned by Simulink) and block_name is the name of the block.
 *
 * Use the MATLAB hilite_system command to trace the generated code back
 * to the model.  For example,
 *
 * hilite_system('<S3>')    - opens system 3
 * hilite_system('<S3>/Kp') - opens and selects block Kp which resides in S3
 *
 * Here is the system hierarchy for this model
 *
 * '<Root>' : 'VVA_pressure_dynamic_basefreq_sin'
 * '<S1>'   : 'VVA_pressure_dynamic_basefreq_sin/2阀模式'
 * '<S2>'   : 'VVA_pressure_dynamic_basefreq_sin/BaseFreqOpt'
 * '<S3>'   : 'VVA_pressure_dynamic_basefreq_sin/DutyComp_D'
 * '<S4>'   : 'VVA_pressure_dynamic_basefreq_sin/Enabled and Triggered Subsystem1'
 * '<S5>'   : 'VVA_pressure_dynamic_basefreq_sin/Enabled and Triggered Subsystem2'
 * '<S6>'   : 'VVA_pressure_dynamic_basefreq_sin/Enabled and Triggered Subsystem3'
 * '<S7>'   : 'VVA_pressure_dynamic_basefreq_sin/Enabled and Triggered Subsystem4'
 * '<S8>'   : 'VVA_pressure_dynamic_basefreq_sin/Enabled and Triggered Subsystem5'
 * '<S9>'   : 'VVA_pressure_dynamic_basefreq_sin/Enabled and Triggered Subsystem7'
 * '<S10>'  : 'VVA_pressure_dynamic_basefreq_sin/Enabled and Triggered Subsystem8'
 * '<S11>'  : 'VVA_pressure_dynamic_basefreq_sin/FreqPolicy'
 * '<S12>'  : 'VVA_pressure_dynamic_basefreq_sin/FreqSourceSel'
 * '<S13>'  : 'VVA_pressure_dynamic_basefreq_sin/Fuzzy Logic  Controller1'
 * '<S14>'  : 'VVA_pressure_dynamic_basefreq_sin/MATLAB Function'
 * '<S15>'  : 'VVA_pressure_dynamic_basefreq_sin/MATLAB Function1'
 * '<S16>'  : 'VVA_pressure_dynamic_basefreq_sin/MATLAB Function7'
 * '<S17>'  : 'VVA_pressure_dynamic_basefreq_sin/Q1 Q2'
 * '<S18>'  : 'VVA_pressure_dynamic_basefreq_sin/Repeating Sequence Stair2'
 * '<S19>'  : 'VVA_pressure_dynamic_basefreq_sin/Repeating Sequence Stair3'
 * '<S20>'  : 'VVA_pressure_dynamic_basefreq_sin/VL'
 * '<S21>'  : 'VVA_pressure_dynamic_basefreq_sin/dPL_dPR'
 * '<S22>'  : 'VVA_pressure_dynamic_basefreq_sin/e'
 * '<S23>'  : 'VVA_pressure_dynamic_basefreq_sin/parameter_set'
 * '<S24>'  : 'VVA_pressure_dynamic_basefreq_sin/varFreqPWM'
 * '<S25>'  : 'VVA_pressure_dynamic_basefreq_sin/模式选择'
 * '<S26>'  : 'VVA_pressure_dynamic_basefreq_sin/阀滞后1'
 * '<S27>'  : 'VVA_pressure_dynamic_basefreq_sin/频率'
 * '<S28>'  : 'VVA_pressure_dynamic_basefreq_sin/Fuzzy Logic  Controller1/Defuzzify Outputs'
 * '<S29>'  : 'VVA_pressure_dynamic_basefreq_sin/Fuzzy Logic  Controller1/Evaluate Rule Antecedents'
 * '<S30>'  : 'VVA_pressure_dynamic_basefreq_sin/Fuzzy Logic  Controller1/Evaluate Rule Consequents'
 * '<S31>'  : 'VVA_pressure_dynamic_basefreq_sin/Q1 Q2/MATLAB Function2'
 * '<S32>'  : 'VVA_pressure_dynamic_basefreq_sin/Q1 Q2/MATLAB Function3'
 * '<S33>'  : 'VVA_pressure_dynamic_basefreq_sin/Repeating Sequence Stair2/LimitedCounter'
 * '<S34>'  : 'VVA_pressure_dynamic_basefreq_sin/Repeating Sequence Stair2/LimitedCounter/Increment Real World'
 * '<S35>'  : 'VVA_pressure_dynamic_basefreq_sin/Repeating Sequence Stair2/LimitedCounter/Wrap To Zero'
 * '<S36>'  : 'VVA_pressure_dynamic_basefreq_sin/Repeating Sequence Stair3/LimitedCounter'
 * '<S37>'  : 'VVA_pressure_dynamic_basefreq_sin/Repeating Sequence Stair3/LimitedCounter/Increment Real World'
 * '<S38>'  : 'VVA_pressure_dynamic_basefreq_sin/Repeating Sequence Stair3/LimitedCounter/Wrap To Zero'
 * '<S39>'  : 'VVA_pressure_dynamic_basefreq_sin/阀滞后1/ 电磁阀滞后'
 * '<S40>'  : 'VVA_pressure_dynamic_basefreq_sin/阀滞后1/ 电磁阀滞后2'
 * '<S41>'  : 'VVA_pressure_dynamic_basefreq_sin/阀滞后1/考虑滞后的流量'
 * '<S42>'  : 'VVA_pressure_dynamic_basefreq_sin/阀滞后1/考虑滞后的流量2'
 */
#endif                     /* RTW_HEADER_VVA_pressure_dynamic_basefreq_sin_h_ */
