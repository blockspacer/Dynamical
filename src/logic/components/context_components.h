#ifndef CONTEXT_COMPONENTS_H
#define CONTEXT_COMPONENTS_H

#include <entt/entt.hpp>

#include "chunk_map.h"
#include "chunkdatac.h"
#include "inputc.h"
#include "camerac.h"
#include "renderinfo.h"

template <typename... Types>
class ComponentMap {
public:
    using tuple_type = std::tuple<std::decay_t<Types>...>;
    
    using id = entt::identifier<Types...>;

    static constexpr tuple_type tuple;
    
    template<typename T>
    static constexpr T& get() {
        return tuple.get(id::template type<T>);
    }
};

#endif
