// =============================================================================
// object_oriented.cpp — OOP Primer (Beginner Level)
// =============================================================================
//
// Topics covered (in order):
//   1.  Classes vs Objects — the blueprint analogy
//   2.  Access modifiers — private, public, protected
//   3.  Member initializer lists — the right way to construct
//   4.  Constructors — default, parameterized, copy
//   5.  Destructor — cleanup when an object dies
//   6.  const member functions — read-only contracts
//   7.  The `this` pointer — referring to yourself
//   8.  Getters and setters — controlled access to private data
//   9.  Stack vs Heap — where your object lives in memory
//  10.  Passing objects to functions — the reference rules
//  11.  Static members — shared state across all instances
//  12.  Putting it all together — a realistic mini-example
//
// Compile: g++ -std=c++17 -Wall -Wextra -g -o oop object_oriented.cpp
// Run:     ./oop
// =============================================================================

#include <iostream>
#include <string>

// =============================================================================
// SECTION 1: Classes vs Objects
// =============================================================================
// A CLASS is a blueprint — it defines what data an object holds and what
// operations it can do. An OBJECT is a concrete instance built from that blueprint.
//
// Python analogy:
//   class Dog:                  →   class Dog {
//       def __init__(self):     →       Dog() {}        ← constructor
//           self.name = ""      →       std::string name;
//
// Key C++ difference: you declare the TYPE of every member variable upfront.
// The compiler uses this to compute exactly how many bytes the object needs
// before your program even runs. This is why C++ is fast — no runtime type
// guessing, no dynamic resizing of the object itself.

// =============================================================================
// SECTION 2: Access Modifiers
// =============================================================================
// private   — only the class itself can read/write these (default in `class`)
// public    — anyone can read/write these
// protected — class + subclasses (covered in oop_advanced.cpp)
//
// Rule of thumb: data = private, functions = public.
// This is called ENCAPSULATION — you control HOW your data is accessed,
// so you can validate inputs, add logging, or change internals later without
// breaking code that uses your class.

class Dog {
private:
    // These live inside the Dog object in memory (on stack or heap, wherever
    // the Dog itself lives). No one outside can touch them directly.
    std::string name;
    int age;
    double weight_kg;

public:

    // =========================================================================
    // SECTION 3: Member Initializer List
    // =========================================================================
    // The `: name(n), age(a), weight_kg(w)` part after the constructor
    // signature is called a MEMBER INITIALIZER LIST.
    //
    // WHY use it instead of assigning inside the body {}?
    // Because without it, C++ first DEFAULT-CONSTRUCTS the member (wastes time),
    // then ASSIGNS it (more work). With the initializer list, it constructs
    // directly into the final value — one step instead of two.
    //
    // For std::string this matters. For int/double it's trivial but still
    // the correct habit. In high-performance code, this adds up.
    //
    // ALWAYS use member initializer lists. Make it muscle memory.

    // =========================================================================
    // SECTION 4a: Default Constructor
    // =========================================================================
    // Called when you write: Dog d;
    // No arguments. Sets members to sensible defaults.
    Dog() : name("Unknown"), age(0), weight_kg(0.0) {
        std::cout << "[Dog] Default constructor — created unnamed dog\n";
    }

    // =========================================================================
    // SECTION 4b: Parameterized Constructor
    // =========================================================================
    // Called when you write: Dog d("Rex", 4, 28.5);
    //
    // MEMORY TIP — const std::string& n  (read this carefully):
    //
    //   std::string n        — COPIES the entire string into a new variable.
    //                          If the string is 100 characters, that's 100 bytes
    //                          allocated and copied just to call this function.
    //
    //   const std::string& n — passes a REFERENCE (essentially a pointer, 8 bytes).
    //                          No copy. The & means "alias for the original".
    //                          The const means "I promise not to modify it".
    //
    // Rule: for any parameter larger than a pointer (8 bytes on 64-bit systems),
    // pass by const reference. This means: std::string, std::vector, any class,
    // any struct. For int, double, bool, char — copy is fine (they're tiny).
    //
    // In HPC/AI code this is critical. Passing a 1MB weight tensor by value
    // would copy 1MB on every function call. That kills performance.
    Dog(const std::string& n, int a, double w)
        : name(n), age(a), weight_kg(w) {
        std::cout << "[Dog] Parameterized constructor — created: " << name << "\n";
    }

