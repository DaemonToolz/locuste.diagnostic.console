#pragma once
#include "enums.h"
#include <string.h>
#include <utility>
namespace modules{

    class DataParser {
        public:
            DataParser();
            DataParser(const std::string&);
            virtual ~DataParser();
            int           Available();
            ModuleStatus* Output();
            ModuleStatus* Parse(const std::string&);
            ModuleStatus* Parse();
        protected:
        private:
            std::string _input;
            ModuleStatus* _output;
            int _available;

    };

}