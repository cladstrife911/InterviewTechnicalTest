/*****************************************************************************/
/* INCLUSION OF STANDARD HEADERS                                             */
/*****************************************************************************/
#include <stdio.h>
#include <assert.h>
#include <stddef.h>
#include <string.h>

/*****************************************************************************/
/* INCLUSION OF OWNER HEADERS                                                */
/*****************************************************************************/
#include "Analyzer.h"

/*****************************************************************************/
/* LOCAL FUNCTION PROTOTYPES                                                 */
/*****************************************************************************/
void vidFeedAnalyzer(char* cmd);

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

  printf("%d\r\n", u16Analyzer_foo(1));
  printf("%d\r\n", u16Analyzer_foo(10));
  printf("%d\r\n", u16Analyzer_foo(100));
  printf("%d\r\n", u16Analyzer_foo(1000));

  /* trigger uart_error_handler*/
  for(int i=0;i<10;i++){
    vidAnalyzer_putChar(i);
  }

  //valid frame
  vidFeedAnalyzer("!0360AABBCC012345?");

  //invalid frame
  vidFeedAnalyzer("!2361AABBCC012345?");

  assert(enuAnalayzer_Ok == enuAnalyzer_Deinit());

  return 0;
 }

void vidFeedAnalyzer(char* cmd)
{
  printf("==> vidFeedAnalyzer: %s\r\n",cmd);
  for(unsigned int i=0;i<strlen(cmd);i++)
  {
    vidAnalyzer_putChar(cmd[i]);
  }
}

 void uart_error_handler(void)
 {
   printf("# uart_error_handler\r\n");
 }

 void frame_handler(uint64_t u64Cmd, uint8_t *pu8Payload, uint16_t u16PayloadSize)
 {
   printf("# frame_handler: Cmd=%lx, Payload=%s, PayloadSize=%d\r\n", u64Cmd, pu8Payload, u16PayloadSize);
   // (void)u64Cmd;
   // (void)pu8Payload;
   // (void)u16PayloadSize;
 }
