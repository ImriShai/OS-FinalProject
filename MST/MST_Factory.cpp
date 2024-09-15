#include "MST_Factory.hpp"
#include "Prim.hpp"
#include "Kruskal.hpp"
#include "Tarjan.hpp"
#include "Boruvka.hpp"

MST_Factory *MST_Factory::instance = nullptr;

std::map<std::string, MST_Strategy *> MST_Factory::strats = {{"prim", nullptr}, {"kruskal", nullptr}, {"tarjan", nullptr}, {"boruvka", nullptr}};


MST_Factory *MST_Factory::getInstance()
{
    if (instance == nullptr)
    {
        instance = new MST_Factory();
        strats["prim"] = new Prim{};
        strats["kruskal"] = new Kruskal{};
        strats["tarjan"] = new Tarjan{};
        strats["boruvka"] = new Boruvka{};
        std::atexit(cleanUp);
    }
    return instance;
}

MST_Strategy* MST_Factory::createMST(std::string type)
{
    if(strats.find(type) != strats.end())
    {
        return strats[type];
    }
    throw std::invalid_argument("Invalid MST Strategy");
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