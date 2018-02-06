#pragma once
class CConfiguration
{
public:
	CConfiguration();
	~CConfiguration();

	/* General Server Settings */
	int ServerPort;
	
	void PopulateValues();

	INIReader* GetReader() { return _inih; }
	bool IsConfigLoaded() { return configOpened; }

private:
	INIReader* _inih;
	
	bool configOpened;
	
	std::string configFilename;

	std::string sections(INIReader &reader);
};

