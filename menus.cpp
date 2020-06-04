#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <vector>
#include <string>
#include <map>
#include <sstream>
#include <iterator>
#include <algorithm>
#include <regex>
#include <stdlib.h> 
#include <sys/types.h>


#include <signal.h>
#include "./globals.h"
#include "./pipes/PipeHandler.h"
using namespace std; 
using namespace pipes;

void close_pipe();
void help();
void clearAndCall();
void callCommand();
void displayBrainMenu();
void displaySchedulerMenu();
void displayMainMenu(); 
void cursorUp();
void cursorDown();
void addScreenContent(const string& content);
void readScreenContent();
void clearScreenContent();

typedef void (*pfunc)(void);

map<int, pfunc>* authorizedKeys;
map<string, pfunc> *menuFunction;
string selectedMenuName = "main";
map<string,map<string, pfunc>> *availableCommands;
map<string,string> *helpMenu;

PipeHandler* DiagnosticPipe; 


int getch(void){
  struct termios oldt,newt;
  int ch;
  tcgetattr( STDIN_FILENO, &oldt );
  newt = oldt;
  newt.c_lflag &= ~( ICANON | ECHO );
  tcsetattr( STDIN_FILENO, TCSANOW, &newt );
  ch = getchar();
  tcsetattr( STDIN_FILENO, TCSANOW, &oldt );
  return ch;
}

vector<string> splitInput(const string input){
    istringstream iss(lastCommand);
    vector<std::string> parsedMenu(istream_iterator<string>{iss}, istream_iterator<string>());
    return parsedMenu;
}

std::vector<std::string> split(const string& input, const string& regex) {
    std::regex re(regex);
    std::sregex_token_iterator
        first{input.begin(), input.end(), re, -1},
        last;
    return {first, last};
}

void selectMenu(){
    auto parsedMenu = splitInput(lastCommand);
    if(parsedMenu.size() > 1 && menuFunction->find(parsedMenu[1]) != menuFunction->end()){
        selectedMenuName = parsedMenu[1];
    }
}

void displaySelectedMenu(){
    (*menuFunction)[selectedMenuName]();
}

void deleteMenus(){
    close_pipe();
    delete DiagnosticPipe;
    cout << "Pipe supprimée" << endl;
    delete authorizedKeys;
    cout << "Raccourcis supprimés"  << endl;
}


void displayBrainMenu(){
    if(running){
        cout << "Menu de l'unité de contrôle" << endl;
        cout << "Début du journal d'événement" << endl;
        readScreenContent();
        cout << "Fin du journal" << endl << endl;
    }
}

void displaySchedulerMenu(){
    cout << "Menu de l'ordonanceur" << endl;

   
}

void displayMainMenu(){
    if(running){
        cout << "Liste des applications LOCUSTE" << endl;
        pid_lock.lock();
        if(locusteApps != nullptr){
            for (auto& elem : (*locusteApps)) {
                cout << "[" << elem.first << "] - " << (elem.second > 0 ? " OK": " KO") << ". Pipe " << (locusteAppPipes->find(elem.first) == locusteAppPipes->end()  ? "indisponible" : "disponible" ) <<  endl;
            }
        }
        pid_lock.unlock();

        cout << endl << "Début du journal d'événement" << endl;
        readScreenContent();
        cout << "Fin du journal" << endl << endl;
    }
}

void quit(){
    running = false;
}

void open_pipe(){
    addScreenContent("Tentative de connexion");
    if(DiagnosticPipe->CreatePipe(selectedMenuName)){
        addScreenContent(("Succès de la connexion à " + selectedMenuName));
    } else {
        addScreenContent(("Echec de la connexion à " +  selectedMenuName));
    }
}

void close_pipe(){
    if(DiagnosticPipe->DeletePipe()){
        addScreenContent(("Succès de la déconnexion de " + selectedMenuName));
    } else {
        addScreenContent(("Echec de la déconnexion de " +  selectedMenuName));
    }
}

void sendAction(){
    if(DiagnosticPipe->Initialized()){
        DiagnosticPipe->SetCommand(lastCommand);
    }  
}


