/*****************************************************************************/
/* PROTECTION AGAINST MULTIPLE INCLUSION                                     */
/*****************************************************************************/
#ifndef _ANALYZER_H_
#define _ANALYZER_H_

/*****************************************************************************/
/* GLOBAL MACROS AND DEFINED CONSTANTS                                       */
/*****************************************************************************/

/*****************************************************************************/
/* INCLUSION OF STANDARD HEADERS                                             */
/*****************************************************************************/
#include "stdint.h"

/*****************************************************************************/
/*  GLOBAL DATA TYPES                                                        */
/*****************************************************************************/

/*!
* \brief function pointer type definition for the error callback
* \n takes no parameter and return nothing
*/
typedef void(*Analyzer_vidErrorCbk)(void);

/*!
* \brief definition for the function pointer that will be used when a valid cmd is received
* \param u64Cmd is the cmd received (not part of the payload)
* \param pu8Payload is the buffer containing the payload
* \param u16PayloadSize is the lenght of pu8Payload
*/
typedef void(*Analyzer_vidFrameHandlerCbk)(uint64_t u64Cmd, uint8_t *pu8Payload, uint16_t u16PayloadSize);

/*****************************************************************************/
/*  GLOBAL ENUM                                                              */
/*****************************************************************************/
typedef enum{
  enuAnalayzer_Ok,
  enuAnalayzer_Error,
}enuAnalyzer_RetType_t;

/*****************************************************************************/
/*  GLOBAL STRUCTURES                                                        */
/*****************************************************************************/

/*****************************************************************************/
/* GLOBAL FUNCTION PROTOTYPES                                                */
/*****************************************************************************/

/*!
* \brief Init function for the Analyzer module
* \n call only one time
* \return enuAnalayzer_Ok if Analyzer is initialized properly, else otherwise
*/
extern enuAnalyzer_RetType_t enuAnalyzer_Init(void);

/*!
* \brief Deinit function for the Analyzer module
* \n precondition: enuAnalyzer_Init
* \return enuAnalayzer_Ok if Analyzer is uninitialized properly, else otherwise
*/
extern enuAnalyzer_RetType_t enuAnalyzer_Deinit(void);

/*
* \brief this function is called by UART IRQ when a byte is received
*/
extern void vidAnalyzer_putChar(uint8_t u8ReceivedByte);

/*!
* \brief Init function for the Analyzer module
* \n precondition: enuAnalyzer_Init
* \return enuAnalayzer_Ok if parameter is valid, else otherwise
*/
extern enuAnalyzer_RetType_t enuAnalyzer_RegisterCallbacks(Analyzer_vidFrameHandlerCbk pFrameHandler,\
                                                            Analyzer_vidErrorCbk pErrorCbk);

#endif /*_ANALYZER_H_*/
