#pragma once

#include <string.h>

namespace pipes{
    class PipeHandler {
        public:
            PipeHandler();
            ~PipeHandler();
            bool CreatePipe();
            bool SendCommand();
            void ReadResult();
        private:
            std::string _appName; 

            bool OpenPipe();
            bool ClosePipe();
            void OnError();
        protected:
    };

}