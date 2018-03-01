#pragma once
class CCustomDataManager
{
public:
	CCustomDataManager();
	~CCustomDataManager();

	void						LoadScripts();

	std::vector<CCustomData*>	GetItems() { return cItems; }
	CCustomData*				GetItemAt(int index) { return cItems.at(index); }
	CCustomData*				GetItemByName(std::string name);

	int							InsertItem(CCustomData* cData) { cItems.push_back(cData); return (cItems.size() - 1); }
	void						RemoveItem(CCustomData* cData) { cItems.erase(std::remove(cItems.begin(), cItems.end(), cData), cItems.end()); }
	void						RemoveItemAt(int index) { cItems.erase(cItems.begin() + index); }

	CCustomData*				InsertScript(bool type_relative, std::string filename, CustomDataType type, std::experimental::filesystem::path path_when_absolute = std::experimental::filesystem::path());

	std::string					GetCustomDataTypeString(CustomDataType type);

protected:
	std::vector<CCustomData*> cItems;
};

