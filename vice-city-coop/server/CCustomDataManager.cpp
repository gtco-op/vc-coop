#include "server.h"

CCustomDataManager::CCustomDataManager(){}
CCustomDataManager::~CCustomDataManager(){}

CCustomData* CCustomDataManager::InsertScript(bool type_relative, std::string filename, CustomDataType type, std::experimental::filesystem::path path_when_absolute)
{
	std::string path;

	if (type_relative)
	{
		path = GetExecutablePath();

		if (type == TYPE_CLIENT_SCRIPT)
			path.append("\\scripts\\client\\");
		else if (type == TYPE_SERVER_SCRIPT)
			path.append("\\scripts\\server\\");

		path.append(filename);
	}
	else
	{
		path.append(filename);
	}
	
	CCustomData* cData = nullptr;
	CLua* gLua = new CLua(path);
	while (!gLua->GetLuaStatus()) {}
	
	double dataLen = gLua->compiledScriptOutput.size() + sizeof(dataLen);
	char* databuf = new char[gLua->compiledScriptOutput.size() + sizeof(dataLen)];
	sprintf(databuf, "%f", dataLen);
	memcpy(databuf + sizeof(double), gLua->compiledScriptOutput.c_str(), gLua->GetScript().second);

	cData = new CCustomData(path, type, databuf, dataLen);
	this->InsertItem(cData);
	
	gLua->compiledScriptOutput.clear();
	delete[] databuf;

	return cData;
}
CCustomData* CCustomDataManager::GetItemByName(std::string name)
{
	for (auto i : cItems)	{
		if (strstr(i->GetName().c_str(), name.c_str()))
			return i;
	}
	return nullptr;
}
std::string CCustomDataManager::GetCustomDataTypeString(CustomDataType type)
{
	switch (type)
	{
	case TYPE_CLIENT_SCRIPT_DATA:
		return std::string("Client Script Data");
	case TYPE_SERVER_SCRIPT:
		return std::string("Server Script");
	case TYPE_CLIENT_SCRIPT:
		return std::string("Client Script");
	case TYPE_SCM:
		return std::string("SCM File");
	case TYPE_MISC:
		return std::string("Misc. File");
	}
	return std::string("");
}