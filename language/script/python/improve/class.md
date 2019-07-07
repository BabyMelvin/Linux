# 1.类和实例变量
实例变量用于每一个实例嗾使唯一的数据,类变量用于类的所有实例共享的属性和方法

```python
class Dog:
	"""
	08-clas variant,shared by all instances
	"""
	tricks = []
	kind = 'canine'
	
	def __init__(self, name):
		self.name = name

	def add_trick(self, tricks):
		self.tricks.append(tricks)

d = Dog('Fido')
e = Dog('Buddy')
print(d.kind)
# 类名直接调用有效
Dog.kind = 'hhhhhh'
print(d.name)
print(e.name)
d.kind = 'SHE'
print(e.kind)
```

# 2.magic method

```python
class Vector2D:
    def __init__(self, x, y):
        self.x = x
        self.y = y

    def __add__(self, other):
        return Vector2D(self.x + other.x, self.y + other.y)

    def __sub__(self, other):
        return Vector2D(self.x - other.x, self.y - other.y)

    def __mul__(self, other):
        return Vector2D(self.x * other.x, self.y * other.y)

    def __truediv__(self, other):
        return Vector2D(self.x / other.x, self.y / other.x)

    def __floordiv__(self, other):
        return Vector2D(self.x // other.x, self.y // other.y)

    def __mod__(self, other):
        return Vector2D(self.x % other.x, self.y % other.y)

    def __pow__(self, power, modulo=None):
        return Vector2D(self.x ** power, self.y ** power)

    def __and__(self, other):
        return Vector2D(self.x & other.x, self.y & other.y)

    def __or__(self, other):
        return Vector2D(self.x | other.x, self.y | other.y)

    def __xor__(self, other):
        return Vector2D(self.x ^ other.x, self.y ^ other.y)

    def __lt__(self, other) -> "<":
        pass

    def __le__(self, other) -> "<=":
        pass

    def __getitem__(self, item) -> "get indexing":
        pass

    def __contains__(self, item) -> "for in":
        pass
```

# 3.对象声明周期

1. 定义属于哪个类
2. 实例化对象，__init__
3. 在内存分配之前调用__new__方法
4. 销毁对下岗__del__

# 4.类方法声明
由类调用，并且传递类参数

```python
class Rectangle:
	def __init(self, width, height):
		self.width = width
		self.height = height

	def calculate_area(self):
		return self.width * self.height

	@classmethod
	def new_square(cls, side_length)
		return cls(side_lengh, side_length)

square = Rectangle.new_square(5)
print(square.calculate_area())
```

# 5.静态方法
静态方法和类方法类似，除了不接受额外参数。想普通方法属于类的。

```python
class Pizza:
    def __init__(self, toppings):
        self.toppings = toppings

    @staticmethod
    def validate_topping(topping):
        if topping == "pineaple":
            raise ValueError("No pineapppls")
        else:
            return True

ingredients = ["cheese", "onions", "spam"]
if all(Pizza.validate_topping(i) for i in ingredients):
    pizza = Pizza(ingredients)
```

# 6.属性被设为只读,不能改变
属性被设为只读,不能改变
`pizza.pineapple_allowed=True`属性可以根据下面的方法进行设置`setter/getter`

```python
class Pizza1:
    def __init__(self, topping):
        self.topping = topping
        self._pineapple_allowed = False

    @property
    def pineapple_allowed(self):
        return self._pineapple_allowed

    @pineapple_allowed.setter
    def pineapple_allowed(self, value):
        if value:
            password = input("Enter the password!")
            if password == "SwOrdf1sh":
                self._pineapple_allowed = value
            else:
                raise ValueError("Alert intruder!")


pizza = Pizza1(["cheese", "tomato"])
print(pizza.pineapple_allowed)
pizza.pineapple_allowed = True
print(pizza.pineapple_allowed)
```