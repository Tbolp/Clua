# Clua

Clua is used to register c++ classes and functions in lua.

# Example

```
int add(int p1, int p2) {
	return p1 + p2;
}

class Point {
public:
	Point(double pX, double pY):
		_x(pX),_y(pY){}
	void add(Point pPoint) {
		_x += pPoint._x;
		_y += pPoint._y;
	}
private:
	double _x, _y;
};

int main() {
	lua_State* l = luaL_newstate();
	CLua reg(l);
	reg.registerCFuntion("c_add", add);
	reg.registerClass<Point>("CPoint");
	reg.registerCXXConstructorFuntion<Point, double, double>("new");
	reg.registerCXXMemberFuntion("add", &Point::add);
	luaL_dostring(l, "					\
		res = c_add(4, 5);				\
		p1 = CPoint:new(res, res+10);			\
		p2 = CPoint:new(2, 4.5);			\
		p1:add(p2);");
	lua_close(l);
	return 0;
}
```