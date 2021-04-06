#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

using std::string;
using std::vector;
using std::cout;
using std::endl;
using namespace std;

typedef struct
{
	int score;
	char file_name[260];
}TFileProp;

bool GreaterSort(TFileProp a, TFileProp b)
{
	return (a.score > b.score);
}

void StructSort() {

	vector<TFileProp> myvector;

	TFileProp t1 = { 3, "C" };
	TFileProp t2 = { 1, "A" };
	TFileProp t3 = { 2, "B" };

	myvector.push_back(t1);
	myvector.push_back(t2);
	myvector.push_back(t3);

	std::sort(myvector.begin(), myvector.end(), GreaterSort);

	for (std::vector<TFileProp>::iterator it = myvector.begin(); it != myvector.end(); ++it)
		printf("%d %s\n", it->score, it->file_name);
}