#include "./PipeHandler.h"
#include "../modules/enums.h"
#include "../modules/DataParser.cpp"

#include "../globals.h"
#include <string.h>
#include <atomic>
#include <future>

#include <stdio.h> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <mutex>
#include <chrono>
#include <thread>

using namespace pipes;
using namespace std;
using namespace modules;

void addScreenContent(const std::string&);
void readScreenContent();

PipeHandler::PipeHandler(){
    this->_parser = new DataParser();
}

PipeHandler::~PipeHandler(){
    delete this->_parser;
}

bool PipeHandler::CreatePipe(const string &name){
    pid_lock.lock();
    
    if(locusteApps == nullptr || ((*locusteApps)[name]) <= 0){
        pid_lock.unlock();
        return false;
    }
    pid_lock.unlock();
    this->_appName = name;
    this->ClosePipe();
    this->_initialized = false;
    addScreenContent("Recherche des pipes...");
    if(locusteAppPipes->find(name) != locusteAppPipes->end()){
        addScreenContent("Information trouvée...");
        this->_initialized = true;
        this->_pipes = (*locusteAppPipes)[name];
        this->_r_thread = async(launch::async,&PipeHandler::_p_read, this);
        this->_w_thread = async(launch::async,&PipeHandler::_p_write, this);
    
    }
    return  this->OpenPipe();

}


bool PipeHandler::OpenPipe(){
    if(this->_initialized){
        struct stat st; 
        int result = stat(this->_pipes.outputPipe.c_str(), &st);
        
        if(result == 0){
            mkfifo(this->_pipes.outputPipe.c_str(), 0666); 
            addScreenContent("Pipe sortante créée");
        }

    
        result = stat(this->_pipes.inputPipe.c_str(), &st);
        if(result == 0){
            mkfifo(this->_pipes.inputPipe.c_str(), 0666); 
            addScreenContent("Pipe entrante créée");
        }
   
        addScreenContent("Pipes OK...");
        return true;
    }
    addScreenContent("Pipes KO...");
    return false;
}



bool PipeHandler::ClosePipe(){
    addScreenContent("Fermeture des pipes");
    if(this->_initialized){
        this->_initialized = false;
        return true;
    }
    return false;
}

bool PipeHandler::DeletePipe(){
     return this->ClosePipe();
}



void PipeHandler::tryClosePipe(const int& ref){
    try{
        close(ref);
    } catch (exception& _){

    }
}

void PipeHandler::_displayParsedData(){
    ModuleStatus* output = this->_parser->Output();
    if(output != nullptr){
        for(auto index = 0; index < this->_parser->Available(); ++index){
            ostringstream os;
            os << "[" << output[index].Sys << "." << output[index].SubSys << "]." 
                            << output[index].RawSys << " > " << output[index].RawSubSys << " : " 
                            <<  (output[index].Running  ? "En cours d'exécution" : "Arrêté");
            addScreenContent(os.str());
        }
    }
}

// Cette section cause le freeze
void PipeHandler::_p_read(){
    addScreenContent("Démarrage de la lecture du flux entrant");
    while(this->_initialized){
        this->_r_pipe = open(this->_pipes.inputPipe.c_str(), O_RDONLY);// | O_NONBLOCK); 
        try {
            char buffer[1024]; 
            int st = read(this->_r_pipe, buffer, 1024); 
            if(st > 0){
                string output = buffer;
                
                if(output.length() > 0){
                    bool r = this->_parser->Parse(output);
                    if(r) this->_displayParsedData();
                } 
            }
            this->tryClosePipe(this->_r_pipe);
            this_thread::sleep_for(chrono::milliseconds(100));
        } catch(exception& ex){
            this->tryClosePipe(this->_r_pipe);
            break;
        }
    }
    addScreenContent("Arrêt de la lecture du flux entrant");
}


void PipeHandler::_p_write(){
    addScreenContent("Démarrage de l'écriture du flux sortant");
    while(this->_initialized){
       if(this->_onCommand){
            this->_w_pipe = open(this->_pipes.outputPipe.c_str(), O_WRONLY);

            addScreenContent("Envoi de la commande");
            string cmdCopy = this->readCommand();
            try{
                write(this->_w_pipe, cmdCopy.c_str(), cmdCopy.length());
                this->SetCommand(""); 
                addScreenContent(("Commande " + cmdCopy + " envoyée"));
            } catch(exception& ex){
                this->tryClosePipe(this->_w_pipe);
                break;
            } 
            this->tryClosePipe(this->_w_pipe);
       } 
        this_thread::sleep_for(chrono::milliseconds(100));
       
    }
    addScreenContent("Arrêt de l'écriture du flux sortant");
    this->SetCommand("");
}

string PipeHandler::readCommand(){
    const std::lock_guard<std::mutex> lock(this->_cmd_lock);
    return this->_command;
}

void PipeHandler::SetCommand(const string& cmd){
    const std::lock_guard<std::mutex> lock(this->_cmd_lock);
    this->_command = cmd;

    if(cmd != ""){
        this->_onCommand = true ;
    } else {
        this->_onCommand = false;
    }
}

bool PipeHandler::Initialized(){
    return this->_initialized;
}