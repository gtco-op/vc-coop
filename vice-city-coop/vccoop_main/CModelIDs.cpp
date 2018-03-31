#ifdef VCCOOP_SERVER
#include "..\server\server.h"
#else
#include "main.h"
#endif

const int CModelIDs::objModelIDs[MAX_OBJ_MODEL_ID] = { 300,301,302,303,304,305,306,307,308,309,310,311,312,313,314,315,316,317,318,319,320,321,322,323,324,330,331,332,333,334,335,336,337,338,339,340,341,342,343,344,345,346,347,348,349,350,351,352,353,354,355,356,357,358,359,360,361,362,363,364,365,366,367,368,369,370,371,372,373,374,375,376,377,378,379,380,381,382,383,384,385,386,387,388,389,390,391,392,393,394,395,396,397,398,399,400,401,402,403,404,405,406,407,408,409,410,411,412,413,414,415,416,417,418,419,420,421,422,423,424,425,426,427,428,429,430,431,432,440,441,442,443,444,445,446,447,448,449,450,451,452,453,454,455,456,457,458,459,460,461,462,463,464,465,466,467,468,469,470,471,472,473,474,475,476,477,478,500,501,502,503,504,505,506,507,508,509,510,511,512,513,514,515,516,517,518,519,520,521,522,523,524,525,526,527,528,529,530,531,532,533,534,535,536,537,538,539,540,541,542,543,544,545,546,547,548,549,550,551,552,553,554,555,556,557,558,559,560,561,562,563,564,565,566,567,568,569,570,571,572,573,574,575,576,577,578,579,580,581,582,583,584,587,588,589,590,591,592,593,594,595,596,597,598,599,600,601,602,603,604,605,606,607,608,609,611,613,614,615,617,620,633,634,635,636,637,638,640 };
const int CModelIDs::pedModelIDs[MAX_PED_MODEL_ID] = { 1,2,3,4,5,6,7,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106 };
const int CModelIDs::vehModelIDs[MAX_VEH_MODEL_ID] = { 130,131,132,133,134,135,136,137,138,139,140,141,142,143,144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,160,161,162,163,164,166,167,168,169,170,171,172,173,174,175,176,177,178,179,182,183,184,185,186,187,188,189,190,191,192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,208,209,210,211,212,213,214,215,216,217,218,219,220,221,222,223,224,225,226,227,228,229,230,231,232,233,234,235,236 };
const int CModelIDs::vehColorIDs[MAX_VEH_COLOR_ID] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94 };

int CModelIDs::GetWeaponModelFromType(int weaponID)
{
	switch (weaponID)
	{
		case 0:	return 293;
		case 1: return 259;
		case 2: return 260;
		case 3: return 261;
		case 4: return 262;
		case 5: return 263;
		case 6: return 264;
		case 7: return 265;
		case 8: return 266;
		case 9: return 267;
		case 10:return 268;
		case 11:return 269;
		case 12:return 270;
		case 13:return 291;
		case 14:return 271;
		case 15:return 272;
		case 16:return 273;
		case 17:return 274;
		case 18:return 275;
		case 19:return 277;
		case 20:return 278;
		case 21:return 279;
		case 22:return 281;
		case 23:return 282;
		case 24:return 283;
		case 25:return 284;
		case 26:return 280;
		case 27:return 276;
		case 28:return 285;
		case 29:return 286;
		case 30:return 287;
		case 31:return 288;
		case 32:return 289;
		case 33:return 290;
		case 36:return 292;
	}
	return -1;
}
bool CModelIDs::IsValidObjectModel(int modelID)
{
	return (std::find(std::begin(objModelIDs), std::end(objModelIDs), modelID) != std::end(objModelIDs));
}
bool CModelIDs::IsValidPedModel(int modelID)
{
	return (std::find(std::begin(pedModelIDs), std::end(pedModelIDs), modelID) != std::end(pedModelIDs));
}
bool CModelIDs::IsValidVehicleModel(int modelID)
{
	return (std::find(std::begin(vehModelIDs), std::end(vehModelIDs), modelID) != std::end(vehModelIDs));
}
int CModelIDs::GetRandomModel(eModelType type)
{
	srand(time(NULL));

	if (type == MODEL_OBJ)
	{
		return objModelIDs[rand() % MAX_OBJ_MODEL_ID];
	}
	else if (type == MODEL_PED)
	{
		return pedModelIDs[rand() % MAX_PED_MODEL_ID];
	}
	else
	{
		return vehModelIDs[rand() % MAX_VEH_MODEL_ID];
	}
}
int CModelIDs::GetRandomVehicleColor()
{
	srand(time(NULL));

	return vehColorIDs[rand() % MAX_VEH_COLOR_ID];
}