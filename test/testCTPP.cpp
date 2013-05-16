#include "file_processing.h"
using namespace ctpp;
int main(void){
  FileProcessing testFileProcessing("../test.txt");
  testFileProcessing.SentenceSegementation();
  return 1;  
}