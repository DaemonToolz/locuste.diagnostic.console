#include <iostream>  
#include "DataParser.h"
#include "enums.h"
#include "../globals.h"
#include <string.h>
#include <utility>
#include <vector>
#include <sstream>
#include "../globals.h"

using namespace modules;
using namespace std;
    
void addScreenContent(const std::string&);

System FindSystem(const std::string&);
SubSystem FindSubsystem(const std::string&);
vector<std::string> split(const string&, const string&) ;

DataParser::DataParser(){  this->_input = "";}
DataParser::~DataParser(){}
DataParser::DataParser(const string& input ){
    this->_input = input;
}

ModuleStatus* DataParser::Output(){
    return this->_output;
}

ModuleStatus* DataParser::Parse(){
    try{
        addScreenContent("Parsage de l'information...");
        if(this->_input.at(this->_input.length()-1)==',')this->_input.pop_back();
        addScreenContent(this->_input);
        if(this->_output != nullptr) delete[] this->_output;
        auto vec = split(this->_input, ",");
        this->_output = new ModuleStatus[this->_available = vec.size()];
        ModuleStatus module;
        int index = 0;
        for(auto sub : vec){
            string moduleStr = sub.substr (sub.find("[") + 1, sub.find("]") - 1);
            string data = moduleStr.substr(0, moduleStr.find("."));
            module.Sys = FindSystem(data);
            module.RawSys = data;
            data = moduleStr.substr(moduleStr.find(".")+1);
            module.SubSys = FindSubsystem(data);
            module.RawSubSys = data;

            istringstream( sub.substr(sub.find(":")+1)) >> std::boolalpha >> module.Running;
            this->_output[index] = module;
            index++;
            addScreenContent("Module répertorié : " + moduleStr);
        }
        return this->_output;
    } catch(exception& ex){
        addScreenContent(ex.what());
        if(this->_output != nullptr) delete[] this->_output;
        this->_available = 0;
        return nullptr;
    }
}

ModuleStatus* DataParser::Parse(const string& input){
    this->_input = input;
    return this->Parse();
}

int DataParser::Available(){
    return this->_available;
}