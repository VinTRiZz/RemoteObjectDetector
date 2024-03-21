#include "System/module.hpp"

Components::ModuleClass::ModuleClass(const Components::ModuleConfiguration &config) :
    m_config {config}
{

}

Components::Module Components::ModuleClass::createModule(const Components::ModuleConfiguration &config)
{
    Module result;
    result = Module(
        new ModuleClass(config),
        std::default_delete<ModuleClass>()
    );

    result->m_pSelf = result;
    return result;
}

Components::ModuleClass::ModuleClass(const Components::ModuleClass &m) :
    m_config{m.m_config},
    m_pSelf{m.m_pSelf}
{

}

Components::ModuleClass::ModuleClass(Components::ModuleClass &&m) :
    m_config{static_cast<decltype(m_config)&&>(m.m_config)},
    m_pSelf{m.m_pSelf}
{

}

Components::ModuleClass::~ModuleClass()
{
    // Do something on detele if needed
}

Components::ModuleTypes Components::ModuleClass::type() const
{
    return m_config.type;
}

Components::ModuleUid Components::ModuleClass::uid() const
{
    return reinterpret_cast<ModuleUid>(m_pSelf.lock().get());
}

std::string Components::ModuleClass::name() const
{
    return m_config.name;
}

Components::ModuleStatus Components::ModuleClass::status() const
{
    return m_status;
}

void Components::ModuleClass::setStatus(Components::ModuleStatus s)
{
    m_status = s;
}

void Components::ModuleConfiguration::addRequiredConnectionUid(ModuleUid _uid)
{
    for (auto uid : requiredConnections)
    {
        if (uid == _uid)
            return;
    }
    requiredConnections.push_back(_uid);
}

void Components::ModuleConfiguration::addRequiredConnectionType(ModuleTypes _type)
{
    for (auto t : requiredConnectionTypes)
    {
        if (t == _type)
            return;
    }

    for (auto uid : requiredConnections)
    {
        if (reinterpret_cast<Components::ModuleClass*>(uid)->type() == _type)
            return;
    }

    requiredConnectionTypes.push_back(_type);
}

Components::Message Components::ModuleClass::sendToModuleUid(Components::ModuleUid _uid, const std::string& msg)
{
    Message messageToSend = MessageStruct::create(this->uid(), _uid, msg);
    for (auto con : m_connections)
    {
        if (con->uid() == _uid)
            return con->process(messageToSend);
    }
    return Message();
}

Components::Message Components::ModuleClass::sendToModuleType(Components::ModuleTypes _type, const std::string& msg)
{
    Message messageToSend = MessageStruct::create(this->uid(), 0, msg);
    for (auto con : m_connections)
    {
        if (con->type() == _type)
        {
            messageToSend->receiverUid = con->uid();
            return con->process(messageToSend);
        }
    }
    return Message();
}

std::vector<Components::Module> Components::ModuleClass::connections() const
{
    return m_connections;
}

Components::Message Components::ModuleClass::process(Components::Message msg)
{
    if (m_config.inputProcessor)
        return m_config.inputProcessor(msg);
    return {};
}

void Components::ModuleClass::init()
{
    if (m_config.initFunction)
        setStatus(m_config.initFunction(m_pSelf.lock()));
}

std::future<Components::ModuleStatus> Components::ModuleClass::initAsync()
{
    if (m_config.initFunction)
        return std::async(m_config.initFunction, m_pSelf.lock());
    return {};
}

std::shared_ptr<std::thread> Components::ModuleClass::startThread()
{
    if (m_config.workFunction)
        return std::shared_ptr<std::thread>(
                    new std::thread(m_config.workFunction, m_pSelf.lock()),
                    [](std::thread * pThread)
                    {
                        if (pThread->joinable())
                            pThread->join();
                        delete pThread;
                    }
                );
    return {};
}

std::future<Components::ModuleExitCode> Components::ModuleClass::startAsync()
{
    if (m_config.workFunction)
        return std::async(m_config.workFunction, m_pSelf.lock());
    return {};
}

void Components::ModuleClass::stop()
{
    if (m_config.stopFunction)
        m_config.stopFunction(m_pSelf.lock());
}



void Components::ModuleClass::lock()
{
    m_workingThreadMx.lock();
}

void Components::ModuleClass::unlock()
{
    m_workingThreadMx.unlock();
}



void Components::ModuleClass::sleep_us(uint64_t time)
{
    std::this_thread::sleep_for(std::chrono::microseconds(time));
}

void Components::ModuleClass::sleep_ms(uint64_t time)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(time));
}

void Components::ModuleClass::sleep_s(uint64_t time)
{
    std::this_thread::sleep_for(std::chrono::seconds(time));
}
