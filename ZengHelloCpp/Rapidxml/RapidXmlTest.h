#pragma once

#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_print.hpp"
#include "rapidxml/rapidxml_utils.hpp"

class RapidXmlTest
{
public:
	bool test_createxml();
	bool test_accessdom();
	bool test_insertnode();
	bool test_removenode();
	bool tets_modifynode();
	bool IsXmlNameValid(const std::string& name);
	std::string GetXmlConfVal(const std::string& name);
	bool SetXmlConfVal(const std::string& name, const std::string& value);
	std::string GetXmlConfValEx(const std::string& pathname);
	bool SetXmlConfValEx(const std::string& pathname, const std::string& value);
private:
};