/*******************************************************************************
* File Name: Key_Switch.h  
* Version 2.20
*
* Description:
*  This file contains the Alias definitions for Per-Pin APIs in cypins.h. 
*  Information on using these APIs can be found in the System Reference Guide.
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_PINS_Key_Switch_ALIASES_H) /* Pins Key_Switch_ALIASES_H */
#define CY_PINS_Key_Switch_ALIASES_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"


/***************************************
*              Constants        
***************************************/
#define Key_Switch_0			(Key_Switch__0__PC)
#define Key_Switch_0_PS		(Key_Switch__0__PS)
#define Key_Switch_0_PC		(Key_Switch__0__PC)
#define Key_Switch_0_DR		(Key_Switch__0__DR)
#define Key_Switch_0_SHIFT	(Key_Switch__0__SHIFT)
#define Key_Switch_0_INTR	((uint16)((uint16)0x0003u << (Key_Switch__0__SHIFT*2u)))

#define Key_Switch_INTR_ALL	 ((uint16)(Key_Switch_0_INTR))


#endif /* End Pins Key_Switch_ALIASES_H */


/* [] END OF FILE */
