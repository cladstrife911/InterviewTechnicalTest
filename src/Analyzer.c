
/*****************************************************************************/
/* INCLUSION OF STANDARD HEADERS                                             */
/*****************************************************************************/
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
/*required for memset*/
#include <string.h>

/*****************************************************************************/
/* INCLUSION OF OWNER HEADERS                                                */
/*****************************************************************************/
#include "Analyzer.h"
#include "Analyzer_cfg.h"

/*****************************************************************************/
/* LOCAL MACROS AND DEFINED CONSTANTS                                        */
/*****************************************************************************/
#define u8ANALYZER_START_OF_FRAME_CHAR '!'
#define u8ANALYZER_END_OF_FRAME_CHAR '?'
#define u8ANALYZER_PAYLOAD_LEN_SIZE ((uint8_t)2)
#define u8ANALYZER_MSB_CMD_MASK ((uint8_t)0b1110)
#define u8ANALYZER_MSB_LEN_MASK ((uint8_t)0b0001)
#define u8ANALYZER_MSB_CMD_OFFSET ((uint8_t)1)
#define u8ANALYZER_BITS_IN_BYTE ((uint8_t)8)

#ifdef DEBUG_ANALYZER
  #include <stdio.h>
  #define PRINTF(...) printf(__VA_ARGS__)
#else
  #define PRINTF(...) ((void)0)
#endif

/*****************************************************************************/
/*  LOCAL ENUM                                                               */
/*****************************************************************************/

typedef enum{
  enuIdle=0,
  enuStartOfFrame,
  enuLenght,
  enuCmd,
  enuPayload,
  enuEndOfFrame,
  enuError,
} enuReceiverState_t;

/*****************************************************************************/
/*  LOCAL STRUCTURES                                                         */
/*****************************************************************************/

/*****************************************************************************/
/*  LOCAL VARIABLES                                                          */
/*****************************************************************************/
static bool LOC_bIsInitialized;
static enuReceiverState_t LOC_enuReceiverState;
static uint8_t LOC_u8ErrorCounter;
static uint16_t LOC_u16PayloadLen;
static uint8_t LOC_u8CommandLen;
static uint64_t LOC_u64Command;
static uint8_t LOC_au8Payload[u16ANALYZER_MAX_PAYLOAD_SIZE];

/*pointer to the frame_handler callback*/
static Analyzer_vidFrameHandlerCbk LOC_pFrameHandlerCbk;
/*pointer to the uart_error_handler callback*/
static Analyzer_vidErrorCbk LOC_pErrorCbk;
/*****************************************************************************/
/* LOCAL FUNCTION PROTOTYPES                                                 */
/*****************************************************************************/
static void vidInitLocalVar(void);
static bool bIsCharValid(uint8_t u8Char);
static uint8_t u8AsciiToU8(uint8_t u8Char);

/*****************************************************************************/
/* GLOBAL FUNCTION                                                            */
/*****************************************************************************/
enuAnalyzer_RetType_t enuAnalyzer_Init(void)
{
  enuAnalyzer_RetType_t enuRetVal = enuAnalayzer_Error;

  if(!LOC_bIsInitialized){
    vidInitLocalVar();

    LOC_bIsInitialized = true;
    enuRetVal = enuAnalayzer_Ok;
  }else{
    enuRetVal = enuAnalayzer_Error;
  }

  return enuRetVal;
}

enuAnalyzer_RetType_t enuAnalyzer_Deinit(void)
{
  enuAnalyzer_RetType_t enuRetVal = enuAnalayzer_Error;
  if(LOC_bIsInitialized){
    vidInitLocalVar();
    enuRetVal = enuAnalayzer_Ok;
  }else{
    enuRetVal = enuAnalayzer_Error;
  }
  return enuRetVal;
}

