#include "sen_file_processing.h"
namespace ctpp {
SenFileProcessing::SenFileProcessing(std::string fileName, std::string outDir): FileProcessing(fileName, outDir)
{

}

SenFileProcessing::~SenFileProcessing()
{

}

void SenFileProcessing::GetBinaryFile(void )
{
    ChangeFileName("-bin",outDir,".mat");
    openIOFile();
    char buf[MAXLINE];
    string line;
    int i;
    while(inFile.eof() || inFile.getline(buf,MAXLINE)) {// Read each line in the file.
	line = buf;
        unsigned char curChar;
	if(line.length() > ROW_LENGTH)
	  continue;
        for(i=0; i<line.length(); i++) {
            curChar = line[i] & 0x0ff;
            for(int j=BYTE_LENGTH-1; j>=0; j--)
            {
                BinaryLine[i*BYTE_LENGTH+j] = curChar % 2 + '0';
                curChar = curChar >> 1;
            }
        }
        for(;i<ROW_LENGTH;i++)// Set the remain number to 0;
	  for(int j=0;j<BYTE_LENGTH;j++)
	    BinaryLine[i*BYTE_LENGTH+j] = '0';
	outFile << BinaryLine << endl;
    }
}

}// end namespace ctpp
