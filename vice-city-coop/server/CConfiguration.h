#pragma once
class CConfiguration
{
public:
	CConfiguration();
	~CConfiguration();

	/* Populates values from config file */
	void PopulateValues();

	/* Retrieves INI reader instance */
	INIReader* GetReader() { return _inih; }

	/* Returns TRUE if config file loaded successfully */
	bool IsConfigLoaded() { return configOpened; }

private:
	INIReader* _inih;
	
	bool configOpened;
	
	std::string configFilename;

	std::string sections(INIReader &reader);
};

