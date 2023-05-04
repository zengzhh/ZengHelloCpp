#include "RapidXmlTest.h"
#include <iostream>
#include <windows.h>
#include <io.h>
#include "../../Common/Utils.h"

using namespace std;

bool RapidXmlTest::test_createxml()
{
	cout << "**** test_createxml()" << endl;
	string XMLFileName = "Rapidxml\\a.xml";

	if (_access(XMLFileName.c_str(), 0) == 0)
	{
		cout << "file exists, delete!" << endl;
		DeleteFileA(XMLFileName.c_str());
	}

	// DOM
	rapidxml::xml_document<> doc;

	// node_declaration
	rapidxml::xml_node<>* declaration = doc.allocate_node(rapidxml::node_declaration);
	declaration->append_attribute(doc.allocate_attribute("version", "1.0"));
	declaration->append_attribute(doc.allocate_attribute("encoding", "utf-8"));
	doc.append_node(declaration);

	// node_pi
	//rapidxml::xml_node<>* root = doc.allocate_node(rapidxml::node_pi, doc.allocate_string("xml version='1.0' encoding='utf-8'"));
	//doc.append_node(root);

	// node_comment
	rapidxml::xml_node<>* comment = doc.allocate_node(rapidxml::node_comment, 0, " powered by rapidxml ");
	doc.append_node(comment);

	// node_element 
	rapidxml::xml_node<>* node = doc.allocate_node(rapidxml::node_element, "config");
	doc.append_node(node);

	// node_element 
	rapidxml::xml_node<>* to = doc.allocate_node(rapidxml::node_element, "to", "George");
	node->append_node(to);

	// node_element
	rapidxml::xml_node<>* from = doc.allocate_node(rapidxml::node_element, "from", "John");
	from->append_attribute(doc.allocate_attribute("gender", "male"));
	from->append_attribute(doc.allocate_attribute("id", "007"));
	node->append_node(from);

	// node_element
	rapidxml::xml_node<>* filename = doc.allocate_node(rapidxml::node_element, "filename", XMLFileName.c_str());
	node->append_node(filename);

	// node_element
	rapidxml::xml_node<>* host = doc.allocate_node(rapidxml::node_element, "host", "http://192.168.0.1");
	node->append_node(host);

	string text = "after create xml:\n";
	rapidxml::print(std::back_inserter(text), doc, 0);
	std::cout << text << std::endl;

	// write DOM to file
	std::ofstream outfile(XMLFileName.c_str(), std::ios::out);
	if (outfile)
	{
		outfile << doc;
		outfile.close();
	}
	doc.clear();
	return false;
}

bool RapidXmlTest::test_accessdom()
{
	cout << "**** test_accessdom()" << endl;
	string XMLFileName = "Rapidxml\\a.xml";
	if (_access(XMLFileName.c_str(), 0) != 0)
	{
		cout << "file not exist!" << endl;
		return false;
	}
	// load xml
	rapidxml::file<> fdoc(XMLFileName.c_str());
	cout << fdoc.data() << endl;
	cout << "length of xml:" << fdoc.size() << endl;
	rapidxml::xml_document<> doc; // character type defaults to char
	doc.parse<0>(fdoc.data());    // 0 means default parse flags
	// root node
	rapidxml::xml_node<>* root = doc.first_node();
	cout << "root:" << root << endl;
	cout << *root << endl;
	cout << "root name:" << root->name() << endl;
	cout << "root name_size:" << root->name_size() << endl << endl;

	// first_node
	rapidxml::xml_node<>* node_first = root->first_node();
	cout << "first node of root:" << endl << *node_first;
	// last_node
	rapidxml::xml_node<>* node_last = root->last_node();
	cout << "last node of root:" << endl << *node_last << endl;

	// node by name
	rapidxml::xml_node<>* node = root->first_node("from");
	rapidxml::xml_attribute<>* attr;
	// attribute
	attr = node->first_attribute();
	cout << "attr_name:" << attr->name() << endl;
	cout << "attr_value:" << attr->value() << endl;
	attr = attr->next_attribute();
	cout << "attr_name:" << attr->name() << endl;
	cout << "attr_value:" << attr->value() << endl << endl;

	// sibling
	for (; node_first != NULL; node_first = node_first->next_sibling())
	{
		cout << "sib:" << *node_first;
		cout << "sib name:" << node_first->name() << endl;
		cout << "sib value:" << node_first->value() << endl << endl;
	}
	doc.clear();
	return false;
}

