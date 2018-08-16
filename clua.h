#ifndef CLUA_HEADER
#define CLUA_HEADER

#include <tuple>
#include <exception>

template<typename ...T>
struct resolve_args;

template<>
struct resolve_args<> {
	static std::tuple<> resolve(lua_State *L, int nargs) {
		return std::make_tuple();
	}
};

// args is int
template<typename ...B>
struct resolve_args<int, B...> {
	static std::tuple<int, B...> resolve(lua_State *L, int nargs) {
    if (lua_isinteger(L, -nargs) == 0)
      throw exception("args is not integer !!!");
		int i = (int)lua_tointeger(L, -nargs);
		return std::tuple_cat(std::make_tuple(i), resolve_args<B...>::resolve(L, nargs - 1));
	}
};

// args is double
template<typename ...B>
struct resolve_args<double, B...> {
	static std::tuple<double, B...> resolve(lua_State *L, int nargs) {
    if (lua_isnumber(L, -nargs) == 0)
      throw exception("args is not double !!!");
		double i = (double)lua_tonumber(L, -nargs);
		return std::tuple_cat(std::make_tuple(i), resolve_args<B...>::resolve(L, nargs - 1));
	}
};

// args is float
template<typename ...B>
struct resolve_args<float, B...> {
	static std::tuple<float, B...> resolve(lua_State *L, int nargs) {
    if (lua_isnumber(L, -nargs) == 0)
      throw exception("args is not float !!!");
		float i = (float)lua_tonumber(L, -nargs);
		return std::tuple_cat(std::make_tuple(i), resolve_args<B...>::resolve(L, nargs - 1));
	}
};

// args is bool
template<typename ...B>
struct resolve_args<bool, B...> {
	static std::tuple<bool, B...> resolve(lua_State *L, int nargs) {
    if (lua_isboolean(L, -nargs) == 0)
      throw exception("args is not bool !!!");
		bool i = lua_toboolean(L, -nargs) != 0;
		return std::tuple_cat(std::make_tuple(i), resolve_args<B...>::resolve(L, nargs - 1));
	}
};

// args is char *
template<typename ...B>
struct resolve_args<char*, B...> {
	static std::tuple<char*, B...> resolve(lua_State *L, int nargs) {
    if (lua_isstring(L, -nargs) == 0)
      throw exception("args is not string !!!");
		char* i = const_cast<char*>(lua_tostring(L, -nargs));
		return std::tuple_cat(std::make_tuple(i), resolve_args<B...>::resolve(L, nargs - 1));
	}
};

// args is const char*
template<typename ...B>
struct resolve_args<const char*, B...> {
	static std::tuple<const char*, B...> resolve(lua_State *L, int nargs) {
    if (lua_isstring(L, -nargs) == 0)
      throw exception("args is not string !!!");
		const char* i = (lua_tostring(L, -nargs));
		return std::tuple_cat(std::make_tuple(i), resolve_args<B...>::resolve(L, nargs - 1));
	}
};

// args is pointer
template<typename C, typename ...B>
struct resolve_args<C*, B...> {
	static std::tuple<C*, B...> resolve(lua_State *L, int nargs) {
    if ((lua_isuserdata(L, -nargs) == 0) && (lua_islightuserdata(L, -nargs) == 0))
      throw exception("args is not pointer !!!");
		C* i = (C*)lua_touserdata(L, -nargs);
		return std::tuple_cat(std::make_tuple(i), resolve_args<B...>::resolve(L, nargs - 1));
	}
};

// args is class
template<typename C, typename ...B>
struct resolve_args<C, B...> {
  static std::tuple<C, B...> resolve(lua_State *L, int nargs) {
    if (lua_isuserdata(L, -nargs) == 0)
      throw exception("args is not class !!!");
    C* i = (C*)lua_touserdata(L, -nargs);
    return std::tuple_cat(std::make_tuple(*i), resolve_args<B...>::resolve(L, nargs - 1));
  }
};

template<typename T>
struct resolve_fp;

template<typename R, typename ...Args>
struct resolve_fp<R(*)(Args...)> {
	static const int nargs = sizeof...(Args);
	using output_type = R;
	static std::tuple<Args...> resolve(lua_State *L) {
		return resolve_args<Args...>::resolve(L, nargs);
	}
};

template<typename R, typename C, typename ...Args>
struct resolve_fp<R(C::*)(Args...)> {
	static const int nargs = sizeof...(Args);
	using output_type = R;
	static std::tuple<Args...> resolve(lua_State *L) {
		return resolve_args<Args...>::resolve(L, nargs);
	}
};

template<int N, typename R, typename T, T f>
struct process_fp;

template<typename R, typename T, T f>
struct process_fp<0, R, T, f> {
	template<typename Head, typename ...Args>
	static void process(lua_State *L, Head t, Args... args) {
		R ret = f(args...);
		ret_fp<R>::ret(L, ret);
	}
};

template<typename T, T f>
struct process_fp<0, void, T, f> {
	template<typename Head, typename ...Args>
	static void process(lua_State *L, Head t, Args... args) {
		f(args...);
	}
};

template<int N, typename R, typename T, T f>
struct process_fp {
	template<typename Head, typename ...Tail>
	static void process(lua_State *L, Head t, Tail... args) {
		auto i = std::get<N - 1>(t);
		process_fp<N - 1, R, T, f>::process(L, t, i, args...);
	}
};

template<int N, typename R, typename C, typename T, T f>
struct process_cfp;

template<typename R, typename C, typename T, T f>
struct process_cfp<0, R, C, T, f> {
	template<typename Head, typename ...Tail>
	static void process(lua_State *L, Head t, Tail... args) {
		auto cls = (C*)lua_touserdata(L, -resolve_fp<T>::nargs - 1);
		R ret = (cls->*f)(args...);
		ret_fp<R>::ret(L, ret);
	}
};

