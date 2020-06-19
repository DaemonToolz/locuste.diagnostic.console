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

            bool Parse(const std::string&);
            bool Parse();

            void ExtractPipeMessage();
        protected:
        private:
            std::string _input;
            std::string _r_input;
            
            ModuleStatus* _output;
            int _available;

            bool _modstt;
            int _msg_sz; 
    };

}