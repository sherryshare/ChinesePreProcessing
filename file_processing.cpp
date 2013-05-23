#include "file_processing.h"
namespace ctpp {
using namespace std;
FileProcessing::FileProcessing(string fileName, string outDir, bool isRawFile)
    :fileName(fileName),isRawFile(isRawFile),outDir(outDir)
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
    int bytesNum;

    while(!lineStr.empty()) {
        bytesNum = GetUTF8BytesNum(lineStr[0]);
        //wordStr = lineStr.substr(0,1);
        // When this char is a Western Character
        if(lineStr.length() < bytesNum) { // Remain bytes is less than a complete character.
            lineStr.erase();
            break;
        }
        if (bytesNum == 1) {
            //Delete the blank,tab,enter,wrap character and store the others
            if ((lineStr[0]!=32 && lineStr[0]!=9 && lineStr[0]!=13 && lineStr[0]!=10)
                    || (lineStr[0] == 32 && lastWordIsEngCh))// Blanks between English words can't be deleted.
                retStr=retStr.insert(retStr.length(),1,lineStr[0]);//add char to the end of the string
            if((lineStr[0]>='a' && lineStr[0]<='z') || (lineStr[0]>='A' && lineStr[0] <= 'Z'))
                lastWordIsEngCh = true;
            else
                lastWordIsEngCh = false;
            lineStr = lineStr.substr(1);
        }
        // When this char is a Chinese Character
        else if(bytesNum == 3) {
	    wordStr = lineStr.substr(0,3);
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
            lastWordIsEngCh = false;
        }
        else if(bytesNum) {
            retStr += lineStr.substr(0,bytesNum);
            lineStr = lineStr.substr(bytesNum);

        }
        else{// Wrong UTF-8 word
	  lineStr = lineStr.erase(0,1);	  
	}
        if(lineStr.length()>1) {
            lineStr.erase(0,lineStr.find_first_not_of(" "));//delete the blank spaces on the left of the remain string.
        }
    }
    return retStr;
}

