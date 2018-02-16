#pragma once


#ifdef _DEBUG

#define INIT_CONSOLE(fh) AllocConsole(); SetConsoleTitleA("GWCAComm Server Debug"); freopen_s(&fh,"CONOUT$","w",stdout)
#define KILL_CONSOLE(fh) fclose(fh); FreeConsole();
#define DBG(msg,...) printf(msg"\n",__VA_ARGS__)
#define CMD_DBG(cmd,param_format,...) printf("<CMD = %s> Args - "param_format,#cmd,__VA_ARGS__)

#else

#define INIT_CONSOLE(fh)
#define CONSOLE_DBG(msg,...)
#define CMD_DBG(cmd,param_format,...)
#define DBG(msg,...)
#define KILL_CONSOLE(fh)

#endif