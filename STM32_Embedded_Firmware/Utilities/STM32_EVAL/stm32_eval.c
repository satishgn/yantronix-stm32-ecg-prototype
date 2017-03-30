/*****************************************************************************
* File    : @file
* Company : Yantronix
* Author  : Satish Nair
* Mailto  : satishgn77@gmail.com
*****************************************************************************/
  
/* Includes ------------------------------------------------------------------*/
#include "stm32_eval.h"
  
#ifdef USE_STM32_PRIMER2
 #include "stm32_primer2/stm32_primer2.c"
#elif defined USE_STM32_H103
 #include "stm32_h103/stm32_h103.c" 
#elif defined USE_STM32_VL_DISCOVERY
 #include "stm32_vl_discovery/stm32_vl_discovery.c" 
#elif defined USE_STM32_SGN101 
 #include "stm32_sgn101/stm32_sgn101.c"
#elif defined USE_STM32_SGN10X 
 #include "stm32_sgn10x/stm32_sgn10x.c" 
#else 
 #error "Please select first the STM32 EVAL board to be used (in stm32_eval.h)"
#endif
