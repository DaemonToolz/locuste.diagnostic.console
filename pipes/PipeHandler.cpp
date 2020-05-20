#include "./PipeHandler.h"
#include <string.h>
using namespace pipes;
PipeHandler::PipeHandler(const std::string& app){
    this->_appName = app;
}

PipeHandler::~PipeHandler(){

}