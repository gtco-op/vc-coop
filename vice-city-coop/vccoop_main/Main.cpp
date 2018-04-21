#include "main.h"
#include <d3d9.h>

CCore *core = nullptr;
sStartParams gStartParams;


/*************************************************************************
* CommandLineToArgvA            [SHELL32.@]
*
* MODIFIED FROM https://www.winehq.org/ project
* We must interpret the quotes in the command line to rebuild the argv
* array correctly:
* - arguments are separated by spaces or tabs
* - quotes serve as optional argument delimiters
*   '"a b"'   -> 'a b'
* - escaped quotes must be converted back to '"'
*   '\"'      -> '"'
* - consecutive backslashes preceding a quote see their number halved with
*   the remainder escaping the quote:
*   2n   backslashes + quote -> n backslashes + quote as an argument delimiter
*   2n+1 backslashes + quote -> n backslashes + literal quote
* - backslashes that are not followed by a quote are copied literally:
*   'a\b'     -> 'a\b'
*   'a\\b'    -> 'a\\b'
* - in quoted strings, consecutive quotes see their number divided by three
*   with the remainder modulo 3 deciding whether to close the string or not.
*   Note that the opening quote must be counted in the consecutive quotes,
*   that's the (1+) below:
*   (1+) 3n   quotes -> n quotes
*   (1+) 3n+1 quotes -> n quotes plus closes the quoted string
*   (1+) 3n+2 quotes -> n+1 quotes plus closes the quoted string
* - in unquoted strings, the first quote opens the quoted string and the
*   remaining consecutive quotes follow the above rule.
*/
LPSTR* WINAPI CommandLineToArgvA(LPSTR lpCmdline, int* numargs)
{
	DWORD argc;
	LPSTR  *argv;
	LPSTR s;
	LPSTR d;
	LPSTR cmdline;
	int qcount, bcount;

	if (!numargs || *lpCmdline == 0)
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return NULL;
	}

	/* --- First count the arguments */
	argc = 1;
	s = lpCmdline;
	/* The first argument, the executable path, follows special rules */
	if (*s == '"')
	{
		/* The executable path ends at the next quote, no matter what */
		s++;
		while (*s)
			if (*s++ == '"')
				break;
	}
	else
	{
		/* The executable path ends at the next space, no matter what */
		while (*s && *s != ' ' && *s != '\t')
			s++;
	}
	/* skip to the first argument, if any */
	while (*s == ' ' || *s == '\t')
		s++;
	if (*s)
		argc++;

	/* Analyze the remaining arguments */
	qcount = bcount = 0;
	while (*s)
	{
		if ((*s == ' ' || *s == '\t') && qcount == 0)
		{
			/* skip to the next argument and count it if any */
			while (*s == ' ' || *s == '\t')
				s++;
			if (*s)
				argc++;
			bcount = 0;
		}
		else if (*s == '\\')
		{
			/* '\', count them */
			bcount++;
			s++;
		}
		else if (*s == '"')
		{
			/* '"' */
			if ((bcount & 1) == 0)
				qcount++; /* unescaped '"' */
			s++;
			bcount = 0;
			/* consecutive quotes, see comment in copying code below */
			while (*s == '"')
			{
				qcount++;
				s++;
			}
			qcount = qcount % 3;
			if (qcount == 2)
				qcount = 0;
		}
		else
		{
			/* a regular character */
			bcount = 0;
			s++;
		}
	}

	/* Allocate in a single lump, the string array, and the strings that go
	* with it. This way the caller can make a single LocalFree() call to free
	* both, as per MSDN.
	*/
	argv = (LPSTR*)LocalAlloc(LMEM_FIXED, (argc + 1) * sizeof(LPSTR) + (strlen(lpCmdline) + 1) * sizeof(char));
	if (!argv)
		return NULL;
	cmdline = (LPSTR)(argv + argc + 1);
	strcpy(cmdline, lpCmdline);

	/* --- Then split and copy the arguments */
	argv[0] = d = cmdline;
	argc = 1;
	/* The first argument, the executable path, follows special rules */
	if (*d == '"')
	{
		/* The executable path ends at the next quote, no matter what */
		s = d + 1;
		while (*s)
		{
			if (*s == '"')
			{
				s++;
				break;
			}
			*d++ = *s++;
		}
	}
	else
	{
		/* The executable path ends at the next space, no matter what */
		while (*d && *d != ' ' && *d != '\t')
			d++;
		s = d;
		if (*s)
			s++;
	}
	/* close the executable path */
	*d++ = 0;
	/* skip to the first argument and initialize it if any */
	while (*s == ' ' || *s == '\t')
		s++;
	if (!*s)
	{
		/* There are no parameters so we are all done */
		argv[argc] = NULL;
		*numargs = argc;
		return argv;
	}

	/* Split and copy the remaining arguments */
	argv[argc++] = d;
	qcount = bcount = 0;
	while (*s)
	{
		if ((*s == ' ' || *s == '\t') && qcount == 0)
		{
			/* close the argument */
			*d++ = 0;
			bcount = 0;

			/* skip to the next one and initialize it if any */
			do {
				s++;
			} while (*s == ' ' || *s == '\t');
			if (*s)
				argv[argc++] = d;
		}
		else if (*s == '\\')
		{
			*d++ = *s++;
			bcount++;
		}
		else if (*s == '"')
		{
			if ((bcount & 1) == 0)
			{
				/* Preceded by an even number of '\', this is half that
				* number of '\', plus a quote which we erase.
				*/
				d -= bcount / 2;
				qcount++;
			}
			else
			{
				/* Preceded by an odd number of '\', this is half that
				* number of '\' followed by a '"'
				*/
				d = d - bcount / 2 - 1;
				*d++ = '"';
			}
			s++;
			bcount = 0;
			/* Now count the number of consecutive quotes. Note that qcount
			* already takes into account the opening quote if any, as well as
			* the quote that lead us here.
			*/
			while (*s == '"')
			{
				if (++qcount == 3)
				{
					*d++ = '"';
					qcount = 0;
				}
				s++;
			}
			if (qcount == 2)
				qcount = 0;
		}
		else
		{
			/* a regular character */
			*d++ = *s++;
			bcount = 0;
		}
	}
	*d = '\0';
	argv[argc] = NULL;
	*numargs = argc;

	return argv;
}

