#ifndef IVW_LAMBDA2FUNC_H
#define IVW_LAMBDA2FUNC_H

#include <vtkObject.h>
#include<type_traits>
#include<utility>

namespace inviwo {
	template<typename Callable>
	union storage
	{
		storage() {}
		~storage() {}
		std::decay_t<Callable> callable;
	};

	template<int, typename Callable, typename Ret, typename... Args>
	auto fnptr_(Callable&& c, Ret(*)(Args...))
	{
		static bool used = false;
		static storage<Callable> s;
		using type = decltype(s.callable);

		if (used)
			s.callable.~type();
		new (&s.callable) type(std::forward<Callable>(c));
		used = true;

		return [](Args... args) -> Ret {
			return Ret(s.callable(args...));
		};
	}

	template<typename Fn, int N = 0, typename Callable>
	Fn* fnptr(Callable&& c)
	{
		return fnptr_<N>(std::forward<Callable>(c), (Fn*)nullptr);
	}
}

#endif  // IVW_LAMBDA2FUNC_H
