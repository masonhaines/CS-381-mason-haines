//  3/11 goes over component storages 

// can only store one type of thing 
#include <cstddef>
#include <cassert>
#include <cstdint>
#include <vector>

using Entity = uint8_t;

struct ComponentStorage {
    size_t elementSize = -1;
    std::vector<std::byte> data; // buffer that is 4 bytes long 

    ComponentStorage() : elementSize(-1), data(1, std::byte{0}) {};
    ComponentStorage (size_t elementSize) : elementSize(elementSize) {data.resize(5 * elementSize);}

    template<typename Tcomponent> 
    ComponentStorage(Tcomponent reference = {}) : ComponentStorage(sizeof(Tcomponent)) {}

    template<typename Tcomponent> 
    Tcomponent& Get(Entity e) {
        assert(sizeof(Tcomponent) == elementSize);
        assert(e < (data.size() / elementSize));
        return *(Tcomponent*)(data.data() + e * elementSize);
    }

    template<typename Tcomponent> 
    std::pair<Tcomponent&, Entity> Allocate(size_t count = 1) {
        assert(sizeof(Tcomponent) == elementSize);
        assert (count < 255);
        auto originalEnd = data.size();
        data.insert(data.end(), elementSize * count, std::byte{0});
        for (size_t i = 0; i < count - 1; i ++) 
            new(data.data() + originalEnd + i * elementSize) Tcomponent();
        return {
            *new(data.data() + data.size() - elementSize) Tcomponent(), data.size() / elementSize
        };

    }

    template<typename Tcomponent> 
    Tcomponent& GetOrAllocate(Entity e) {
        assert(sizeof(Tcomponent) == elementSize);
        size_t size = data.size() / elementSize;
        if (size <= e) {
            Allocate<Tcomponent>(std::max<int64_t>(int64_t(e) - size, 1));
        }
        return Get<Tcomponent>(e);
    }
};