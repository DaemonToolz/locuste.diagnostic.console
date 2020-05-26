#pragma once
#include<string>
#include<map>
#include "./pipes/PipeHandler.h"
#include "./models.h"

typedef void (*pfunc)(void);

extern bool running;
extern std::string lastCommand;
extern std::string selectedMenuName;
extern std::map<std::string,int> *locusteApps;
extern std::map<std::string,std::map<std::string, pfunc>> *availableCommands;
extern std::map<std::string,CommunicationPipe> *locusteAppPipes;

extern pipes::PipeHandler* DiagnosticPipe;