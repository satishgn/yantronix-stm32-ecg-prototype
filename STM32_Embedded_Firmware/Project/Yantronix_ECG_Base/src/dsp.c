/*****************************************************************************
* File    : @file
* Company : Yantronix
* Author  : Satish Nair
* Mailto  : satishgn77@gmail.com
*****************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "hw_config.h"

uint16_t filter_type = 1;  //Default LP filter
uint16_t scale_factor = 1;
uint16_t filter_offset = ADC_DATA_MAX_VALUE / 2;

#ifdef FLOATING_POINT_PROCESS
/*************** Floating point Process ****************/
void iir_mk_filter(uint16_t* iir_output, uint16_t* iir_input, uint16_t size)
{
    uint16_t i;

    for(i = 0; i < size; i++)
    {
        switch (filter_type)
        {
            case 1:
                //30Hz 4th order Butterworth LP filter, Fs=125Hz
                xv[0] = xv[1]; xv[1] = xv[2]; xv[2] = xv[3]; xv[3] = xv[4];
                xv[4] = (float)*iir_input++ / (1.209590303e+01 / scale_factor);
                yv[0] = yv[1]; yv[1] = yv[2]; yv[2] = yv[3]; yv[3] = yv[4];
                yv[4] = (xv[0] + xv[4]) + 4 * (xv[1] + xv[3]) + 6 * xv[2]
                             + (-0.0181370770 * yv[0]) + (0.0329072181 * yv[1])
                             + (-0.4937426526 * yv[2]) + (0.1562105841 * yv[3]);
                *iir_output++ = (uint16_t)(yv[4] - filter_offset * (scale_factor - 1)); //For LP & BS filters                       
                break;

            case 2:
                //1Hz-30Hz 4th order Butterworth BP filter, Fs=125Hz
                xv[0] = xv[1]; xv[1] = xv[2]; xv[2] = xv[3]; xv[3] = xv[4];
                xv[4] = (float)*iir_input++ / (3.796330621e+00 / scale_factor);
                yv[0] = yv[1]; yv[1] = yv[2]; yv[2] = yv[3]; yv[3] = yv[4];
                yv[4] = (xv[0] + xv[4]) - 2 * xv[2]
                             + (-0.1746814133 * yv[0]) + (0.4597046484 * yv[1])
                             + (-1.3220128045 * yv[2]) + (2.0342085435 * yv[3]);
                *iir_output++ = (uint16_t)(yv[4] + filter_offset);  //For HP & BP filters                     
                break;

            case 3:
                //6 * 2nd order Butterworth Band pass cascaded filters, 1Hz - 30Hz, Fs=125, optional-first 2 with gain of 'scale_factor' each.
                xv_bp1[0] = xv_bp1[1]; xv_bp1[1] = xv_bp1[2];
                xv_bp1[2] = (float)*iir_input++ / (1.977594035e+00 / scale_factor);
                yv_bp1[0] = yv_bp1[1]; yv_bp1[1] = yv_bp1[2];
                yv_bp1[2] = (xv_bp1[2] - xv_bp1[0])
                             + (-0.0566090212 * yv_bp1[0]) + (1.0078744416 * yv_bp1[1]);

                xv_bp2[0] = xv_bp2[1]; xv_bp2[1] = xv_bp2[2];
                xv_bp2[2] = yv_bp1[2] / (1.977594035e+00 / scale_factor);
                yv_bp2[0] = yv_bp2[1]; yv_bp2[1] = yv_bp2[2];
                yv_bp2[2] = (xv_bp2[2] - xv_bp2[0])
                             + (-0.0566090212 * yv_bp2[0]) + (1.0078744416 * yv_bp2[1]);

                xv_bp3[0] = xv_bp3[1]; xv_bp3[1] = xv_bp3[2];
                xv_bp3[2] = yv_bp2[2] / 1.977594035e+00;
                yv_bp3[0] = yv_bp3[1]; yv_bp3[1] = yv_bp3[2];
                yv_bp3[2] = (xv_bp3[2] - xv_bp3[0])
                             + (-0.0566090212 * yv_bp3[0]) + (1.0078744416 * yv_bp3[1]);

                xv_bp4[0] = xv_bp4[1]; xv_bp4[1] = xv_bp4[2];
                xv_bp4[2] = yv_bp3[2] / 1.977594035e+00;
                yv_bp4[0] = yv_bp4[1]; yv_bp4[1] = yv_bp4[2];
                yv_bp4[2] = (xv_bp4[2] - xv_bp4[0])
                             + (-0.0566090212 * yv_bp4[0]) + (1.0078744416 * yv_bp4[1]);

                xv_bp5[0] = xv_bp5[1]; xv_bp5[1] = xv_bp5[2];
                xv_bp5[2] = yv_bp4[2] / 1.977594035e+00;
                yv_bp5[0] = yv_bp5[1]; yv_bp5[1] = yv_bp5[2];
                yv_bp5[2] = (xv_bp5[2] - xv_bp5[0])
                             + (-0.0566090212 * yv_bp5[0]) + (1.0078744416 * yv_bp5[1]);

                xv_bp6[0] = xv_bp6[1]; xv_bp6[1] = xv_bp6[2];
                xv_bp6[2] = yv_bp5[2] / 1.977594035e+00;
                yv_bp6[0] = yv_bp6[1]; yv_bp6[1] = yv_bp6[2];
                yv_bp6[2] = (xv_bp6[2] - xv_bp6[0])
                             + (-0.0566090212 * yv_bp6[0]) + (1.0078744416 * yv_bp6[1]);

                *iir_output++ = (uint16_t)(yv_bp6[2] + filter_offset);
                break;
        }
    }
}
/*******************************************************/
#endif

