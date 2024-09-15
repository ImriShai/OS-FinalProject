#ifndef BORUVKA_HPP
#define BORUVKA_HPP

#include "MST_Strategy.hpp"
#include "../DataStruct/UnionFind.hpp"

class Boruvka : public MST_Strategy
{
public:
    Graph* operator()(Graph *g);
};

#endif // BORUVKA_HPP