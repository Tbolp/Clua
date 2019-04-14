#pragma once

#include <string>
#include <cassert>

#ifdef __CLUA_DLL__
#define CLUADLLEXPORT(dllname)	\
extern "C" __declspec(dllexport) int __cdecl luaopen_##dllname
#endif

template<typename Cls>
struct Clsinfo {
	static std::string cls_name;
};

template<typename Cls>
std::string Clsinfo<Cls>::cls_name;

template<typename ...T>
struct resolve_args;

template<>
struct resolve_args<> {
	static auto resolve(lua_State* l, int nargs) {
		return std::make_tuple();
	}
};

template<typename ...Args>
struct resolve_args<bool, Args...> {
	static auto resolve(lua_State* l, int nargs) {
		assert(lua_isboolean(l, -nargs) == 1);
		auto i = (bool)lua_toboolean(l, -nargs);
		return std::tuple_cat(std::make_tuple(i), resolve_args<Args...>::resolve(l, nargs - 1));
	}
};

template<typename ...Args>
struct resolve_args<char, Args...> {
	static auto resolve(lua_State* l, int nargs) {
		assert(lua_isstring(l, -nargs) == 1);
		auto i = (char)lua_tostring(l, -nargs)[0];
		return std::tuple_cat(std::make_tuple(i), resolve_args<Args...>::resolve(l, nargs - 1));
	}
};

template<typename ...Args>
struct resolve_args<unsigned char, Args...> {
	static auto resolve(lua_State* l, int nargs) {
		assert(lua_isstring(l, -nargs) == 1);
		auto i = (unsigned char)lua_tostring(l, -nargs)[0];
		return std::tuple_cat(std::make_tuple(i), resolve_args<Args...>::resolve(l, nargs - 1));
	}
};

template<typename ...Args>
struct resolve_args<short int, Args...> {
	static auto resolve(lua_State* l, int nargs) {
		assert(lua_isinteger(l, -nargs) == 1);
		auto i = (short int)lua_tointeger(l, -nargs);
		return std::tuple_cat(std::make_tuple(i), resolve_args<Args...>::resolve(l, nargs - 1));
	}
};

template<typename ...Args>
struct resolve_args<unsigned short int, Args...> {
	static auto resolve(lua_State* l, int nargs) {
		assert(lua_isinteger(l, -nargs) == 1);
		auto i = (unsigned short int)lua_tointeger(l , -nargs);
		return std::tuple_cat(std::make_tuple(i), resolve_args<Args...>::resolve(l, nargs - 1));
	}
};

template<typename ...Args>
struct resolve_args<int, Args...> {
	static auto resolve(lua_State* l, int nargs) {
		assert(lua_isinteger(l, -nargs) == 1);
		auto i = (int)lua_tointeger(l, -nargs);
		return std::tuple_cat(std::make_tuple(i), resolve_args<Args...>::resolve(l, nargs - 1));
	}
};

/*template<typename ...Args>
struct resolve_args<int&, Args...> {
	static auto resolve(lua_State* l, int nargs) {
		assert(lua_isinteger(l, -nargs) == 1);
		int v = (int)lua_tointeger(l, -nargs);
		int* i = (int*)lua_newuserdata(l, sizeof(int));
		new (i) int(v);
		return std::tuple_cat(std::tuple<std::reference_wrapper<int>>(std::ref(*i)), resolve_args<Args...>::resolve(l, nargs));
	}
};

template<typename ...Args>
struct resolve_args<const int&, Args...> {
	static auto resolve(lua_State* l, int nargs) {
		assert(lua_isinteger(l, -nargs) == 1);
		int v = (int)lua_tointeger(l, -nargs);
		int* i = (int*)lua_newuserdata(l, sizeof(int));
		new (i) int(v); 
		return std::tuple_cat(std::tuple<std::reference_wrapper<const int>>(std::cref(*i)), resolve_args<Args...>::resolve(l, nargs));
	}
};*/

template<typename ...Args>
struct resolve_args<unsigned int, Args...> {
	static auto resolve(lua_State* l, int nargs) {
		assert(lua_isinteger(l, -nargs) == 1);
		auto i = (unsigned int)lua_tointeger(l, -nargs);
		return std::tuple_cat(std::make_tuple(i), resolve_args<Args...>::resolve(l, nargs - 1));
	}
};

