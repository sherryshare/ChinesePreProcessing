#ifndef CTPP_FILE_PROCESSING_H
#define CTPP_FILE_PROCESSING_H
#include <fstream>
#include <iostream>
#define MAXLINE 2048
namespace ctpp {
using namespace std;
class FileProcessing {
public:
    FileProcessing(string fileName, string outDir = "");
    virtual ~FileProcessing();
protected:
    //Add a new affix to the original file with a new suffix
    void ChangeFileName(string newAffix, string newPreffix = "", string newSuffix = ".txt");
    int openIOFile(bool inBinaryOpen = false, bool outBinaryOpen = false);
    void closeIOFile(void);
protected:
    string fileName;
    string newFileName;
    ifstream inFile;
    ofstream outFile;
    string outDir;
};// end class FileProcessing
}// end namespace ctpp
#endif
