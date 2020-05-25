#include <stdio.h>
#include <termios.h>
#include <unistd.h>

#include <string>
#include <map>
using namespace std; 

void displayBrainMenu();
void displaySchedulerMenu();

typedef void (*pfunc)(void);
map<string, pfunc> *menuFunction;

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

void selectMenus(const CommunicationPipe& pipes){
    
}

void initMenus(){
    menuFunction = new map<string, pfunc>();
    (*menuFunction)["main"] = displayMainMenu;
    (*menuFunction)["locuste.service.brain"] = displayBrainMenu;
    (*menuFunction)["locuste.service.osm"] = displaySchedulerMenu;
}

void deleteMenus(){
    delete menuFunction;
}

void displayBrainMenu(){

}

void displaySchedulerMenu(){

}