template<typename ...Args>
struct resolve_args<long int, Args...> {
	static auto resolve(lua_State* l, int nargs) {
		assert(lua_isinteger(l, -nargs) == 1);
		auto i = (long int)lua_tointeger(l, -nargs);
		return std::tuple_cat(std::make_tuple(i), resolve_args<Args...>::resolve(l, nargs - 1));
	}
};

template<typename ...Args>
struct resolve_args<unsigned long int, Args...> {
	static auto resolve(lua_State* l, int nargs) {
		assert(lua_isinteger(l, -nargs) == 1);
		auto i = (unsigned long int)lua_tointeger(l, -nargs);
		return std::tuple_cat(std::make_tuple(i), resolve_args<Args...>::resolve(l, nargs - 1));
	}
};

template<typename ...Args>
struct resolve_args<long long int, Args...> {
	static auto resolve(lua_State* l, int nargs) {
		assert(lua_isinteger(l, -nargs) == 1);
		auto i = (long long int)lua_tointeger(l, -nargs);
		return std::tuple_cat(std::make_tuple(i), resolve_args<Args...>::resolve(l, nargs - 1));
	}
};

template<typename ...Args>
struct resolve_args<unsigned long long int, Args...> {
	static auto resolve(lua_State* l, int nargs) {
		assert(lua_isinteger(l, -nargs) == 1);
		auto i = (unsigned long long int)lua_tointeger(l, -nargs);
		return std::tuple_cat(std::make_tuple(i), resolve_args<Args...>::resolve(l, nargs - 1));
	}
};

template<typename ...Args>
struct resolve_args<float, Args...> {
	static auto resolve(lua_State* l, int nargs) {
		assert(lua_isnumber(l, -nargs) == 1);
		auto i = (float)lua_tonumber(l, -nargs);
		return std::tuple_cat(std::make_tuple(i), resolve_args<Args...>::resolve(l, nargs - 1));
	}
};

template<typename ...Args>
struct resolve_args<double, Args...> {
	static auto resolve(lua_State* l, int nargs) {
		assert(lua_isnumber(l, -nargs) == 1);
		auto i = (double)lua_tonumber(l, -nargs);
		return std::tuple_cat(std::make_tuple(i), resolve_args<Args...>::resolve(l, nargs - 1));
	}
};

template<typename ...Args>
struct resolve_args<long double, Args...> {
	static auto resolve(lua_State* l, int nargs) {
		assert(lua_isnumber(l, -nargs) == 1);
		auto i = (long double)lua_tonumber(l, -nargs);
		return std::tuple_cat(std::make_tuple(i), resolve_args<Args...>::resolve(l, nargs - 1));
	}
};

template<typename ...Args>
struct resolve_args<const char*, Args...> {
	static auto resolve(lua_State* l, int nargs) {
		assert(lua_isstring(l, -nargs) == 1);
		auto i = (const char*)lua_tostring(l, -nargs);
		return std::tuple_cat(std::make_tuple(i), resolve_args<Args...>::resolve(l, nargs - 1));
	}
};

template<typename ...Args>
struct resolve_args<std::string, Args...> {
	static auto resolve(lua_State* l, int nargs) {
		assert(lua_isstring(l, -nargs) == 1);
		std::string i = (const char*)lua_tostring(l, -nargs);
		return std::tuple_cat(std::make_tuple(i), resolve_args<Args...>::resolve(l, nargs - 1));
	}
};

template<typename T, typename ...Args>
struct resolve_args<T*, Args...> {
	static auto resolve(lua_State* l, int nargs) {
		assert(lua_isuserdata(l, -nargs) == 1 || lua_islightuserdata(l, -nargs) == 1);
		auto i = (T*)lua_touserdata(l, -nargs);
		return std::tuple_cat(std::make_tuple(i), resolve_args<Args...>::resolve(l, nargs - 1));
	}
};

