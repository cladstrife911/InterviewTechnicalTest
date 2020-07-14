#include <stdio.h>
#include <Analyzer.h>maekm

int main(int argc, char *argv[])
{
  (void)argc;
  (void)argv;

  enuAnalyzer_Init();
  printf("%d\r\n", iAnalyzer_foo(1));
  printf("%d\r\n", iAnalyzer_foo(10));
  printf("%d\r\n", iAnalyzer_foo(100));
  printf("%d\r\n", iAnalyzer_foo(1000));

  return 0;
 }
