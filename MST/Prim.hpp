#include "MST_Strategy.hpp"
#include <queue>
#include <vector>





class Prim : public MST_Strategy
{
public:
    Graph operator()(Graph *g);
};