template<typename T, typename ...Args>
struct resolve_args<T&, Args...> {
	static auto resolve(lua_State* l, int nargs) {
		assert(lua_isuserdata(l, -nargs) == 1 || lua_islightuserdata(l, -nargs) == 1);
		auto i = (T*)lua_touserdata(l, -nargs);
		return std::tuple_cat(std::make_tuple(std::ref(*i)), resolve_args<Args...>::resolve(l, nargs - 1));
	}
};

template<typename T, typename ...Args>
struct resolve_args<T, Args...> {
	static auto resolve(lua_State* l, int nargs) {
		assert(lua_isuserdata(l, -nargs) == 1 || lua_islightuserdata(l, -nargs) == 1);
		auto i = (T*)lua_touserdata(l, -nargs);
		return std::tuple_cat(std::make_tuple(*i), resolve_args<Args...>::resolve(l, nargs - 1));
	}
};

template<typename T>
struct resolve_ret;

template<>
struct resolve_ret<bool> {
	static void resolve(lua_State* l, bool value) {
		lua_pushboolean(l, value);
	}
};

template<>
struct resolve_ret<char> {
	static void resolve(lua_State* l, char value) {
		lua_pushinteger(l, value);
	}
};

template<>
struct resolve_ret<unsigned char> {
	static void resolve(lua_State* l, unsigned char value) {
		lua_pushinteger(l, value);
	}
};

template<>
struct resolve_ret<short int> {
	static void resolve(lua_State* l, short int value) {
		lua_pushinteger(l, value);
	}
};

template<>
struct resolve_ret<unsigned short int> {
	static void resolve(lua_State* l, unsigned short int value) {
		lua_pushinteger(l, value);
	}
};

template<>
struct resolve_ret<int> {
	static void resolve(lua_State* l, int value) {
		lua_pushinteger(l, value);
	}
};

template<>
struct resolve_ret<unsigned int> {
	static void resolve(lua_State* l, unsigned int value) {
		lua_pushinteger(l, value);
	}
};

template<>
struct resolve_ret<long int> {
	static void resolve(lua_State* l, long int value) {
		lua_pushinteger(l, value);
	}
};

template<>
struct resolve_ret<unsigned long int> {
	static void resolve(lua_State* l, unsigned long int value) {
		lua_pushinteger(l, value);
	}
};

template<>
struct resolve_ret<long long int> {
	static void resolve(lua_State* l, long long int value) {
		lua_pushinteger(l, value);
	}
};

template<>
struct resolve_ret<unsigned long long int> {
	static void resolve(lua_State* l, unsigned long long int value) {
		lua_pushinteger(l, value);
	}
};

template<>
struct resolve_ret<float> {
	static void resolve(lua_State* l, float value) {
		lua_pushnumber(l, value);
	}
};

template<>
struct resolve_ret<double> {
	static void resolve(lua_State* l, double value) {
		lua_pushnumber(l, value);
	}
};

template<>
struct resolve_ret<long double> {
	static void resolve(lua_State* l, long double value) {
		lua_pushnumber(l, value);
	}
};

template<>
struct resolve_ret<const char*> {
	static void resolve(lua_State* l, const char* value) {
		lua_pushstring(l, value);
	}
};

template<>
struct resolve_ret<std::string> {
	static void resolve(lua_State* l, std::string value) {
		lua_pushstring(l, value.c_str());
	}
};

template<typename T>
struct resolve_ret<T*> {
	static void resolve(lua_State* l, T* value) {
		lua_pushlightuserdata(l, value);
	}
};

template<typename T>
struct resolve_ret<T&> {
	static void resolve(lua_State* l, T& value) {
		lua_pushlightuserdata(l, &value);
	}
};

template<typename T>
struct resolve_ret {
	static void resolve(lua_State* l, T value) {
		auto cls = (T*)lua_newuserdata(l, sizeof(T));
		new (cls) T(std::move(value));
		assert(lua_getfield(l, LUA_REGISTRYINDEX, Clsinfo<T>::cls_name.c_str()) == LUA_TTABLE);
		lua_setmetatable(l, -2);
	}
};

template<int N, typename R>
struct wrap_function;

