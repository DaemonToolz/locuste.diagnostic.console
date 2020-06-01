#pragma once
#include<string>
#include<map>
#include<vector>
#include<mutex>
#include <iostream>
#include "./pipes/PipeHandler.h"
#include "./models.h"

typedef void (*pfunc)(void);

extern bool running;
extern std::string lastCommand;
extern std::string selectedMenuName;
extern std::map<std::string,int> *locusteApps;
extern std::map<std::string,std::map<std::string, pfunc>> *availableCommands;
extern std::map<std::string,std::string> *helpMenu;
extern std::map<std::string,CommunicationPipe> *locusteAppPipes;
extern std::string selectedHelp;
extern pipes::PipeHandler* DiagnosticPipe;
extern std::mutex pid_lock;

extern std::map<std::string,std::map<std::string, std::string>> *diagnosedSystemOverview;

std::vector<std::string>* screenContent; 
std::mutex sc_lock;

void addScreenContent(const std::string& content){
    sc_lock.lock();
    if(screenContent != nullptr){
        std::string nc = "["+std::to_string(screenContent->size()) + "] - " + content;
        screenContent->push_back(nc);
    }
    sc_lock.unlock();
}

void readScreenContent(){
    sc_lock.lock();
    if(screenContent != nullptr && !screenContent->empty()){
        for(auto& str : (*screenContent)){
            std::cout << str << std::endl;
        }
    }
    sc_lock.unlock();
}

void clearScreenContent(){
    sc_lock.lock();
    if(screenContent != nullptr){
        screenContent->clear();
    }
    sc_lock.unlock();
}