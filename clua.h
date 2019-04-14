#pragma once
#include <string>
#include <tuple>
#include <exception>

#define LUAMODULE(name)	\
extern "C" __declspec(dllexport) int luaopen_##name(lua_State* pState)

using CLuaException = std::exception;

template<typename T>
struct RetCount {
	static constexpr int value = 1;
};

template<>
struct RetCount<void> {
	static constexpr int value = 0;
};

template<typename T>
struct ResolveArgs {
	static auto resolve(lua_State* pState, int pIndex) {
		return lua_touserdata(pState, pIndex);
	}
};

#define RESOLVEINT(type)	\
template<>					\
struct ResolveArgs<type> {	\
	static auto resolve(lua_State* pState, int pIndex) {	\
		return lua_tointeger(pState, pIndex);				\
	}														\
};	

RESOLVEINT(short)
RESOLVEINT(unsigned short)
RESOLVEINT(int)
RESOLVEINT(unsigned int)
RESOLVEINT(long)
RESOLVEINT(unsigned long)
RESOLVEINT(long long)
RESOLVEINT(unsigned long long)

#undef RESOLVEINT

template<>
struct ResolveArgs<float> {
	static auto resolve(lua_State* pState, int pIndex) {
		return lua_tonumber(pState, pIndex);
	}
};

template<>
struct ResolveArgs<double> {
	static auto resolve(lua_State* pState, int pIndex) {
		return lua_tonumber(pState, pIndex);
	}
};

template<>
struct ResolveArgs<const char*> {
	static auto resolve(lua_State* pState, int pIndex) {
		return lua_tostring(pState, pIndex);
	}
};

template<typename T>
struct ResolveRet {
	static void resolve(lua_State* l, T value) {
		lua_pushlightuserdata(l, value);
	}
};

#define RESOLVEINT(type)	\
template<>					\
struct ResolveRet<type> {	\
	static void resolve(lua_State* l, type value) {	\
		lua_pushinteger(l, value);					\
	}	\
};

RESOLVEINT(short)
RESOLVEINT(unsigned short)
RESOLVEINT(int)
RESOLVEINT(unsigned int)
RESOLVEINT(long)
RESOLVEINT(unsigned long)
RESOLVEINT(long long)
RESOLVEINT(unsigned long long)

#undef RESOLVEINT

template<>
struct ResolveRet<float> {
	static void resolve(lua_State* l, float value) {
		lua_pushnumber(l, value);
	}
};

template<>
struct ResolveRet<double> {
	static void resolve(lua_State* l, double value) {
		lua_pushnumber(l, value);
	}
};

template<int N, typename Ret, typename ...Args>
struct CLuaFun {
	template<typename ...Param>
	static int body(lua_State* pState, Param ...args) {
		return CLuaFun<N - 1, Ret, Args...>::body(
			pState,
			ResolveArgs<std::tuple_element<N - 1, std::tuple<Args...>>::type>::resolve(pState, N),
			args...);
	}
};

template<typename Ret, typename ...Args>
struct CLuaFun<0, Ret, Args...> {
	template<typename ...Args>
	static int body(lua_State* pState, Args... args) {
		auto fpointer = (Ret(*)(Args...))lua_touserdata(pState, lua_upvalueindex(1));
		ResolveRet<Ret>::resolve(pState, fpointer(args...));
		return 1;
	}
};

template<typename ...Args>
struct CLuaFun<0, void, Args...> {
	template<typename ...Args>
	static int body(lua_State* pState, Args... args) {
		auto fpointer = (void(*)(Args...))lua_touserdata(pState, lua_upvalueindex(1));
		return 0;
	}
};

template<int N, typename Cls, typename ...Args>
struct CLuaCXXConstructorFun {
	template<typename ...Param>
	static int body(lua_State* pState, Param ...args) {
		return CLuaCXXConstructorFun<N - 1, Cls, Args...>::body(
			pState,
			ResolveArgs<std::tuple_element<N - 1, std::tuple<Args...>>::type>::resolve(pState, N + 1),
			args...);
	}
};

template<typename Cls, typename ...Args>
struct CLuaCXXConstructorFun<0, Cls, Args...> {
	template<typename ...Param>
	static int body(lua_State* pState, Param ...args) {
		auto obj = lua_newuserdata(pState, sizeof(Cls));
		lua_pushvalue(pState, 1);
		lua_setmetatable(pState, -2);
		new (obj) Cls(args...);
		return 1;
	}
};

