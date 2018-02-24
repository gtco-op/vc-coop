#include "server.h"

CCustomDataManager::CCustomDataManager(){}
CCustomDataManager::~CCustomDataManager(){}

CCustomData* CCustomDataManager::InsertScript(bool type_relative, std::string filename, CustomDataType type, std::experimental::filesystem::path path_when_absolute)
{
	int dataLen = 0;
	std::stringstream buffer;
	std::string path;

	if (type_relative)
	{
		path = GetExecutablePath();

		if (type == TYPE_CLIENT_SCRIPT)
			path.append("\\scripts\\client\\");
		else if (type == TYPE_SERVER_SCRIPT)
			path.append("\\scripts\\server\\");
	}
	else
	{
		path.append(filename);
	}
	
	CCustomData* cData = nullptr;
	char* databuf = { 0 };
	std::ifstream t(path);
	buffer << t.rdbuf();

	std::string dataData = buffer.str();
	dataLen = buffer.str().size() + 1;

	databuf = new char[dataLen + sizeof(dataLen)];
	memset(databuf, 0, (dataLen + sizeof(dataLen)));
	sprintf(databuf, "%s", dataData.c_str());

	CLua* gLua = new CLua(filename, databuf, dataLen);
	while (!gLua->GetLuaStatus()) {}
	delete[] databuf;

	databuf = new char[gLua->compiledScriptOutput.size() + sizeof(dataLen)];
	dataLen = gLua->compiledScriptOutput.size();
	
	sprintf(databuf, "%d", dataLen);
	memcpy(databuf + 4, gLua->compiledScriptOutput.c_str(), dataLen);

	if(!type_relative)
		cData = new CCustomData(path_when_absolute.filename().string(), type, databuf, dataLen);
	else
		cData = new CCustomData(filename, type, databuf, dataLen);

	this->InsertItem(cData);
	
	gLua->compiledScriptOutput.clear();
	t.close();
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