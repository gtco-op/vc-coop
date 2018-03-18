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
#if defined(_MSC_VER)
			path.append("\\Scripts\\Client\\");
#else
			path.append("./Scripts/Client/");
#endif
		else if (type == TYPE_SERVER_SCRIPT)
#if defined(_MSC_VER)
			path.append("\\Scripts\\Server\\");
#else
			path.append("./Scripts/Server/");
#endif
		path.append(filename);
	}
	else
	{
		path.append(filename);
	}

	CCustomData* cData = nullptr;

	double dataLen = 0;
	char* databuf = { 0 };

	if (type == TYPE_SERVER_SCRIPT)
	{
		std::ostringstream buf; std::ifstream input(path.c_str()); buf << input.rdbuf(); 
		std::string buffer = buf.str();
		if (buffer.empty())
			return 0;

		databuf = new char[buffer.size() + 1];
		dataLen = buffer.size() + 1;
		memcpy(databuf, buffer.c_str(), dataLen);

		cData			= new CCustomData(path, type, databuf, dataLen);
		gGamemodeScript = new CLuaScript(cData);
	}
	else if (type == TYPE_CLIENT_SCRIPT)
	{
		CLuaDumper* gLua = new CLuaDumper(path);
		while (!gLua->GetLuaStatus()) {}

		dataLen = gLua->compiledScriptOutput.size() + sizeof(dataLen);
		databuf = new char[gLua->compiledScriptOutput.size() + sizeof(dataLen)];

		sprintf(databuf, "%f", dataLen);
		memcpy(databuf + sizeof(double), gLua->compiledScriptOutput.c_str(), gLua->GetScript().second);

		gLua->compiledScriptOutput.clear();
	}

	this->InsertItem(cData);
	
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
void CCustomDataManager::LoadScripts()
{
#if defined(_MSC_VER)
	for (auto& p : std::experimental::filesystem::recursive_directory_iterator(GetExecutablePath().append("\\Scripts\\Client")))
#else
	for (auto& p : std::experimental::filesystem::recursive_directory_iterator("./Scripts/Client/"))
#endif
		if (p.path().extension() == std::string(".lua"))
			InsertScript(false, p.path().string().c_str(), TYPE_CLIENT_SCRIPT, p.path());
}