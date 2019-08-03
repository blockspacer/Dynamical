#ifndef ARGUMENT_PARSER_H
#define ARGUMENT_PARSER_H

#include "entt/entt.hpp"

class ArgumentParser {
public:
    ArgumentParser(entt::registry& reg, int argc, char** argv);
};

#endif
