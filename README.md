# Clua

Clua is used to register c++ classes and functions in lua.

# Example

main.cpp
```
int main(){
	using namespace std;
	unique_ptr<lua_State, decltype(&lua_close)> lua_state_mgr(luaL_newstate(), lua_close);
	lua_State* state = lua_state_mgr.get();
	luaL_openlibs(state);

	// all variable in global
	CLua clua(state);

	// register c function
	clua.registerCFunction("f1", static_cast<void(*)()>([]()->void {
		ogger::WriteMessage("f1 with no return value and args");
	}));

	// register c++ class
	clua.registerClass<vector<int>>("vector");
	clua.registerCXXConstructorFunction<vector<int>, size_t>("new");

	// register c++ member function
	clua.registerCXXMemberFunction<vector<int>>(
		"pop_back",
		&vector<int>::pop_back
	);

	// register c function as c++ member function
	clua.registerCXXMemberFunction<vector<int>>(
		"push_back", 
		static_cast<void(*)(vector<int>*, int)>([](vector<int>* p, int v) {
			p->push_back(v);
	}));

	// register c++ instance
	vector<int> v;
	v.push_back(2);
	clua.registerCXXInstance("c_vec", &v);

	return luaL_dofile(state, "test.lua");
}
```

test.lua
```
f1()
v = vector:new(5)
v:pop_back()
v:push_back(1)
vector.push_back(c_vec, 3)
```