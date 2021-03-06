#include <iostream>
#include <string.h>
#include <map>
#include <exception>
#include <unistd.h>
#include <thread>  
#include <chrono> 
#include <future>
#include <mutex>
#include "./models.h"
#include "./globals.h"
#include "./menus.cpp"
#include "./pipes/PipeHandler.cpp"
#include "./processFinder.cpp"
using namespace std;


/*
g++ -std=c++17 -Wall -c "locuste.system.connector.cpp"
g++ -std=c++17 -lpthread -Wall -c "locuste.system.connector.cpp" 

g++ -std=c++17  -Wall -o  "locuste.system.connector" "locuste.system.connector.cpp"
g++ -std=c++17  -Wall -lpthread -o  "locuste.system.connector" "locuste.system.connector.cpp"
*/
void initMenus(); // 
void selectMenu(); // 
void displaySelectedMenu(); // 
void displayFooter(); //
void callCommand();
void deleteMenus(); // Suppression des menus
void keyboardInput(); // Sélection du menu / app (A déplacer)
void refreshPID(); // Récupération de l'id du processus (A déplacer)
void initPipeMaps(); // Initialisation des différentes maps (A déplacer)
void clear();
void displayHeader(); // Afficher le menu courant (A déplacer)
int getch();

bool running = true;
string lastCommand;
map<string,int> *locusteApps;
map<string,CommunicationPipe> *locusteAppPipes;

mutex pid_lock;
string selectedHelp = "";

int main (int argc, const char * argv[]) {
    initPipeMaps();
    initMenus();
    future<void> future = async(keyboardInput);
    lastCommand = "";
    try {
        while(running){
            refreshPID();
            displayHeader();
            displaySelectedMenu();
            displayFooter();
            this_thread::sleep_for(chrono::milliseconds(100));
        }
    } catch (exception& ex) {
        cout << "Une erreur est survenue " << endl;
        cout << ex.what() << endl;
    }

    cout << "Arrêt en cours " << endl;
    cout << "Menus supprimés " << endl;
    locusteAppPipes->clear();
    delete locusteAppPipes;
    cout << "Pipes supprimées " << endl;
    screenContent->clear();
    delete screenContent;
    cout << "Journaux vidés " << endl;
    locusteApps->clear();
    delete locusteApps;
    cout << "Applications disponibles supprimées" << endl;
    deleteMenus();
    availableCommands->clear();
    delete availableCommands;
    cout << "Commandes supprimées" << endl;
    menuFunction->clear();
    delete menuFunction;
    cout << "Menu supprimé" << endl;

    return 0;
}

void initPipeMaps(){
    locusteApps = new map<string,int>();
    locusteAppPipes = new map<string,CommunicationPipe>(); 
    screenContent = new vector<string>();
    (*locusteAppPipes)["locuste.service.brain"] = CommunicationPipe();
    (*locusteAppPipes)["locuste.service.brain"].inputPipe =  "/tmp/locuste.diagnostic.brain";
    (*locusteAppPipes)["locuste.service.brain"].outputPipe =  "/tmp/locuste.brain.diagnostic";
    
    (*locusteAppPipes)["locuste.service.osm"] = CommunicationPipe();
    (*locusteAppPipes)["locuste.service.osm"].inputPipe =  "/tmp/locuste.diagnostic.scheduler";
    (*locusteAppPipes)["locuste.service.osm"].outputPipe =  "/tmp/locuste.scheduler.diagnostic";
    
}

void refreshPID(){
    pid_lock.lock();
    if(locusteApps != nullptr){
        (*locusteApps)["locuste.service.brain"] = getProcIdByName("locuste.service.brain");
        (*locusteApps)["locuste.service.osm"] = getProcIdByName("locuste.service.osm");
        (*locusteApps)["locuste.drone.automata"] = getProcIdByName("locuste.drone.automata");
    }
    pid_lock.unlock();
}

void clear() {
    cout << "\x1B[2J\x1B[H";
}

void displayHeader(){
    clear();
    cout << "Application de débuggage et diagnostic locuste.system.connector" << endl << endl;
    
    cout << "Dernière aide : " << selectedHelp << endl<< endl;
    cout << "Touche entrée pour valider, Retour arrière pour supprimer la lettre" << endl;
    cout << "Saisie : " << lastCommand  << endl << endl;
    
}

void displayFooter(){
    cout << "Commandes disponibles pour ce menu : ";
    for (auto& elem : (*availableCommands)[selectedMenuName]) {
        cout << "[" << elem.first << "] ";
	}
    cout << endl << endl;

    cout << "Menu : ";
    for (auto& elem : (*menuFunction)) {
        cout << (elem.first == selectedMenuName ? "[" : "") << elem.first << (elem.first == selectedMenuName ? "] " : " ");
	}
    cout << endl;
}

bool isAuthorized(const char& input){
    return authorizedKeys->find(static_cast<int>(input)) != authorizedKeys->end();
}

void callAuthorized(const char& input){
    auto it = authorizedKeys->find(static_cast<int>(input));
    if(it != authorizedKeys->end() && it->second != nullptr){
        (it->second)();
    }
}

void keyboardInput(){
    char lastChar; 
    while(running){
        lastChar = getch();
        if((lastChar < 32 || lastChar > 127) && !isAuthorized(lastChar)) {
            continue;
        }

        if(isAuthorized(lastChar)){
            callAuthorized(lastChar);
            continue;
        } 

        if(lastChar == 127 || lastChar == 8) {
            if(lastCommand.length() > 0 ){
                lastCommand.pop_back();
            }
        } else {
            lastCommand.push_back(lastChar);
        }
    }
}

