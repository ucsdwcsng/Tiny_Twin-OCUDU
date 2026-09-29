/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*****************************************************************************
Source    nas_log.h

Version   0.1

Date    2012/02/28

Product   NAS stack

Subsystem Utilities

Description Usefull logging functions

*****************************************************************************/
#ifndef __NAS_LOG_H__
#define __NAS_LOG_H__

#if defined(NAS_BUILT_IN_UE)
# include "common/utils/LOG/log.h"
# undef LOG_TRACE
#endif

/****************************************************************************/
/*********************  G L O B A L    C O N S T A N T S  *******************/
/****************************************************************************/

/* -----------------------
 * Logging severity levels
 * -----------------------
 *  OFF : Disables logging trace utilities.
 *  DEBUG : Only used for debug purpose. Should be removed from the code.
 *  INFO  : Informational trace
 *  WARNING : The program displays the warning message and doesn't stop.
 *  ERROR : The program displays the error message and usually exits or
 *      runs appropriate procedure.
 *  FUNC  : Prints trace when entering/leaving to/from function. Usefull
 *      to display the function's calling tree information at runtime.
 *  ON  : Enables logging traces excepted FUNC.
 *  ALL : Turns on ALL logging traces.
 */
/* Logging severity type */
typedef enum {
  DEBUG,
  INFO,
  WARNING,
  ERROR,
  FUNC_IN,
  FUNC_OUT,
  LOG_SEVERITY_MAX
} log_severity_t;

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#ifdef LOG_E

# define LOG_TRACE(s, x, args...)                               \
do {                                                            \
    switch (s) {                                                \
        case ERROR:     LOG_E(NAS, " %s:%d  " x "\n", __FILENAME__, __LINE__, ##args); break;  \
        case WARNING:   LOG_W(NAS, " %s:%d  " x "\n", __FILENAME__, __LINE__, ##args); break;  \
        case INFO:      LOG_I(NAS, " %s:%d  " x "\n", __FILENAME__, __LINE__, ##args); break;  \
        default:        LOG_D(NAS, " %s:%d  " x "\n", __FILENAME__, __LINE__, ##args); break;  \
    }                                                           \
} while (0)

# define LOG_DUMP(dATA, lEN)   LOG_DUMPMSG(NAS, DEBUG_NAS,dATA, lEN, " Dump %d:\n", lEN)                                                 
# define LOG_FUNC_IN  LOG_ENTER(NAS)
# define LOG_FUNC_OUT  LOG_END(NAS)
# define LOG_FUNC_RETURN(rETURNcODE) LOG_RETURN(NAS,rETURNcODE)
#else
# define LOG_TRACE(s, x, args...)  
# define LOG_DUMP(dATA, lEN)   LOG_DUMPMSG(NAS, LOG_DUMP_CHAR,dATA, lEN,  " Dump %d:\n", lEN)

# define LOG_FUNC_IN 
# define LOG_FUNC_OUT 
# define LOG_FUNC_RETURN(rETURNcODE)  return  rETURNcODE                                  \

#endif

#endif /* __NAS_LOG_H__*/