#ifdef FIXED_POINT_PROCESS
/*************** Fixed point Process ****************/
void iir_mk_filter(uint16_t* iir_output, uint16_t* iir_input, uint16_t size)
{
    uint16_t i;

    for(i = 0; i < size; i++)
    {
        switch (filter_type)
        {
            case 1:
                //30Hz 4th order Butterworth LP filter, Fs=125Hz
                xv[0] = xv[1]; xv[1] = xv[2]; xv[2] = xv[3]; xv[3] = xv[4];
                xv[4] = *iir_input++;
                yv[0] = yv[1]; yv[1] = yv[2]; yv[2] = yv[3]; yv[3] = yv[4];
                yv[4] = (scale_factor * 2709 * ((xv[0] + xv[4]) + 4 * (xv[1] + xv[3]) + 6 * xv[2])
                             + (-594 * yv[0]) + (1078 * yv[1])
                             + (-16178 * yv[2]) + (5118 * yv[3])) >> 15;
                *iir_output++ = (uint16_t)(yv[4] - filter_offset * (scale_factor - 1)); //For LP & BS filters                       
                break;

            case 2:
                //1Hz-30Hz 4th order Butterworth BP filter, Fs=125Hz
                xv[0] = xv[1]; xv[1] = xv[2]; xv[2] = xv[3]; xv[3] = xv[4];
                xv[4] = *iir_input++;
                yv[0] = yv[1]; yv[1] = yv[2]; yv[2] = yv[3]; yv[3] = yv[4];
                yv[4] = (scale_factor * 8631 * ((xv[0] + xv[4]) - 2 * xv[2])
                             + (-5723 * yv[0]) + (15063 * yv[1])
                             + (-43319 * yv[2]) + (66656 * yv[3])) >> 15;
                *iir_output++ = (uint16_t)(yv[4] + filter_offset);  //For HP & BP filters                     
                break;

            case 3:
                //6 * 2nd order Butterworth Band pass cascaded filters, 1Hz - 30Hz, Fs=125, optional-first 2 with gain of 'scale_factor' each.
                xv_bp1[0] = xv_bp1[1]; xv_bp1[1] = xv_bp1[2];
                xv_bp1[2] = *iir_input++;
                yv_bp1[0] = yv_bp1[1]; yv_bp1[1] = yv_bp1[2];
                yv_bp1[2] = (scale_factor * 16569 * (xv_bp1[2] - xv_bp1[0])
                             + (-1854 * yv_bp1[0]) + (33026 * yv_bp1[1])) >> 15;

                xv_bp2[0] = xv_bp2[1]; xv_bp2[1] = xv_bp2[2];
                xv_bp2[2] = yv_bp1[2];
                yv_bp2[0] = yv_bp2[1]; yv_bp2[1] = yv_bp2[2];
                yv_bp2[2] = (scale_factor * 16569 * (xv_bp2[2] - xv_bp2[0])
                             + (-1854 * yv_bp2[0]) + (33026 * yv_bp2[1])) >> 15;

                xv_bp3[0] = xv_bp3[1]; xv_bp3[1] = xv_bp3[2];
                xv_bp3[2] = yv_bp2[2];
                yv_bp3[0] = yv_bp3[1]; yv_bp3[1] = yv_bp3[2];
                yv_bp3[2] = (16569 * (xv_bp3[2] - xv_bp3[0])
                             + (-1854 * yv_bp3[0]) + (33026 * yv_bp3[1])) >> 15;

                xv_bp4[0] = xv_bp4[1]; xv_bp4[1] = xv_bp4[2];
                xv_bp4[2] = yv_bp3[2];
                yv_bp4[0] = yv_bp4[1]; yv_bp4[1] = yv_bp4[2];
                yv_bp4[2] = (16569 * (xv_bp4[2] - xv_bp4[0])
                             + (-1854 * yv_bp4[0]) + (33026 * yv_bp4[1])) >> 15;

                xv_bp5[0] = xv_bp5[1]; xv_bp5[1] = xv_bp5[2];
                xv_bp5[2] = yv_bp4[2];
                yv_bp5[0] = yv_bp5[1]; yv_bp5[1] = yv_bp5[2];
                yv_bp5[2] = (16569 * (xv_bp5[2] - xv_bp5[0])
                             + (-1854 * yv_bp5[0]) + (33026 * yv_bp5[1])) >> 15;

                xv_bp6[0] = xv_bp6[1]; xv_bp6[1] = xv_bp6[2];
                xv_bp6[2] = yv_bp5[2];
                yv_bp6[0] = yv_bp6[1]; yv_bp6[1] = yv_bp6[2];
                yv_bp6[2] = (16569 * (xv_bp6[2] - xv_bp6[0])
                             + (-1854 * yv_bp6[0]) + (33026 * yv_bp6[1])) >> 15;

                *iir_output++ = (uint16_t)(yv_bp6[2] + filter_offset);
                break;
        }
    }
}
/*******************************************************/
#endif

