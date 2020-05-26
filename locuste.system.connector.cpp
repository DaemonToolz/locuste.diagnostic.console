#include <iostream>
#include <string.h>
#include <map>
#include <exception>
#include <unistd.h>
#include <thread>  
#include <chrono> 
#include <future>
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

    delete locusteAppPipes;
    deleteMenus();

    return 0;
}

void initPipeMaps(){
    locusteApps = new map<string,int>();
    locusteAppPipes = new map<string,CommunicationPipe>(); 

    (*locusteAppPipes)["locuste.service.brain"] = CommunicationPipe();
    (*locusteAppPipes)["locuste.service.brain"].inputPipe =  "locuste.diagnostic.brain";
    (*locusteAppPipes)["locuste.service.brain"].outputPipe =  "locuste.brain.diagnostic";
    
    (*locusteAppPipes)["locuste.service.osm"] = CommunicationPipe();
    (*locusteAppPipes)["locuste.service.osm"].inputPipe =  "locuste.diagnostic.scheduler";
    (*locusteAppPipes)["locuste.service.osm"].outputPipe =  "locuste.scheduler.diagnostic";
    
}

void refreshPID(){
    (*locusteApps)["locuste.service.brain"] = getProcIdByName("locuste.service.brain");
    (*locusteApps)["locuste.service.osm"] = getProcIdByName("locuste.service.osm");
    (*locusteApps)["locuste.drone.automata"] = getProcIdByName("locuste.drone.automata");
}

void clear() {
    cout << "\x1B[2J\x1B[H";
}

void displayHeader(){
    clear();
    cout << "Application de débuggage et diagnostic locuste.system.connector" << endl << endl;
    cout << "Touche entrée pour valider, Retour arrière pour supprimer la lettre" << endl;
    cout << "Saisie : " << lastCommand  << endl << endl;
}

void displayFooter(){
    cout << "Commandes disponibles pour ce menu : ";
    for (auto& elem : (*availableCommands)[selectedMenuName]) {
        cout << elem.first << " ";
	}
    cout << endl << endl;

    cout << "Menu : ";
    for (auto& elem : (*menuFunction)) {
        cout << (elem.first == selectedMenuName ? "[" : "") << elem.first << (elem.first == selectedMenuName ? "] " : " ");
	}
    cout << endl;
}

void keyboardInput(){
    char lastChar; 
    while(running){
        lastChar = getch();
        
        if(lastChar == '\t') {
            continue;
        }

        if(lastChar == 127 || lastChar == 8) {
            if(lastCommand.length() > 0 ){
                lastCommand.pop_back();
            }
        } else if(lastChar == '\r' || lastChar == '\n' ){
            callCommand();
            lastCommand = "";
        } else {
            lastCommand.push_back(lastChar);
        }
    }
}

