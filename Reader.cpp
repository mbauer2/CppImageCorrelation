#include<Reader.h>
#include<fstream>
#include<iostream>
#include<sstream>
#include<math.h>
using std::endl;
using std::cout;
using std::ifstream;
using std::ofstream;
using std::istream;
using std::ostream;
#include<vector>
using std::vector;
using std::string;
using std::getline;
using std::stringstream;
#include<limits>
using std::numeric_limits;

int Reader::readMagic(istream& istr, double fakeMax, double fakeMin){
	line_counter = 1;
	string textline;
	getline(istr,textline);
	//setting temporary max value to read header
	m = std::numeric_limits<double>::max();
	stringstream line_str(textline);
	int pos;
	if(!istr.fail()){
		char isP = line_str.get();
		isNum = line_str.get();
		///////CHECK for PNUM and choose
		if(isP != 'P'){
		  cout<< "Error on line 1, first character must be P" << endl;
		  return -1;
		}
		else{
			int headerInfo;
			pos = line_str.tellg();
			vector<int> importantInfo;
			int cont;
			for(int i = 0; i < 3; i++){
				cont = 0;
				while(cont == 0){
					char res = getNextNumber<int>(line_str, fakeMax, fakeMin, 0, headerInfo);
					if(res == 'a'){//no more numbers found on this line, no errors
						if(istr.eof()){
							cout << "Error on line " << line_counter-1 << ", improper file format" << endl;
							return -1;
						}
						line_counter++;
						getline(istr, textline);
						line_str.str(textline);
						line_str.clear();
						line_str.seekg(0);
						pos = 0;
					}
					else if(res == 'c'){
						cout << "Error on line " << line_counter << ", invalid input in file." << endl;
						return -1;
					}
					else{
						//cout << "Next Header Value: " << headerInfo << endl;
						if(headerInfo < 0){
							cout << headerInfo << endl;
							cout << "Error on line " << line_counter << ", negative header values not allowed." << endl;
				  		return -1;
						}
						pos = line_str.tellg();
						if(pos < 0){
							getline(istr, textline);
							line_str.str(textline);
							line_str.seekg(0);
							line_str.clear();
							pos = 0;
							line_counter++;
						}
						//cout << "pushing headerInfo: " << headerInfo << endl;
						importantInfo.push_back(headerInfo);
						cont = 1;
					}
				}
			}
			//cout << "m from header reader is " << importantInfo[2] << endl;
			w = importantInfo[0];
			h = importantInfo[1];
			s = w*h;
			m = importantInfo[2];
			tran = line_str.str();
			posTran = line_str.tellg();
			streamPosTran = istr.tellg();
			
		}
	//	cout << "Exiting header" << endl;
		return 0;
	}
	else{
		cout<< "Error on line 1, Improper file format" << endl;
		return -1;
	}
}
