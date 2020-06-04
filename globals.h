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
extern std::map<int, pfunc>* authorizedKeys;

std::vector<std::string>* screenContent; 
std::mutex sc_lock;
std::mutex ln_lock;

uint minLine = 0,
    maxLine = minLine + 20;

void cursorDown();
void cursorUp();
void addScreenContent(const std::string& content){
    sc_lock.lock();
    if(screenContent != nullptr){
        std::string nc = "["+std::to_string(screenContent->size()) + "] - " + content;
        screenContent->push_back(nc);
        cursorDown();
    }
    sc_lock.unlock();
}

void readScreenContent(){
    sc_lock.lock();
    if(screenContent != nullptr && !screenContent->empty()){
        auto localIndex = maxLine >= screenContent->size() ? (uint)screenContent->size():maxLine;
        auto localMin = minLine >= screenContent->size() || minLine < 0 ? 0 : minLine;
        for(; localMin < localIndex; ++localMin){
            std::cout << (*screenContent)[localMin] << std::endl;
        }
     
    }
    sc_lock.unlock();
}

void clearScreenContent(){
    sc_lock.lock();
    if(screenContent != nullptr){
        screenContent->clear();
        minLine = 0;
        maxLine = minLine + 20;
    }
    sc_lock.unlock();
}

void cursorDown(){
    ln_lock.lock();

    if(screenContent != nullptr && minLine < (uint)screenContent->size() - 21){
        minLine++;
        maxLine++;
    }

    ln_lock.unlock();
}

void cursorUp(){
    ln_lock.lock();
  
    if(minLine > 0){
        minLine--;
        maxLine--;
    }

    ln_lock.unlock();
}