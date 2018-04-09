#include "server.h"

CConfiguration::CConfiguration()
{
	this->_inih			= nullptr;
	this->configOpened	= false;

	if (this->configFilename.empty())
		this->configFilename = VCCOOP_DEFAULT_SERVER_CONFIG;
	
	this->_inih = new INIReader(this->configFilename);
	if (this->GetReader()->ParseError() < 0) {
		gLog->Log("[CConfiguration] %s could not be loaded.\n", this->configFilename.c_str());
		this->configOpened = false;
	} else {
		gLog->Log("[CConfiguration] %s loaded successfully.\n", this->configFilename.c_str());
		this->configOpened = true;
	}

	char* szPath = new char[MAX_PATH];
	memset(szPath, 0, MAX_PATH);
#if defined(_MSC_VER)
	strcpy(szPath, GetExecutablePath().append("\\Logs").c_str());
#else
	strcpy(szPath, "./Logs");
#endif
	
	std::experimental::filesystem::path szDir = szPath;
	if (!std::experimental::filesystem::create_directory(szDir))
		this->foldersPresent = false;
	else
		this->foldersPresent = true;

	memset(szPath, 0, MAX_PATH);
#if defined(_MSC_VER)
	strcpy(szPath, GetExecutablePath().append("\\Scripts").c_str());
#else
	szPath = new char[MAX_PATH];
	strcpy(szPath, "./Scripts");
#endif
	szDir = szPath;
	if(!std::experimental::filesystem::create_directory(szDir))
		this->foldersPresent = false;
	else
		this->foldersPresent = true;

	memset(szPath, 0, MAX_PATH);
#if defined(_MSC_VER)
	strcpy(szPath, GetExecutablePath().append("\\Scripts\\Client").c_str());
#else
	strcpy(szPath, "./Scripts/Client");
#endif
	szDir = szPath;
	if(!std::experimental::filesystem::create_directory(szDir))
		this->foldersPresent = false;
	else
		this->foldersPresent = true;

	memset(szPath, 0, MAX_PATH);
#if defined(_MSC_VER)
	strcpy(szPath, GetExecutablePath().append("\\Scripts\\Server").c_str());
#else
	strcpy(szPath, "./Scripts/Server");
#endif
	szDir = szPath;
	if(!std::experimental::filesystem::create_directory(szDir))
		this->foldersPresent = false;
	else
		this->foldersPresent = true;

	if (std::experimental::filesystem::status_known(std::experimental::filesystem::file_status{}) ? 
		std::experimental::filesystem::exists(std::experimental::filesystem::file_status{}) : 
		std::experimental::filesystem::exists(szDir))	{
		this->foldersPresent = true;
	} 
	
	memset(szPath, 0, MAX_PATH);
#if defined(_MSC_VER)
	strcpy(szPath, GetExecutablePath().append("\\Scripts\\Client").c_str());
#else
	strcpy(szPath, "./Scripts/Client");
#endif
	szDir = szPath;
	if (std::experimental::filesystem::status_known(std::experimental::filesystem::file_status{}) ?
		std::experimental::filesystem::exists(std::experimental::filesystem::file_status{}) :
		std::experimental::filesystem::exists(szDir)) {
		this->foldersPresent = true;
	}

	memset(szPath, 0, MAX_PATH);
#if defined(_MSC_VER)
	strcpy(szPath, GetExecutablePath().append("\\Scripts").c_str());
#else
	strcpy(szPath, "./Scripts");
#endif
	szDir = szPath;
	if (std::experimental::filesystem::status_known(std::experimental::filesystem::file_status{}) ?
		std::experimental::filesystem::exists(std::experimental::filesystem::file_status{}) :
		std::experimental::filesystem::exists(szDir)) {
		this->foldersPresent = true;
	}

	memset(szPath, 0, MAX_PATH);
#if defined(_MSC_VER)
	strcpy(szPath, GetExecutablePath().append("\\Logs").c_str());
#else
	strcpy(szPath, "./Logs");
#endif
	szDir = szPath;
	if (std::experimental::filesystem::status_known(std::experimental::filesystem::file_status{}) ?
		std::experimental::filesystem::exists(std::experimental::filesystem::file_status{}) :
		std::experimental::filesystem::exists(szDir)) {
		this->foldersPresent = true;
	}

	delete[] szPath;
}
void CConfiguration::PopulateValues()
{
	/* Populate configuration values from INI */
	/* Default values specified in config.h */
	gServerNetwork->ServerPort = gConfig->GetReader()->GetInteger("Server", "Port", VCCOOP_DEFAULT_SERVER_PORT);
	gServerNetwork->ServerSecret = gConfig->GetReader()->GetInteger("Server", "Secret", VCCOOP_DEFAULT_SERVER_SECRET);
}
std::string CConfiguration::sections(INIReader &reader)
{
	std::stringstream ss;
	std::set<std::string> sections = reader.Sections();
	for (std::set<std::string>::iterator it = sections.begin(); it != sections.end(); ++it)
		ss << *it << ",";
	return ss.str();
}
bool CConfiguration::AutodetectServerGamemode()
{
	bool res = false;

	ServerGamemodePath = gConfig->GetReader()->Get("Server", "Gamemode", "");
	if (ServerGamemodePath.empty()) {
#ifdef VCCOOP_VERBOSE_LOG
		gLog->Log("[CConfiguration][WARNING] No user-defined server game mode defined. Auto-detecting.\n");
#endif


#if defined(_MSC_VER)
		for (auto& p : std::experimental::filesystem::recursive_directory_iterator(GetExecutablePath().append("\\Scripts\\Server")))
#else
		for (auto& p : std::experimental::filesystem::recursive_directory_iterator("./Scripts/Server/"))
#endif
		{
			if (p.path().extension() == std::string(".lua"))
			{
				CCustomData* temp = gDataMgr->InsertScript(false, p.path().string().c_str(), TYPE_SERVER_SCRIPT, p.path());
				{
					gLog->Log("[CConfiguration] Using %s script for gamemode.\n", temp->GetName().c_str());

					ServerGamemodePath = temp->GetName();

					res = true;
				}
			}
		}
		if (!res)
		{
			gLog->Log("[CConfiguration][ERROR] Could not auto-detect server game mode.\n");
		}
	}
	else
	{
		ifstream tmp(ServerGamemodePath);
		if (!tmp) {
			gLog->Log("[CConfiguration][ERROR] Could not open user-defined server game mode. Auto-detecting.\n");

#if defined(_MSC_VER)
			for (auto& p : std::experimental::filesystem::recursive_directory_iterator(GetExecutablePath().append("\\Scripts\\Server")))
#else
			for (auto& p : std::experimental::filesystem::recursive_directory_iterator("./Scripts/Server/"))
#endif
			{
				if (p.path().extension() == std::string(".lua"))
				{
					CCustomData* temp = gDataMgr->InsertScript(false, p.path().string().c_str(), TYPE_SERVER_SCRIPT, p.path());
					
					{
						ServerGamemodePath = temp->GetName();

						if(!ServerGamemodePath.empty())
							gLog->Log("[CConfiguration] Loaded gamemode successfully.\n");
						else {
							gLog->Log("[CConfiguration] Unable to load user-defined gamemode.\n");
							return false;
						}
						return true;
					}
				}
			}
			res = false;
		}
		else if(res == false)
		{
			CCustomData* temp = gDataMgr->InsertScript(false, ServerGamemodePath, TYPE_SERVER_SCRIPT);
			if (!temp)
			{
				gLog->Log("[CConfiguration][ERROR] Could not create CCustomData object for server game mode.\n");
				res = false;
			}
			else {
				res = true;
			}
		}
	}
#ifdef VCCOOP_VERBOSE_LOG
	gLog->Log("[CConfiguration] Auto-detection of gamemode was %s\n", (res?"successful.":"unsuccessful."));
#endif
	return res;
}
unsigned int CConfiguration::CRC32(unsigned char *message) {
	int i, j;

	unsigned int byte, crc;

	i = 0;
	crc = 0xFFFFFFFF;
	while (message[i] != 0) {
		byte = message[i];    // Get next byte.

		crc = crc ^ byte;

		for (j = 7; j >= 0; j--) {
			crc = (crc >> 1) ^ (crc & 1 ? 0xEDB88320 : 0);
		}
		i = i + 1;
	}
	return ~crc;
}