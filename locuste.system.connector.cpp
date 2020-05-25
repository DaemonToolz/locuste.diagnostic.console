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

g++ -Wall -o  "locuste.system.connector" "locuste.system.connector.cpp"
g++ -Wall -lpthread -o  "locuste.system.connector" "locuste.system.connector.cpp"
*/
void initMenus(); // Initialisation des menus
void deleteMenus(); // Suppression des menus
static void selectApp(); // Sélection du menu / app (A déplacer)
static void refreshPID(); // Récupération de l'id du processus (A déplacer)
static void initMaps(); // Initialisation des différentes maps (A déplacer)
static void clear();
static void displayMenu(); // Afficher le menu courant (A déplacer)
int getch();

bool running = true;
string lastCommand;
map<string,int> *locusteApps;
map<string,CommunicationPipe> *locusteAppPipes;


int main (int argc, const char * argv[]) {
    initMaps();
    initMenus();
    future<void> future = async(selectApp);
    lastCommand = "";
    try {
        while(running){
            refreshPID();
            displayMenu();
            this_thread::sleep_for(chrono::milliseconds(100));
        }
    } catch (exception& ex) {
        cout << "Une erreur est survenue " << endl;
        cout << ex.what() << endl;
    }

    delete locusteApps;
    delete locusteAppPipes;
    deleteMenus();

    return 0;
}

static void initMaps(){
    locusteApps = new map<string,int>();
    locusteAppPipes = new map<string,CommunicationPipe>(); 

    (*locusteAppPipes)["locuste.service.brain"] = CommunicationPipe();
    (*locusteAppPipes)["locuste.service.brain"].inputPipe =  "locuste.diagnostic.brain";
    (*locusteAppPipes)["locuste.service.brain"].outputPipe =  "locuste.brain.diagnostic";
    
    (*locusteAppPipes)["locuste.service.osm"] = CommunicationPipe();
    (*locusteAppPipes)["locuste.service.osm"].inputPipe =  "locuste.diagnostic.scheduler";
    (*locusteAppPipes)["locuste.service.osm"].outputPipe =  "locuste.scheduler.diagnostic";
    
}

static void refreshPID(){
    (*locusteApps)["locuste.service.brain"] = getProcIdByName("locuste.service.brain");
    (*locusteApps)["locuste.service.osm"] = getProcIdByName("locuste.service.osm");
    (*locusteApps)["locuste.drone.automata"] = getProcIdByName("locuste.drone.automata");
    
}

static void clear() {
    // CSI[2J clears screen, CSI[H moves the cursor to top-left corner
    cout << "\x1B[2J\x1B[H";
}

static void displayMenu(){
    clear();
    cout << "Application de débuggage et diagnostic locuste.system.connector" << endl << endl;

    cout << "Touche entrée pour valider, Retour arrière pour supprimer la lettre" << endl;
    cout << "Saisie : " << lastCommand  << endl << endl;

    cout << "Liste des applications LOCUSTE disponibles " << endl;
    for (auto& elem : (*locusteApps)) {
        cout << "[" << elem.first << "] - " << (elem.second > 0 ? " OK": " KO") << " ( " << (locusteAppPipes->find(elem.first) == locusteAppPipes->end()  ? "INDISPONIBLE" : "DISPONIBLE" )<< " ) " <<  endl;
	}
    cout << "Commandes disponibles pour ce menu: Diagnose [Application], Quit" << endl;
}

static void selectApp(){
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
            if(lastCommand == "Quit"){
                running = false;
            }

            if(lastCommand.rfind("Diagnose", 0) == 0){
                // Changer de menu, à déplacer dans menus.cpp
            }
      
        } else {
            lastCommand.push_back(lastChar);
        }
    }
}

