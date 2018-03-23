#include "main.h"

CConfiguration::CConfiguration()
{
	this->_inih				= nullptr;
	this->configOpened		= false;
	this->configPopulated	= false;

	if (this->configFilename.empty())
		this->configFilename = VCCOOP_DEFAULT_CLIENT_CONFIG;
	
	this->_inih = new INIReader(this->configFilename);
	if (this->GetReader()->ParseError() < 0) {
		gLog->Log("[CConfiguration] %s could not be loaded. Creating default config file.\n", this->configFilename.c_str());

		FILE* f = fopen(VCCOOP_DEFAULT_CLIENT_CONFIG, "w");
		if (f)		{
			char buffer[256];
			sprintf(buffer, "[Client]\nNickname=" VCCOOP_DEFAULT_NICKNAME "\nChatTimestamp=false\n[Server]\nServerAddress=" VCCOOP_DEFAULT_SERVER_ADDRESS "\nServerPort=%d", VCCOOP_DEFAULT_SERVER_PORT);
			fputs(buffer, f);
			fclose(f);
			gLog->Log("[CConfiguration] Created default config file.\n");
			this->configError = false;
		} else {
			gLog->Log("[CConfiguration] Failed to create default config file.\n");
			this->configError = true;
		}
	}
	else {
		gLog->Log("[CConfiguration] %s loaded successfully.\n", this->configFilename.c_str());
		this->configOpened = true;
	}

	char* szPath = new char[MAX_PATH];
	memset(szPath, 0, MAX_PATH);
	strcpy(szPath, GetExecutablePath().append("\\vccoop").c_str());
	std::experimental::filesystem::path szDir = szPath;

	std::string path(GetExecutablePath().append("\\vccoop"));
	if (std::experimental::filesystem::create_directory(path.c_str())) {
		this->configError = false;
	}
	if (std::experimental::filesystem::status_known(std::experimental::filesystem::file_status{}) ?
		std::experimental::filesystem::exists(std::experimental::filesystem::file_status{}) :
		std::experimental::filesystem::exists(szPath)) {
		this->configError= false;
	}

	delete[] szPath;
}
CConfiguration::~CConfiguration()
{
	gLog->Log("[CConfiguration] CConfiguration shutting down\n");
}
void CConfiguration::PopulateValues(char srvIP[15], int& srvPort, char cliName[25])
{
	if (configPopulated)
		return;

	/* Populate configuration values from INI */
	/* Default values specified in config.h */
	std::string tmp = this->GetReader()->Get("Server", "ServerAddress", VCCOOP_DEFAULT_SERVER_ADDRESS);
	strcpy(this->ServerAddress, tmp.c_str());

	this->ServerPort = this->GetReader()->GetInteger("Server", "ServerPort", VCCOOP_DEFAULT_SERVER_PORT);

	std::string name = this->GetReader()->Get("Client", "Nickname", VCCOOP_DEFAULT_NICKNAME);
	strcpy(this->Nickname, name.c_str());

	bool chattimestamp = this->GetReader()->GetBoolean("Client", "ChatTimestamp", false);
		
	strcpy(srvIP, this->ServerAddress);
	srvPort = this->ServerPort;
	strcpy(cliName, this->Nickname);
	this->DisplayChatTimestamp = chattimestamp;
	
	gLog->Log("[CConfiguration] Settings loaded from configuration file.\n");

	configPopulated = true;
}
std::string CConfiguration::sections(INIReader &reader)
{
	std::stringstream ss;
	std::set<std::string> sections = reader.Sections();
	for (std::set<std::string>::iterator it = sections.begin(); it != sections.end(); ++it)
		ss << *it << ",";
	return ss.str();
}