template<int N, typename Cls, typename Ret, typename ...Args>
struct CLuaCXXMemberFun {
	template<typename ...Param>
	static int body(lua_State* pState, Param ...args) {
		return CLuaCXXMemberFun<N - 1, Cls, Ret, Args...>::body(
			pState,
			ResolveArgs<std::tuple_element<N - 1, std::tuple<Args...>>::type>::resolve(pState, N + 1),
			args...);
	}
};

template<typename Cls, typename Ret, typename ...Args>
struct CLuaCXXMemberFun<0, Cls, Ret, Args...> {
	template<typename ...Args>
	static int body(lua_State* pState, Args... args) {
		typedef Ret(Cls::*FnPtr)(Args...);
		auto fpointer = *reinterpret_cast<FnPtr*>(lua_touserdata(pState, lua_upvalueindex(1)));
		auto obj = (Cls*)lua_touserdata(pState, 1);
		ResolveRet<Ret>::resolve(pState, (obj->*fpointer)(args...));
		return 1;
	}
};

template<typename Cls, typename ...Args>
struct CLuaCXXMemberFun<0, Cls, void, Args...> {
	template<typename ...Args>
	static int body(lua_State* pState, Args... args) {
		typedef void(Cls::*FnPtr)(Args...);
		auto fpointer = *reinterpret_cast<FnPtr*>(lua_touserdata(pState, lua_upvalueindex(1)));
		auto obj = (Cls*)lua_touserdata(pState, 1);
		(obj->*fpointer)(args...);
		return 0;
	}
};

template<typename Cls>
struct CLuaCXXDeconstructor {
	static int body(lua_State* pState) {
		auto obj = (Cls*)lua_touserdata(pState, -1);
		if(obj) obj->~Cls();
		return 0;
	}
};

template<typename Cls>
struct ClassInfo {
	static std::string name;
};

template<typename Cls>
std::string ClassInfo<Cls>::name;

class CLua {
private:

public:
	CLua(lua_State* pState, const char* pTable = "") :
		_state(pState),
		_table(pTable) {
		if (pTable != "") {
			lua_newtable(_state);
			lua_setglobal(_state, pTable);
		}
	}

	template<typename Cls>
	void registerClass(const char* pName, const char* pParent = "") {
		ClassInfo<Cls>::name = pName;
		std::string pname = pParent;
		if (_table.empty()) {
			if (lua_getglobal(_state, pName) != LUA_TNIL)
				throw CLuaException("Clua.registerClass:  Value exists in global!");
			if (!pname.empty())
				if (lua_getglobal(_state, pParent) != LUA_TTABLE)
					throw CLuaException("Clua.registerClass:  Parent doesn't exists in global!");
			lua_settop(_state, 0);
			lua_newtable(_state);
			lua_pushvalue(_state, -1);
			lua_setfield(_state, -2, "__index");
			lua_pushcclosure(_state, CLuaCXXDeconstructor<Cls>::body, 0);
			lua_setfield(_state, -2, "__gc");
			if (!pname.empty()) {
				lua_getglobal(_state, pParent);
				lua_setmetatable(_state, -2);
			}
			lua_setglobal(_state, pName);
		} else {
			lua_getglobal(_state, _table.c_str());
			if (lua_getfield(_state, -1, pName) != LUA_TNIL)
				throw CLuaException("Clua.registerClass:  Value exists in global!");
			if (!pname.empty())
				if (lua_getfield(_state, -1, pParent) != LUA_TTABLE)
					throw CLuaException("Clua.registerClass:  Parent doesn't exists in global!");
			lua_settop(_state, 0);
			lua_getglobal(_state, _table.c_str());
			lua_newtable(_state);
			lua_pushvalue(_state, -1);
			lua_setfield(_state, -2, "__index");
			lua_pushcclosure(_state, CLuaCXXDeconstructor<Cls>::body, 0);
			lua_setfield(_state, -2, "__gc");
			if (!pname.empty()) {
				lua_getfield(_state, -2, pParent);
				lua_setmetatable(_state, -2);
			}
			lua_setfield(_state, -2, pName);
		}
	}

