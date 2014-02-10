#ifndef IMAGE_TPP_INCLUDE
#define IMAGE_TPP_INCLUDE

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
using std::string;
using std::getline;
using std::stringstream;
#include<limits>
using std::numeric_limits;

template <typename TYPE>
class Image{
	public:
		Image(istream& istr);
		Image(TYPE* d, int h, int w, int s, int m);
		~Image();
		Image& operator = (Image img);
		Image(const Image& img);
		void SwapImg(Image& img);
		
		TYPE at(int r, int c) const;
		TYPE Min(const TYPE* d, int sizeOf) const;
		TYPE Max(const TYPE* d, int sizeOf) const;
		
		inline TYPE* getData() const {return data;}
		inline int getRows() const {return rows;}
		inline int getCols() const {return cols;}
		inline int getSize() const {return size;}
		inline int getCorrect() const {return correct;}
		inline int getMaxPix() const {return maxPix;}
		
		int calcRange(int hi, int lo) const;
		TYPE getEdgeMean(int center, int totalNums, int tL, int tR, int tU, int tD, int curRow, int curCol) const;
		TYPE compareImages(const Image& templateImage, TYPE templateMean, const TYPE* templateDiffs, const TYPE tempSDev) const;
		TYPE calcDiff(const TYPE* tempDiffs, TYPE tempDiffSum, int index, int r, int c, int curRow, int curCol) const;
		TYPE* getTemplateDiffs(TYPE mean) const;
		
	private:
		TYPE* data;
		TYPE** rowPtr;
		int rows;
		int cols;
		int size;
		int maxPix;
		int correct;
};

template <typename TYPE>
Image<TYPE>::Image(istream& istr){
	//Removed, no longer needed
}

//Constructor
template <typename TYPE>
Image<TYPE>::Image(TYPE* d, int h, int w, int s, int m){
	data = d;
	rows = h;
	cols = w;
	size = s;
	maxPix = m;
	correct = 1;
	
	rowPtr = new TYPE*[rows];
	TYPE* temp = data;
	for(int i = 0; i < rows; i++){
			rowPtr[i] = temp;
			temp += cols;
	}
}

//Destructor, delete allocated memory if necessary
template <typename TYPE>
Image<TYPE>::~Image(){
	if(data != NULL){
		delete [] data;
		data = NULL;
	}
	if(rowPtr != NULL){
		delete [] rowPtr;
		rowPtr = NULL;
	}
}

template <typename TYPE>
Image<TYPE>& Image<TYPE>::operator =(Image img){
	SwapImg(img);
	return *this;
}

//Copy constructor
template <typename TYPE>
Image<TYPE>::Image(const Image& img){
	rows = img.rows;
	cols = img.cols;
	size = img.size;
	maxPix = img.maxPix;
	correct = img.correct;
	
	data = new TYPE[size];
	TYPE* end = img.data + size;
	TYPE* oldPtr = img.data;
	TYPE* newPtr = data;
	for(; oldPtr!=end; oldPtr++, newPtr++){
		*newPtr = *oldPtr;
	}
	rowPtr = new TYPE*[rows];
	TYPE* temp = data;
	for(int i = 0; i < rows; i++){
		rowPtr[i] = temp;
		temp += cols;
	}
}

template <typename TYPE>
void Image<TYPE>::SwapImg(Image& img){
		
		TYPE* dTemp = data;
		data = img.data;
		img.data = dTemp;
		
		TYPE** rTemp = rowPtr;
		rowPtr = img.rowPtr;
		img.rowPtr = rTemp;
		
		int rowTemp = rows;
		rows = img.rows;
		img.rows = rowTemp;
		
		int colTemp = cols;
		cols = img.cols;
		img.cols = colTemp;
		
		int sizeTemp = size;
		size = img.size;
		img.size = sizeTemp;
		
		int mTemp = maxPix;
		maxPix = img.maxPix;
		img.maxPix = mTemp;
		
		int corrTemp = correct;
		correct = img.correct;
		img.correct = corrTemp;
}

template <typename TYPE>
TYPE Image<TYPE>::at(int r, int c) const{
	return rowPtr[r][c];
}

