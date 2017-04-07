#ifndef CLUA_HEADER
#define CLUA_HEADER

#include <tuple>

template<typename ...T>
struct resolve_args;

template<>
struct resolve_args<> {
	static std::tuple<> resolve(lua_State *L, int nargs) {
		return std::make_tuple();
	}
};

template<typename ...B>
struct resolve_args<int, B...> {
	static std::tuple<int, B...> resolve(lua_State *L, int nargs) {
		int i = (int)lua_tointeger(L, -nargs);
		return std::tuple_cat(std::make_tuple(i), resolve_args<B...>::resolve(L, nargs - 1));
	}
};

template<typename ...B>
struct resolve_args<double, B...> {
	static std::tuple<double, B...> resolve(lua_State *L, int nargs) {
		double i = (double)lua_tonumber(L, -nargs);
		return std::tuple_cat(std::make_tuple(i), resolve_args<B...>::resolve(L, nargs - 1));
	}
};

template<typename ...B>
struct resolve_args<float, B...> {
	static std::tuple<float, B...> resolve(lua_State *L, int nargs) {
		float i = (float)lua_tonumber(L, -nargs);
		return std::tuple_cat(std::make_tuple(i), resolve_args<B...>::resolve(L, nargs - 1));
	}
};

template<typename ...B>
struct resolve_args<bool, B...> {
	static std::tuple<bool, B...> resolve(lua_State *L, int nargs) {
		bool i = lua_toboolean(L, -nargs) != 0;
		return std::tuple_cat(std::make_tuple(i), resolve_args<B...>::resolve(L, nargs - 1));
	}
};

template<typename ...B>
struct resolve_args<char*, B...> {
	static std::tuple<char*, B...> resolve(lua_State *L, int nargs) {
		char* i = const_cast<char*>(lua_tostring(L, -nargs));
		return std::tuple_cat(std::make_tuple(i), resolve_args<B...>::resolve(L, nargs - 1));
	}
};

template<typename ...B>
struct resolve_args<const char*, B...> {
	static std::tuple<const char*, B...> resolve(lua_State *L, int nargs) {
		const char* i = (lua_tostring(L, -nargs));
		return std::tuple_cat(std::make_tuple(i), resolve_args<B...>::resolve(L, nargs - 1));
	}
};

template<typename C, typename ...B>
struct resolve_args<C*, B...> {
	static std::tuple<C*, B...> resolve(lua_State *L, int nargs) {
		C* i = (C*)lua_touserdata(L, -nargs);
		return std::tuple_cat(std::make_tuple(i), resolve_args<B...>::resolve(L, nargs - 1));
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

template<>
struct ret_fp<int> {
	static void ret(lua_State *L, int value) {
		lua_pushinteger(L, value);
	}
};

template<>
struct ret_fp<double> {
	static void ret(lua_State *L, double value) {
		lua_pushnumber(L, value);
	}
};

template<>
struct ret_fp<float> {
	static void ret(lua_State *L, float value) {
		lua_pushnumber(L, value);
	}
};

template<>
struct ret_fp<bool> {
	static void ret(lua_State *L, bool value) {
		lua_pushboolean(L, value);
	}
};

template<>
struct ret_fp<char *> {
	static void ret(lua_State *L, const char* value) {
		lua_pushstring(L, value);
	}
};

template<>
struct ret_fp<const char *> {
	static void ret(lua_State *L, const char* value) {
		lua_pushstring(L, value);
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