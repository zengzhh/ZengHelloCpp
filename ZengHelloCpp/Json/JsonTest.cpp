#include "JsonTest.h"
#include "json/json.h"

#include <cstdio>

using namespace std;

void JsonTest::test_parsejson()
{
	//cout << "**** test_parsejson()" << endl;
	//string str = "{\"data\":{\"action\":11,\"strategy\":{\"algorithmkey\":\"ABCDEFG\",\"bindings\":[{\"app\":\"WPS.EXE\",\"ext\":\".doc,.xls,.txt\"},{\"app\":\"WINWORD.EXE\",\"ext\":\".doc\"},{\"app\":\"EXCEL.EXE\",\"ext\":\".xls\"}],\"suffix\":\".doc,.docx,.xls,.xlsx,.ppt,.pptx,.txt,.xmind,.png,.jpg,.jpeg,.tlf\",\"scantype\":1,\"algorithm\":\"AES128\"}},\"signalId\":156,\"command\":12}";
	//Json::Value root;
	//Json::Reader reader;
	//reader.parse(str, root);

	//auto data = root["data"];
	//auto strategy = data["strategy"];
	//auto bindings = strategy["bindings"];
	//auto sbindings = strategy["bindings"].isArray();
	//// json to string
	//Json::FastWriter writer;
	//std::string jsonarray = writer.write(bindings);
	//cout << "sbindings: " << jsonarray << endl;
	//// Array
	//for (Json::Value::ArrayIndex i = 0; i < bindings.size(); i++)
	//{
	//	cout << bindings[i]["app"].asString() << ": " << bindings[i]["ext"].asString() << endl;
	//}

	/*string strParam("{\"type\":103,\"data\":{\"username\":\"Administrator\",\"password\":\"0bfe1427e0ba76a06581acc38f8b3c31\",\"validTmpKey\":\"tmpKeyStr\"}}");
	Json::Reader reader;
	Json::FastWriter writer;
	Json::Value root;
	int code = 1;
	int apiCode = 0;
	string apiMsg = "ok";
	string strData(strParam);
	if (reader.parse(strData, root)) {
		printf("[passwd] parse succ");
		if (root.isMember("data"))
		{
			printf("[passwd] has data");
			Json::Value data = root["data"];
			string username1 = data["username1"].asString();
			printf("[passwd] username1 %s", username1.c_str());
			bool isValid = data.isMember("username") && data.isMember("password");
			if (isValid)
			{
				string username = data["username"].asString();
				string password = data["password"].asString();
			}
		}
	}*/

	std::string data("{}");
	Json::Value root;
	Json::Reader reader;
	if (reader.parse(data, root)) {
		// 不崩溃
		if (root.isObject() && root.isMember("vid"))
		{
			printf("ok");
		}
		// 崩溃
		if (root.isMember("vid"))
		{
			printf("dump");
		}
	}
	printf("end");
}

