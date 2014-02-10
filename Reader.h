#ifndef IMAGE_H_INCLUDE
#define IMAGE_H_INCLUDE

#include<fstream>
#include<iostream>
#include<sstream>
#include<math.h>
#include<typeinfo>
using std::endl;
using std::cout;
using std::ifstream;
using std::ofstream;
using std::istream;
using std::ostream;
using std::string;
using std::getline;
using std::stringstream;

class Reader{
	public:
		int readMagic(istream& istr, double fakeMax, double fakeMin);
		template <typename TYPE>
		TYPE* readFile(istream& istr, double tempMax, double tempMin, int isTemp);
		template <typename TYPE>
		char getNextNumber(stringstream& line_str, double tempMax, double tempMin, int isTemp, TYPE& nextNum);
		
		
    
    inline int getIsNum(){return isNum;}
    inline int getS(){return s;}
    inline int getW(){return w;}
    inline int getH(){return h;}
    inline int getM(){return m;}
    
    
    
  private:
  	int line_counter;
  	int isNum;
  	int s;
  	int w;
  	int h;
  	double m;
  	string tran;
  	int posTran;
  	int streamPosTran;
};

		//Run through the file and print errors if there is any invlaid input
		template <typename TYPE>
		TYPE* Reader::readFile(istream& istr, double tempMax, double tempMin, int isTemp){
			TYPE* readData = new TYPE[s];
			int inputCount = 0;
			string textline = tran;
			int pos = posTran;
			stringstream line_str(textline);
			line_str.seekg(pos);		
			while(!istr.fail()){
				TYPE nextNum;
				char res = getNextNumber<TYPE>(line_str, tempMax, tempMin, isTemp, nextNum);
				if(res == 'a'){//End of line has been reached but there were no errors
					if(istr.eof()){
						break;
					}
					line_counter++;
					getline(istr, textline);
					line_str.str(textline);
					line_str.seekg(0);
					line_str.clear();
					pos = 0;
				}
				else if(res == 'c'){
					cout << "Error on line " << line_counter << ", invalid input in file" << endl;
					delete [] readData;
					return NULL;
				}
				else{
					if(nextNum < m){//valid input, check to make sure it is less than the max value
						if(inputCount == s){//Inputs have exceeded the given file size
							cout << "Error on line " << line_counter << ", too many numbers in file" << endl;
							delete [] readData;
							return NULL;
						}
						readData[inputCount] = nextNum;
						inputCount++;
						pos = line_str.tellg();
						if(pos < 0){
							getline(istr, textline);
							line_str.str(textline);
							line_str.seekg(0);
							line_str.clear();
							pos = 0;
							line_counter++;
						}
					}
					else{
						cout << "Error on line " << line_counter << ", " << nextNum << " is >= " << m << endl;
						delete [] readData;
						return NULL;
					}
				}
			}
			if(inputCount < s){
				cout << "Error on line " << (line_counter-1) << ", not enough numbers in file" << endl;
				delete [] readData;
				return NULL;
			}
			else if(inputCount > s){
				cout << "Too many numbers in file" << endl;
				delete [] readData;
				return NULL;
			}
			else{
				return readData;
			}
		}
	
		//Searches through the file for the next valid input	
		template <typename TYPE>
		char Reader::getNextNumber(stringstream& line_str, double tempMax, double tempMin, int isTemp, TYPE& nextNum){
			double toRead;
			TYPE toReturn;
			if(line_str.eof() && line_str.fail()){ //	go to next line
				return 'a';
			}
			else if(line_str.fail()){
				//clear stream, get character, if # then continue to next line and increment line counter, else ERROR
				line_str.clear();
				char checkPound;
				line_str >> checkPound;
				if(checkPound == '#' || line_str.eof()){
		  		return 'a';
				}
				else{
		  		return 'c';
				}
			}
		else{
			///NEEDS TO CLIP OR FAIL
			if(isTemp == 1){
				if (toRead > m || toRead < -m || (toRead < tempMin || toRead > tempMax)){
					return 'c';
				}
				else{
					toReturn = (TYPE)toRead;
					nextNum = toReturn;
					return 'd';
				}
			}else{
				if(toRead > m || toRead < -m){
					return 'c';
				}
				if(toRead < tempMin){
					toRead = tempMin;					
				}
				else if(toRead > tempMax){
					toRead = tempMax;
				}
				toReturn = (TYPE)toRead;
				nextNum = toReturn;
				return 'd';	//successful, move on to next important number.
			}
		}
	}

#endif
