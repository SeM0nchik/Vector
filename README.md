
# Vector – A Header-Only Dynamic Array Implementation

This project is a **C++ header-only implementation of a dynamic array container (`Vector`)**, built from scratch as part of an academic project.  
The goal was to understand memory management, iterators, exception safety, and generic programming in C++ while recreating a simplified yet powerful version of `std::vector`.

---

## ✨ Features

- **Header-only design** – include and use directly, no compilation step required.
- **Dynamic memory management** with manual allocation/deallocation.
- **Iterator support**:
  - Forward, const, reverse, and const-reverse iterators.
- **Exception safety** in constructors, copy/move semantics, and modifiers.
- **Capacity management**:
  - `Reserve`, `ShrinkToFit`, `Resize`.
- **Element access**:
  - `At`, `Front`, `Back`, `Data`, `operator[]`.
- **Modifiers**:
  - `PushBack`, `PopBack`, `EmplaceBack`, `Clear`, `Swap`.
- **Comparison operators**:
  - `==, !=, <, <=, >, >=`.
- **RAII-compliant** with proper destructors, copy/move constructors, and assignment operators.

---

## 🚀 Usage

Since this project is header-only, simply include it in your project:

```cpp
#include "vector.h"
#include <iostream>

int main() {
    Vector<int> v;

    v.PushBack(10);
    v.PushBack(20);
    v.EmplaceBack(30);

    for (auto &el : v) {
        std::cout << el << " ";
    }
    // Output: 10 20 30
}
````


## 📚 Learning Outcomes

Through building this project, I gained experience in:

* **Manual memory management** (`new`, `delete`, placement new, destruction).
* **Template metaprogramming** to create a generic container.
* **Iterator design** following STL conventions.
* **Strong exception safety guarantees** in resource acquisition and release.
* **Performance optimization** via capacity management (`Reserve`, doubling strategy).

---

## 🔧 Build & Integration

* **Requirements:** C++17 or later.
* **Integration:** Just add the `vector.h` file to your include path.

Example with `g++`:

```bash
g++ -std=c++17 main.cpp -o main
```

---

## 📌 Notes

* This is a **student project** and is **not intended to replace `std::vector`** in production.
* Implemented purely for **academic and learning purposes**, to demonstrate knowledge of C++ internals and STL-like design.


