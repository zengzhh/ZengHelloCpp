#include "JsonTest.h"
#include "json/json.h"

using namespace std;

void JsonTest::test_parsejson()
{
	cout << "**** test_parsejson()" << endl;
	string str = "{\"data\":{\"action\":11,\"strategy\":{\"algorithmkey\":\"ABCDEFG\",\"bindings\":[{\"app\":\"WPS.EXE\",\"ext\":\".doc,.xls,.txt\"},{\"app\":\"WINWORD.EXE\",\"ext\":\".doc\"},{\"app\":\"EXCEL.EXE\",\"ext\":\".xls\"}],\"suffix\":\".doc,.docx,.xls,.xlsx,.ppt,.pptx,.txt,.xmind,.png,.jpg,.jpeg,.tlf\",\"scantype\":1,\"algorithm\":\"AES128\"}},\"signalId\":156,\"command\":12}";
	Json::Value root;
	Json::Reader reader;
	reader.parse(str, root);

	auto data = root["data"];
	auto strategy = data["strategy"];
	auto bindings = strategy["bindings"];
	auto sbindings = strategy["bindings"].isArray();
	// json to string
	Json::FastWriter writer;
	std::string jsonarray = writer.write(bindings);
	cout << "sbindings: " << jsonarray << endl;
	// Array
	for (Json::Value::ArrayIndex i = 0; i < bindings.size(); i++)
	{
		cout << bindings[i]["app"].asString() << ": " << bindings[i]["ext"].asString() << endl;
	}
}

void JsonTest::test_buildjson()
{
	cout << "**** test_buildjson()" << endl;
	Json::Value root;
	Json::FastWriter writer;
	root["type"] = 101;
	Json::Value data;
	data["state"] = 1;
	data["rate"] = 60;
	root["data"] = data;
	std::string json = writer.write(root);
	printf("json: %s\n", json.c_str());
}
