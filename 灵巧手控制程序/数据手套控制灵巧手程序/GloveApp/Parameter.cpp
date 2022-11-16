#include "Parameter.h"
#include "INIParser.h"

Parameter::Parameter()
{
	com = "COM5";
	ipPort = 5200;
	check[0] = 0;
	check[1] = 0;
	for (auto i = 0; i < INDEX_LEN; i++)
	{
		spinBoxMinValue[i] = 0;
		spinBoxMaxValue[i] = 255;
	}
	this->loadParameter();
}

Parameter::~Parameter()
{
	this->saveParameter();
}

void Parameter::loadParameter()
{
	INIParser ini_parser;

	if (ini_parser.isExist("conf.ini") == false)
	{
		ini_parser.createINI("conf.ini");
		this->saveParameter();
	}

	ini_parser.readINI("conf.ini");

	com = ini_parser.getValue("COM", "Port");
	ipPort = ini_parser.getIntValue("IP", "Port");
	check[0] = ini_parser.getIntValue("chechBox", "Glove");
	check[1] = ini_parser.getIntValue("chechBox", "Hand");
	for (auto i = 0; i < INDEX_LEN; i++)
	{
		spinBoxMinValue[i] = ini_parser.getIntValue("spinBox"+std::to_string(i), "Min");
		spinBoxMaxValue[i] = ini_parser.getIntValue("spinBox"+std::to_string(i), "Max");
	}
}

void Parameter::saveParameter()
{
	INIParser ini_parser;
	ini_parser.readINI("conf.ini");
	ini_parser.clear();

	ini_parser.setValue("COM", "Port", com);
	ini_parser.setValue("IP", "Port", ipPort);
	ini_parser.setValue("chechBox", "Glove", check[0]);
	ini_parser.setValue("chechBox", "Hand", check[1]);
	for (auto i = 0; i < INDEX_LEN; i++)
	{
		ini_parser.setValue("spinBox"+std::to_string(i), "Min", spinBoxMinValue[i]);
		ini_parser.setValue("spinBox"+std::to_string(i), "Max", spinBoxMaxValue[i]);
	}
	ini_parser.writeINI("conf.ini");
}
