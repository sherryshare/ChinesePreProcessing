#ifndef CTPP_SEN_FILE_PROCESSING_H
#define CTPP_SEN_FILE_PROCESSING_H
#include "file_processing.h"
#define MAX_LINE_WORDS 100
#define ROW_LENGTH MAX_LINE_WORDS * 3
#define BYTE_LENGTH 8
namespace ctpp {
  using namespace std;
class SenFileProcessing: public FileProcessing{
public:
    SenFileProcessing(std::string fileName, std::string outDir = "");
    ~SenFileProcessing();
    void GetBinaryFile(void);  
protected:
  char BinaryLine[ROW_LENGTH*BYTE_LENGTH];
  
};// end class SenFileProcessing
  
}// end namespace ctpp
#endif 
