#pragma once

#include <array>
#include <memory>
#include <unordered_map>
#include <boost/optional.hpp>
#include <nihstro/shader_bytecode.h>
#include "video_core/shader/shader.h"
#include "video_core/shader/decompiler/control_flow.h"

namespace Pica{
namespace Shader{
namespace Decompiler{

const unsigned PROGRAM_LEN = MAX_PROGRAM_CODE_LENGTH;
const unsigned SWIZZLE_LEN = MAX_SWIZZLE_DATA_LENGTH;
 const unsigned MAX_INPUT_REG = 16; //TODO
 const unsigned MAX_OUTPUT_REG = 16; //TODO

typedef std::array<unsigned, PROGRAM_LEN> ProgramArray;
typedef std::array<unsigned, SWIZZLE_LEN> SwizzleArray;

template<typename T>
using Option<T> = boost::optional<T>;

template<typename T>
using Rc<T> = std::shared_ptr<T>;

template<typename T>
using ProcMap = std::unordered_map<Region,T>;

template<typename T>
using ProcPair = std::pair<Region,T>;

struct Region{
    unsigned first;
    unsigned last;
    bool operator==(const Region& other) const;
};

struct RegionHash{
    std::size_t operator () (const Region& h) const {
        auto start = std::hash<unsigned>{}(h.start);
        auto end = std::hash<unsigned>{}(h.end);
        return start ^ end;
    }
};


class Decompiler{
public:
    Decompiler();
    std::string Decompile(ProgramArray & program, SwizzleArray & swizzle, unsigned entry_point);
};

}}}
