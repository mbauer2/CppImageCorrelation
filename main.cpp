#include<Image.tpp>
#include<Reader.h>
#include<iostream>
using std::cout;
using std::endl;
#include <fstream>
using std::ifstream;
using std::ofstream;
#include<limits>
using std::numeric_limits;

//This was an assignment for my C++ class that was part of a semester-long development process.
//This particular part needed to read in two images in pgm format that could be of any type 
//so using templates was necessary. When run, the program with find the closest match between
//the source image and the target image to determine if the template image is inside the target image,
//all with the intentions of doing operations as quickly as possible. It recieved 100% with the required
//test cases and was on of the fastest, if not the fastest in the class.


//Helper function, find sum of squares 
template<typename TYPE>
TYPE getTemplateSDev(TYPE* vals, TYPE mean, int size){
	TYPE sum = 0;
	
	for(int i = 0; i < size; i++){
		sum += vals[i]*vals[i];
	}

	return sum;
}

//Excessive function inputs not preferred, but helps avoid re-doing things because of different template types
template<typename TYPE>
int driveIt(Reader& readTemplate,  int tempRows, int tempCols, int tempSize, int mPix, TYPE& toChange, double typeMax, double typeMin, istream& istr1, istream& istr2){
  //Read in the rest of the templates image
  TYPE* templateBody = readTemplate.readFile<TYPE>(istr2, typeMax, typeMin, 1);
  if(templateBody == NULL){
  	cout << "00ps Template" << endl;
  	return -1;
  }
  Image<TYPE> templateImage(templateBody, tempRows, tempCols, tempSize, mPix);

  Reader readTarget;
  int isValid2 = readTarget.readMagic(istr1, numeric_limits<double>::infinity(), -numeric_limits<double>::infinity());
  if(isValid2 == -1){
  	cout << "00ps Target" << endl;
  	return -1;
  }
  int targetRows = readTarget.getH();
  int targetCols = readTarget.getW();
  int targetSize = readTarget.getS();
  int tmPix = readTarget.getM();
  
  if(tempRows > targetRows || tempCols > targetCols || tempSize > targetSize){
  	cout << "Template dimensions may not be larger than the target. " << endl;
  	return -1;
  }
  //cout << "tmPix is " << tmPix << endl;
  TYPE* targetBody = readTarget.readFile<TYPE>(istr1, typeMax, typeMin, 0);
  if(targetBody == NULL){
  	cout << "00ps Target" << endl;
  	return -1;
  }
  //Make image
  Image<TYPE> targetImage(targetBody, targetRows, targetCols, targetSize, tmPix);
  //Get template info
  TYPE templateMean = templateImage.getEdgeMean(0, tempRows*tempCols, 0, tempCols-1, 0, tempRows-1, 0, 0);
  TYPE* tempDiffs = templateImage.getTemplateDiffs(templateMean);
  TYPE tempSDev = getTemplateSDev(tempDiffs, templateMean, tempSize);
  if(tempSDev == 0){
  	cout << "No correlations defined, template is constant." << endl;
  	delete [] tempDiffs;
  	return -1;
  }
  TYPE r1 =  targetImage.compareImages(templateImage, templateMean, tempDiffs, tempSDev);
  //This is to indicate if any matches have been found and was later refined to not depend on numeric_limits
  if (r1 == -numeric_limits<TYPE>::infinity()){
  	if(0 == (int)r1){
  		
  	} else{
  		delete [] tempDiffs;
  		cout << "No correlations defined, constant image." << endl;
  		return -1;
  	}
  }
  toChange = r1;
  delete [] tempDiffs;
	return 1;
}

