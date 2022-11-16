#pragma once
#include <String>
#define INDEX_LEN 18
class Parameter
{
public:
	Parameter();
	~Parameter();

public:
	void loadParameter();
	void saveParameter();

public:
	std::string com;
	int ipPort;
	int check[2];
	int spinBoxMinValue[INDEX_LEN];
	int spinBoxMaxValue[INDEX_LEN];
};