template<typename C, typename T, T f>
struct process_cfp<0, void, C, T, f> {
	template<typename Head, typename ...Tail>
	static void process(lua_State *L, Head t, Tail... args) {
		auto cls = (C*)lua_touserdata(L, -resolve_fp<T>::nargs - 1);
		(cls->*f)(args...);
	}
};

template<int N, typename R, typename C, typename T, T f>
struct process_cfp {
	template<typename Head, typename ...Tail>
	static void process(lua_State *L, Head t, Tail... args) {
		auto i = std::get<N - 1>(t);
		process_cfp<N - 1, R, C, T, f>::process(L, t, i, args...);
	}
};

template<int N, typename R, typename C, typename T, T f>
struct process_cfp;

template<int N, typename C>
struct process_ccfp;

template<typename C>
struct process_ccfp<0, C> {
	template<typename Head, typename ...Tail>
	static void process(lua_State *L, Head t, Tail... args) {
		C* cls = (C*)lua_newuserdata(L, sizeof(C));
		new (cls)C(args...); 
		lua_getglobal(L, clua_class<C>::class_name);
		lua_setmetatable(L, -2);
	}
};

template<int N, typename C>
struct process_ccfp {
	template<typename Head, typename ...Tail>
	static void process(lua_State *L, Head t, Tail... args) {
		auto i = std::get<N - 1>(t);
		process_ccfp<N - 1, C>::process(L, t, i, args...);
	}
};

template<typename T>
struct ret_fp;

// return is int
template<>
struct ret_fp<int> {
	static void ret(lua_State *L, int value) {
		lua_pushinteger(L, value);
	}
};

// return is double
template<>
struct ret_fp<double> {
	static void ret(lua_State *L, double value) {
		lua_pushnumber(L, value);
	}
};

// return is float
template<>
struct ret_fp<float> {
	static void ret(lua_State *L, float value) {
		lua_pushnumber(L, value);
	}
};

// return is bool
template<>
struct ret_fp<bool> {
	static void ret(lua_State *L, bool value) {
		lua_pushboolean(L, value);
	}
};

// return is char *
template<>
struct ret_fp<char *> {
	static void ret(lua_State *L, const char* value) {
		lua_pushstring(L, value);
	}
};

// return is const char *
template<>
struct ret_fp<const char *> {
	static void ret(lua_State *L, const char* value) {
		lua_pushstring(L, value);
	}
};

// return is pointer
template<typename C>
struct ret_fp<C*> {
  static void ret(lua_State *L, C* value) {
    lua_pushlightuserdata(L, (void *)value);
  }
};

// return is class
template<typename C>
struct ret_fp {
  static void ret(lua_State *L, C value) {
    lua_newuserdata(L, sizeof(C));
    new (cls)C(value);
    lua_getglobal(L, clua_class<C>::class_name);
    lua_setmetatable(L, -2);
  }
};

template<typename T, T f>
int clua_function(lua_State *L) {
	auto args = resolve_fp<T>::resolve(L);
	process_fp<resolve_fp<T>::nargs, resolve_fp<T>::output_type, T, f>::process(L, args);
	if (std::is_same<resolve_fp<T>::output_type, void>::value)
		return 0;
	return 1;
}

template<typename C, typename T, T f>
int clua_cfunction(lua_State* L) {
	auto args = resolve_fp<T>::resolve(L);
	process_cfp<resolve_fp<T>::nargs, resolve_fp<T>::output_type, C, T, f>::process(L, args);
	if (std::is_same<resolve_fp<T>::output_type, void>::value)
		return 0;
	return 1;
}

template<typename C, typename ...Args>
int clua_ccfunction(lua_State* L) {
	auto args = resolve_args<Args...>::resolve(L, sizeof...(Args));
	process_ccfp<sizeof...(Args), C>::process(L, args);
	return 1;
}

template<typename C>
int clua_cdfunction(lua_State* L) {
	C* cls = (C*)lua_touserdata(L, -1);
	cls->~C();
	return 0;
}

template<typename T, T f>
void clua_register(lua_State *L, const char* n) {
	int(*F)(lua_State*);
	F = clua_function<T, f>;
	lua_register(L, n, F);
}

template<typename C>
class clua_class {
public:
	static void registerClass(lua_State *L) {
		lua_newtable(L);
		lua_pushvalue(L, -1);
		lua_setfield(L, -1, "__index");
		auto cls_dcon = clua_cdfunction<C>;
		lua_pushcfunction(L, cls_dcon);
		lua_setfield(L, -2, "__gc");
    if (clua_class<C>::class_name == nullptr)
      throw std::exception("Class doesn't have name in lua!!!");
		lua_setglobal(L, clua_class<C>::class_name);
	}
	template<typename ...Args>
	static void registerConstructor(lua_State *L, const char* n = "new") {
		lua_getglobal(L, clua_class<C>::class_name);
		auto cls_new = clua_ccfunction<C, Args...>;
		lua_pushcfunction(L, cls_new);
		lua_setfield(L, -2, n);
		lua_setglobal(L, clua_class<C>::class_name);
	}
	template<typename T, T f>
	static void registerMemberFun(lua_State *L, const char* n) {
		auto mf = clua_cfunction<C, T, f>;
		lua_getglobal(L, clua_class<C>::class_name);
		lua_pushcfunction(L, mf);
		lua_setfield(L, -2, n);
		lua_setglobal(L, clua_class<C>::class_name);
	}
public:
	static char* class_name;
};

template<typename C>
char* clua_class<C>::class_name = nullptr;

#endif