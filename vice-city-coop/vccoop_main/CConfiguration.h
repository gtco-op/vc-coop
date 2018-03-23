#pragma once
class CConfiguration
{
public:
	/* Server IP Address */
	char ServerAddress[15];

	/* Server Port */
	int ServerPort;

	/* Client Nickname */
	char Nickname[25];

	/* Chat Timestamp Display */
	bool DisplayChatTimestamp;
	
	/* Populates values from INI file */
	void PopulateValues(char IP[15], int& Port, char Name[25]);
	
	/* Retrieves the INI reader instance */
	INIReader* GetReader() { return _inih; }

	/* Returns TRUE if config has been loaded successfully */
	bool IsConfigLoaded() { return configOpened; }

	/* Returns TRUE if there are no errors with the configuration.
	   This includes whether the vccoop directory has been created or
	   the config was unreadable or otherwise unable to be populated. */
	bool IsDataDirValid() { return !configError; }

public:
	CConfiguration();
	~CConfiguration();

private:
	INIReader* _inih;

	bool configError;
	
	bool configOpened;

	bool configPopulated;
	
	std::string configFilename;

	std::string sections(INIReader &reader);
};