bool RapidXmlTest::test_insertnode()
{
	cout << "**** test_insertnode()" << endl;
	string XMLFileName = "Rapidxml\\a.xml";
	// load xml
	rapidxml::file<> fdoc(XMLFileName.c_str());
	cout << fdoc.data() << endl;
	rapidxml::xml_document<> doc;
	doc.parse<0>(fdoc.data());

	// root node
	rapidxml::xml_node<>* root = doc.first_node();

	// add node
	rapidxml::xml_node<>* new_node = root->first_node("keyboard");
	if (new_node == nullptr)
	{
		new_node = doc.allocate_node(rapidxml::node_element, "keyboard", "logitech");
		new_node->append_attribute(doc.allocate_attribute("interface", "USB"));
		root->append_node(new_node);
	}
	string text = "after insert new_node:\n";
	rapidxml::print(std::back_inserter(text), doc, 0);
	std::cout << text << std::endl;

	// write DOM to file
	std::ofstream outfile(XMLFileName.c_str(), std::ios::out);
	if (outfile)
	{
		outfile << doc;
		outfile.close();
	}
	doc.clear();
	return false;
}

bool RapidXmlTest::test_removenode()
{
	cout << "**** test_removenode()" << endl;
	string XMLFileName = "Rapidxml\\a.xml";

	// load xml
	rapidxml::file<> fdoc(XMLFileName.c_str());
	cout << fdoc.data() << endl;
	rapidxml::xml_document<> doc;
	doc.parse<0>(fdoc.data());

	// root node
	rapidxml::xml_node<>* root = doc.first_node();

	// remove node
	rapidxml::xml_node<>* node = root->first_node("keyboard");
	if (node != nullptr)
		root->remove_node(node);
	string text = "after remove node:\n";
	rapidxml::print(std::back_inserter(text), doc, 0);
	std::cout << text << std::endl;

	// write DOM to file
	std::ofstream outfile(XMLFileName.c_str(), std::ios::out);
	if (outfile)
	{
		outfile << doc;
		outfile.close();
	}
	doc.clear();
	return false;
}

bool RapidXmlTest::tets_modifynode()
{
	cout << "tets_modifynode()" << endl;
	string XMLFileName = "Rapidxml\\a.xml";

	// load xml
	rapidxml::file<> fdoc(XMLFileName.c_str());
	cout << fdoc.data() << endl;
	rapidxml::xml_document<> doc;
	doc.parse<rapidxml::parse_no_data_nodes>(fdoc.data()); // rapidxml::parse_no_data_nodes

	// root node
	rapidxml::xml_node<>* root = doc.first_node();
	std::cout << root->name() << std::endl;

	std::string str = "http://192.168.0.3";
	root->first_node("host")->value(str.c_str());

	string text = "after modify node:\n";
	rapidxml::print(std::back_inserter(text), doc, 0);
	std::cout << text << std::endl;

	// write DOM to file
	std::ofstream outfile(XMLFileName.c_str(), std::ios::out);
	if (outfile)
	{
		outfile << doc;
		outfile.close();
	}
	doc.clear();
	return false;
}

bool RapidXmlTest::IsXmlNameValid(const std::string& name)
{
	int len = name.length();
	if (len == 0 || name.find('.') == 0 || name.rfind('.') == (len - 1) || name.find("..") != std::string::npos)
	{
		return false;
	}
	const char* chs = name.c_str();
	int cnt = 0;
	for (int i = 0; i < len; i++) {
		if (isalpha(chs[i]) || chs[i] == '_' || chs[i] == '.')
			cnt++;
		else
			break;
	}
	if (cnt == len)
		return true;
	return false;
}

string RapidXmlTest::GetXmlConfVal(const string& name)
{
	cout << "GetXmlConfVal()" << endl;
	string res;
	if (name.size() == 0)
	{
		return res;
	}
	string XMLFileName = "Rapidxml\\a.xml";
	// load xml
	rapidxml::file<> fdoc(XMLFileName.c_str());
	// parse xml
	rapidxml::xml_document<> doc; // character type defaults to char
	doc.parse<0>(fdoc.data());    // 0 means default parse flags
	// root node
	rapidxml::xml_node<>* root = doc.first_node();
	// target node
	rapidxml::xml_node<>* tnode = root->first_node(name.c_str());
	if (tnode != nullptr)
	{
		res = string(tnode->value());
	}
	doc.clear();
	return res;
}