    // =========================================================================
    // SECTION 4c: Copy Constructor
    // =========================================================================
    // Called when you write: Dog d2 = d1;  or  Dog d2(d1);
    // C++ generates a default one that copies all members, but it's important
    // to know it exists — especially once you manage heap memory manually,
    // where a shallow copy of a pointer causes double-free bugs.
    //
    // const Dog& other — we read from `other` without copying the Dog itself.
    Dog(const Dog& other)
        : name(other.name), age(other.age), weight_kg(other.weight_kg) {
        std::cout << "[Dog] Copy constructor — copied: " << name << "\n";
    }

    // =========================================================================
    // SECTION 5: Destructor
    // =========================================================================
    // Called automatically when the object goes out of scope (stack) or is
    // `delete`d (heap). Tilde prefix, no arguments, no return type.
    //
    // Use it to: free heap memory, close file handles, release GPU memory,
    // disconnect from sockets. This is the C++ RAII pattern — resources are
    // tied to object lifetime and cleaned up deterministically.
    //
    // Our Dog doesn't own any heap memory yet, so the body is empty.
    // When we cover smart pointers and GPU buffers, destructors become critical.
    ~Dog() {
        std::cout << "[Dog] Destructor — destroyed: " << name << "\n";
    }

    // =========================================================================
    // SECTION 6: const Member Functions
    // =========================================================================
    // The `const` after the parameter list means: this function will NOT modify
    // any member variables of this object. The compiler enforces this.
    //
    // Always mark getters and any read-only function as const. This lets you:
    //   1. Pass objects as `const Dog&` to functions without losing functionality
    //   2. Catch accidental mutations at compile time (free bug detection)
    //   3. Signal to callers that calling this is safe and has no side effects
    //
    // Returning `const std::string&` — returns a reference to the internal string.
    // No copy. The caller can read it but not mutate it (const). This is the
    // correct pattern for returning member strings/vectors from getters.
    const std::string& get_name() const { return name; }
    int get_age() const { return age; }
    double get_weight() const { return weight_kg; }

    // =========================================================================
    // SECTION 7 & 8: The `this` Pointer and Setters
    // =========================================================================
    // `this` is a pointer to the current object. Inside a member function,
    // `this->name` and `name` refer to the same thing — UNLESS a parameter
    // shadows the member name, as below.
    //
    // Setters also give you a place to validate input before writing to private
    // data. You'd never get that if the member were public.
    void set_name(const std::string& name) {
        this->name = name;     // this->name = member variable, name = parameter
    }

    void set_age(int age) {
        if (age >= 0 && age <= 30)
            this->age = age;
        else
            std::cout << "[Dog] Invalid age rejected: " << age << "\n";
    }

    void set_weight(double weight_kg) {
        if (weight_kg > 0.0)
            this->weight_kg = weight_kg;
    }

    // =========================================================================
    // Regular member functions
    // =========================================================================
    // MEMORY TIP: takes `const Dog& other` — we read from `other`, no copy.
    // Both `this` object and `other` are const — neither gets modified.
    void compare_weight(const Dog& other) const {
        if (weight_kg > other.weight_kg)
            std::cout << name << " is heavier than " << other.name << "\n";
        else if (weight_kg < other.weight_kg)
            std::cout << name << " is lighter than " << other.name << "\n";
        else
            std::cout << name << " and " << other.name << " weigh the same\n";
    }

    void display() const {
        std::cout << "  Dog { name: \"" << name
                  << "\", age: " << age
                  << ", weight: " << weight_kg << " kg }\n";
    }
};

