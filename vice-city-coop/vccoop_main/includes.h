// GTA VC Classes

#include "plugin_vc.h"
#include "game_vc\CFont.h"
#include "game_vc\CPed.h"
#include "game_vc\CHud.h"
#include "game_vc\CRGBA.h"
#include "game_vc\CHud.h"
#include "game_vc\CPopulation.h"
#include "game_vc\CWorld.h"
#include "game_vc\ePedType.h"
#include "game_vc\CCivilianPed.h"
#include "game_vc\CPad.h"
#include "extensions\ScriptCommands.h"
#include "game_vc\CStreaming.h"
// Some Functions (i use to type faste xD)
void HelpMessageForever(char *text) {
	CHud::SetHelpMessage(text, 0, 1, 0);
}