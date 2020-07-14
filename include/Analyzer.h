#ifndef _ANALYZER_H_
#define _ANALYZER_H_


typedef enum{
  enuAnalayzer_Ok,
  enuAnalayzer,
}enuAnalyzer_RetType_t;


enuAnalyzer_RetType_t enuAnalyzer_Init();
int iAnalyzer_foo(int var);

#endif /*_ANALYZER_H_*/