template<>
struct wrap_function<0, void> {
	template<typename Head, typename ...Args>
	static int run(lua_State *l, Head t, Args... args) {
		typedef void(*FnPtr)(Args...);
		auto f = reinterpret_cast<FnPtr>(lua_touserdata(l, lua_upvalueindex(1)));
		f(args...);
		return 0;
	}
};

template<typename Ret>
struct wrap_function<0, Ret> {
	template<typename Head, typename ...Args>
	static int run(lua_State *l, Head t, Args... args) {
		typedef Ret(*FnPtr)(Args...);
		auto f = reinterpret_cast<FnPtr>(lua_touserdata(l, lua_upvalueindex(1)));
		auto ret = f(args...);
		resolve_ret<Ret>::resolve(l, ret);
		return 1;
	}
};

template<int N, typename Ret>
struct wrap_function {
	template<typename Head, typename ...Tail>
	static int run(lua_State* l, Head t, Tail... args) {
		auto i = std::get<N - 1>(t);
		return wrap_function<N - 1, Ret>::run(l, t, i, args...);
	}
};

template<typename Ret, typename ...Args>
int clua_function(lua_State* l) {
	auto args = resolve_args<Args...>::resolve(l, sizeof...(Args));
	return wrap_function<sizeof...(Args), Ret>::run(l, args);
}

template<int N, typename Cls>
struct wrap_constructor_function;

template<typename Cls>
struct wrap_constructor_function<0, Cls> {
	template<typename Head, typename ...Args>
	static int run(lua_State *l, Head t, Args... args) {
		auto cls = reinterpret_cast<Cls*>(lua_newuserdata(l, sizeof(Cls)));
		new (cls)Cls(args...);
		lua_getfield(l, LUA_REGISTRYINDEX, Clsinfo<Cls>::cls_name.c_str());
		lua_setmetatable(l, -2);
		return 1;
	}
};

template<int N, typename Cls>
struct wrap_constructor_function {
	template<typename Head, typename ...Tail>
	static int run(lua_State* l, Head t, Tail... args) {
		auto i = std::get<N - 1>(t);
		return wrap_constructor_function<N - 1, Cls>::run(l, t, i, args...);
	}
};

template<int N, typename Cls, typename Ret>
struct wrap_member_function;

template<typename Cls>
struct wrap_member_function<0, Cls, void> {
	template<typename Head, typename ...Args>
	static int run(lua_State *l, Head t, Args... args) {
		assert(lua_isuserdata(l, 1) == 1 || lua_islightuserdata(l, 1) == 1);
		typedef void(Cls::*FnPtr)(Args...);
		auto cls = reinterpret_cast<Cls*>(lua_touserdata(l, 1));
		auto f = *reinterpret_cast<FnPtr*>(lua_touserdata(l, lua_upvalueindex(1)));
		(cls->*f)(args...);
		return 0;
	}
};

template<typename Cls, typename Ret>
struct wrap_member_function<0, Cls, Ret> {
	template<typename Head, typename ...Args>
	static int run(lua_State *l, Head t, Args... args) {
		assert(lua_isuserdata(l, 1) == 1 || lua_islightuserdata(l, 1) == 1);
		typedef Ret(Cls::*FnPtr)(Args...);
		auto cls = reinterpret_cast<Cls*>(lua_touserdata(l, 1));
		auto f = *reinterpret_cast<FnPtr*>(lua_touserdata(l, lua_upvalueindex(1)));
		auto ret = (cls->*f)(args...);
		resolve_ret<Ret>::resolve(l, ret);
		return 1;
	}
};

template<int N, typename Cls, typename Ret>
struct wrap_member_function {
	template<typename Head, typename ...Tail>
	static int run(lua_State *l, Head t, Tail... args) {
		auto i = std::get<N - 1>(t);
		return wrap_member_function<N - 1, Cls, Ret>::run(l, t, i, args...);
	}
};

template<typename Cls, typename ...Args>
int clua_constructor_function(lua_State* l) {
	auto args = resolve_args<Args...>::resolve(l, sizeof...(Args));
	return wrap_constructor_function<sizeof...(Args), Cls>::run(l, args);
}