void JsonTest::test_buildjson()
{
	cout << "**** test_buildjson()" << endl;
	//Json::Value root;
	//Json::FastWriter writer;
	//root["type"] = 101;
	//Json::Value data;
	//data["state"] = 1;
	//data["rate"] = 60;
	//root["data"] = data;
	//std::string json = writer.write(root);
	//printf("json: %s\n", json.c_str());

	string kafkaStr("{\"command\":27,\"data\":{\"type\":2},\"signalId\":5733060}");
	Json::FastWriter writer;
	Json::Reader reader;
	Json::Value root;
	if (reader.parse(kafkaStr, root) && root.isMember("command")) {
		int command = root["command"].asInt();
		int64_t signalId = root["signalId"].asDouble();
		root["receiveTime"] = (int)time(NULL);
		std::string json = writer.write(root);
		printf("json: %s\n", json.c_str());
	}
	cout << "end" << endl;
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
	// forbdList 禁用列表
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

#include <Windows.h>
#define g_info printf
#define g_debug printf
#define g_warning printf

namespace Utility {
	BOOL RegGetDword(const WCHAR* lpSubKey, const WCHAR* lpValueName, DWORD* value) {
		REGSAM samDesired = KEY_ALL_ACCESS;
		g_debug("[RegGetDword] %S %S", lpSubKey, lpValueName);
		DWORD type;
		BYTE data[4];
		DWORD cbData = 4;
		HKEY key;
		BOOL result = false;
		LONG res = RegOpenKeyEx(HKEY_LOCAL_MACHINE, lpSubKey, 0, samDesired, &key);
		if (res == ERROR_SUCCESS) {
			res = RegQueryValueEx(key, lpValueName, NULL, &type, (LPBYTE)data, &cbData);
			if (res == ERROR_SUCCESS) {
				if (type == REG_DWORD)
				{
					*value = *((PDWORD)data);
					g_debug("Value: %lu", *value);
					result = true;
				}
			}
			else {
				g_warning("RegQueryValueEx Error: %d", res);
			}
			RegCloseKey(key);
		}
		else {
			g_warning("RegOpenKeyEx Error: %d", res);
		}
		return result;
	}

	BOOL RegSetDword(const WCHAR* lpSubKey, const WCHAR* lpValueName, DWORD data) {
		REGSAM samDesired = KEY_ALL_ACCESS;
		g_debug("[RegSetDword] %S %S %lu", lpSubKey, lpValueName, data);
		BOOL result = FALSE;
		HKEY hkResult;
		DWORD dwDisposition;
		LONG res = RegCreateKeyEx(HKEY_LOCAL_MACHINE, lpSubKey, 0, NULL, REG_OPTION_NON_VOLATILE, samDesired, NULL, &hkResult, &dwDisposition);
		if (res == ERROR_SUCCESS)
		{
			RegCloseKey(hkResult);
			DWORD cbData = sizeof(data);
			res = RegSetKeyValue(HKEY_LOCAL_MACHINE, lpSubKey, lpValueName, REG_DWORD, &data, cbData);
			if (res == ERROR_SUCCESS)
			{
				g_debug("Success, dwDisposition: %d", dwDisposition);
				result = TRUE;
			}
			else
			{
				g_warning("RegSetKeyValue Error: %d", res);
			}
		}
		else
		{
			g_warning("RegCreateKeyEx: %d, dwDisposition: %d", res, dwDisposition);
		}
		return result;
	}

	const std::wstring Utf8ToUnicode(const std::string& src)
	{
		int n = ::MultiByteToWideChar(CP_UTF8, 0, src.c_str(), -1, nullptr, 0);
		std::vector<wchar_t> temp(n);
		::MultiByteToWideChar(CP_UTF8, 0, src.c_str(), -1, &temp[0], n);
		return &temp[0];
	}
}

void JsonTest::test_ClinkProtect()
{
	string str = "{\"command\":8,\"data\":{\"ClinkProtect\":{\"path\":\"64\",\"name\":\"streamctl\",\"type\":0}}}";
	Json::Value root;
	Json::Reader reader;
	reader.parse(str, root);
	auto data = root["data"];
	if (data.isMember("ClinkProtect"))
	{
		auto ClinkProtect = data["ClinkProtect"];
		if (ClinkProtect.isMember("path") && ClinkProtect.isMember("name") && ClinkProtect.isMember("type"))
		{
			string pathStr = ClinkProtect["path"].asString();
			string nameStr = ClinkProtect["name"].asString();
			int newType = ClinkProtect["type"].asInt();
			string regPath = "SOFTWARE\\ecloudsoft\\Mirror\\ClinkProtect\\RegDlls\\" + pathStr + "\\" + nameStr;
			wstring wRegPath = Utility::Utf8ToUnicode(regPath);
			DWORD dTypeOld = 255, dTypeNew = 255;
			if (Utility::RegGetDword(wRegPath.c_str(), L"type", &dTypeOld))
			{
				Utility::RegSetDword(wRegPath.c_str(), L"type", newType);
				Utility::RegGetDword(wRegPath.c_str(), L"type", &dTypeNew);
				g_info("[ClinkProtect] %s %s type: %d -> %d", pathStr.c_str(), nameStr.c_str(), dTypeOld, dTypeNew);
			}
			else {
				g_info("[ClinkProtect] access %s failed", regPath.c_str());
			}
		}
	}
	Sleep(10000);
}