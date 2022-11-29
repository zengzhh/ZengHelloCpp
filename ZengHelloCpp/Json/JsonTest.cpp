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

void JsonTest::test_parsearray()
{
	cout << "**** test_parsejson()" << endl;
	//string str = "{\"code\":0,\"data\":{\"mode\":2,\"softwareControlPanelReportInterval\":24,\"forbdList\":[{\"md5\":\"fa644ceaeb815541188ce65eff536505\",\"name\":\"1.exe\"},{\"md5\":\"091d1f2a7c9d53914c841045868c2de7\",\"name\":\"MemoryAnalyzer.exe\"}],\"forbdRuleIds\":\"387,381\",\"switch\":1}}";
	string str = "{\"code\":0,\"data\":{\"mode\":2,\"softwareControlPanelReportInterval\":24,\"blocklist\":[{\"md5\":\"2a644ceaeb815541188ce65eff536505\",\"name\":\"2.exe\"}],\"forbdList\":[{\"md5\":\"fa644ceaeb815541188ce65eff536505\",\"name\":\"1.exe\"},{\"md5\":\"091d1f2a7c9d53914c841045868c2de7\",\"name\":\"MemoryAnalyzer.exe\"}],\"forbdRuleIds\":\"387,381\",\"switch\":1}}";
	Json::Value root;
	Json::Reader reader;
	Json::FastWriter w;
	reader.parse(str, root);
	auto data = root["data"];
	Json::Value newblocklist;
	Json::Value jvblocklist = data["blocklist"];
	if (jvblocklist.size() > 0)
	{
		newblocklist = jvblocklist;
	}
	printf("blocklist size is %d\n", newblocklist.size());
	// forbdList Ω˚”√¡–±Ì
	if (data.isMember("forbdList") && data["forbdList"].isArray()) {
		Json::Value _forbdListJV = data["forbdList"];
		printf("forbdList size is %d\n", _forbdListJV.size());
		for (Json::Value::ArrayIndex i = 0; i < _forbdListJV.size(); i++) {
			Json::Value temp = _forbdListJV[i];
			newblocklist.append(temp);
		}
		auto strData = w.write(newblocklist);
		printf("now block list size is %d, %s\n", newblocklist.size(), strData.c_str());
	}
}
