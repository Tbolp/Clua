# Clua

Clua is used to register c++ classes and functions in lua.

# Example

## Register Function
<main.cpp>
```
int add(int i, int j) {
	return i + j;
}
int main(){
	lua_State* L;
	L = luaL_newstate();
	luaL_openlibs(L);
	clua_register<int(*)(int, int), add>(L, "add");
	luaL_dofile(L, "hello.lua");
	lua_close(L); 
}
```
<hello.lua>
```
print(add(1, 2))
```
## Register Class
<main.cpp>
```
class Test {
public:
	Test(int id = 13):id(id) {}
	void setID(int id) { this->id = id; }
	int getID() { return this->id; }
private:
	int id;
};

int main(){
	lua_State* L;
	L = luaL_newstate();
	luaL_openlibs(L);
	clua_class<Test>::class_name = "Test";
	clua_class<Test>::registerClass(L);
	clua_class<Test>::registerConstructor<int>(L);
	clua_class<Test>::registerMemberFun<void(Test::*)(int), &Test::setID>(L, "setID");
	clua_class<Test>::registerMemberFun<int(Test::*)(), &Test::getID>(L, "getID");
	luaL_dofile(L, "hello.lua");
	lua_close(L);   
}
```
<hello.lua>
```
a = Test.new(32)
print(a:getID())
a:setID(15)
print(a:getID())
```