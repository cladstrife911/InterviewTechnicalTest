/*****************************************************************************/
/* INCLUSION OF STANDARD HEADERS                                             */
/*****************************************************************************/
#include <stdio.h>
#include <assert.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>

/*****************************************************************************/
/* INCLUSION OF OWNER HEADERS                                                */
/*****************************************************************************/
#include "Analyzer.h"
#include "Analyzer_loc.h" //for static functions

/*****************************************************************************/
/* LOCAL VARIABLES                                                           */
/*****************************************************************************/
static bool LOC_bFrameHandlerOk=false;
static bool LOC_bErrorHandlerTriggered=false;

/*****************************************************************************/
/* LOCAL FUNCTION PROTOTYPES                                                 */
/*****************************************************************************/
void vidFeedAnalyzer(char* cmd, bool isTestExpectedTrue);

void uart_error_handler(void);
void frame_handler(uint64_t u64Cmd, uint8_t *pu8Payload, uint16_t u16PayloadSize);

/*****************************************************************************/
/* LOCAL FUNCTION                                                            */
/*****************************************************************************/
int main(int argc, char *argv[])
{
  (void)argc;
  (void)argv;

  assert(enuAnalayzer_Error == enuAnalyzer_Deinit());
  assert(enuAnalayzer_Ok == enuAnalyzer_Init());
  assert(enuAnalayzer_Error == enuAnalyzer_Init());
  assert(enuAnalayzer_Error == enuAnalyzer_RegisterCallbacks(NULL, NULL));
  assert(enuAnalayzer_Ok == enuAnalyzer_RegisterCallbacks(&frame_handler, &uart_error_handler));

  /* trigger uart_error_handler*/
  for(int i=0;i<10;i++){
    vidAnalyzer_putChar(i);
  }
  assert(LOC_bErrorHandlerTriggered);

  vidFeedAnalyzer("!0360AABBCC012345?", true);   //valid frame : cmd len =3
  vidFeedAnalyzer("!0340AABB012345?", true);   //valid frame: cmd len=2
  vidFeedAnalyzer("!0040AABB?", true);   //valid frame: payload len 0
  vidFeedAnalyzer("!1340AABB01234567890123456789012345678901234567?", true);   //valid frame

  vidFeedAnalyzer("!0360AABBCC0123450", false);   //invalid frame : invalid EOF
  vidFeedAnalyzer("!FF41AABB012345?", false);     //invalid frame: payload size exceeded
  vidFeedAnalyzer("!0360A?ABBCC012345?", false);  //invalid frame: '?' not well placed
  vidFeedAnalyzer("!0300AABBCC012345?", false);   //invalid frame: cmd len = 0
  vidFeedAnalyzer("!03600AABBCC?", false);        //invalid frame: missing payload
  vidFeedAnalyzer("!?", false);                   //invalid frame: missing size
  vidFeedAnalyzer("!boom?", false);               //invalid frame: invalid size
  vidFeedAnalyzer("!0360AABBxC012345?", false);    //invalid frame: invalid char
  vidFeedAnalyzer("!03?", false);    //invalid frame: truncated len

  assert(enuAnalayzer_Ok == enuAnalyzer_Deinit());

  /* test the static functions*/
  assert(bIsCharValid('1') == true);
  assert(bIsCharValid('0') == true);
  assert(bIsCharValid('A') == true);
  assert(bIsCharValid('F') == true);
  assert(bIsCharValid('G') == false);
  assert(bIsCharValid(' ') == false);
  assert(u8AsciiToU8('0') == 0);
  assert(u8AsciiToU8('9') == 9);
  assert(u8AsciiToU8('A') == 0x0A);
  assert(u8AsciiToU8('F') == 0x0F);
  assert(u8AsciiToU8('x') == 0xFF);


  printf("Test done with success!\r\n");
  return 0;
 }

void vidFeedAnalyzer(char* cmd, bool isTestExpectedTrue)
{
  LOC_bErrorHandlerTriggered = false;
  printf("==> vidFeedAnalyzer: %s\r\n",cmd);
  for(unsigned int i=0;i<strlen(cmd);i++)
  {
    vidAnalyzer_putChar(cmd[i]);
  }

  assert(isTestExpectedTrue == LOC_bFrameHandlerOk);
  LOC_bFrameHandlerOk = false;
}

 void uart_error_handler(void)
 {
   // printf("# uart_error_handler\r\n");
   LOC_bErrorHandlerTriggered = true;
 }

 void frame_handler(uint64_t u64Cmd, uint8_t *pu8Payload, uint16_t u16PayloadSize)
 {
   printf("\tframe_handler: Cmd=%lx, PayloadSize=%d ", u64Cmd, u16PayloadSize);
   printf("Payload: ");
   for(int i=0;i<(int)strlen((const char*)pu8Payload);i++){
     printf("%02x", pu8Payload[i]);
   }
   printf("\r\n");

   LOC_bFrameHandlerOk = true;
 }
