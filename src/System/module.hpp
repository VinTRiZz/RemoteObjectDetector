#ifndef COMPONENTS_PROJECT_MODULE_H
#define COMPONENTS_PROJECT_MODULE_H

// Types
#include <stdint.h>
#include <string>
#include <vector>

// Shared ptr
#include <memory>

// Processor, initer, stopper and worker
#include <functional>

// Async starting
#include <future>


namespace Components
{


// Defines what is this module
enum ModuleTypes : uint16_t
{
    MODULE_TYPE_UNKNOWN,        // Can be used to detect dead modules, etc
    MODULE_TYPE_TEST_MODULE,    // To test work of modules
    MAIN_TIMER_MODULE,          // Main timer module for signals on a time
    COMMISSION_MODULE,          // Commission module
    MOTHERBOARD_MODULE,         // Motherboard module
    CPU_MODULE,                 // CPU module
    GPU_MANAGER_MODULE,         // GPU Manager module (AMD, Nvidia, etc.)
    RAM_MODULE,                 // RAM card manager module
    DRIVE_MODULE,               // HDD, SSD drives module
    NETWORK_MANAGER_MODULE,     // Network manager
    CONNECTOR_MODULE,           // Connector to backend
};


// Enum to describe current status of a module
enum ModuleStatus : int8_t
{
    MODULE_STATUS_ERROR     = -1,   // Not inited, for example
    MODULE_STATUS_INITED    = 0,    // Init function must return this on success
    MODULE_STATUS_RUNNING   = 1,    // Working (see workFunction in configuration)
    MODULE_STATUS_STOPPING  = 2     // Stop is in process
};


// Exit codes of module start() execution
enum ModuleExitCode : int8_t
{
    MODULE_EXIT_CODE_ERROR,
    MODULE_EXIT_CODE_STOPPED,
    MODULE_EXIT_CODE_INTERRUPT,
    MODULE_EXIT_CODE_SUCCESS
};


typedef uint64_t ModuleUid;

// Structure to send messages between modules, see Message typedef
struct MessageStruct
{
    static std::shared_ptr<MessageStruct> create(ModuleUid sender, ModuleUid receiver, const std::string& payload)
    {
        std::shared_ptr<MessageStruct> result = std::make_shared<MessageStruct>(
            MessageStruct()
        );

        result->senderUid = sender;
        result->receiverUid = receiver;
        result->payload = payload;

        return result;
    }

    ModuleUid senderUid;
    ModuleUid receiverUid;
    std::string payload;
};


typedef std::shared_ptr<MessageStruct> Message;
class ModuleClass;
typedef std::shared_ptr<ModuleClass> Module;
typedef std::weak_ptr<ModuleClass> ModuleWeak;


// Setup struct
struct ModuleConfiguration
{

    bool initAsync {false}; // !!! WARNING !!! DEPENDS ON INIT ORDER !!!
                            // Switch to true if module can init async with modules,
                            // added in queue after it

    bool workAsync {true};  // Switch to false if need a thread

    ModuleTypes type;
    std::string name {"Unknown module"}; // Optional

    std::function<ModuleStatus(Module)>     initFunction;
    std::function<ModuleExitCode(Module)>   workFunction;
    std::function<Message(Message)>         inputProcessor;
    std::function<void(Module)>             stopFunction;
};

// Class to process messages between system modules
class ModuleClass
{
private:
    friend struct ModuleConfiguration;

    ModuleWeak m_pSelf; // To work with handlers

    ModuleConfiguration m_config; // Configuration
    ModuleStatus m_status {ModuleStatus::MODULE_STATUS_ERROR}; // Status to detect errors

    // Connections
    std::vector<Module> m_connections;
    std::vector<ModuleUid> m_requiredConnections;
    std::vector<ModuleTypes> m_requiredConnectionTypes;

    // For non-async using
    std::shared_ptr<std::thread> m_workingThread;
    std::mutex m_workingThreadMx;

    // Constructor
    ModuleClass(const ModuleConfiguration& config);

public:
    // Used to create a module (created just to add self pointer)
    static Module createModule(const ModuleConfiguration &config);

    ModuleClass(const ModuleClass& m);
    ModuleClass(ModuleClass&& m);
    ~ModuleClass();


    // Intermodule connection work
    ModuleTypes type() const;
    ModuleUid uid() const; // Actually just a pointer to an object

    // Get module name
    std::string name() const;


    // ModuleClass status work
    ModuleStatus status() const;
    void setStatus(ModuleStatus s);


    // Used in MainApp class
    void addRequiredConnectionUid(ModuleUid _uid);
    void addRequiredConnectionType(ModuleTypes _type);

    // Connections work
    std::vector<Module> connections() const;
    Message process(Message msg);

    // Used in threaded, otherwise useless
    // (locks internal data mutex)
    void lock();
    void unlock();

private:
    friend class MainApp; // To work with methods down here

    // ModuleClass setup and start/stop working
    void init();
    std::future<ModuleStatus> initAsync();
    std::future<ModuleExitCode> startAsync();
    std::shared_ptr<std::thread> startThread();
    void stop();
};

}

#endif // COMPONENTS_PROJECT_MODULE_H
