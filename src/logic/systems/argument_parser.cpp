#include "argument_parser.h"
#include "logic/components/arguments.h"

ArgumentParser::ArgumentParser(entt::registry& reg, int argc, char ** argv) {
    auto& args = reg.set<Arguments>();
    args.argc = argc;
    args.argv = argv;
}
