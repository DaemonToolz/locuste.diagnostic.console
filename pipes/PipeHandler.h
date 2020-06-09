#pragma once

#include "../models.h"
#include "../modules/enums.h"
#include "../modules/DataParser.h"

#include <string.h>
#include <atomic>
#include <future>
#include <mutex>

namespace pipes{

    class PipeHandler {
        public:
            PipeHandler();
            ~PipeHandler();
            bool CreatePipe(const std::string&);
            bool SendCommand();
            void ReadResult();
            bool DeletePipe();

            void SetCommand(const std::string&);
            bool Initialized();
        private:
            modules::DataParser* _parser;

            std::atomic<bool> _initialized;
            std::atomic<bool> _onCommand;
            std::string _command;  
            std::mutex  _cmd_lock;

            int _w_pipe, _r_pipe;
            std::string _appName; 
            CommunicationPipe _pipes;
         

            std::future<void> _w_thread, _r_thread;
            void _displayParsedData();
            void _p_read();
            void _p_write();
            void tryClosePipe(const int&);
            std::string readCommand();
        protected:

            bool OpenPipe();
            bool ClosePipe();

    };

}