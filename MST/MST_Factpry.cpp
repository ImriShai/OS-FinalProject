#include "MST_Factory.hpp"
#include "Prim.hpp"
#include "Kruskal.hpp"

MST_Factory *MST_Factory::instance = nullptr;

std::map<std::string, MST_Strategy *> MST_Factory::strats = {{"Prim", nullptr}, {"Kruskal", nullptr}};


MST_Factory *MST_Factory::getInstance()
{
    if (instance == nullptr)
    {
        instance = new MST_Factory();
        strats["prim"] = new Prim{};
        strats["kruskal"] = new Kruskal{};
        std::atexit(cleanUp);
    }
    return instance;
}

MST_Strategy *MST_Factory::createMST(std::string type)
{
    return strats[type];
}

void MST_Factory::cleanUp()
{
    if (instance != nullptr)
    {
        delete instance;
        instance = nullptr;
        for (auto &strat : strats)
        {
            delete strat.second;
            strat.second = nullptr;
        }
    }
}