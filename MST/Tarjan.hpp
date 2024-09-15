#pragma once
#include "MST_Strategy.hpp"
#include <algorithm>
#include <vector>
#include <queue>
#include <limits>
#include <memory>

class KKT: public MST_Strategy
{
    public:
        Graph* operator()(Graph *g);
};