int FileProcessing::GetUTF8BytesNum(char utf8Char)
{
    if(utf8Char >= 0)
        return 1;
    else if((utf8Char & 0x0e0) == 0x0c0)
        return 2;
    else if((utf8Char & 0x0f0) == 0x0e0)
        return 3;
    else if((utf8Char & 0x0f8) == 0x0f0)
        return 4;
    else if((utf8Char & 0x0fc) == 0x0f8)
        return 5;
    else if((utf8Char & 0x0fe) == 0x0fc)
        return 6;
    else {
        cout << "Wrong UTF-8 char" << endl;
        return 0;//wrong input
    }
}
bool FileProcessing::JudgeUTF8NextChar(char utf8Char)
// Judge whether the next char in a utf-8 word is correct.
{
    if((utf8Char & 0x0c0) == 0x080)
        return true;// The correct bytes in utf-8 (not the first one)
    else
        return false;// Not a utf-8 bytes.
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
    int bytesNum;

    while(i < len && !(foundSentence && puncVecChar.empty() && puncVecStr.empty())) {
        bytesNum = GetUTF8BytesNum(lineStr[i]);
        if(bytesNum == 1) { // The English character
            string punctureStrEnd = E_PUNCTURE_END,punctureStrLeft = E_PUNCTURE_LEFT,
                   punctureStrRight = E_PUNCTURE_RIGHT,punctureStrLR = E_PUNCTURE_LR;
            if(lastWordIsEndPunc && punctureStrEnd.find(lineStr[i]) == punctureStrEnd.npos) {
                // Not an Ending English puncture while the previous word is an ending puncture.
                if(puncVecChar.empty() && puncVecStr.empty()) {
                    //  All stacks are empty.
                    foundSentence = true;// Only countinuous ending puncture (or with right puncture) can result in a complete sentence.
                    break;
                }
                else if(puncVecChar.empty() && puncVecStr.size() == 1 && puncVecStr.back() == "“") {
                    // Cut the sentences between the “ ” into pieces.
                    puncVecStr.pop_back();//Delete the last one.
                    foundSentence = true;
                    lineStr = lineStr.substr(0,i) + "”“" + lineStr.substr(i);
                    i+=3;
                    break;
                }
                else if(punctureStrRight.find(lineStr[i]) == punctureStrRight.npos && !(puncVecChar.size() == 1
                        && punctureStrLR.find(puncVecChar.back()) != punctureStrLR.npos
                        && punctureStrLR.find(lineStr[i]) != punctureStrLR.npos)) { // Not the right or left-right puncture.
                    lastWordIsEndPunc = false;
                }
            }
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
                    && !puncVecChar.empty() && punctureStrLeft.find(puncVecChar.back())==punctureStrRight.find(lineStr[i])) {
                // When finding the English right puncture with the relevant left one on top of the stack.
                // If not, ignore it.
                puncVecChar.pop_back();//Delete the left one.
            }
            else if(punctureStrEnd.find(lineStr[i])!=punctureStrEnd.npos  // When finding the English ending puncture.
                    && !((lineStr[i]=='.' || lineStr[i]==':') && lineStr[i-1]<='9' && lineStr[i-1]>='0'
                         && lineStr[i-1]<='9' && lineStr[i-1]>='0')) { // Deal with the decimal & time,eg. 8:00
                lastWordIsEndPunc = true;
                //foundSentence=true;
            }
            else if(lastWordIsEndPunc && puncVecChar.empty() && puncVecStr.empty()) {
                // Other English character while the last word is an ending puncture and stacks are empty.
                foundSentence = true;
                i--;
            }
            else if(lastWordIsEndPunc && puncVecChar.empty()// Cut the sentences between the “ ” into pieces.
                    && puncVecStr.size() == 1 && puncVecStr.back() == "“" && lineStr.substr(i-3,3) != "“") {
                puncVecStr.pop_back();//Delete the last one.
                foundSentence = true;
                lineStr = lineStr.substr(0,i) + "”“" + lineStr.substr(i);
                i+=2;
            }
            else// Other English character
                lastWordIsEndPunc = false;
            i++;
        }
        else if(bytesNum) {
            int j;
            for(j = 1; j < bytesNum; j++)
                if(!JudgeUTF8NextChar(lineStr[j+i]))
                    break;
            if(j != bytesNum) { // Wrong Character
                lineStr = lineStr.erase(i,j+1);
                continue;
            }
            if(bytesNum == 3) { // The Chinese character
                wordStr = lineStr.substr(i,3);// Get the Chinese character, ignoring the 2 bytes Chinese character
                string punctureStrEnd = C_PUNCTURE_END,punctureStrLeft = C_PUNCTURE_LEFT,
                       punctureStrRight = C_PUNCTURE_RIGHT;
                if(lastWordIsEndPunc && punctureStrEnd.find(wordStr.c_str()) == punctureStrEnd.npos) {
                    // Not an Ending Chinese puncture while the previous word is an ending puncture.
                    if(puncVecChar.empty() && puncVecStr.empty()) {
                        //  All stacks are empty.
                        foundSentence = true;// Only countinuous ending puncture (or with right puncture) can result in a complete sentence.
                        break;
                    }
                    else if(puncVecChar.empty() && wordStr != "”" && puncVecStr.size() == 1 && puncVecStr.back() == "“") {
                        // Cut the sentences between the “ ” into pieces.
                        puncVecStr.pop_back();//Delete the last one.
                        foundSentence = true;
                        lineStr = lineStr.substr(0,i) + "”“" + lineStr.substr(i);
                        i+=3;
                        break;
                    }
                    else if(punctureStrRight.find(wordStr.c_str()) == punctureStrRight.npos)
                        // Not the right puncture.
                        lastWordIsEndPunc = false;
                }
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
            }
            i+=bytesNum;
        }
        else// Wrong first byte.
            lineStr = lineStr.erase(i,1);
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
    ChangeFileName("-sen",outDir);
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