// =============================================================================
// SECTION 9: Stack vs Heap — Where Does Your Object Live?
// =============================================================================
//
// STACK (automatic storage):
//   Dog rex("Rex", 4, 28.5);
//   - Object lives in the function's stack frame
//   - Destroyed AUTOMATICALLY when the {} block it lives in closes
//   - Fast allocation (just move the stack pointer)
//   - Size must be known at compile time
//
// HEAP (dynamic storage):
//   Dog* rex = new Dog("Rex", 4, 28.5);
//   - Object lives on the heap until YOU free it
//   - Destructor NOT called until you call `delete rex`
//   - Slower allocation (OS call), but size can be decided at runtime
//   - If you forget `delete`, you have a MEMORY LEAK
//
// Modern C++ (Phase 2) replaces raw `new`/`delete` with smart pointers
// (std::unique_ptr, std::shared_ptr) that delete automatically.
// For now: every `new` must have exactly one `delete`.
//
// HPC relevance: GPU memory (cudaMalloc) follows the same heap pattern.
// In CUDA, you'll write cudaMalloc/cudaFree for GPU buffers — same concept.

void demonstrate_heap() {
    std::cout << "\n--- Heap allocation ---\n";
    Dog* heap_dog = new Dog("Heap Hound", 5, 30.0);    // constructor runs
    heap_dog->display();                                // -> dereferences pointer
    delete heap_dog;                                    // destructor runs here
    heap_dog = nullptr;   // defensive: makes accidental use of deleted pointer
                          // crash immediately rather than silently corrupt memory
}

// =============================================================================
// SECTION 10: Passing Objects to Functions — The Parameter Rules
// =============================================================================
//
// There are three ways to pass an object to a function. Only two are good.
//
//   void f(Dog d)          — COPY: entire object duplicated. Avoid for large types.
//   void f(const Dog& d)   — CONST REF: read-only alias. No copy. Use for inputs.
//   void f(Dog& d)         — REF: mutable alias. No copy. Use when you need to modify.
//
// Ask yourself: "Does this function need to modify the object?"
//   No  → const Dog&
//   Yes → Dog&
//   It's a tiny primitive (int, char) → just copy it
//
// This is the single most important C++ memory habit. Get it right from day one.

void print_dog(const Dog& d) {          // read-only: const reference
    d.display();
}

void birthday(Dog& d) {                 // mutates: plain reference
    d.set_age(d.get_age() + 1);
    std::cout << "  Happy birthday, " << d.get_name()
              << "! Now " << d.get_age() << " years old.\n";
}

// =============================================================================
// SECTION 11: Static Members
// =============================================================================
// A `static` member variable belongs to the CLASS, not to any object.
// All instances share the same static variable.
//
// Common uses:
//   - Counting how many instances exist (shown below)
//   - Shared configuration or registry
//   - In AI frameworks: tracking allocated tensor memory globally
//
// Static member functions can be called without an object: Counter::get_count()
// They have no `this` pointer — they can only touch static members.

class Counter {
private:
    static int count;     // declared here, defined below the class
    int id;

public:
    Counter() : id(++count) {
        std::cout << "  Counter #" << id << " born.  Total alive: " << count << "\n";
    }
    ~Counter() {
        --count;
        std::cout << "  Counter #" << id << " died.   Total alive: " << count << "\n";
    }
    static int get_count() { return count; }
};

// Static members must be defined (given storage) exactly once, outside the class.
int Counter::count = 0;

// =============================================================================
// SECTION 12: Realistic Mini-Example
// =============================================================================
// A Tensor class skeleton — the kind of thing you'd find in a deep learning
// framework (PyTorch, TensorRT). It uses everything from above.
// We fake the data with a size field — no actual heap allocation yet
// (that comes in the memory management lesson).

class Tensor {
private:
    std::string name;
    int rows;
    int cols;
    bool on_gpu;

public:
    Tensor(const std::string& name, int rows, int cols, bool on_gpu = false)
        : name(name), rows(rows), cols(cols), on_gpu(on_gpu) {
        std::cout << "[Tensor] Allocated \"" << name << "\" ("
                  << rows << "x" << cols << ") on "
                  << (on_gpu ? "GPU" : "CPU") << "\n";
    }

