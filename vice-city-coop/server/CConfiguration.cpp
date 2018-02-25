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
	gServerNetwork->ServerSecret = gConfig->GetReader()->GetInteger("Server", "Secret", VCCOOP_DEFAULT_SERVER_SECRET);

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
bool CConfiguration::AutodetectServerGamemode()
{
	bool res = false;

	ServerGamemodePath = gConfig->GetReader()->Get("Server", "Gamemode", "");
	if (ServerGamemodePath.empty()) {
		gLog->Log("[CConfiguration] No user-defined server game mode defined. Auto-detecting.\n");

		for (auto& p : std::experimental::filesystem::recursive_directory_iterator(GetExecutablePath().append("\\scripts\\server")))
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
			gLog->Log("[CConfiguration] Could not auto-detect server game mode.\n");
		}
	}
	else
	{
		ifstream tmp(ServerGamemodePath);
		if (!tmp) {
			gLog->Log("[CConfiguration] Could not open user-defined server game mode. Auto-detecting.\n");

			for (auto& p : std::experimental::filesystem::recursive_directory_iterator(GetExecutablePath().append("\\scripts\\server")))
			{
				if (p.path().extension() == std::string(".lua"))
				{
					CCustomData* temp = gDataMgr->InsertScript(false, p.path().string().c_str(), TYPE_SERVER_SCRIPT, p.path());
					{
						gLog->Log("[CConfiguration] Using %s script for gamemode.\n", temp->GetName().c_str());

						ServerGamemodePath = temp->GetName();
						gLog->Log("[CConfiguration] Attempt to detect server game mode returned successfully.\n");
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
				gLog->Log("[CConfiguration] Could not create CCustomData object for server game mode.\n");
				res = false;
			}
			else {
				res = true;
			}
		}
	}
	gLog->Log("[CConfiguration] Attempt to detect server game mode returned %s\n", (res?"successfully.":"unsuccessfully."));
	return res;
}