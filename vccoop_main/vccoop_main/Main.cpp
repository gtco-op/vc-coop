#include "includes.h"

using namespace plugin;

class MyPlugin {
public:
    MyPlugin() {
		if (KeyPressed(VK_F9)){
			CHud::SetHelpMessage(L"~b~ Connecting to Vice City CO-OP \n ~h~ Press ~t~ ENTER  to connect and ~o~ ESC to cancel", 0,1,0);
			if (KeyPressed(VK_RETURN)){ //connects to the server

			}
			else if (KeyPressed(VK_ESCAPE)){

				CHud::SetHelpMessage(L" ", 1, 0, 0); // dismiss HelpMessage

			}
			

		}
        
    }
} myPlugin;