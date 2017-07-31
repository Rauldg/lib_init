#include "InitHelper.hpp"
#include <orocos_cpp/Spawner.hpp>
#include <orocos_cpp/PluginHelper.hpp>
#include <orocos_cpp/LoggingHelper.hpp>
#include "log_replay/ReplayTask.hpp"
#include "log_replay/ReplayPort.hpp"
#include <rock_replay/ReplayHandler.hpp>
#include <rock_replay/ReplayGUI.h>
#include <typelib/pluginmanager.hh>

InitHelper::InitHelper(orocos_cpp::TransformerHelper& trHelper, orocos_cpp::ConfigurationHelper& confHelper) :
    trHelper(trHelper),
    confHelper(confHelper),
    loggingActive(false)
{

}

bool InitHelper::start(init::Base& toStart)
{
    std::vector<orocos_cpp::Deployment *> startedDpl;
    startDeploymentRecursive(toStart, startedDpl);

    orocos_cpp::Spawner &spawner(orocos_cpp::Spawner::getInstace());

    spawner.waitUntilAllReady(base::Time::fromSeconds(5.0));

    std::vector<init::Base *> started;

    startTasksRecursive(toStart, started);

    if(loggingActive)
    {
        orocos_cpp::LoggingHelper lHelper;
    
        lHelper.logTasks(excludeList);
    }
    
    return true;
}

bool InitHelper::start(init::Base& toStart, int argc, char** argv)
{
    Typelib::PluginManager::self manager;
    ReplayHandler *replay = new ReplayHandler();

    QApplication a(argc, argv);
    ReplayGui gui;
    
    startReplayRecursive(toStart, *replay);
    
    replay->loadStreams(argc, argv, ReplayHandler::WHITELIST);

    start(toStart);

    //note, the gui takes ownership of the replay handler
    gui.initReplayHandler(replay, "CommonReplay");
    
    gui.updateTaskView();
    gui.show();    
    
    return a.exec();    
}


template <class T>
bool contains(T *searched, const std::vector<T *> &vec)
{
    for(const T *t: vec)
    {
        if(t == searched )
            return true;
    }
    
    return false;
}


void InitHelper::activateLogging(const std::vector< std::string >& excludeList)
{
    this->excludeList = excludeList;
    loggingActive = true;
}

void InitHelper::printDependencies(init::Base& toStart, int level)
{
    for(int i = 0; i < level; i++)
        std::cout << " ";
    
    std::cout << toStart.getName() << std::endl;
    
    //start all dependencies
    for(init::Base *dep : toStart.getDependencies())
    {

        
        printDependencies(*dep, level + 1);
        
    }    
}

bool InitHelper::startDeploymentRecursive(init::Base& toStart, std::vector< orocos_cpp::Deployment* >& started)
{
    //start all dependencies
    for(init::Base *dep : toStart.getDependencies())
    {
        startDeploymentRecursive(*dep, started);
    }

    orocos_cpp::Spawner &spawner(orocos_cpp::Spawner::getInstace());
    orocos_cpp::PluginHelper helper;


    //start all tasks of this unit
    for(init::DependentTaskBase *dtb : toStart.getDependendTasks())
    {
        if(contains(dtb->getDeployment().get(), started))
            continue;
        
        started.push_back(dtb->getDeployment().get());

        std::cout << "Init::startDeploymentRecursive : Starting deployment " << dtb->getDeployment()->getName() << "Pointer is " << dtb->getDeployment() << std::endl;
        for(const std::string &tk : dtb->getDeployment()->getNeededTypekits())
        {
            helper.loadTypekitAndTransports(tk);
        }

        spawner.spawnDeployment(dtb->getDeployment().get());
    }

    return true;
}

bool InitHelper::startReplayRecursive(init::Base& toStart, ReplayHandler &replay)
{
    //start all dependencies
    for(init::Base *dep : toStart.getDependencies())
    {
        startReplayRecursive(*dep, replay);
    }

    //start all tasks of this unit
    for(log_replay::ReplayTask *rt : toStart.getReplayTasks())
    {
        for(log_replay::ReplayPortBase *p: rt->getPorts())
        {
            const std::string streamName(rt->getName() + "." + p->getName());
            
            std::cout << "InitHelper::startReplayRecursive : Adding log stream " << streamName << std::endl;
            replay.pushStream(streamName);
        }
    }
    return true;
}


bool InitHelper::startTasksRecursive(init::Base& toStart, std::vector< init::Base* >& started)
{
    for(init::Base *s : started)
    {
        if(s == &toStart)
        {
            return true;
        }
    }


    //start all dependencies
    for(init::Base *dep : toStart.getDependencies())
    {
        bool allreadyStarted = false;
        for(init::Base *s : started)
        {
            if(s == dep)
            {
                allreadyStarted = true;
                break;
            }
        }

        if(allreadyStarted)
            continue;

        startTasksRecursive(*dep, started);
    }

    toStart.initProxies();
    toStart.connect();
    toStart.applyConfig(confHelper);
    toStart.setupTransformer(trHelper);
    toStart.configure();
    toStart.start();

    started.push_back(&toStart);

    return true;
}