template<typename Cls>
int clua_deconstructor_function(lua_State* l) {
	if (lua_isuserdata(l, -1) == 1) {
		auto cls = reinterpret_cast<Cls*>(lua_touserdata(l, -1));
		cls->~Cls();
	}
	else if(lua_istable(l, -1) != 1)
		assert(false);
	return 0;
}

template<typename Cls, typename Ret, typename ...Args>
int clua_classmember_function(lua_State* l) {
	auto args = resolve_args<Args...>::resolve(l, sizeof...(Args));
	return wrap_member_function<sizeof...(Args), Cls, Ret>::run(l, args);
}

class CLua {
public:

#ifndef __CLUA_DLL__
	CLua(lua_State* l, std::string n = ""):
		_lua_state(l),
		_pkg_name(n){
		assert(l != nullptr);
		if (_pkg_name != "") {
			lua_newtable(l);
			lua_setglobal(l, _pkg_name.c_str());
		}
	}
#else
	CLua(lua_State* l) :
		_lua_state(l) {
		assert(l != nullptr);
		lua_newtable(l);
	}
#endif

	template<typename T>
	CLua& registerFunction(std::string fn, T f) {
		_registerFunction(fn, f);
		return *this;
	}

	template<typename Cls, typename ...Args>
	CLua& registerClass(std::string cls, std::string pcls = "", std::string fn = "new") {
		assert(cls != "");
		auto lf = clua_deconstructor_function<Cls>;
		Clsinfo<Cls>::cls_name = cls;
#ifndef __CLUA_DLL__
		addConstructor<std::is_abstract<Cls>::value, Cls, Args...>::add(_lua_state, _pkg_name, fn);
#else
		addConstructor<std::is_abstract<Cls>::value, Cls, Args...>::add(_lua_state, fn);
#endif
		lua_newtable(_lua_state);
		lua_pushvalue(_lua_state, -1);
		lua_setfield(_lua_state, -2, "__index");
		lua_pushcclosure(_lua_state, lf, 0);
		lua_setfield(_lua_state, -2, "__gc");
		if (pcls != "") {
			assert(lua_getfield(_lua_state, LUA_REGISTRYINDEX, pcls.c_str()) == LUA_TTABLE);
			lua_setmetatable(_lua_state, -2);
		}
		lua_setfield(_lua_state, LUA_REGISTRYINDEX, cls.c_str());
		return *this;
	}

	template<typename Cls, typename ...Args>
	CLua& registerClassContructor(std::string cls, std::string fn) {
#ifndef __CLUA_DLL__
		addConstructor<std::is_abstract<Cls>::value, Cls, Args...>::add(_lua_state, _pkg_name, fn);
#else
		addConstructor<std::is_abstract<Cls>::value, Cls, Args...>::add(_lua_state, fn);
#endif
		return *this;
	}

	template<typename T>
	CLua& registerClassMemberFunction(std::string fn, T f) {
		_registerClassMemberFunction(fn, f);
		return *this;
	}

	template<typename Cls, typename T>
	CLua& registerClassStaticFunction(std::string fn, T f) {
		_registerClassStaticFunction<Cls>(fn, f);
		return *this;
	}

	~CLua() {}
	
private:

	lua_State*	_lua_state;
#ifndef __CLUA_DLL__
	std::string _pkg_name;
#endif

	template<typename Ret, typename ...Args>
	void _registerFunction(std::string fn, Ret(*f)(Args...)) {
		auto lf = clua_function<Ret, Args...>;
#ifndef __CLUA_DLL__
		if (_pkg_name != "") {
			assert(lua_getglobal(_lua_state, _pkg_name.c_str()) == LUA_TTABLE);
			lua_pushlightuserdata(_lua_state, reinterpret_cast<void*>(f));
			lua_pushcclosure(_lua_state, lf, 1);
			lua_setfield(_lua_state, -2, fn.c_str());
			lua_pop(_lua_state, 1);
		}
		else {
			lua_pushlightuserdata(_lua_state, reinterpret_cast<void*>(f));
			lua_pushcclosure(_lua_state, lf, 1);
			lua_setglobal(_lua_state, fn.c_str());
		}
#else
		lua_pushlightuserdata(_lua_state, reinterpret_cast<void*>(f));
		lua_pushcclosure(_lua_state, lf, 1);
		lua_setfield(_lua_state, -2, fn.c_str());
#endif
	}

