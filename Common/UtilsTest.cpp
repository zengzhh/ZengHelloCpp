#include "UtilsTest.h"
#include <string>
#include <vector>
#include <iostream>
#include "Utils.h"

using namespace std;

bool UtilsTest::test_SplitString()
{
	cout << "**** test_SplitString()" << endl;
	string str = "";
	vector<string> v;
	Utils::SplitString(str, v, ".");
	for (int i = 0; i < v.size(); i++)
	{
		cout << i << ": " << v[i] << endl;
	}
	return false;
}
