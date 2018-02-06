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
	}
	else {
		gLog->Log("[CConfiguration] %s loaded successfully.\n", this->configFilename.c_str());
		this->configOpened = true;
	}
}
CConfiguration::~CConfiguration()
{

}
void CConfiguration::PopulateValues()
{
	/* Populate configuration values from INI */
	/* Default values specified in config.h */
	gServerNetwork->ServerPort = gConfig->GetReader()->GetInteger("Server", "Port", VCCOOP_DEFAULT_SERVER_PORT);
	
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