bool RapidXmlTest::SetXmlConfVal(const string& name, const string& value)
{
	cout << "SetXmlConfVal()" << endl;
	if (name.size() == 0 || value.size() == 0)
	{
		return false;
	}
	string XMLFileName = "Rapidxml\\a.xml";
	// load xml
	rapidxml::file<> fdoc(XMLFileName.c_str());
	// parse xml
	rapidxml::xml_document<> doc; // character type defaults to char
	doc.parse<0>(fdoc.data());    // 0 means default parse flags
	// root node
	rapidxml::xml_node<>* root = doc.first_node();
	// node_declaration
	rapidxml::xml_node<>* declaration = doc.allocate_node(rapidxml::node_declaration);
	declaration->append_attribute(doc.allocate_attribute("version", "1.0"));
	declaration->append_attribute(doc.allocate_attribute("encoding", "utf-8"));
	doc.insert_node(root, declaration);
	// target node
	rapidxml::xml_node<>* new_node = doc.allocate_node(rapidxml::node_element, name.c_str(), value.c_str());
	rapidxml::xml_node<>* old_node = root->first_node(name.c_str());
	root->insert_node(old_node, new_node);
	if (old_node != nullptr)
	{
		for (rapidxml::xml_attribute<>* attr = old_node->first_attribute(); attr; attr = attr->next_attribute())
		{
			rapidxml::xml_attribute<>* copy = doc.allocate_attribute(doc.allocate_string(attr->name()), doc.allocate_string(attr->value()));
			new_node->append_attribute(copy);
		}
		root->remove_node(old_node);
	}
	// write DOM to file
	std::ofstream outfile(XMLFileName.c_str(), std::ios::out);
	if (outfile)
	{
		outfile << doc;
		outfile.close();
	}
	doc.clear();
	return true;
}

string RapidXmlTest::GetXmlConfValEx(const std::string& pathname)
{
	cout << "GetXmlConfValEx()" << endl;
	string res;
	if (pathname.size() == 0)
	{
		return res;
	}
	string XMLFileName = "Rapidxml\\a.xml";
	// load xml
	rapidxml::file<> fdoc(XMLFileName.c_str());
	// parse xml
	rapidxml::xml_document<> doc; // character type defaults to char
	doc.parse<0>(fdoc.data());    // 0 means default parse flags
	// root node
	rapidxml::xml_node<>* root = doc.first_node();
	// target node
	rapidxml::xml_node<>* pnode = root;
	rapidxml::xml_node<>* tnode = nullptr;
	vector<string> v;
	Utils::SplitString(pathname, v, ".");
	for (vector<string>::size_type i = 0; i != v.size(); i++)
	{
		tnode = pnode->first_node(v[i].c_str());
		if (tnode == nullptr)
		{
			break;
		}
		pnode = tnode;
	}
	if (tnode != nullptr)
	{
		res = string(tnode->value());
	}
	doc.clear();
	return res;
}

bool RapidXmlTest::SetXmlConfValEx(const string& pathname, const string& value)
{
	cout << "SetXmlConfValEx()" << endl;
	if (pathname.size() == 0 || value.size() == 0)
	{
		return false;
	}
	string XMLFileName = "Rapidxml\\a.xml";
	// load xml
	rapidxml::file<> fdoc(XMLFileName.c_str());
	// parse xml
	rapidxml::xml_document<> doc; // character type defaults to char
	doc.parse<0>(fdoc.data());    // 0 means default parse flags
	// root node
	rapidxml::xml_node<>* root = doc.first_node();
	// node_declaration
	rapidxml::xml_node<>* declaration = doc.allocate_node(rapidxml::node_declaration);
	declaration->append_attribute(doc.allocate_attribute("version", "1.0"));
	declaration->append_attribute(doc.allocate_attribute("encoding", "utf-8"));
	doc.insert_node(root, declaration);
	// target node
	rapidxml::xml_node<>* pnode = root;
	vector<string> v;
	Utils::SplitString(pathname, v, ".");
	for (vector<string>::size_type i = 0; i < v.size() - 1; i++)
	{
		rapidxml::xml_node<>* tnode = pnode->first_node(v[i].c_str());
		if (tnode == nullptr)
		{
			tnode = doc.allocate_node(rapidxml::node_element, v[i].c_str());
			pnode->append_node(tnode);
		}
		pnode = tnode;
	}
	rapidxml::xml_node<>* old_node = pnode->first_node(v[v.size() - 1].c_str());
	rapidxml::xml_node<>* new_node = doc.allocate_node(rapidxml::node_element, v[v.size() - 1].c_str(), value.c_str());
	pnode->insert_node(old_node, new_node);
	if (old_node != nullptr)
	{
		for (rapidxml::xml_attribute<>* attr = old_node->first_attribute(); attr; attr = attr->next_attribute())
		{
			rapidxml::xml_attribute<>* copy = doc.allocate_attribute(doc.allocate_string(attr->name()), doc.allocate_string(attr->value()));
			new_node->append_attribute(copy);
		}
		pnode->remove_node(old_node);
	}
	// write DOM to file
	std::ofstream outfile(XMLFileName.c_str(), std::ios::out);
	if (outfile)
	{
		outfile << doc;
		outfile.close();
	}
	doc.clear();
	return true;
}