/*******************************************************************************
* File Name: Countdown_PM.c
* Version 2.70
*
*  Description:
*     This file provides the power management source code to API for the
*     Timer.
*
*   Note:
*     None
*
*******************************************************************************
* Copyright 2008-2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
********************************************************************************/

#include "Countdown.h"

static Countdown_backupStruct Countdown_backup;


/*******************************************************************************
* Function Name: Countdown_SaveConfig
********************************************************************************
*
* Summary:
*     Save the current user configuration
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  Countdown_backup:  Variables of this global structure are modified to
*  store the values of non retention configuration registers when Sleep() API is
*  called.
*
*******************************************************************************/
void Countdown_SaveConfig(void) 
{
    #if (!Countdown_UsingFixedFunction)
        Countdown_backup.TimerUdb = Countdown_ReadCounter();
        Countdown_backup.InterruptMaskValue = Countdown_STATUS_MASK;
        #if (Countdown_UsingHWCaptureCounter)
            Countdown_backup.TimerCaptureCounter = Countdown_ReadCaptureCount();
        #endif /* Back Up capture counter register  */

        #if(!Countdown_UDB_CONTROL_REG_REMOVED)
            Countdown_backup.TimerControlRegister = Countdown_ReadControlRegister();
        #endif /* Backup the enable state of the Timer component */
    #endif /* Backup non retention registers in UDB implementation. All fixed function registers are retention */
}


/*******************************************************************************
* Function Name: Countdown_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores the current user configuration.
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  Countdown_backup:  Variables of this global structure are used to
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void Countdown_RestoreConfig(void) 
{   
    #if (!Countdown_UsingFixedFunction)

        Countdown_WriteCounter(Countdown_backup.TimerUdb);
        Countdown_STATUS_MASK =Countdown_backup.InterruptMaskValue;
        #if (Countdown_UsingHWCaptureCounter)
            Countdown_SetCaptureCount(Countdown_backup.TimerCaptureCounter);
        #endif /* Restore Capture counter register*/

        #if(!Countdown_UDB_CONTROL_REG_REMOVED)
            Countdown_WriteControlRegister(Countdown_backup.TimerControlRegister);
        #endif /* Restore the enable state of the Timer component */
    #endif /* Restore non retention registers in the UDB implementation only */
}


/*******************************************************************************
* Function Name: Countdown_Sleep
********************************************************************************
*
* Summary:
*     Stop and Save the user configuration
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  Countdown_backup.TimerEnableState:  Is modified depending on the
*  enable state of the block before entering sleep mode.
*
*******************************************************************************/
void Countdown_Sleep(void) 
{
    #if(!Countdown_UDB_CONTROL_REG_REMOVED)
        /* Save Counter's enable state */
        if(Countdown_CTRL_ENABLE == (Countdown_CONTROL & Countdown_CTRL_ENABLE))
        {
            /* Timer is enabled */
            Countdown_backup.TimerEnableState = 1u;
        }
        else
        {
            /* Timer is disabled */
            Countdown_backup.TimerEnableState = 0u;
        }
    #endif /* Back up enable state from the Timer control register */
    Countdown_Stop();
    Countdown_SaveConfig();
}


/*******************************************************************************
* Function Name: Countdown_Wakeup
********************************************************************************
*
* Summary:
*  Restores and enables the user configuration
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  Countdown_backup.enableState:  Is used to restore the enable state of
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void Countdown_Wakeup(void) 
{
    Countdown_RestoreConfig();
    #if(!Countdown_UDB_CONTROL_REG_REMOVED)
        if(Countdown_backup.TimerEnableState == 1u)
        {     /* Enable Timer's operation */
                Countdown_Enable();
        } /* Do nothing if Timer was disabled before */
    #endif /* Remove this code section if Control register is removed */
}


/* [] END OF FILE */