    ~Tensor() {
        std::cout << "[Tensor] Released \"" << name << "\"\n";
        // In real code: if (on_gpu) cudaFree(data_ptr);
        //               else         free(data_ptr);
    }

    int num_elements() const { return rows * cols; }
    const std::string& get_name() const { return name; }

    void describe() const {
        std::cout << "  Tensor \"" << name << "\": shape=(" << rows << ", " << cols
                  << "), elements=" << num_elements()
                  << ", device=" << (on_gpu ? "GPU" : "CPU") << "\n";
    }

    // Matrix multiply shape check — const ref inputs, no copies
    static bool can_matmul(const Tensor& a, const Tensor& b) {
        return a.cols == b.rows;
    }
};

// =============================================================================
// MAIN
// =============================================================================
int main() {

    // -------------------------------------------------------------------------
    std::cout << "\n====== SECTION 4: Constructors ======\n";
    Dog unnamed;                         // default constructor
    Dog rex("Rex", 4, 28.5);            // parameterized constructor
    Dog rex_copy = rex;                  // copy constructor

    // -------------------------------------------------------------------------
    std::cout << "\n====== SECTION 6-8: Getters, Setters, this ======\n";
    rex.display();
    rex.set_age(5);
    rex.set_name("Rex the Great");
    rex.set_age(-1);                     // rejected by setter validation
    rex.display();

    // -------------------------------------------------------------------------
    std::cout << "\n====== SECTION 9: Stack vs Heap ======\n";
    demonstrate_heap();
    // rex, unnamed, rex_copy are all on the stack — destructors fire
    // automatically at the end of main() in reverse order of construction.

    // -------------------------------------------------------------------------
    std::cout << "\n====== SECTION 10: Passing to Functions ======\n";
    unnamed.set_name("Buddy");
    unnamed.set_age(2);
    unnamed.set_weight(15.0);
    print_dog(rex);                      // const ref — no copy
    birthday(unnamed);                   // ref — mutates unnamed
    rex.compare_weight(unnamed);

    // -------------------------------------------------------------------------
    std::cout << "\n====== SECTION 11: Static Members ======\n";
    std::cout << "Before block: " << Counter::get_count() << " counters alive\n";
    {
        Counter a, b, c;
        std::cout << "  Inside block: " << Counter::get_count() << " alive\n";
    }   // a, b, c destroyed here (LIFO: c first, then b, then a)
    std::cout << "After block:  " << Counter::get_count() << " counters alive\n";

    // -------------------------------------------------------------------------
    std::cout << "\n====== SECTION 12: Tensor Skeleton ======\n";
    Tensor weights("fc1.weight", 512, 784);
    Tensor activations("relu_out", 512, 1, false);
    Tensor gpu_buf("embed", 128, 64, true);

    weights.describe();
    activations.describe();
    gpu_buf.describe();

    std::cout << "Can matmul weights x activations? "
              << (Tensor::can_matmul(weights, activations) ? "yes" : "no") << "\n";

    // -------------------------------------------------------------------------
    std::cout << "\n====== END: Stack objects destroyed (reverse order) ======\n";
    return 0;
    // gpu_buf, activations, weights, unnamed, rex_copy, rex, unnamed
    // destructors fire automatically here in reverse construction order.
}

// =============================================================================
// WHAT'S NEXT: oop_advanced.cpp
// =============================================================================
// Now that you understand the basics of a single class, the next file covers:
//   - Inheritance: a Poodle IS-A Dog (subclasses)
//   - Polymorphism: treating a Poodle as a Dog generically
//   - Virtual functions: runtime dispatch (how PyTorch's Module system works)
//   - Abstract classes / interfaces: defining contracts
//   - Operator overloading: making Tensor + Tensor work naturally
// =============================================================================
