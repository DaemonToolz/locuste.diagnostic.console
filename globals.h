#pragma once
#include<string>
#include<map>
#include "./models.h"

extern bool running;
extern std::string lastCommand;
extern std::map<std::string,int> *locusteApps;
extern std::map<std::string,CommunicationPipe> *locusteAppPipes;

