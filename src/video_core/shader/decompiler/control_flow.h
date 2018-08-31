#pragma once

#include <array>
#include <vector>
#include <unordered_map>
#include <boost/optional.hpp>
#include "video_core/shader/shader.h"
#include "video_core/shader/decompiler/decompiler.h"

namespace Pica{
namespace Shader{
namespace Decompiler{


class ControlFlow{
public:
    struct Node;
    struct Edge;
    struct Cond;

    // The first block
    Rc<Node> start;
    // the last block
    Rc<Node> end;

    ControlFlow();
    virtual ~ControlFlow();

    void build( ProgramArray & program
              , unsigned first
              , unsigned last
              , ProcMap<ControlFlow> & proc);
private:
    // Nodes sorted by there instruction.
    std::vector<Rc<Node>> nodes_by_index;

    // returns the node which belongs the the instruction.
    Option<Rc<Node>> find(unsigned instr) const;
    // returns the index in tne nodes_by_index vector of the node belonging to the instruction.
    Option<unsigned> find_index(unsigned instr) const;

    // Split a block which contains the instruction given in to 2 blocks starting with the given instruction.
    void split_at(unsigned instr);
    void edge(Rc<Node> from,Rc<Node> to,Option<Cond> cond);

    // Block generating pass
    void build_blocks( ProgramArray & program
                     , unsigned first
                     , unsigned last);

    // Edge generating pass
    void build_edges( ProgramArray & program
                    , unsigned first
                    , unsigned last
                    , ProcMap<ControlFlow> & proc);
};

struct ControlFlow::Cond{
    unsigned instr;
    bool true_edge;
};

struct ControlFlow::Node{
    unsigned first;
    unsigned last;

    std::vector<Rc<Edge>> out;
    std::vector<Rc<Edge>> in;

    bool is_end();

    virtual ~Node() = default;
};

struct ControlFlow::Edge{
    Rc<Node> to;
    Rc<Node> from;

    Option<Cond> cond;
};



}// namespace
}// namespace
}// namespace
