class Number:
    def __init__(self):
        self._a = None

    @property
    def a(self):
        return self._a

    @a.setter
    def a(self, a: int):
        if isinstance(a, int):
            self._a = a


def main():
    integer = Number()
    print(f"The value of a is: {integer.a}")
    integer.a = int(input("\n Enter a new value for a: "))
    print("\n The new value of a is :", integer.a)


if __name__ == "__main__":
    main()