/*
* 6th order IIR filter designed by the bilinear transform method using mkfilter
* http://www-users.cs.york.ac.uk/~fisher/mkfilter/trad.html 
* filtertype  =  Chebyshev  
* passtype  =  Lowpass  
* ripple  =  -1.0dB (only for Butterworth/Chebyshev)
* order  =  6  
* samplerate  =  500Hz  
* corner1  =  40Hz 
*/
/*
void iir_mk_filter(uint16_t* iir_output, uint16_t* iir_input, uint16_t size)
{
    uint16_t i;

    for(i = 0; i < size; i++)
    {
        xv[0] = xv[1]; xv[1] = xv[2]; xv[2] = xv[3]; xv[3] = xv[4]; xv[4] = xv[5]; xv[5] = xv[6]; 
        xv[6] = (float)*iir_input++ / GAIN;
        yv[0] = yv[1]; yv[1] = yv[2]; yv[2] = yv[3]; yv[3] = yv[4]; yv[4] = yv[5]; yv[5] = yv[6]; 

        //Butterworth Low Pass filter
        yv[6] =   (xv[0] + xv[6]) + 6 * (xv[1] + xv[5]) + 15 * (xv[2] + xv[4])
                     + 20 * xv[3]
                     + ( -0.1396600417 * yv[0]) + (  1.1086708553 * yv[1])
                     + ( -3.7230194289 * yv[2]) + (  6.7850160254 * yv[3])
                     + ( -7.0995038188 * yv[4]) + (  4.0616439992 * yv[5]);

        //Chebyshev Low Pass filter
        // yv[6] =   (xv[0] + xv[6]) + 6 * (xv[1] + xv[5]) + 15 * (xv[2] + xv[4])
                     // + 20 * xv[3]
                     // + ( -0.6274134206 * yv[0]) + (  3.8077318277 * yv[1])
                     // + ( -9.8794904669 * yv[2]) + ( 14.0294765250 * yv[3])
                     // + (-11.5082731440 * yv[4]) + (  5.1770599674 * yv[5]);
    
        *iir_output++ = (uint16_t)yv[6];
    }
}
*/
/*
* ECG filter designed by the bilinear transform method using Cascaded 2nd order IIR mkfilters  
* filtertype  =  Butterworth  
* passtype  =  BP + BS  
* ripple  =
* order  =  2x2 + 2x2 
* samplerate  =  500Hz  
* corner1/corner2  = 0.05 - 100Hz/45 - 55Hz
*/
/*
void iir_mk_ecg_filter(uint16_t* iir_output, uint16_t* iir_input, uint16_t size)
{
    uint16_t i;

    for (i = 0; i < size; i++)
    {
        //2 Band pass filters with gain of 4 each, 0.05 - 100Hz, Fs=500
        xv_bp1[0] = xv_bp1[1]; xv_bp1[1] = xv_bp1[2];
        xv_bp1[2] = (float)*iir_input++ / (2.170592316 / 4);
        yv_bp1[0] = yv_bp1[1]; yv_bp1[1] = yv_bp1[2];
        yv_bp1[2] = (xv_bp1[2] - xv_bp1[0])
                     + (-0.1587064965 * yv_bp1[0]) + (1.1581776675 * yv_bp1[1]);

        xv_bp2[0] = xv_bp2[1]; xv_bp2[1] = xv_bp2[2];
        xv_bp2[2] = yv_bp1[2] / (2.170592316 / 4);
        yv_bp2[0] = yv_bp2[1]; yv_bp2[1] = yv_bp2[2];
        yv_bp2[2] = (xv_bp2[2] - xv_bp2[0])
                     + (-0.1587064965 * yv_bp2[0]) + (1.1581776675 * yv_bp2[1]);

        //2 Band stop filters, 45 - 55Hz(50Hz Notch), Fs=500
        xv_bs1[0] = xv_bs1[1]; xv_bs1[1] = xv_bs1[2];
        xv_bs1[2] = yv_bp2[2] / 1.062914667;
        yv_bs1[0] = yv_bs1[1]; yv_bs1[1] = yv_bs1[2];
        yv_bs1[2] = (xv_bs1[0] + xv_bs1[2]) - 1.6212331220 * xv_bs1[1]
                     + (-0.8816185924 * yv_bs1[0]) + (1.5252711924 * yv_bs1[1]);

        xv_bs2[0] = xv_bs2[1]; xv_bs2[1] = xv_bs2[2];
        xv_bs2[2] = yv_bs1[2] / 1.062914667;
        yv_bs2[0] = yv_bs2[1]; yv_bs2[1] = yv_bs2[2];
        yv_bs2[2] = (xv_bs2[0] + xv_bs2[2]) - 1.6212331220 * xv_bs2[1]
                     + (-0.8816185924 * yv_bs2[0]) + (1.5252711924 * yv_bs2[1]);

        *iir_output++ = (uint16_t)(yv_bs2[2] + filter_offset);
    }
}
*/
/*
//Fixed point
void iir_mk_ecg_filter(uint16_t* iir_output, uint16_t* iir_input, uint16_t size)
{
    uint16_t i;

    for (i = 0; i < size; i++)
    {
        //4 Band pass filters, 0.05 - 100Hz, Fs=500, first 2 with gain of 4 each.
        xv_bp1[0] = xv_bp1[1]; xv_bp1[1] = xv_bp1[2];
        xv_bp1[2] = *iir_input++;
        yv_bp1[0] = yv_bp1[1]; yv_bp1[1] = yv_bp1[2];
        yv_bp1[2] = (6891 * (xv_bp1[2] - xv_bp1[0])
                     + (-650 * yv_bp1[0]) + (4745 * yv_bp1[1])) >> 12;

        xv_bp2[0] = xv_bp2[1]; xv_bp2[1] = xv_bp2[2];
        xv_bp2[2] = yv_bp1[2];
        yv_bp2[0] = yv_bp2[1]; yv_bp2[1] = yv_bp2[2];
        yv_bp2[2] = (6891 * (xv_bp2[2] - xv_bp2[0])
                     + (-650 * yv_bp2[0]) + (4745 * yv_bp2[1])) >> 12;

        xv_bp3[0] = xv_bp3[1]; xv_bp3[1] = xv_bp3[2];
        xv_bp3[2] = yv_bp2[2];
        yv_bp3[0] = yv_bp3[1]; yv_bp3[1] = yv_bp3[2];
        yv_bp3[2] = (1723 * (xv_bp3[2] - xv_bp3[0])
                     + (-650 * yv_bp3[0]) + (4745 * yv_bp3[1])) >> 12;

        xv_bp4[0] = xv_bp4[1]; xv_bp4[1] = xv_bp4[2];
        xv_bp4[2] = yv_bp3[2];
        yv_bp4[0] = yv_bp4[1]; yv_bp4[1] = yv_bp4[2];
        yv_bp4[2] = (1723 * (xv_bp4[2] - xv_bp4[0])
                     + (-650 * yv_bp4[0]) + (4745 * yv_bp4[1])) >> 12;
                     
        xv_bp5[0] = xv_bp5[1]; xv_bp5[1] = xv_bp5[2];
        xv_bp5[2] = yv_bp4[2];
        yv_bp5[0] = yv_bp5[1]; yv_bp5[1] = yv_bp5[2];
        yv_bp5[2] = (1723 * (xv_bp5[2] - xv_bp5[0])
                     + (-650 * yv_bp5[0]) + (4745 * yv_bp5[1])) >> 12;                     
        
        //2 Band stop filters, 45 - 55Hz(50Hz Notch), Fs=500
        xv_bs1[0] = xv_bs1[1]; xv_bs1[1] = xv_bs1[2];
        xv_bs1[2] = yv_bp4[2];
        yv_bs1[0] = yv_bs1[1]; yv_bs1[1] = yv_bs1[2];
        yv_bs1[2] = (4096 * (xv_bs1[0] + xv_bs1[2]) - 6627 * xv_bs1[1]
                     + (-3598 * yv_bs1[0]) + (6211 * yv_bs1[1])) >> 12;
       
        xv_bs2[0] = xv_bs2[1]; xv_bs2[1] = xv_bs2[2];
        xv_bs2[2] = yv_bs1[2];
        yv_bs2[0] = yv_bs2[1]; yv_bs2[1] = yv_bs2[2];
        yv_bs2[2] = (4096 * (xv_bs2[0] + xv_bs2[2]) - 6627 * xv_bs2[1]
                     + (-3598 * yv_bs2[0]) + (6211 * yv_bs2[1])) >> 12;
        
        //Ensure that the output voltage is centered within the output range
#ifdef USE_ADC_OVERSAMPLING    
        if ((yv_bs2[2] + filterOffset) > 24000) filterOffset -= 1;
        if ((yv_bs2[2] + filterOffset) < 8000) filterOffset += 1;
#else
        if ((yv_bs2[2] + filterOffset) > 3000) filterOffset -= 1;
        if ((yv_bs2[2] + filterOffset) < 1000) filterOffset += 1;
#endif

        //*iir_output++ = (yv_bs4[2] + filterOffset) & 0xffff;    //0xfff
        *iir_output++ = (uint16_t)(yv_bs2[2] + filterOffset);
    }
}
*/
/*
//Fixed point
void iir_mk_ecg_filter(uint16_t* iir_output, uint16_t* iir_input, uint16_t size)
{
    uint16_t i;

    for(i = 0; i < size; i++)
    {
        iBIn = *iir_input++;
    
        // 5 Band pass 0.05 - 100Hz Fs=500 first 2 with gain of 4 each.
        iBOut = (6891*iBIn-6891*iBIn2+4745*iBOut1-650*iBOut2)>>12;
        iBO10 = (6891*iBOut-6891*iBOut2+4745*iBO11-650*iBO12)>>12;
        iBO20 = (1723*iBO10-1723*iBO12+4745*iBO21-650*iBO22)>>12;
        iBO30 = (1723*iBO20-1723*iBO22+4745*iBO31-650*iBO32)>>12;
        iNIn = (1723*iBO30-1723*iBO32+4745*iNIn1-650*iNIn2)>>12;

        // 2 notch filters.
        iNOut = (4096*iNIn-6627*iNIn1+4096*iNIn2+6211*iNOut1-3598*iNOut2)>>12;
        iN3O = (4096*iNOut-6627*iNOut1+4096*iNOut2+6211*iN3O1-3598*iN3O2)>>12;

        //Save delayed values for filters.
        iBIn2 = iBIn1;
        iBIn1 = iBIn;
    
        iBOut2 = iBOut1;
        iBOut1 = iBOut;

        iBO12 = iBO11;
        iBO11 = iBO10;

        iBO22 = iBO21;
        iBO21 = iBO20;

        iBO32 = iBO31;
        iBO31 = iBO30;

        iNIn2 = iNIn1;
        iNIn1 = iNIn;

        iNOut2 = iNOut1;
        iNOut1 = iNOut;

        iN3O2 = iN3O1;
        iN3O1 = iN3O;
                
        //Ensure that the output voltage is centered within the output range
#ifdef USE_ADC_OVERSAMPLING    
        if ((iN3O + filterOffset) > 24000) filterOffset -= 1;
        if ((iN3O + filterOffset) < 8000) filterOffset += 1;
#else
        if ((iN3O + filterOffset) > 3000) filterOffset -= 1;
        if ((iN3O + filterOffset) < 1000) filterOffset += 1;
#endif

        *iir_output++ = (uint16_t)(iN3O + filterOffset);
    }
}
*/

