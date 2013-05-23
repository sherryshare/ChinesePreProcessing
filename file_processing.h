#ifndef CTPP_FILE_PROCESSING_H
#define CTPP_FILE_PROCESSING_H
#include <iostream>
#include <fstream>
#include <vector>

#define MAXLINE 2048
#define MAXTITLE 60
#define E_PUNCTURE_END ".!?:;"  // English ending punctures
#define E_PUNCTURE_LEFT "([{" // English left punctures
#define E_PUNCTURE_RIGHT ")]}" // English right punctures (showing order according to the left ones)
#define E_PUNCTURE_LR	"\042" // English puncture pairs without differing left and right
// The English single quotation marks has been ignored because it can be used either in pairs or alone.
#define C_PUNCTURE_END "。！？；…" // Chinese ending punctures, without Chinese colon
#define C_PUNCTURE_LEFT "“‘（《"       // Chinese left punctures
#define C_PUNCTURE_RIGHT "”’）》"       // Chinese right punctures (showing order according to the left ones)
namespace ctpp {
using namespace std;
class FileProcessing {
  // All Chinese words are regarded as UTF-8 format.
public:
    FileProcessing(string fileName, string outDir = "",bool isRawFile = true);
    ~FileProcessing();
    void SentenceSegementation(void);
    void GetRawFormatFromAnnotatedFile(void);
protected:
    //Add a new affix to the original file with a new suffix
    void ChangeFileName(string newAffix, string newPreffix = "", string newSuffix = ".txt");
    int openIOFile(void);
    inline void closeIOFile(void);
    //functions for processing one line
    string RemoveSpaceOneLine(string lineStr);
    int GetSentence(string &lineStr);
    string ChangeBracket(string lineStr);
    char ChangeFullCaseToHalf(string fullCaseStr);
    int GetUTF8BytesNum(char utf8Char);
    bool JudgeUTF8NextChar(char utf8Char);

protected:
    string fileName;
    string newFileName;
    bool isRawFile;
    ifstream inFile;
    ofstream outFile;
    string outDir;
};//end class FileProcessing


}//end namespace ctpp
#endif
