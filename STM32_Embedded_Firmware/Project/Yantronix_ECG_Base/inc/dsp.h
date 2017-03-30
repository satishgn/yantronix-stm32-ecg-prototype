/*****************************************************************************
* File    : @file
* Company : Yantronix
* Author  : Satish Nair
* Mailto  : satishgn77@gmail.com
*****************************************************************************/

#ifndef DSP_H
#define DSP_H

#define FLOATING_POINT_PROCESS
// #define FIXED_POINT_PROCESS

/**
* IIR MK Filter
*/
#ifdef FLOATING_POINT_PROCESS
/*************** Floating point Process ****************/
//4th Order IIR Filter
float xv[5];
float yv[5];

//Cascade of 2nd Order IIR filters
/*
//4 Low pass filters
float xv_lp1[3];
float yv_lp1[3];
float xv_lp2[3];
float yv_lp2[3];
float xv_lp3[3];
float yv_lp3[3];
float xv_lp4[3];
float yv_lp4[3];
//4 High pass filters
float xv_hp1[3];
float yv_hp1[3];
float xv_hp2[3];
float yv_hp2[3];
float xv_hp3[3];
float yv_hp3[3];
float xv_hp4[3];
float yv_hp4[3];
//4 Band stop (Notch) filters
float xv_bs1[3];
float yv_bs1[3];
float xv_bs2[3];
float yv_bs2[3];
float xv_bs3[3];
float yv_bs3[3];
float xv_bs4[3];
float yv_bs4[3];
*/
//6 Band pass filters
float xv_bp1[3];
float yv_bp1[3];
float xv_bp2[3];
float yv_bp2[3];
float xv_bp3[3];
float yv_bp3[3];
float xv_bp4[3];
float yv_bp4[3];
float xv_bp5[3];
float yv_bp5[3];
float xv_bp6[3];
float yv_bp6[3];
/*******************************************************/
#endif

#ifdef FIXED_POINT_PROCESS
/*************** Fixed point Process ****************/
//4th Order IIR Filter
int32_t xv[5];
int32_t yv[5];

//Cascade of 2nd Order IIR filters
/*
//4 Low pass filters
int32_t xv_lp1[3];
int32_t yv_lp1[3];
int32_t xv_lp2[3];
int32_t yv_lp2[3];
int32_t xv_lp3[3];
int32_t yv_lp3[3];
int32_t xv_lp4[3];
int32_t yv_lp4[3];
//4 High pass filters
int32_t xv_hp1[3];
int32_t yv_hp1[3];
int32_t xv_hp2[3];
int32_t yv_hp2[3];
int32_t xv_hp3[3];
int32_t yv_hp3[3];
int32_t xv_hp4[3];
int32_t yv_hp4[3];
//4 Band stop (Notch) filters
int32_t xv_bs1[3];
int32_t yv_bs1[3];
int32_t xv_bs2[3];
int32_t yv_bs2[3];
int32_t xv_bs3[3];
int32_t yv_bs3[3];
int32_t xv_bs4[3];
int32_t yv_bs4[3];
*/
//6 Band pass filters
int32_t xv_bp1[3];
int32_t yv_bp1[3];
int32_t xv_bp2[3];
int32_t yv_bp2[3];
int32_t xv_bp3[3];
int32_t yv_bp3[3];
int32_t xv_bp4[3];
int32_t yv_bp4[3];
int32_t xv_bp5[3];
int32_t yv_bp5[3];
int32_t xv_bp6[3];
int32_t yv_bp6[3];
/*******************************************************/
#endif

/*
//Fixed point
int32_t iBIn, iBIn1, iBIn2;
int32_t iBOut, iBOut1, iBOut2;
int32_t iBO10, iBO11, iBO12;
int32_t iBO20, iBO21, iBO22;
int32_t iBO30, iBO31, iBO32;
int32_t iNIn, iNIn1, iNIn2;
int32_t iNOut, iNOut1, iNOut2;
int32_t iN3O, iN3O1, iN3O2;
*/

//4th Order IIR QRS Filter
float xv_qrs[5];
float yv_qrs[5];

// Shared functions
void iir_mk_filter(uint16_t* iir_output, uint16_t* iir_input, uint16_t size);
// void iir_mk_ecg_filter(uint16_t* iir_output, uint16_t* iir_input, uint16_t size);

void qrs_filter(uint16_t* filter_output, uint16_t* filter_input, uint16_t size);

#endif
