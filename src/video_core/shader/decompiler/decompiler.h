#pragma once

#include <array>
#include <memory>
#include <unordered_map>
#include <boost/optional.hpp>
#include <nihstro/shader_bytecode.h>
#include "video_core/shader/shader.h"

namespace Pica{
namespace Shader{
namespace Decompiler{

const unsigned PROGRAM_LEN = MAX_PROGRAM_CODE_LENGTH;
const unsigned SWIZZLE_LEN = MAX_SWIZZLE_DATA_LENGTH;
const unsigned MAX_INPUT_REG = 16; //TODO
const unsigned MAX_OUTPUT_REG = 16; //TODO

using ProgramArray = std::array<unsigned, PROGRAM_LEN>;
using SwizzleArray = std::array<unsigned, SWIZZLE_LEN>;

struct Region{
    unsigned first;
    unsigned last;
    bool operator==(const Region& other) const;
};

struct RegionHash{
    std::size_t operator () (const Region& h) const {
        auto start = std::hash<unsigned>{}(h.first);
        auto end = std::hash<unsigned>{}(h.last);
        return start ^ end;
    }
};

template<typename T>
using Option = boost::optional<T>;

template<typename T>
using Rc = std::shared_ptr<T>;

template<typename T>
using ProcMap = std::unordered_map<Region,T,RegionHash>;

template<typename T>
using ProcPair = std::pair<Region,T>;

class Decompiler{
public:
    Decompiler();
    static Option<std::string> decompile(ProgramArray & program, SwizzleArray & swizzle, unsigned entry_point);
};

}}}