	template<bool, typename Cls, typename ...Args>
	struct addConstructor;

#ifndef __CLUA_DLL__
	template<typename Cls, typename ...Args>
	struct addConstructor<true, Cls, Args...> {
		static void add(lua_State* l, std::string pkg_name, std::string fn) {

		}
	};
#else
	template<typename Cls, typename ...Args>
	struct addConstructor<true, Cls, Args...> {
		static void add(lua_State* l, std::string fn) {

		}
	};
#endif

#ifndef __CLUA_DLL__
	template<typename Cls, typename ...Args>
	struct addConstructor<false, Cls, Args...> {
		static void add(lua_State* l, std::string pkg_name, std::string fn) {
			auto lf = clua_constructor_function<Cls, Args...>;
			if (pkg_name != "") {
				lua_getglobal(l, pkg_name.c_str());
				lua_newtable(l);
				lua_pushcclosure(l, lf, 0);
				lua_setfield(l, -2, fn.c_str());
				lua_setfield(l, -2, Clsinfo<Cls>::cls_name.c_str());
				lua_pop(l, 1);
			}
			else {
				lua_newtable(l);
				lua_pushcclosure(l, lf, 0);
				lua_setfield(l, -2, "new");
				lua_setglobal(l, Clsinfo<Cls>::cls_name.c_str());
			}
		}
	};
#else
	template<typename Cls, typename ...Args>
	struct addConstructor<false, Cls, Args...> {
		static void add(lua_State* l, std::string fn) {
			auto lf = clua_constructor_function<Cls, Args...>;
			lua_newtable(l);
			lua_pushcclosure(l, lf, 0);
			lua_setfield(l, -2, fn.c_str());
			lua_setfield(l, -2, Clsinfo<Cls>::cls_name.c_str());
		}
	};
#endif

	template<typename Cls, typename Ret, typename ...Args>
	void _registerClassMemberFunction(std::string fn, Ret(Cls::*f)(Args...)) {
		typedef Ret(Cls::*FnPtr)(Args...);
		auto lf = clua_classmember_function<Cls, Ret, Args...>;
		assert(lua_getfield(_lua_state, LUA_REGISTRYINDEX, Clsinfo<Cls>::cls_name.c_str()) == LUA_TTABLE);
		new (lua_newuserdata(_lua_state, sizeof(FnPtr))) FnPtr(f);
		lua_pushcclosure(_lua_state, lf, 1);
		lua_setfield(_lua_state, -2, fn.c_str());
		lua_pop(_lua_state, 1);
	}

	template<typename Cls, typename Ret, typename ...Args>
	void _registerClassStaticFunction(std::string fn, Ret(*f)(Args...)) {
		auto lf = clua_function<Ret, Args...>;
#ifndef __CLUA_DLL__
		if (_pkg_name != "") {
			lua_getglobal(_lua_state, _pkg_name.c_str());
			lua_getfield(_lua_state, -1, Clsinfo<Cls>::cls_name.c_str());
			lua_pushlightuserdata(_lua_state, reinterpret_cast<void*>(f));
			lua_pushcclosure(_lua_state, lf, 1);
			lua_setfield(_lua_state, -2, fn.c_str());
			lua_pop(_lua_state, 2);
		} 
		else {
			lua_getglobal(_lua_state, Clsinfo<Cls>::cls_name.c_str());
			lua_pushlightuserdata(_lua_state, reinterpret_cast<void*>(f));
			lua_pushcclosure(_lua_state, lf, 1);
			lua_setfield(_lua_state, -2, fn.c_str());
			lua_pop(_lua_state, 1);
		}
#else
		lua_getfield(_lua_state, -1, Clsinfo<Cls>::cls_name.c_str());
		lua_pushlightuserdata(_lua_state, reinterpret_cast<void*>(f));
		lua_pushcclosure(_lua_state, lf, 1);
		lua_setfield(_lua_state, -2, fn.c_str());
		lua_pop(_lua_state, 1);
#endif
	}

};