	template<typename Cls, typename ...Args>
	void registerCXXConstructorFuntion(const char* pName) {
		if (ClassInfo<Cls>::name.empty())
			throw CLuaException("Clua.registerCXXConstructorFuntion: Class doesn't exists in global!");
		if (_table.empty()) {
			lua_getglobal(_state, ClassInfo<Cls>::name.c_str());
			lua_CFunction fpointer = CLuaCXXConstructorFun<sizeof...(Args), Cls, Args...>::body;
			lua_pushcclosure(_state, fpointer, 0);
			lua_setfield(_state, -2, pName);
		} else {
			/// pass
		}
	}

	template<typename Cls, typename Ret, typename ...Args>
	void registerCXXMemberFuntion(const char* pName, Ret(Cls::*pFun)(Args...)) {
		typedef Ret(Cls::*FnPtr)(Args...);
		if (ClassInfo<Cls>::name.empty())
			throw CLuaException("Clua.registerCXXMemberFuntion: Class doesn't exists in global!");
		if (_table.empty()) {
			lua_getglobal(_state, ClassInfo<Cls>::name.c_str());
			lua_CFunction fpointer = CLuaCXXMemberFun<sizeof...(Args), Cls, Ret, Args...>::body;
			new (lua_newuserdata(_state, sizeof(FnPtr))) FnPtr(pFun);
			lua_pushcclosure(_state, fpointer, 1);
			lua_setfield(_state, -2, pName);
		} else {
			lua_getglobal(_state, _table.c_str());
			lua_getfield(_state, -1, ClassInfo<Cls>::name.c_str());
			lua_CFunction fpointer = CLuaCXXMemberFun<sizeof...(Args), Cls, Ret, Args...>::body;
			new (lua_newuserdata(_state, sizeof(FnPtr))) FnPtr(pFun);
			lua_pushcclosure(_state, fpointer, 1);
			lua_setfield(_state, -2, pName);
		}
	}

	template<typename Cls, typename Ret, typename ...Args>
	void registerCXXMemberFuntion(const char* pName, Ret(Cls::*pFun)(Args...) const) {
		typedef Ret(Cls::*FnPtr)(Args...) const;
		if (ClassInfo<Cls>::name.empty())
			throw CLuaException("Clua.registerCXXMemberFuntion: Class doesn't exists in global!");
		if (_table.empty()) {
			lua_getglobal(_state, ClassInfo<Cls>::name.c_str());
			lua_CFunction fpointer = CLuaCXXMemberFun<sizeof...(Args), Cls, Ret, Args...>::body;
			new (lua_newuserdata(_state, sizeof(FnPtr))) FnPtr(pFun);
			lua_pushcclosure(_state, fpointer, 1);
			lua_setfield(_state, -2, pName);
		} else {
			lua_getglobal(_state, _table.c_str());
			lua_getfield(_state, -1, ClassInfo<Cls>::name.c_str());
			lua_CFunction fpointer = CLuaCXXMemberFun<sizeof...(Args), Cls, Ret, Args...>::body;
			new (lua_newuserdata(_state, sizeof(FnPtr))) FnPtr(pFun);
			lua_pushcclosure(_state, fpointer, 1);
			lua_setfield(_state, -2, pName);
		}
	}

	template<typename Ret, typename ...Args>
	void registerCFuntion(const char* pName, Ret(*pFun)(Args...)) {
		if (_table.empty()) {
			lua_pushlightuserdata(_state, pFun);
			lua_CFunction fpointer = CLuaFun<sizeof...(Args), Ret, Args...>::body;
			lua_pushcclosure(_state, fpointer, 1);
			/*if (lua_getglobal(_state, pName) != LUA_TNIL)
				throw CLuaException("Clua.registerCFunction: Value exists in global!");
			lua_pop(_state, 1);*/
			lua_setglobal(_state, pName);
		} else {
			if (lua_getglobal(_state, _table.c_str()) != LUA_TTABLE)
				throw CLuaException("Clua.registerCFuntion: Can't find table!");
			/*if (lua_getfield(_state, -1, pName) != LUA_TNIL)
				throw CLuaException("Clua.registerCFunction: Value exists in table!");*/
			lua_pushlightuserdata(_state, pFun);
			lua_CFunction fpointer = CLuaFun<sizeof...(Args), Ret, Args...>::body;
			lua_pushcclosure(_state, fpointer, 1);
			lua_setfield(_state, -2, pName);
		}
	}

private:
	lua_State*	_state;
	std::string _table;
};