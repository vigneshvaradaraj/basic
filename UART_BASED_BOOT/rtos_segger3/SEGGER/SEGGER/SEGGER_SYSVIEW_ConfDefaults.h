/*********************************************************************
 *                    SEGGER Microcontroller GmbH                     *
 *                        The Embedded Experts                        *
 **********************************************************************
 *                                                                    *
 *            (c) 1995 - 2019 SEGGER Microcontroller GmbH             *
 *                                                                    *
 *       www.segger.com     Support: support@segger.com               *
 *                                                                    *
 **********************************************************************
 *                                                                    *
 *       SEGGER SystemView * Real-time application analysis           *
 *                                                                    *
 **********************************************************************
 *                                                                    *
 * All rights reserved.                                               *
 *                                                                    *
 * SEGGER strongly recommends to not make any changes                 *
 * to or modify the source code of this software in order to stay     *
 * compatible with the SystemView and RTT protocol, and J-Link.       *
 *                                                                    *
 * Redistribution and use in source and binary forms, with or         *
 * without modification, are permitted provided that the following    *
 * condition is met:                                                  *
 *                                                                    *
 * o Redistributions of source code must retain the above copyright   *
 *   notice, this condition and the following disclaimer.             *
 *                                                                    *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND             *
 * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,        *
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF           *
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE           *
 * DISCLAIMED. IN NO EVENT SHALL SEGGER Microcontroller BE LIABLE FOR *
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR           *
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT  *
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;    *
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF      *
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT          *
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE  *
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH   *
 * DAMAGE.                                                            *
 *                                                                    *
 **********************************************************************
 *                                                                    *
 *       SystemView version: V3.12                                    *
 *                                                                    *
 **********************************************************************
 -------------------------- END-OF-HEADER -----------------------------
 File    : SEGGER_SYSVIEW_ConfDefaults.h
 Purpose : Defines defaults for configurable defines used in
 SEGGER SystemView.
 Revision: $Rev: 9599 $
 */

#ifndef SEGGER_SYSVIEW_CONFDEFAULTS_H
#define SEGGER_SYSVIEW_CONFDEFAULTS_H

/*********************************************************************
 *
 *       #include Section
 *
 **********************************************************************
 */

#include "SEGGER_SYSVIEW_Conf.h"
#include "SEGGER_RTT_Conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/*********************************************************************
 *
 *       Configuration defaults
 *
 **********************************************************************
 */

// Number of bytes that SystemView uses for a buffer.
#ifndef   SEGGER_SYSVIEW_RTT_BUFFER_SIZE
  #define SEGGER_SYSVIEW_RTT_BUFFER_SIZE    1024
#endif

// The RTT channel that SystemView will use.
#ifndef   SEGGER_SYSVIEW_RTT_CHANNEL
  #define SEGGER_SYSVIEW_RTT_CHANNEL        0
#endif
// Sanity check of RTT channel
#if (SEGGER_SYSVIEW_RTT_CHANNEL == 0) && (SEGGER_RTT_MAX_NUM_UP_BUFFERS < 2)
  #error "SEGGER_RTT_MAX_NUM_UP_BUFFERS in SEGGER_RTT_Conf.h has to be > 1!"
#elif (SEGGER_SYSVIEW_RTT_CHANNEL >= SEGGER_RTT_MAX_NUM_UP_BUFFERS)
  #error "SEGGER_RTT_MAX_NUM_UP_BUFFERS  in SEGGER_RTT_Conf.h has to be > SEGGER_SYSVIEW_RTT_CHANNEL!"
#endif

// Place the SystemView buffer into its own/the RTT section
#if !(defined SEGGER_SYSVIEW_BUFFER_SECTION) && (defined SEGGER_RTT_SECTION)
  #define SEGGER_SYSVIEW_BUFFER_SECTION            SEGGER_RTT_SECTION
#endif

// Retrieve a system timestamp.  This gets the Cortex-M cycle counter.
#ifndef   SEGGER_SYSVIEW_GET_TIMESTAMP
  #error "SEGGER_SYSVIEW_GET_TIMESTAMP has to be defined in SEGGER_SYSVIEW_Conf.h!"
#endif

// Define number of valid bits low-order delivered by clock source.
#ifndef   SEGGER_SYSVIEW_TIMESTAMP_BITS
  #define SEGGER_SYSVIEW_TIMESTAMP_BITS     32
#endif

// Lowest Id reported by the Application.
#ifndef   SEGGER_SYSVIEW_ID_BASE 
  #define SEGGER_SYSVIEW_ID_BASE            0
#endif

// Number of bits to shift Ids to save bandwidth
#ifndef   SEGGER_SYSVIEW_ID_SHIFT
  #define SEGGER_SYSVIEW_ID_SHIFT           0
#endif

#ifndef   SEGGER_SYSVIEW_GET_INTERRUPT_ID
  #error "SEGGER_SYSVIEW_GET_INTERRUPT_ID has to be defined in SEGGER_SYSVIEW_Conf.h!"
#endif

#ifndef   SEGGER_SYSVIEW_MAX_ARGUMENTS
#define SEGGER_SYSVIEW_MAX_ARGUMENTS      16
#endif

#ifndef   SEGGER_SYSVIEW_MAX_STRING_LEN
#define SEGGER_SYSVIEW_MAX_STRING_LEN     128
#endif

#ifndef   SEGGER_SYSVIEW_PRINTF_IMPLICIT_FORMAT
#define SEGGER_SYSVIEW_PRINTF_IMPLICIT_FORMAT 0
#endif

// Use a static buffer instead of a buffer on the stack for packets
#ifndef   SEGGER_SYSVIEW_USE_STATIC_BUFFER
  #define SEGGER_SYSVIEW_USE_STATIC_BUFFER  1
#endif

// Maximum packet size used by SystemView for the static buffer
#ifndef   SEGGER_SYSVIEW_MAX_PACKET_SIZE
#define SEGGER_SYSVIEW_MAX_PACKET_SIZE   SEGGER_SYSVIEW_INFO_SIZE + SEGGER_SYSVIEW_MAX_STRING_LEN + 2 * SEGGER_SYSVIEW_QUANTA_U32 + SEGGER_SYSVIEW_MAX_ARGUMENTS * SEGGER_SYSVIEW_QUANTA_U32
#endif

// Use post-mortem analysis instead of real-time analysis
#ifndef   SEGGER_SYSVIEW_POST_MORTEM_MODE
  #define SEGGER_SYSVIEW_POST_MORTEM_MODE   0
#endif

// Configure how frequently syncronization is sent
#ifndef   SEGGER_SYSVIEW_SYNC_PERIOD_SHIFT
#define SEGGER_SYSVIEW_SYNC_PERIOD_SHIFT  8
#endif

// Lock SystemView (nestable)
#ifndef   SEGGER_SYSVIEW_LOCK
#define SEGGER_SYSVIEW_LOCK()             SEGGER_RTT_LOCK()
#endif

// Unlock SystemView (nestable)
#ifndef   SEGGER_SYSVIEW_UNLOCK
#define SEGGER_SYSVIEW_UNLOCK()           SEGGER_RTT_UNLOCK()
#endif

#ifdef __cplusplus
}
#endif

#endif

/*************************** End of file ****************************/
