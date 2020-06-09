#pragma once
#include<exception>
#include<unordered_map>
namespace modules{

    struct SystemNotFound : public std::exception {
        const char * what () const throw () {
            return "Système inconnu";
        }
    };

    struct SubSystemNotFound : public std::exception {
        const char * what () const throw () {
            return "Sous-système inconnu";
        }
    };


    enum System {
        SUnknown = -1,
        Brain, // BrainSystem
        Scheduler, // MapScheduler
        External // External
    };

    enum SubSystem {
        // Section commune
        SSUnknown = -1,
        FileWatcher, // Watcher
        SocketServer, // SocketServer
        SocketHandler, // SocketHandler
        HttpServer, // HttpServer
        Runner, // MainThread
        RPCServer, // RPCServer
        Diagnostics, // Diagnostics
        // Section dédiée / software
        SchedulerConnection, // SchedulerConnection
        BrainConnection, // BrainConnection
        MapHandler, // MapHandler
        FlightManager, // FlightManager
        // External
        VideoServer, // VideoServer
        VideoStream // VideoStream
    };

    class ModuleStatus  {
        public : 
            std::string RawSys;
            System Sys; 
            std::string RawSubSys;
            SubSystem SubSys;
            bool Running;
    };


    static std::unordered_map<std::string,System> const SystemTable = { 
        {"Brain",System::Brain}, {"Scheduler",System::Scheduler}, {"External",System::External} 
    };

    static std::unordered_map<std::string,SubSystem> const SubSystemTable = { 
        {"FileWatcher",SubSystem::FileWatcher}, {"SocketServer",SubSystem::SocketServer}, {"SocketHandler",SubSystem::SocketHandler} ,
        {"Runner",SubSystem::Runner}, {"RPCServer",SubSystem::RPCServer}, {"Diagnostics",SubSystem::Diagnostics}, {"HttpServer",SubSystem::HttpServer},
        {"SchedulerConnection",SubSystem::SchedulerConnection}, {"BrainConnection",SubSystem::BrainConnection}, {"MapHandler",SubSystem::MapHandler} ,
        {"FlightManager",SubSystem::FlightManager}, {"VideoServer",SubSystem::VideoServer}, {"VideoStream",SubSystem::VideoStream}
    };

    System FindSystem(const std::string& str){
        auto it = SystemTable.find(str);
        if (it != SystemTable.end()) {
            return it->second;
        } else { 
            return System::SUnknown;
        }    
    }
    SubSystem FindSubsystem(const std::string& str){
        auto it = SubSystemTable.find(str);
        if (it != SubSystemTable.end()) {
            return it->second;
        } else { 
            return SubSystem::SSUnknown; 
        }    
    }

}