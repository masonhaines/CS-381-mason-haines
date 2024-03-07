#include <functional>

namespace cs381 {

    template<typename>
    struct Delegate {};

    template<typename Return, typename... Arguments>
    struct Delegate<Return(Arguments...)> {
        std::vector<std::function<Return(Arguments...)>> functions;

        template<typename Class>
        using MethodType = Return(Class::*)(Arguments... );

        // how do i return a value from this 
        void operator()(Arguments... args) {
            for(auto& f: functions)
                f(args...);
        }
        // how do we remove functions ?

        // for class method reference 
        template<typename Class>
        void connect(Class& cls, MethodType<Class> f) {
            this->operator+=([cls, f](Arguments... args)-> Return {
                return cls.*f;// fancy operator only used for this one thing ever 
            });
        }

        void operator += (std::function<Return(Arguments...) > f) {
            functions.push_back(f);
        }
    };
}