/*
Digital filter designed by mkfilter/mkshape/gencode   A.J. Fisher
Command line: /www/usr/fisher/helpers/mkfilter -Bu -Hp -o 4 -a 8.0000000000e-02 0.0000000000e+00 -l
filtertype  =  Butterworth  
passtype  =  Highpass  
order  =  4  
samplerate  =  125 Hz
corner  =  10 Hz
*/
void qrs_filter(uint16_t* filter_output, uint16_t* filter_input, uint16_t size)
{
    uint16_t i;

    for(i = 0; i < size; i++)
    {
        //10Hz 4th order Butterworth HP filter, Fs=125Hz    
        xv_qrs[0] = xv_qrs[1]; xv_qrs[1] = xv_qrs[2]; xv_qrs[2] = xv_qrs[3]; xv_qrs[3] = xv_qrs[4]; 
        xv_qrs[4] = (float)*filter_input++ / 1.944591400e+00;
        yv_qrs[0] = yv_qrs[1]; yv_qrs[1] = yv_qrs[2]; yv_qrs[2] = yv_qrs[3]; yv_qrs[3] = yv_qrs[4]; 
        yv_qrs[4] =   (xv_qrs[0] + xv_qrs[4]) - 4 * (xv_qrs[1] + xv_qrs[3]) + 6 * xv_qrs[2]
                     + ( -0.2644548164 * yv_qrs[0]) + (  1.4034846714 * yv_qrs[1])
                     + ( -2.8673991091 * yv_qrs[2]) + (  2.6926109870 * yv_qrs[3]);
        *filter_output++ = (uint16_t)(yv_qrs[4] + filter_offset);
    }
}