//Find the minimum in a dataset
template <typename TYPE>
TYPE Image<TYPE>::Min(const TYPE* d, int sizeOf) const{
	TYPE minVal = d[0];
	TYPE* temp = d+1;
	TYPE* end = d + sizeOf;
	for(; temp != end; temp++){
		if(*temp < minVal){
			minVal = *temp;
		}
	}
	return minVal;
}

//Find maximum in a dataset
template <typename TYPE>
TYPE Image<TYPE>::Max(const TYPE* d, int sizeOf) const{
	TYPE maxVal = d[0];
	TYPE* temp = d+1;
	TYPE* end = d + sizeOf;
	for(; temp != end; temp++){
		if(*temp > maxVal){
			maxVal = *temp;
		}
	}
	return maxVal;
}

//Need mostly for previous assignment iteration. Calculates a window of operation for
//standard deviation calculations 
template <typename TYPE>
int Image<TYPE>::calcRange(int hi, int lo) const{
	if(lo == 0){
		return 0;
	} else if(hi == 0){
		return 1;
	} else{
		return 2;
	}
}

//Calculate mean value in of a window of a specified height and width
//tL = to left, tR = to right, etc. 
template <typename TYPE>
TYPE Image<TYPE>::getEdgeMean(int center, int totalNums, int tL, int tR, int tU, int tD, int curRow, int curCol) const{
	TYPE tot = 0;
	tD += curRow;
	tR += curCol;
	for (int i = curRow; i <= tD; i++){
		for (int j = curCol; j <= tR; j++){
			//int currIn = center + i*cols + j;
			tot += rowPtr[i][j];
		}
	}
	TYPE mean = tot/totalNums;
	return mean;
}

//Takes in template image information and finds the maximum correlation value(-1 to 1) by comparing it against every possible
//overlap
template <typename TYPE>
TYPE Image<TYPE>::compareImages(const Image<TYPE>& templateImage, TYPE templateMean, const TYPE* templateDiffs, TYPE tempSDev) const{
	int boundX = templateImage.cols;
	int boundY = templateImage.rows;
	int farRight = cols - boundX + 1;
	int farDown = rows - boundY + 1;
	double currMax = -numeric_limits<double>::infinity();
	for(int i = 0; i < farDown; i++){
		for(int j = 0; j < farRight; j++){
			TYPE cVal = calcDiff(templateDiffs, tempSDev, 0, boundY, boundX, i, j);
			if(cVal == 1){
				return 1;
			}
			if(cVal > currMax){
				currMax = cVal;
			}
		}
	}		
	return currMax;
}

//Loop through all pixels in a given window to calculate deviations from the given mean
template <typename TYPE>
TYPE Image<TYPE>::calcDiff(const TYPE* tempDiffs, TYPE tempDiffSum, int index, int r, int c, int curRow, int curCol) const{
	TYPE targetMean = getEdgeMean(index, r*c, 0, c-1, 0, r-1, curRow, curCol);
	TYPE num = 0;
	TYPE den = 0;
	int count = 0;
	int maxRow = curRow + r;
	int maxCol = curCol + c;
	for(int i = curRow; i < maxRow; i++){
		for(int j = curCol; j < maxCol; j++){
			//int currIn = index + i*cols + j;
			TYPE targetDiff = rowPtr[i][j] - targetMean;
			num += targetDiff*tempDiffs[count];
			count++;
			den += targetDiff*targetDiff;
			
		}
	}
	den *= tempDiffSum;
	den = sqrt(den);
	
	TYPE toReturn;
	if(den == 0){
		toReturn = -numeric_limits<TYPE>::infinity();
	}
	else{
		toReturn = num/den;
	}
	return toReturn;
}

//Optimization function to avoid doing repeat calculations on the template image
template <typename TYPE>
TYPE* Image<TYPE>::getTemplateDiffs(TYPE mean) const{
	TYPE* toReturn = new TYPE[size];
	for(int i = 0; i < size; i++){
		toReturn[i] = data[i] - mean;
	}
	return toReturn;
}

#endif