enuAnalyzer_RetType_t enuAnalyzer_RegisterCallbacks(Analyzer_vidFrameHandlerCbk pFrameHandler,\
                                                    Analyzer_vidErrorCbk pErrorCbk)
{
  enuAnalyzer_RetType_t enuRetVal = enuAnalayzer_Error;

  assert(LOC_bIsInitialized);

  if(NULL != pFrameHandler && NULL != pErrorCbk){
    LOC_pFrameHandlerCbk = pFrameHandler;
    LOC_pErrorCbk = pErrorCbk;
    enuRetVal = enuAnalayzer_Ok;
  }else{
    enuRetVal = enuAnalayzer_Error;
  }

  return enuRetVal;
}

void vidAnalyzer_putChar(uint8_t u8ReceivedByte)
{
  static bool bIsLsbReceived = false;
  static uint8_t u8CmdByteReceived = 0;
  static uint8_t u8LenByteReceived = 0;
  static uint16_t u16PayloadByteReceived = 0;

  // PRINTF("# vidAnalyzer_putChar %d, %c\r\n", LOC_enuReceiverState, u8ReceivedByte);

  switch(LOC_enuReceiverState)
  {
    case enuError:
    case enuIdle:
    {
      if(u8ANALYZER_START_OF_FRAME_CHAR == u8ReceivedByte)
      {
        LOC_enuReceiverState = enuStartOfFrame;
        u8LenByteReceived = 0;
      }else{
        /*unexcepted byte received*/
        LOC_enuReceiverState = enuError;
      }
    }
    break;

    case enuStartOfFrame:
    {
      if(bIsCharValid(u8ReceivedByte))
      {
        u8LenByteReceived++;
        /*Little endian so the LSB is received first*/
        if(!bIsLsbReceived)
        {
          /*Handle the LSB*/
          if(u8LenByteReceived%2!=0){
            LOC_u16PayloadLen = ((uint16_t)u8AsciiToU8(u8ReceivedByte))<<4;
          }else{
            LOC_u16PayloadLen += (uint16_t)u8AsciiToU8(u8ReceivedByte);
            bIsLsbReceived = true;
          }
          PRINTF("\tLOC_u16PayloadLen %x\r\n", LOC_u16PayloadLen);
        }else
        {
          /*Handle the MSB*/
          if(u8LenByteReceived%2!=0){
            LOC_u16PayloadLen += ((uint16_t)(u8AsciiToU8(u8ReceivedByte) & u8ANALYZER_MSB_LEN_MASK) << 12);
            LOC_u8CommandLen = (u8AsciiToU8(u8ReceivedByte) & u8ANALYZER_MSB_CMD_MASK) >> u8ANALYZER_MSB_CMD_OFFSET;
            PRINTF("\tLOC_u8CommandLen %x\r\n", LOC_u8CommandLen);
          }else{
            LOC_u16PayloadLen += (((uint16_t)u8AsciiToU8(u8ReceivedByte))<<8);

            //assume only payloadlen = 0 is valid
            if(0 != LOC_u8CommandLen){
              LOC_u64Command = 0;
              u8CmdByteReceived = 0;
              u8LenByteReceived = 0;
              PRINTF("\t\tCMD Len %d, Payload Len %d\r\n", LOC_u8CommandLen, LOC_u16PayloadLen);
              // LOC_u16PayloadLen += u8ReceivedByte;
              bIsLsbReceived = false;
              LOC_enuReceiverState = enuCmd;
            }else{
              LOC_enuReceiverState = enuError;
            }
          }

          // PRINTF("\tLOC_u16PayloadLen %d\r\n", LOC_u16PayloadLen);
        }
      }else{
        /*invalid char received*/
        LOC_enuReceiverState = enuError;
      }
    }
    break;

    case enuCmd:
    {
      u8CmdByteReceived++;
      if(bIsCharValid(u8ReceivedByte))
      {
        PRINTF("\tenuCmd %x\r\n", u8AsciiToU8(u8ReceivedByte));
        if((u8CmdByteReceived/2) <= LOC_u8CommandLen)
        {
            LOC_u64Command += ((uint16_t)u8AsciiToU8(u8ReceivedByte) << ((u8CmdByteReceived-1)*4));
        }

        if(u8CmdByteReceived/2 == LOC_u8CommandLen){
            PRINTF("\t\t CMD %lx\r\n", LOC_u64Command);
            u16PayloadByteReceived = 0;
            /*move to payload reception*/
            LOC_enuReceiverState = enuPayload;
        }
      }else{
        /*invalid char received*/
        LOC_enuReceiverState = enuError;
      }
    }
    break;

    case enuPayload:
    {
      u16PayloadByteReceived++;
      /*check valid byte or end of frame and buffer overflow*/
      if(bIsCharValid(u8ReceivedByte) || (u8ANALYZER_END_OF_FRAME_CHAR == u8ReceivedByte) )
      {
        /*EOF is received*/
        if(u8ANALYZER_END_OF_FRAME_CHAR == u8ReceivedByte){
          PRINTF("\tenuPayload EOF ");
          //and length is valid
          if((u16PayloadByteReceived-1)/2 == LOC_u16PayloadLen){
            PRINTF("OK\r\n");
            LOC_au8Payload[LOC_u16PayloadLen] = '\0';
            /*frame reception is complete*/
            LOC_pFrameHandlerCbk(LOC_u64Command, LOC_au8Payload, LOC_u16PayloadLen);
            LOC_u8ErrorCounter=0;
            LOC_enuReceiverState = enuIdle;
          }else{
            PRINTF("KO\r\n");
            LOC_enuReceiverState = enuError;
          }
        }else{
          //continue payload reception
          if((u16ANALYZER_MAX_PAYLOAD_SIZE-1) <= u16PayloadByteReceived/2 )
          {
            //Max frame length reached
            LOC_enuReceiverState = enuError;
          }else{
            PRINTF("\t enuPayload %x, %d\r\n",u8AsciiToU8(u8ReceivedByte) ,u16PayloadByteReceived);
            //Append data to the payload buffer
            if(u16PayloadByteReceived%2!=0){
              LOC_au8Payload[u16PayloadByteReceived/2] = u8AsciiToU8(u8ReceivedByte)<<4;
            }else{
              LOC_au8Payload[u16PayloadByteReceived/2-1] += u8AsciiToU8(u8ReceivedByte);
            }
          }
        }
      }else{
        /*invalid char received*/
        LOC_enuReceiverState = enuError;
      }
    }
    break;

    default:
    {
      LOC_enuReceiverState = enuError;
    }
    break;
  }

  if(enuError == LOC_enuReceiverState)
  {
    LOC_au8Payload[0] = '\0';
    if(u8ANALYZER_ERROR_MAX <= LOC_u8ErrorCounter++)
    {
      LOC_u8ErrorCounter=0;
      /*Call the error handler*/
      LOC_pErrorCbk();
    }
  }
}

