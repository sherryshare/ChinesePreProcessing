#include "file_processing.h"
namespace ctpp {
using namespace std;
FileProcessing::FileProcessing(string fileName, bool isRawFile):fileName(fileName),isRawFile(isRawFile)
{

}
FileProcessing::~FileProcessing()
{
    closeIOFile();
}


void FileProcessing::ChangeFileName(string newAffix, string newSuffix)
{
    int i=fileName.find_last_of('.');
    int j=fileName.find_last_of('/');//??
    if (i>j) newFileName = fileName.substr(0,i) + newAffix+newSuffix;
    else newFileName = fileName + newAffix + newSuffix;
}
void FileProcessing::GetRawFormatFromAnnotatedFile(void )
{
    ChangeFileName("-raw");
    openIOFile();
    /* TO DO SOMETHING */
    /*get the raw format from the annotated file and write it to the new file.*/
    closeIOFile();
}

int FileProcessing::openIOFile(void )
{
    if(fileName.empty() || newFileName.empty()) {
        cout << "File name is empty!" << endl;
        return -1;
    }
    inFile.open(fileName.c_str());
    if(!inFile.is_open()) {
        cout << "Can't open the file " << fileName << endl;
        return -1;
    }
    outFile.open(newFileName.c_str());
    if(!outFile.is_open()) {
        cout << "Can't open the file " << newFileName << endl;
        inFile.close();
        return -1;
    }
    return 1;
}

void FileProcessing::closeIOFile(void )
{
    if(inFile.is_open())
        inFile.close();
    if(outFile.is_open())
        outFile.close();
}

string FileProcessing::ChangeBracket(string lineStr)
// Change English Bracket << and >> to Chinese Bracket 《 and 》.
{
    int i;

    i=lineStr.find("<<");
    while (i!=lineStr.npos && (lineStr.length()>=(i+2)))
    {
        lineStr=lineStr.substr(0,i)+"《"+lineStr.substr(i+2);
        i=lineStr.find("<<");
    }
    i=lineStr.find(">>");
    while (i!=lineStr.npos && (lineStr.length()>=(i+2)))
    {
        lineStr=lineStr.substr(0,i) + "》" + lineStr.substr(i+2);
        i=lineStr.find(">>");
    }
    return lineStr;
}

char FileProcessing::ChangeFullCaseToHalf(string fullCaseStr)
//Change a full case puncture or character to a half case. UTF-8
{
    char halfCase = 0;
    if(fullCaseStr == "　")
        halfCase = ' ';
    else if(fullCaseStr >= "！" && fullCaseStr <= "＿") {
        halfCase = (fullCaseStr[2] & 0x0ff) - 0x60;
    }
    else if(fullCaseStr >= "｀" && fullCaseStr <= "～") {
        halfCase = (fullCaseStr[2] & 0x0ff) - 0x20;
    }
    return halfCase;
}


string FileProcessing::RemoveSpaceOneLine(string lineStr)
//Delete the Chinese and Western blank in one line.
{

    lineStr.erase(0,lineStr.find_first_not_of(" "));//delete the blank spaces on the left

    string retStr="";
    string wordStr;
    char halfCaseCh;
    bool lastWordIsEngCh=false;

    while(!lineStr.empty()) {
        wordStr = lineStr.substr(0,1);
        // When this char is a Western Character
        if (wordStr[0]>=0) {
            //Delete the blank,tab,enter,wrap character and store the others
            if ((wordStr[0]!=32 && wordStr[0]!=9 && wordStr[0]!=13 && wordStr[0]!=10)
                    || (wordStr[0] == 32 && lastWordIsEngCh))// Blanks between English words can't be deleted.
                retStr=retStr.insert(retStr.length(),1,wordStr[0]);//add char to the end of the string
            if((wordStr[0]>='a' && wordStr[0]<='z') || (wordStr[0]>='A' && wordStr[0] <= 'Z'))
                lastWordIsEngCh = true;
            else
                lastWordIsEngCh = false;
            lineStr = lineStr.substr(1);
        }
        // When this char is a Chinese Character
        else {
            if(lineStr.length()>2) // Check whether the remain string is more than 3 words.
            {
                wordStr = lineStr.substr(0,3);//Get the first 3 words.

            }
            else if(lineStr.length() == 2) { // Remain 2 words.
                wordStr = lineStr.substr(0,2);
//                 if(wordStr[1] < 0) {
//                     retStr += wordStr;
//                     lineStr = lineStr.substr(2);
//                     lastWordIsEngCh = false;
//                 }
//                 else {
//                     retStr += lineStr.substr(0,1);
//                     lineStr = lineStr.substr(1);
//                     lastWordIsEngCh = false;
//                 }
            }
            else { // If the remains are less than 1 words.
                wordStr = lineStr.substr(0,1);
//                 lineStr = lineStr.substr(1);
//                 lastWordIsEngCh = false;

            }
            if(wordStr.length() == 3 && wordStr[1]<0 && wordStr[2]<0)//Complete Chinese word
            {
                //Delete the Chinese full-width blank and store the others (0xe38080)
                halfCaseCh = ChangeFullCaseToHalf(wordStr);
                if(halfCaseCh == 0) { //Not full case, ordinary Chinese word
                    retStr += wordStr;
                    lineStr = lineStr.substr(3);
                    //lastWordIsEngCh = false;
                }
                else { //Full case change to half case, need to be processed in the next loop.
                    lineStr = lineStr.substr(3);
                    lineStr = lineStr.insert(0,1,halfCaseCh);
                    continue;
                }
            }
            else if(wordStr.length()>=2 && wordStr[1] < 0) {
                retStr += wordStr.substr(0,2);
                lineStr = lineStr.substr(2);
                //lastWordIsEngCh = false;
            }
            else {
                retStr += wordStr.substr(0,1);
                lineStr = lineStr.substr(1);
                //lastWordIsEngCh = false;
            }
            lastWordIsEngCh = false;
        }
        if(lineStr.length()>1) {
            lineStr.erase(0,lineStr.find_first_not_of(" "));//delete the blank spaces on the left of the remain string.
        }
    }
    return retStr;

}

int FileProcessing::GetSentence(string& lineStr)
{   // Get a complete sentence in a string with the end index of the sentence returned.
    string wordStr; // Store a Chinese word
    int i=0;
    vector<char> puncVecChar;// Store the left puncture of English puncture pairs in stack.
    vector<string> puncVecStr;// Store the left puncture of Chinese puncture pairs in stack.
    int len=lineStr.length();
    bool foundSentence = false;
    bool lastWordIsEndPunc = false;// Judge whether the last word is an ending puncture.
    //lineStr = RemoveSpaceOneLine(lineStr);

    while(i < len && !(foundSentence && puncVecChar.empty() && puncVecStr.empty())) {
        if(lineStr[i]>0) { // The English character
            string punctureStrEnd = E_PUNCTURE_END,punctureStrLeft = E_PUNCTURE_LEFT,
                   punctureStrRight = E_PUNCTURE_RIGHT,punctureStrLR = E_PUNCTURE_LR;
            if(punctureStrLR.find(lineStr[i])!=punctureStrLR.npos) { // When finding the English left-right puncture.
                if(!puncVecChar.empty() && puncVecChar.back() == lineStr[i]) { // When the left one has been pushed back.
                    puncVecChar.pop_back();//Delete the left one.
                }
                else {
                    puncVecChar.push_back(lineStr[i]);
                }
            }
            else if(punctureStrLeft.find(lineStr[i])!=punctureStrLeft.npos) { // When finding the English left puncture
                puncVecChar.push_back(lineStr[i]);
            }
            else if(punctureStrRight.find(lineStr[i])!=punctureStrLeft.npos
                    && !puncVecChar.empty() && puncVecChar.back()==lineStr[i]-1) {
                // When finding the English right puncture with the relevant left one on top of the stack.
                // If not, ignore it.
                puncVecChar.pop_back();//Delete the left one.
            }
            else if(punctureStrEnd.find(lineStr[i])!=punctureStrEnd.npos  // When finding the English ending puncture.
                    && !(lineStr[i]=='.' && lineStr[i-1]<='9' && lineStr[i-1]>='0'
                         && lineStr[i-1]<='9' && lineStr[i-1]>='0')) { // Deal with the decimal.
                lastWordIsEndPunc = true;
                //foundSentence=true;
            }
            else if(lastWordIsEndPunc && puncVecChar.empty() && puncVecStr.empty()) {
                // Other English character while the last word is an ending puncture and stacks are empty.
                foundSentence = true;
                i--;
            }
            else// Other English character
                lastWordIsEndPunc = false;
            i++;
        }
        else { // The Chinese character
            wordStr = lineStr.substr(i,3);// Get the Chinese character
            string punctureStrEnd = C_PUNCTURE_END,punctureStrLeft = C_PUNCTURE_LEFT,
                   punctureStrRight = C_PUNCTURE_RIGHT;
            if(punctureStrLeft.find(wordStr.c_str())!=punctureStrLeft.npos) { // When finding the Chinese left puncture
                puncVecStr.push_back(wordStr);
            }
            else if(punctureStrRight.find(wordStr.c_str())!=punctureStrRight.npos && !puncVecStr.empty()
                    && punctureStrLeft.find(puncVecStr.back())==punctureStrRight.find(wordStr.c_str())) {
                // When finding the Chinese right puncture with the relevant left one on top of the stack.
                // If not, ignore it.
                puncVecStr.pop_back();//Delete the left one.
            }
            else if(punctureStrEnd.find(wordStr.c_str())!=punctureStrEnd.npos) {// When finding the Chinese end puncture.
                //Regard those ending punctures before the right puncture of puncture pairs to be part of a sentence.
                lastWordIsEndPunc = true;
            }
            else if(lastWordIsEndPunc && puncVecChar.empty() && puncVecStr.empty()) {
                // Other Chinese character while the last word is an ending puncture and stacks are empty.
                foundSentence = true;// Only countinuous ending puncture( or with right puncture) can result in a complete sentence.
                i-=3;
            }
            else// Other Chinese character
                lastWordIsEndPunc = false;
            i+=3;
        }
    }

    if(!(foundSentence && puncVecChar.empty() && puncVecStr.empty()))
        // If left puncture remains without matched right ones, return 0 for none cutting.
        // For the right ones without matched left, ignore them and keep cutting.
        return 0;
    else
        return i;
}

void FileProcessing::SentenceSegementation(void )
{   // Sentence Segmentation function of single file.
    string surplus = ""; // The remain string in the end of a line which cannot form a sentence.
    char buf[MAXLINE];
    string line;
    int i,len;
    ChangeFileName("-sen");
    openIOFile();
    while(inFile.getline(buf,MAXLINE)) {// Read each line in the file.
        line = buf;
        line = RemoveSpaceOneLine(line);// Delete all blanks in one line.
        if(line.empty()) {
            continue;
        }
        line=surplus+line; // Add the remain part of the last line to the present line.
        surplus = ""; // Set surplus to null

        len=line.length(); // Record the total length of the present line.

        while((i=GetSentence(line))>0) { // If a sentence can be read from present line.
            outFile << line.substr(0,i) << endl;// Write the sentence to the out file.
            line.erase(0,i);// Erase the writen part of the file.
        }

        if(!line.empty()) {// If all sentence has been writen and some words remain.
            if(len==line.length() && len < MAXTITLE)  // If the total line has no puncture and is less than MAXTITLE.
                outFile << line << endl; // Write the present line as a line without puncture.
            else
            {   // Not title but words remains.
                surplus = line; // Record remains to the surplus for the next loop.
            }
        }
    }
    if(!surplus.empty())
        // If the remain string isn't empty after last reading, the main reason is the punctures' illegal format.
        // Our way: directly output.
        // Another way: ignore without outputing.
        outFile << surplus << endl;
    closeIOFile();
}

}//end namespace ctpp
