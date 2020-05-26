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
    delete menuFunction;
    delete availableCommands;
    delete locusteApps;
    delete DiagnosticPipe;
}


void displayBrainMenu(){
    cout << "Menu de l'unité de contrôle" << endl;
}

void displaySchedulerMenu(){
    cout << "Menu de l'ordonanceur" << endl;
}

void displayMainMenu(){
    cout << "Liste des applications LOCUSTE" << endl;
    for (auto& elem : (*locusteApps)) {
        cout << "[" << elem.first << "] - " << (elem.second > 0 ? " OK": " KO") << ". Pipe " << (locusteAppPipes->find(elem.first) == locusteAppPipes->end()  ? "indisponible" : "disponible" ) <<  endl;
	}
}

void quit(){
    running = false;
}

void open_pipe(){

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
        {"open_pipe", open_pipe},
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



