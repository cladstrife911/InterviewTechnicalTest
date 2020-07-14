/*
* configuration header for the Analyzer module
*/

#ifndef _ANALYZER_CFG_H_
#define _ANALYZER_CFG_H_

/*enbale this to add some printf for debugging*/
// #define DEBUG_ANALYZER

/*Max size for the payload in byte*/
#define u16ANALYZER_MAX_PAYLOAD_SIZE ((uint16_t)500)
/*number of error before the error handler is called*/
#define u8ANALYZER_ERROR_MAX ((uint8_t)8)

#endif /*_ANALYZER_CFG_H_*/
