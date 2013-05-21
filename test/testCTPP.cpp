#include "file_processing.h"
#include <dirent.h>
using namespace ctpp;
int main(void) {
    DIR *dirPointer;
    struct dirent *dirStructPointer;
    string dirStr="../orgDir";
    string fatherDir = "..",selfDir = ".",tmpFileName;
    //store file names
    vector<string> fileNameVec;


    if((dirPointer=opendir(dirStr.c_str()))==NULL) {
        perror("opendir error");
        return -1;
    }

    while((dirStructPointer=readdir(dirPointer))!=NULL) {
	tmpFileName = dirStructPointer->d_name;
	if(fatherDir == tmpFileName || selfDir == tmpFileName)
            continue;
	fileNameVec.push_back(dirStr+"/"+tmpFileName);
    }

    for(vector<string>::iterator it = fileNameVec.begin(); it != fileNameVec.end(); it++)
    {
        cout<<*it<<endl;
	FileProcessing newFileProcessing(*it);
	newFileProcessing.SentenceSegementation();
    }

/*
    FileProcessing testFileProcessing("../test.txt");
    testFileProcessing.SentenceSegementation();*/
    return 1;
}
