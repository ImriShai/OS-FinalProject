#include "MST_Strategy.hpp"
#include <string>
#include <map>



class MST_Factory
{
    private:
    MST_Factory();
    ~MST_Factory() = default;
    static MST_Factory* instance;
    MST_Factory(const MST_Factory&) = delete;
    MST_Factory& operator=(const MST_Factory&) = delete;
    static std::map<std::string, MST_Strategy*> strats;
    static void cleanUp();
   
public:
    static MST_Strategy* createMST(std::string type);
    static MST_Factory* getInstance();
    
};