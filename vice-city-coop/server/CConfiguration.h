#pragma once
class CConfiguration
{
public:
	CConfiguration();
	~CConfiguration() {};

	/* Autodetect Server Game Mode when none is defined */
	bool AutodetectServerGamemode();

	/* Populates values from config file */
	void PopulateValues();

	/* Retrieves INI reader instance */
	INIReader* GetReader() { return _inih; }

	/* Returns TRUE if config file loaded successfully */
	bool IsConfigLoaded() { return configOpened; }

	/* Returns TRUE if log & script folders are present */
	bool IsDirsValid() { return foldersPresent; }

private:
	INIReader* _inih;
	
	bool configOpened;

	bool foldersPresent;
	
	std::string configFilename;

	std::string sections(INIReader &reader);
};