sStartParams GetParams()
{
	int argc;
	
	LPSTR * argv = CommandLineToArgvA(GetCommandLineA(), &argc);
	sStartParams params;

	for (int i = 1; i < argc; i++)		{
		// example custom URI usage:
		//		vccoop:host=localhost;port=420;
		if (strstr(argv[i], "vccoop:"))		{
			bool autoconnect = false;
			char host[64];
			int port = 0;

			// Host
			std::string webParams = argv[i];
			size_t pos = webParams.find_first_of(';');
			webParams.erase(webParams.begin() + pos, webParams.end());
			sscanf(webParams.c_str(), "vccoop:host=%s", &host);
			
			// Port
			webParams = argv[i];
			pos = webParams.find_first_of(';');
			webParams.erase(webParams.begin(), webParams.begin() + pos);
			webParams.erase(webParams.begin(), webParams.begin() + 1);
			sscanf(webParams.c_str(), "port=%d;", &port);

			params.bWebRequest	= true;
			params.bEmpty		= false;
			params.bConnect		= true;
			params.serverport	= port;

			sprintf(params.serveraddress, "%s", host);
		}
		else if (strstr(argv[i], "-name") && (i+1)<=argc)		{
			sprintf(params.name, "%s", argv[i + 1]);
			params.bEmpty = false;
		}
		else if (strstr(argv[i], "-host") && (i + 1) <= argc) {
			sprintf(params.serveraddress, "%s", argv[i + 1]);
			params.bEmpty = false;
		}
		else if (strstr(argv[i], "-port") && (i + 1) <= argc) {
			params.serverport = atoi(argv[i + 1]);
			params.bEmpty = false;
		}
	}

	LocalFree(argv);
	return params;
}
void await_load_thread(LPVOID* lParams)
{
	bool bLoaded = false;

	if ((sStartParams*)lParams != nullptr) {
		sStartParams* params = (sStartParams*)lParams;
		while (!bLoaded) {
			if (gGame->bLoadingDone && !params->bEmpty) {
				
				gGame->Name = params->name;
				strcpy(Nickname, params->name);
				sprintf(gConfig->Nickname, "%s", params->name);
				
				sprintf(gConfig->ServerAddress, "%s", params->serveraddress);
				sprintf(gNetwork->ServerAddress, "%s", params->serveraddress);
				strcpy(IP, params->serveraddress);
				
				gNetwork->ServerPort = params->serverport;
				gConfig->ServerPort = params->serverport;
				Port = params->serverport;

				bLoaded = true;
			}
			Sleep(VCCOOP_CLIENT_TICK_DELAY);
		}
	}

	if (gLog)
		gLog->Log("[CCore] await_load_thread finished.\n");
}

void CreateLoadThread(sStartParams params)
{
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&await_load_thread, &params, NULL, NULL);
}
void ApplyStartParams(sStartParams params)
{
	gStartParams = params;
	CreateLoadThread(params);
}


class VCCoop 
{
public:
	VCCoop() 
	{
		sStartParams params = GetParams();
		ApplyStartParams(params);

		core = new CCore();
	}
	~VCCoop() 
	{	
		//delete core;
	}
} vcCoop;