/*****************************************************************************/
/* LOCAL FUNCTION                                                            */
/*****************************************************************************/
/*
* \brief initialize all the local variables
*/
static void vidInitLocalVar(void){
  LOC_bIsInitialized = false;
  LOC_enuReceiverState = enuIdle;
  LOC_u8ErrorCounter = 0;
  LOC_u16PayloadLen = 0;
  LOC_u8CommandLen = 0;
  LOC_u64Command = 0;
  LOC_pFrameHandlerCbk = NULL;
  LOC_pErrorCbk = NULL;

  memset(LOC_au8Payload, 0, sizeof(LOC_au8Payload));
}

/*
* \brief check if the char is valid
* \param u8Char the char to test
* \return true if the param is valid, false otherwise
*/
static bool bIsCharValid(uint8_t u8Char)
{
  if((u8Char >='0' && u8Char<='9') || (u8Char >='A' && u8Char<'F')){
    return true;
  }else{
    return false;
  }
}

/*
* \brief convert an ASCII char into HEX
* \param u8Char the char to convert in ASCII format
* \return the HEX value corresponding to the ASCII char, 0xFF for invalid format
*/
static uint8_t u8AsciiToU8(uint8_t u8Char)
{
  if(u8Char >='0' && u8Char<='9'){
    return u8Char-'0';
  }else if(u8Char >='A' && u8Char<'F')
  {
    return u8Char-'A'+10;
  }else{
    return 0xFF;
  }
}
