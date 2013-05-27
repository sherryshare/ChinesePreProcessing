#include "sen_file_processing.h"
#include <dirent.h>
#include <vector>
using namespace ctpp;
int main(void) {
    DIR *dirPointer;
    struct dirent *dirStructPointer;
    string dirStr="../orgDir/sen";
    string fatherDir = "..",selfDir = ".",tmpFileName;
    //store file names
    vector<string> fileNameVec;


    if((dirPointer=opendir(dirStr.c_str()))==NULL) {
        perror("opendir error");
        return -1;
    }

    while((dirStructPointer=readdir(dirPointer))!=NULL) {
	tmpFileName = dirStructPointer->d_name;
	if(dirStructPointer->d_type == DT_DIR)// Is directory, ignore.
	  continue;
	if(fatherDir == tmpFileName || selfDir == tmpFileName)
            continue;
	fileNameVec.push_back(dirStr+"/"+tmpFileName);
    }
    
    //system("mkdir sen");

    for(vector<string>::iterator it = fileNameVec.begin(); it != fileNameVec.end(); it++)
    {
        cout<<*it<<endl;
	SenFileProcessing newSenFileProcessing(*it,"../bin/");
	newSenFileProcessing.GetBinaryFile();
    }

/*
    FileProcessing testFileProcessing("../test.txt");
    testFileProcessing.SentenceSegementation();*/
    return 1;
}
