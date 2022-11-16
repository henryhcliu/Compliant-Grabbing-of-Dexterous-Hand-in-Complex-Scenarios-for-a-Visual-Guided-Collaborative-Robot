#ifndef INI_PARSER_H
#define INI_PARSER_H

#include <vector>
#include <map>
#include <string>

class ININode
{
public:
	ININode(std::string &root, std::string &key, std::string &value) :root(root), key(key), value(value) {}
	std::string root, key, value;
};

class SubNode
{
public:
	void InsertElement(std::string key, std::string value)
	{
		this->sub_node.insert(std::pair<std::string, std::string>(key, value));
	}
	std::map<std::string, std::string> sub_node;
};

class INIParser
{
public:
	bool isExist(std::string path);
	bool createINI(std::string path);
	int readINI(std::string path);
	int writeINI(std::string path);
	void clear() { map_ini.clear(); }

	int getIntValue(std::string root, std::string key);
	double getDoubleValue(std::string root, std::string key);
	std::string getValue(std::string root, std::string key);

	std::vector<ININode>::size_type getSize() { return map_ini.size(); }

	std::vector<ININode>::size_type setValue(std::string root, std::string key, int value);
	std::vector<ININode>::size_type setValue(std::string root, std::string key, double value);
	std::vector<ININode>::size_type setValue(std::string root, std::string key, std::string value);

private:
	std::map<std::string, SubNode> map_ini;
};

#endif // INI_PARSER_H
