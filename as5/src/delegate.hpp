#include <functional>

namespace cs381 {

    template<typename>
    struct Delegate {};

    template<typename Return, typename... Arguments>
    struct Delegate<Return(Arguments...)> {
        std::vector<std::function<Return(Arguments...)>> functions;

        // how do i return a value from this 
        void operator()(Arguments... args) {
            for(auto& f: functions)
                f(args...);
        }
        // how do we remove functions ?

        void operator += (std::function<Return(Arguments...) > f) {
            functions.push_back(f);
        }
    };
}