void startProcess(){
    auto parsedCommand = splitInput(lastCommand);
    
    if(parsedCommand.size() > 1){
        auto proc = locusteApps->find(parsedCommand[1]);
        if(proc != locusteApps->end() && proc->second < 0){
            try {
                addScreenContent(("Tentative de démarrage de l'application " + proc->first));
                std::stringstream ss;
                // Le nom correspond au nom de projet + nom de l'exé (qui sont les mêmes)
                ss << "/home/pi/Documents/locuste/locuste/" << proc->first << "/" << proc->first << "&";
                // Find a way to spawn a new independant process
            
                ss.str(std::string());
            } catch (exception& ex){
                addScreenContent( ex.what());
            }
        }
    }
}

void stopProcess(){
    auto parsedCommand = splitInput(lastCommand);
    
    if(parsedCommand.size() > 1){
        auto proc = locusteApps->find(parsedCommand[1]);
        if(proc != locusteApps->end() && proc->second > 0){
            try{
                addScreenContent(("Arrêt en cours " + proc->first));
                kill(proc->second, SIGINT);
                addScreenContent(("Arrêt réussi " + proc->first));
            } catch (exception& ex){
                addScreenContent(ex.what());
            }
        }
    }
}

void initMenus(){
    DiagnosticPipe = new PipeHandler();
    menuFunction = new map<string, pfunc>();
    helpMenu = new map<string,string>();
    authorizedKeys = new map<int, pfunc>();

    (*menuFunction)["main"] = displayMainMenu;
    (*menuFunction)["locuste.service.brain"] = displayBrainMenu;
    (*menuFunction)["locuste.service.osm"] = displaySchedulerMenu;
    (*menuFunction)["locuste.drone.automata"] = displayMainMenu;
    
    availableCommands = new map<string, map<string, pfunc>>();
    (*availableCommands)["main"] = {
        {"goto", selectMenu},
        {"help", help},
        {"clear", clearScreenContent},
        {"start", startProcess},
        {"stop", stopProcess},
        {"quit", quit}
    };
    (*availableCommands)["locuste.service.brain"] = {
        {"goto", selectMenu},
        {"help", help},
        {"connect", open_pipe},
        {"disconnect", close_pipe},
        {"module", sendAction},
        {"clear", clearScreenContent},
        {"quit", quit}
    };
    (*availableCommands)["locuste.service.osm"] = {
        {"goto", selectMenu},
        {"help", help},
        {"quit", quit}
    };
    (*availableCommands)["locuste.drone.automata"] = {
        {"goto", selectMenu},
        {"help", help},
        {"quit", quit}
    };

    (*helpMenu)["quit"] = "Quitter l'application";
    (*helpMenu)["clear"] = "Nettoyer les journaux d'événements";
    (*helpMenu)["goto"] = "Sélectionner un menu, ex: goto main, goto locuste.service.brain";
    (*helpMenu)["connect"] = "Se connecter à l'application sélectionnée";
    (*helpMenu)["disconnect"] = "Se déconnecter à l'application sélectionnée";
    (*helpMenu)["module"] = "Choisir une action sur un module (list, start, stop, restart)";
    (*helpMenu)["start"] = "Permet de démarrer une des applications listée dans le menu principal";
    (*helpMenu)["stop"] = "Permet de stopper une des applications listée dans le menu principal";
    

    (*authorizedKeys)[static_cast<int>('\n')] = clearAndCall;
    (*authorizedKeys)[static_cast<int>('+')] = cursorDown;
    (*authorizedKeys)[static_cast<int>('-')] = cursorUp;
    
}

void clearAndCall(){
    callCommand();
    lastCommand = "";
}

void callCommand(){
    for (auto& elem : (*availableCommands)[selectedMenuName]) {
        if(lastCommand.rfind(elem.first, 0) == 0){
            (elem.second)();
            break;
        }
   }
}


void help(){
    auto helpVector = split(lastCommand, " ");
    if(helpVector.size() > 1){
        selectedHelp = (*helpMenu)[helpVector[1]];
    }
}