int main(int argc, char* argv[]){
  if(argc == 3){
  	ifstream istr1(argv[1]);
  	ifstream istr2(argv[2]);
  	if(istr1.fail() || istr2.fail()){
  	  cout << "Error: File cannot be read" << endl;
  	  return -1;
  	}

  	Reader readTemplate;
  	int isValid = readTemplate.readMagic(istr2, numeric_limits<double>::infinity(), -numeric_limits<double>::infinity());
  	int tempMax = readTemplate.getM();
  	if(isValid == -1){
  		cout << "00ps Template" << endl;
  		return -1;
  	}
  	int filetype = readTemplate.getIsNum();
  	int tempRows = readTemplate.getH();
  	int tempCols = readTemplate.getW();
  	int tempSize = readTemplate.getS();
  	if(tempSize <= 1){
  		cout << "Template must be larger than one pixel" << endl;
  		return -1;
  	}
        //It is necessary to find information about the template image before reading the body or the source image
        //and then make decisions on what should be passed into the driveIt function
  	int mPix = readTemplate.getM();
  	if(filetype == 50){
  		if(tempMax <= 256){
  			///unsigned int
  			unsigned char toChange = 0;
				int res = driveIt<unsigned char>(readTemplate, tempRows, tempCols, tempSize, mPix, toChange, 255, 0, istr1, istr2);
				if (res == -1){
  				return -1;
  			}
  			else{
  				cout << (int)toChange << endl;
  			}
  		}
  		else if(tempMax > 256 && tempMax <= 65535){
  			//unsigned short
  			unsigned short toChange = 0;
  			int res = driveIt<unsigned short>(readTemplate, tempRows, tempCols, tempSize, mPix, toChange, 65535, 0, istr1, istr2);
  			if (res == -1){
  				return -1;
  			}
  			else{
  				cout << toChange << endl;
  			}
  			

  		}
  		else if(tempMax > 65535 && tempMax <= 4294967296){
  			//unsigned int
  			unsigned int toChange = 0;
				int res = driveIt<unsigned int>(readTemplate, tempRows, tempCols, tempSize, mPix, toChange, 4294967295, 0, istr1, istr2);
				if (res == -1){
  				return -1;
  			}
  			else{
  				cout << toChange << endl;
  			}

  		} else{
  			//unsigned long
  			unsigned long toChange = 0;
				int res = driveIt<unsigned long>(readTemplate,  tempRows, tempCols, tempSize, mPix, toChange, std::numeric_limits<double>::max(), 0, istr1, istr2);
  			if (res == -1){
  				return -1;
  			}
  			else{
  				cout << toChange << endl;
  			}
  		}
  	}
  	else if(filetype == 55){
  		if(tempMax <= 128){
  			//	signed char
  			signed char toChange = 0;
  			int res = driveIt<signed char>(readTemplate, tempRows, tempCols, tempSize, mPix, toChange, 127, -128, istr1, istr2);
				if (res == -1){
  				return -1;
  			}
  			else{
  				cout << (int)toChange << endl;
  			}
  		}
  		else if(tempMax > 128 && tempMax <= 32768){
  			//signed short
  			signed short toChange = 0;
  			int res = driveIt<signed short>(readTemplate, tempRows, tempCols, tempSize, mPix, toChange, 32767, -32768, istr1, istr2);
 				if (res == -1){
  				return -1;
  			}
  			else{
  				cout << toChange << endl;
  			}
  		}
  		else if(tempMax > 32768 && tempMax <= 2147483648){
  			//signed int
  			signed int toChange = 0;
  			int res = driveIt<signed int>(readTemplate, tempRows, tempCols, tempSize, mPix, toChange, 2147483647, -2147483648, istr1, istr2);
				if (res == -1){
  				return -1;
  			}
  			else{
  				cout << toChange << endl;
  			}
  		} else{
  			//signed long
  			signed long toChange = 0;
  			int res = driveIt<signed long>(readTemplate, tempRows, tempCols, tempSize, mPix, toChange, std::numeric_limits<double>::max(), -std::numeric_limits<double>::max(), istr1, istr2);
				if (res == -1){
  				return -1;
  			}
  			else{
  				cout << toChange << endl;
  			}
  		}
  	}
  	else if(filetype == 57){
  		//double
			//Read rest of template image
			double toChange = 0;
  		int res = driveIt<double>(readTemplate, tempRows, tempCols, tempSize, mPix, toChange, std::numeric_limits<double>::max(), -std::numeric_limits<double>::max(), istr1, istr2);
  		if (res == -1){
  				return -1;
  		}
  		else{
  			cout << toChange << endl;
  		}
  	}
  	else if(filetype == 56){
  		float toChange = 0;
  		int res = driveIt<float>(readTemplate, tempRows, tempCols, tempSize, mPix, toChange, std::numeric_limits<double>::max(), -std::numeric_limits<double>::max(), istr1, istr2);
  		if (res == -1){
  			return -1;
  		}
  		else{
  			cout << toChange << endl;
  		}
  	}
  	else{
  		cout << "Invalid file type" << endl;
  		return -1;
  	}
  }
  else{
    cout << "Improper use of arguments" << endl;
    return -1;
  }
}
