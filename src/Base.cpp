#include "Base.hpp"
#include <rtt/transports/corba/TaskContextProxy.hpp>
#include "DependentTask.hpp"

namespace init
{

Base::Base(const std::string &name) : name(name)
{

}

Base::~Base()
{

}

void Base::registerDependency(Base& dependency)
{
    if(&dependency == nullptr)
        throw std::runtime_error("init::Base::registerDependency: Error, tried to register a nullptr on init object with name '" + name + "'");
    
    dependencies.push_back(&dependency);
}

// void Base::registerTask(RTT::TaskContext* task)
// {
//     std::vector< std::string > configs;
//     configs.push_back("default");
// 
//     TaskWithConfig t;
//     t.proxy = task;
//     t.config = configs;
//     t.name = task->getName();
// 
//     allTasks.push_back(t);
// }

void Base::registerTask(DependentTaskBase* task)
{
    if(task == nullptr)
        throw std::runtime_error("init::Base::registerTask: Error, tried to register a nullptr on init object with name '" + name + "'");
        
    TaskWithConfig t(task);
    allTasks.push_back(t);

}

// void Base::registerTask(const std::string name, orocos_cpp::Deployment* deployment)
// {
//     std::vector< std::string > configs;
//     configs.push_back("default");
// 
//     TaskWithConfig t;
//     t.config = configs;
//     t.name = name;
//     t.dpl = deployment;
// 
//     allTasks.push_back(t);
// 
// }

void Base::initProxies()
{
    for(TaskWithConfig &t: allTasks)
    {
        t.proxy = t.task->getProxy();
    }
}

bool Base::connect()
{
    return true;
}

// bool Base::setup()
// {
//     return true;
// }
//

bool Base::applyConfig(orocos_cpp::ConfigurationHelper& confHelper)
{
    for(TaskWithConfig &t: allTasks)
    {
        if(!confHelper.applyConfig(t.proxy, t.task->getConfig()))
        {
            std::string msg("init::Base::Failed to applyConfig [");
            for(const std::string &conf: t.task->getConfig())
                msg += conf + ", ";
            msg += "] for Task " + t.proxy->getName();
            
            std::cout << msg << std::endl;
            
            throw std::runtime_error(msg);
        }
    }
        
    return true;
}

bool Base::setupTransformer(orocos_cpp::TransformerHelper& trHelper)
{
    for(TaskWithConfig &t: allTasks)
    {
        if(!trHelper.configureTransformer(t.proxy))
        {
            throw std::runtime_error("init::Base::Failed to configure transformer for task " + t.proxy->getName());
        }
    }
    return true;
}


bool Base::configure()
{
    for(TaskWithConfig &t: allTasks)
    {
        std::cout << "init::Base::Configuring " << t.proxy->getName() << std::endl;
        if(!t.proxy->configure())
        {
            std::string config = "[";
            for(auto conf: t.task->getConfig())
            {
                config += conf + ", ";
            }
            config += "]";
            throw std::runtime_error("init::Base::Failed to configure task " + t.proxy->getName() + " with configuration " + config);
        }

        std::cout << "init::Base::Configured " << t.proxy->getName() << std::endl;

    }
    return true;
}

bool Base::start()
{
    for(TaskWithConfig &t: allTasks)
    {
        if(!t.proxy->start())
        {
            throw std::runtime_error("init::Base::Failed to start task " + t.proxy->getName());
        }
        std::cout << "init::Base::Started " << t.proxy->getName() << std::endl;

    }

    return true;
}
//
// // void Base::exit()
// // {
// //     msg << "Leaving init state ...\n";
// // };
//
// bool Base::stop()
// {
//     for(TaskWithConfig &t: allTasks)
//     {
//         if(!t.proxy->stop())
//         {
//             throw std::runtime_error("Base::Failed to stop task " + t.proxy->getName());
//         }
//         std::cout << "Base::Stopped " << t.proxy->getName() << std::endl;
//     }
//
//     return true;
// }
}
