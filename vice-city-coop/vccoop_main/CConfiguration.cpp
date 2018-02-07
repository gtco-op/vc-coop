#include "main.h"

CConfiguration::CConfiguration()
{
	this->_inih			= nullptr;
	this->configOpened	= false;

	if (this->configFilename.empty())
		this->configFilename = VCCOOP_DEFAULT_CLIENT_CONFIG;
	
	this->_inih = new INIReader(this->configFilename);
	if (this->GetReader()->ParseError() < 0) {
		gLog->Log("[CConfiguration] %s could not be loaded.\n", this->configFilename.c_str());
	}
	else {
		gLog->Log("[CConfiguration] %s loaded successfully.\n", this->configFilename.c_str());
		this->configOpened = true;
	}
}
CConfiguration::~CConfiguration()
{

}
void CConfiguration::PopulateValues(char srvIP[15], int& srvPort, char cliName[25])
{
	/* Populate configuration values from INI */
	/* Default values specified in config.h */
	std::string tmp = this->GetReader()->Get("Server", "ServerAddress", VCCOOP_DEFAULT_SERVER_ADDRESS);
	strcpy(this->ServerAddress, tmp.c_str());

	this->ServerPort = this->GetReader()->GetInteger("Server", "ServerPort", VCCOOP_DEFAULT_SERVER_PORT);

	std::string name = this->GetReader()->Get("Client", "Nickname", VCCOOP_DEFAULT_NICKNAME);
	strcpy(this->Nickname, name.c_str());

	strcpy(srvIP, this->ServerAddress);
	srvPort = this->ServerPort;
	strcpy(cliName, this->Nickname);
	
	gLog->Log("[CConfiguration] Settings loaded from configuration file.\n");
}
std::string CConfiguration::sections(INIReader &reader)
{
	std::stringstream ss;
	std::set<std::string> sections = reader.Sections();
	for (std::set<std::string>::iterator it = sections.begin(); it != sections.end(); ++it)
		ss << *it << ",";
	return ss.str();
}