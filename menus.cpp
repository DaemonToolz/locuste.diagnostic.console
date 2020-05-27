#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <vector>
#include <string>
#include <map>
#include <sstream>
#include <iterator>
#include "./globals.h"
#include "./pipes/PipeHandler.h"
using namespace std; 
using namespace pipes;


void displayBrainMenu();
void displaySchedulerMenu();
void displayMainMenu(); 

void addScreenContent(const string& content);
void readScreenContent();
void clearScreenContent();
typedef void (*pfunc)(void);

map<string, pfunc> *menuFunction;
string selectedMenuName = "main";
map<string,map<string, pfunc>> *availableCommands;

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

//void selectMenus(const CommunicationPipe& pipes);


void selectMenu(){
    istringstream iss(lastCommand);
    vector<std::string> parsedMenu(istream_iterator<string>{iss}, istream_iterator<string>());

    if(parsedMenu.size() > 1 && menuFunction->find(parsedMenu[1]) != menuFunction->end()){
        selectedMenuName = parsedMenu[1];
    }
}

void displaySelectedMenu(){
    (*menuFunction)[selectedMenuName]();
}

void deleteMenus(){
    DiagnosticPipe->DeletePipe();
    cout << "Pipe fermée" << endl;
    delete DiagnosticPipe;
    cout << "Pipe supprimée" << endl;

    delete availableCommands;
    cout << "Commandes supprimées" << endl;
    delete locusteApps;
    cout << "Applications disponibles supprimées" << endl;
    delete menuFunction;
    cout << "Menu supprimé" << endl;

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

void list_modules(){
    if(DiagnosticPipe->Initialized()){
        DiagnosticPipe->SetCommand(lastCommand);
    }  
}


void initMenus(){
    DiagnosticPipe = new PipeHandler();
    menuFunction = new map<string, pfunc>();

    (*menuFunction)["main"] = displayMainMenu;
    (*menuFunction)["locuste.service.brain"] = displayBrainMenu;
    (*menuFunction)["locuste.service.osm"] = displaySchedulerMenu;
    (*menuFunction)["locuste.drone.automata"] = displayMainMenu;
    
    availableCommands = new map<string, map<string, pfunc>>();
    (*availableCommands)["main"] = {
        {"goto", selectMenu},
        {"quit", quit}
    };
    (*availableCommands)["locuste.service.brain"] = {
        {"goto", selectMenu},
        {"connect", open_pipe},
        {"disconnect", close_pipe},
        {"modules", list_modules},
        {"clear", clearScreenContent},
    
        {"quit", quit}
    };
    (*availableCommands)["locuste.service.osm"] = {
        {"goto", selectMenu},
        {"quit", quit}
    };
    (*availableCommands)["locuste.drone.automata"] = {
        {"goto", selectMenu},
        {"quit", quit}
    };
}


void callCommand(){
    for (auto& elem : (*availableCommands)[selectedMenuName]) {
        if(lastCommand.rfind(elem.first, 0) == 0){
            (elem.second)();
            break;
        }
   }
}



