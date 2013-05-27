#include "file_processing.h"
namespace ctpp {
using namespace std;
FileProcessing::FileProcessing(string fileName, string outDir)
    :fileName(fileName),outDir(outDir)
{

}

FileProcessing::~FileProcessing()
{
  closeIOFile();
}


void FileProcessing::ChangeFileName(string newAffix, string newPreffix, string newSuffix)
{
    int i=fileName.find_last_of('.');
    int j=fileName.find_last_of('/');
    if (i>j) newFileName = fileName.substr(0,j+1) + newPreffix +fileName.substr(j+1,i-j-1) + newAffix+newSuffix;
    else newFileName = newPreffix + fileName + newAffix + newSuffix;
}

void FileProcessing::closeIOFile(void )
{
    if(inFile.is_open())
        inFile.close();
    if(outFile.is_open())
        outFile.close();
}

int FileProcessing::openIOFile(bool inBinaryOpen, bool outBinaryOpen)
{
    if(fileName.empty() || newFileName.empty()) {
        cout << "File name is empty!" << endl;
        return -1;
    }
    if(!inBinaryOpen)
      inFile.open(fileName.c_str());
    else
      inFile.open(fileName.c_str(),ios::binary);
    if(!inFile.is_open()) {
        cout << "Can't open the file " << fileName << endl;
        return -1;
    }
    if(!outBinaryOpen)
      outFile.open(newFileName.c_str());
    else
      outFile.open(newFileName.c_str(),ios::binary);
    if(!outFile.is_open()) {
        cout << "Can't open the file " << newFileName << endl;
        inFile.close();
        return -1;
    }
    return 1;
}



}// end namespace ctpp
