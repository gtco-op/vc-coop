#include "server.h"

CCustomData::CCustomData(std::string szName, CustomDataType data_type, char* data_ptr, size_t data_size, std::string dir)
{
	if (szName.empty())
	{
		return;
	}
	if (data_size <= 0)	
	{
		gLog->Log("[CCustomData] Failed to initialize CCustomData object. Reason: data_size <= 0 (%d)\n", data_size);
		return;
	}
	else
	{
		pDataSize = data_size;
	}
	if (!dir.empty())
	{
		szDir = dir;
	}

	this->szName = szName;
	cType		 = data_type;
	pData		 = new char[pDataSize];

	memset(pData, 0x00, pDataSize);
	memcpy(pData, data_ptr, pDataSize);

	gLog->Log("[CCustomData] Name: %s\t Type: %s\tData Size: %d\n", szName.c_str(), gDataMgr->GetCustomDataTypeString(cType).c_str(), pDataSize);
}
CCustomData::~CCustomData()
{

}