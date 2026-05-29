# C++ Mastery Handbook
### A living textbook — updated as you learn

> Every time we create a new tutorial file or you ask a question, this handbook grows.
> Read it top to bottom once, then use it as a reference.

---

## Table of Contents

1. [Before the Code — How Computers Actually Work](#chapter-1-before-the-code--how-computers-actually-work)
2. [The Linux Kernel — What It Does For Your Program](#chapter-2-the-linux-kernel--what-it-does-for-your-program)
3. [The Compilation Pipeline — From Text to Executable](#chapter-3-the-compilation-pipeline--from-text-to-executable)
4. [Memory — The Most Important Mental Model in All of C++](#chapter-4-memory--the-most-important-mental-model-in-all-of-c)
5. [Variables and Types](#chapter-5-variables-and-types)
6. [Functions and the Call Stack](#chapter-6-functions-and-the-call-stack)
7. [Pointers and References](#chapter-7-pointers-and-references)
8. [Classes and Object-Oriented Programming](#chapter-8-classes-and-object-oriented-programming)
9. [Runtime Behaviour — What Actually Happens When You Run ./oop](#chapter-9-runtime-behaviour--what-actually-happens-when-you-run-oop)
10. [Glossary](#glossary)

---

## Chapter 1: Before the Code — How Computers Actually Work

Before writing a single line of C++, you need a mental model of the machine your code runs on. Without this, the language feels like magic. With it, everything makes sense.

### 1.1 The Hardware Layer

A computer, at its core, is three things:

```
┌─────────────────────────────────────────────────────┐
│                        CPU                          │
│   Fetches instructions, executes them, one by one   │
│   (modern CPUs do this billions of times per second)│
└───────────────────┬─────────────────────────────────┘
                    │ reads/writes data
┌───────────────────▼─────────────────────────────────┐
│                        RAM                          │
│   Temporary storage while the program is running    │
│   Fast, but loses everything when power is cut      │
│   Your variables, your call stack, your objects     │
│   all live here while your program runs             │
└───────────────────┬─────────────────────────────────┘
                    │ persists data
┌───────────────────▼─────────────────────────────────┐
│                        DISK                         │
│   Permanent storage — your .cpp files live here     │
│   Your compiled executable lives here               │
│   Slow compared to RAM (1000x+), but survives reboot│
└─────────────────────────────────────────────────────┘
```

**The CPU** doesn't understand C++. It understands only one thing: machine code — a sequence of binary instructions specific to its architecture (x86-64 on your machine). Instructions like "add these two numbers", "store this value at that memory address", "jump to this other instruction".

**RAM (Random Access Memory)** is a giant numbered array. Every byte has an address — a number from 0 to however many bytes your RAM has. When your C++ code says `int x = 10;`, it is telling the CPU to store the number 10 at some address in RAM.

**Your disk** holds your `.cpp` source files and your compiled binary (`./oop`). When you run the binary, the OS copies it from disk into RAM and tells the CPU to start executing it.

### 1.2 What "Running a Program" Means

When you type `./oop` in your terminal:

```
1. Shell asks the OS kernel: "please run this program"
2. Kernel loads the binary from disk into RAM
3. Kernel creates a PROCESS — an isolated bubble for this program
4. Kernel sets the CPU's instruction pointer to the first instruction
5. CPU executes instructions one by one
6. Eventually main() returns, process exits, RAM is reclaimed
```

A **process** is your program while it's alive. It has its own private slice of RAM, its own CPU time, its own view of the world. Two processes cannot accidentally write into each other's memory — the kernel enforces this. This is why one crashing program doesn't usually take down others.

---

## Chapter 2: The Linux Kernel — What It Does For Your Program

"Kernel" gets overloaded in your learning journey — it will later mean a CUDA GPU function. But right now, **kernel** means the Linux kernel: the core of the operating system.

Think of the Linux kernel as the ultimate manager. It sits between your program and the actual hardware, and it controls everything.

### 2.1 What the Kernel Manages

```
┌─────────────────────────────────────────────────────────┐
│                    Your C++ Program                     │
│               (runs in "user space")                    │
└────────────────────────┬────────────────────────────────┘
                         │  system calls (requests to the OS)
┌────────────────────────▼────────────────────────────────┐
│                   Linux Kernel                          │
│            (runs in "kernel space")                     │
│                                                         │
│  • Process management — starts, stops, schedules procs  │
│  • Memory management  — gives RAM to processes          │
│  • File system        — reads/writes disk               │
│  • Device drivers     — talks to hardware (GPU, NIC...) │
│  • Security           — enforces process isolation      │
└────────────────────────┬────────────────────────────────┘
                         │
┌────────────────────────▼────────────────────────────────┐
│                     Hardware                            │
│              CPU, RAM, Disk, GPU, etc.                  │
└─────────────────────────────────────────────────────────┘
```

Your program lives in **user space**. It cannot touch hardware directly. Every time it needs something from the hardware — print to the terminal, allocate more memory, read a file — it makes a **system call**: a controlled request to the kernel.

### 2.2 What the Kernel Does At Runtime For Your Program

When your `./oop` runs, here is what the kernel silently does for you:

**At startup:**
- Reads the ELF binary (the compiled format on Linux) from disk
- Creates a virtual address space — a private memory map for the process
- Loads the program's code and initial data into RAM
- Sets up the stack (a region of RAM for function calls — explained in Chapter 4)
- Transfers control to your program's actual entry point

**While running:**
- Every time your code calls `std::cout << ...`, that eventually triggers a write() system call — the kernel writes your text to the terminal's file descriptor
- Every time your code calls `new` (heap allocation), the C++ runtime asks the kernel for memory using a system call called `brk` or `mmap`
- The kernel **schedules** your process — decides when it gets CPU time vs other processes running simultaneously on your machine
- If your program crashes (e.g., accesses memory it doesn't own — a segfault), the kernel sends a signal (SIGSEGV) that terminates the process

**At shutdown:**
- When `main()` returns 0, the C++ runtime calls `exit()`
- The kernel reclaims all the RAM that process was using
- File descriptors are closed
- The exit code is returned to the shell (you see this with `echo $?`)

### 2.3 Virtual Memory — Why Your Program Thinks It Has All the RAM

Here is something subtle but important: your process does not see real physical RAM addresses. The kernel gives every process a **virtual address space** — a private, contiguous view of memory that looks like it's all yours, from address 0 to some huge number.

```
What your program sees (virtual):        What RAM actually looks like (physical):
┌────────────────────────┐               ┌──────────────────────────────────────┐
│ 0xFFFFFFFF (high addr) │               │  Kernel's own memory                 │
│   Kernel space         │               │  Process A's stack (your ./oop)      │
│   (off limits to you)  │               │  Process B's heap (your browser)     │
├────────────────────────┤               │  Process C's code (your terminal)    │
│   Stack ↓              │  ──mapping──▶ │  ...scattered around...              │
│   (grows downward)     │               └──────────────────────────────────────┘
├────────────────────────┤
│   Heap ↑               │  The kernel maintains a translation table
│   (grows upward)       │  (called the page table) that maps your
├────────────────────────┤  virtual addresses to real physical ones.
│   Libraries (.so files)│  You never see this. It's transparent.
├────────────────────────┤
│   Program code         │
│   (your compiled .cpp) │
├────────────────────────┤
│   Global variables     │
│ 0x00000000 (low addr)  │
└────────────────────────┘
```

Why does this matter for HPC/CUDA? Because the GPU has its own completely separate memory with its own address space. When you eventually write CUDA code, you'll need to explicitly copy data between CPU memory and GPU memory — they cannot directly access each other. The mental model of "address spaces are isolated" is the foundation of that.

---

## Chapter 3: The Compilation Pipeline — From Text to Executable

You type C++. The CPU understands only machine code. Something has to translate. That something is `g++`, and it does it in four stages.

### 3.1 The Four Stages

```
your_file.cpp
      │
      ▼  Stage 1: PREPROCESSOR
      │  Handles all lines starting with #
      │  #include <iostream> → copy-pastes iostream's contents here
      │  #define MAX 100     → replaces every MAX with 100
      │  Result: a "translation unit" — pure C++ with no # directives
      │
      ▼  Stage 2: COMPILER
      │  Reads C++ and produces assembly language
      │  Assembly is human-readable machine instructions
      │  e.g.:  mov eax, 10      (put 10 in the eax register)
      │         call _ZNSt...    (call std::cout's operator<<)
      │  Also: type checking, error detection, optimization happen here
      │
      ▼  Stage 3: ASSEMBLER
      │  Converts assembly → machine code (binary)
      │  Result: a .o "object file" — binary, but not yet runnable
      │  Unresolved references (e.g., to std::cout) are left as placeholders
      │
      ▼  Stage 4: LINKER
         Combines your .o file with library .o files (like the C++ standard library)
         Fills in all the placeholder references
         Result: a final executable binary — this is your ./oop file
```

When you run `g++ -std=c++17 -Wall -Wextra -g -o oop object_oriented.cpp`, g++ does all four stages invisibly in sequence.

### 3.2 What the Flags Mean

```bash
g++ -std=c++17 -Wall -Wextra -g -o oop object_oriented.cpp
     │          │     │       │  │
     │          │     │       │  └─ output file name (./oop)
     │          │     │       └──── include debug symbols in the binary
     │          │     │             (needed for gdb debugger, crash reports)
     │          │     └──────────── enable extra warnings beyond -Wall
     │          └────────────────── enable all standard warnings
     └───────────────────────────── use C++ 2017 standard features
```

**Warnings vs Errors:**
- An **error** means g++ cannot produce a binary. Fix it before running.
- A **warning** means g++ produced a binary but suspects something is wrong. Always treat warnings as errors — they are almost always bugs. `-Wall -Wextra` turns on many more warning checks.

### 3.3 What Lives In the Binary

The compiled `./oop` file (an ELF binary on Linux) contains:
- **Code section** — the machine instructions (your compiled functions)
- **Data section** — global and static variables with initial values
- **BSS section** — global variables initialized to zero
- **Metadata** — symbol names (function names, variable names) when compiled with `-g`

The symbol names are what allows debuggers to say "error on line 42 of object_oriented.cpp" instead of "error at address 0x401234".

---

## Chapter 4: Memory — The Most Important Mental Model in All of C++

This chapter is the foundation of everything in C++. If you understand memory, you understand the language. If you don't, you'll fight mysterious bugs for years.

Your program's RAM is divided into four regions, each with a different purpose and different rules.

### 4.1 The Four Memory Regions

```
High addresses (e.g., 0x7FFF...)
┌─────────────────────────────────────────┐
│                 STACK                   │
│  Function calls, local variables        │
│  Grows DOWNWARD as functions are called │
│  Shrinks UPWARD as functions return     │
│  Fast. Size fixed at program start.     │
│  Automatically managed — no work needed │
├─────────────────────────────────────────┤
│         (unused / grows toward each)    │
├─────────────────────────────────────────┤
│                 HEAP                    │
│  Dynamically allocated memory           │
│  Grows UPWARD as you allocate           │
│  Manually managed — YOU control it      │
│  `new` allocates. `delete` frees.       │
│  Modern C++: smart pointers manage this │
├─────────────────────────────────────────┤
│            BSS SEGMENT                  │
│  Global/static variables = zero         │
├─────────────────────────────────────────┤
│            DATA SEGMENT                 │
│  Global/static variables with values    │
│  e.g., int Counter::count = 0;          │
├─────────────────────────────────────────┤
│            CODE SEGMENT                 │
│  Your compiled machine instructions     │
│  Read-only. Your functions live here.   │
└─────────────────────────────────────────┘
Low addresses (e.g., 0x0000...)
```

### 4.2 The Stack — Automatic Memory

The stack is RAM that is managed automatically by the CPU and the compiler. You never have to ask for it or release it.

**How it works:**
```
int main() {
    int x = 10;      // x lives on main()'s stack frame
    Dog rex("Rex");  // rex lives on main()'s stack frame
                     // rex's members (name, age, weight) are part of rex's bytes
    greet(rex);      // a new stack frame is pushed for greet()
    // greet() returns → its frame is popped, that memory is immediately reusable
}
// main() returns → its frame is popped, x and rex are gone
// Dog's destructor runs RIGHT BEFORE rex's memory is reclaimed
```

A **stack frame** is the chunk of stack memory allocated for one function call. It holds:
- The function's local variables
- The return address (where to jump back to when this function finishes)
- Saved register values

```
Stack during greet(rex):
┌──────────────────────────────┐  ← top of stack (lowest address currently used)
│ greet()'s stack frame        │
│   const Dog& d = &rex        │  (reference — just an address, 8 bytes)
│   [return address]           │  (where to go when greet() ends)
├──────────────────────────────┤
│ main()'s stack frame         │
│   rex: name="Rex"(24 bytes)  │
│         age=4    (4 bytes)   │
│         weight=28.5(8 bytes) │
│   x: 10          (4 bytes)   │
│   [return address]           │
└──────────────────────────────┘
```

**Key properties of the stack:**
- Allocation is O(1) — just subtract from a register (the stack pointer)
- Size is limited (typically 8 MB on Linux). Overflow = crash ("stack overflow")
- Lifetime = scope. The object dies when the `}` closes. Destructor runs automatically.
- You cannot return a pointer to a stack variable (it'll be gone after the function returns)

### 4.3 The Heap — Manual Memory

The heap is RAM that you explicitly request and explicitly release.

```cpp
Dog* d = new Dog("Rex", 4, 28.5);
//   ^   ^^^
//   │    └── asks the OS for enough bytes to hold a Dog, constructs it there
//   └──────── d is a pointer — an 8-byte variable on the stack holding an ADDRESS
//             d itself is on the stack. The Dog it points to is on the heap.

delete d;
//       └── runs Dog's destructor, then tells the OS: this memory is free again
d = nullptr; // defensive: makes any accidental use of d crash immediately
             // instead of silently corrupting data (undefined behaviour)
```

**What `new` actually does (step by step):**
1. Calculates how many bytes a `Dog` needs (`sizeof(Dog)`)
2. Makes a system call to ask the kernel for that many bytes
3. The kernel finds free RAM and records the allocation
4. Returns the address of that RAM to you as a pointer
5. Calls `Dog`'s constructor at that address

**What `delete` actually does:**
1. Calls the destructor on the object at that address
2. Tells the allocator this memory is free (to be reused)
3. Does NOT zero the memory or change the pointer — that's why you set it to `nullptr`

**What a MEMORY LEAK is:**
```cpp
Dog* d = new Dog("Rex");
// ... use d ...
// forget to delete d
// function returns — d (the pointer on the stack) is gone
// but the Dog on the heap is STILL THERE, forever, unreachable
// this memory is wasted until the process exits
```

In a long-running server or a CUDA kernel manager, leaks accumulate and eventually crash the process. In your GPU future: `cudaMalloc` = `new`, `cudaFree` = `delete`. Same concept, different address space.

### 4.4 Why This Matters For HPC

In high-performance computing, memory management IS performance:

| Concern | Why It Matters |
|---|---|
| Stack vs heap allocation | Stack is ~10x faster to allocate; heap involves OS calls |
| Copying large objects | A copied 100MB matrix is 100MB of RAM and 100MB of memory bandwidth wasted |
| Cache locality | Data accessed together should be stored together in memory (Chapter 3 in roadmap) |
| Memory leaks | Fatal in servers, GPU programs, long-running inference pipelines |
| GPU memory | Completely separate from CPU RAM — explicit copies required |

### 4.5 The Data and BSS Segments

These hold variables that exist for the entire lifetime of the program:

```cpp
int Counter::count = 0;    // DATA segment (has an initial value)
static int total;          // BSS segment (zero-initialized by kernel at startup)
```

`static` inside a function works the same way — the variable lives in the data/BSS segment, not the stack, so it survives across multiple calls to that function.

```cpp
void count_calls() {
    static int n = 0;  // initialized once, then persists
    ++n;
    std::cout << "Called " << n << " times\n";
}
```

---

## Chapter 5: Variables and Types

*File: `variables.cpp`*

### 5.1 What a Variable Is

A variable is a name for a location in memory. When you write:

```cpp
int x = 10;
```

The compiler:
1. Reserves 4 bytes on the stack
2. Stores the binary representation of 10 in those bytes
3. Associates the name `x` with that address

At runtime there is no name `x` — names exist only in your source code. The CPU sees only addresses.

### 5.2 The Fundamental Types and Their Sizes

| Type | Size | Range / Notes |
|---|---|---|
| `bool` | 1 byte | `true` or `false` (stored as 0 or 1) |
| `char` | 1 byte | −128 to 127, or a single ASCII character |
| `int` | 4 bytes | −2,147,483,648 to 2,147,483,647 |
| `long` | 8 bytes | Much larger integer range |
| `float` | 4 bytes | ~7 decimal digits of precision |
| `double` | 8 bytes | ~15 decimal digits of precision |
| `pointer` | 8 bytes | A memory address (on 64-bit systems) |

Why do sizes matter? Because when you lay out a class or a struct, every member occupies exactly `sizeof(member)` bytes. A `Dog` with one `std::string` + one `int` + one `double` occupies roughly 24 + 4 + 8 = 36 bytes (plus potential padding — see Chapter 3 of the roadmap). That's what `new Dog(...)` asks the heap for.

### 5.3 Type Safety

C++ is **statically typed**: every variable's type is known at compile time and never changes. This is unlike Python where a variable can hold an int, then a string.

```cpp
int x = 10;
x = "hello";  // COMPILE ERROR — caught before the program ever runs
```

This is why compiled C++ is fast: the compiler knows exactly what operations are valid and what machine code to emit. No runtime type checking needed.

---

## Chapter 6: Functions and the Call Stack

*File: `test.cpp`*

### 6.1 What a Function Call Does at Runtime

```cpp
int greet_user(int i) {
    cout << "Hello World!" << endl;
    return i + 10;
}

int main() {
    int result = greet_user(57);
}
```

At runtime, calling `greet_user(57)`:

```
1. Push the argument (57) into a register or onto the stack
2. Push the return address (where main() will resume) onto the stack
3. Jump to the first instruction of greet_user()
4. greet_user() executes
5. Compute i + 10 = 67, store in return register (eax on x86)
6. Jump back to the return address in main()
7. main() reads 67 from the return register → stores in `result`
```

This is the **call stack** in action. Every call pushes a frame; every return pops it.

### 6.2 Why Return Type Matters

The return type tells the compiler (and the CPU calling convention) where and how big the return value is. Returning an `int` = 4 bytes in a register. Returning a `std::string` = more complex (could involve heap memory). The compiler handles this, but knowing it exists helps you understand why large return types can be slow (Phase 3 topic: return value optimization).

---

## Chapter 7: Pointers and References

*Files: `understanding classes.cpp`, `object_oriented.cpp`*

### 7.1 What a Pointer Is

A pointer is a variable that holds a **memory address** as its value.

```
int x = 42;           RAM layout:
int* p = &x;          Address 0x1000: [ 42 ] ← x lives here
                      Address 0x1008: [ 0x1000 ] ← p lives here (holds x's address)
```

- `&x` means "the address of x" — gives you the address, not the value
- `*p` means "the value at the address p holds" — this is **dereferencing**
- `p` itself is just an 8-byte integer that stores an address

```cpp
int x = 42;
int* p = &x;     // p holds the address of x
*p = 100;        // writes 100 to wherever p points → x is now 100
std::cout << x;  // prints 100
```

### 7.2 What a Reference Is

A reference is an alias — another name for the same variable. Under the hood, the compiler implements it as a pointer, but you never deal with addresses directly.

```cpp
int x = 42;
int& ref = x;    // ref is just another name for x — they share the same memory
ref = 100;       // x is now 100
```

**Pointer vs Reference:**
| | Pointer (`int*`) | Reference (`int&`) |
|---|---|---|
| Can be null | Yes (`nullptr`) | No — always bound to something |
| Can be reassigned | Yes | No — bound once, forever |
| Syntax for access | `*p` or `p->member` | Same as the original variable |
| Under the hood | An 8-byte address | Also an address, but hidden |

### 7.3 Why `&` in Function Parameters Is About Memory

```cpp
void bad(std::string s)        // copies the entire string — slow for large strings
void good(const std::string& s) // passes the address (8 bytes) — fast, read-only
void mutate(std::string& s)    // passes the address — fast, caller's string modified
```

When you see `const T&` in a function parameter, translate it as:
> "Pass me the address of your object. I promise to only read it, never write."

This is the most important performance habit in C++. In HPC:
- A weight matrix might be 500MB. Copying it on every function call = disaster.
- Passing `const Matrix&` = passing 8 bytes regardless of matrix size.

---

## Chapter 8: Classes and Object-Oriented Programming

*Files: `type_conversion.cpp`, `understanding classes.cpp`, `object_oriented.cpp`*

### 8.1 What a Class Is in Memory

A class is a recipe for a chunk of memory. When you declare:

```cpp
class Dog {
private:
    std::string name;   // ~24 bytes (string stores length + pointer to heap data)
    int age;            // 4 bytes
    double weight_kg;   // 8 bytes
};
```

The compiler computes that a `Dog` needs approximately 36 bytes (plus possible padding for alignment). Every object of type `Dog` — whether on the stack or heap — occupies exactly that many bytes.

```
Dog rex("Rex", 4, 28.5):

Stack:  ┌─────────────────────────────────────┐
        │  name  (24 bytes — std::string)      │
        │  age   (4 bytes)                     │
        │  [4 bytes padding for alignment]     │
        │  weight_kg (8 bytes)                 │
        └─────────────────────────────────────┘
```

Member functions (like `display()`) do NOT live inside the object. They live in the code segment once, and the compiler passes a hidden pointer (`this`) so the function knows which object it's operating on.

### 8.2 Constructors and Destructors in Terms of Memory

**Constructor:** Called immediately after the memory for the object is allocated. Its job is to put the object into a valid, usable state. Without it, the bytes that make up the object would contain random garbage from whatever was in that RAM before.

**Destructor:** Called immediately before the object's memory is reclaimed. Its job is to release anything the object owns — open files, heap memory, GPU buffers, network sockets. After the destructor runs, the bytes go back to the stack or heap pool, and may be reused by the next allocation.

```
Timeline for Dog rex("Rex", 4, 28.5):

  Dog rex declared
       │
       ▼
  Stack frame grows to fit Dog's bytes
       │
       ▼
  Constructor runs → writes "Rex", 4, 28.5 into those bytes
       │
       ▼
  (you use rex normally)
       │
       ▼
  Closing } of rex's scope reached
       │
       ▼
  Destructor runs → releases anything Dog owns (none, in this case)
       │
       ▼
  Stack frame shrinks → those bytes are reusable
```

This deterministic pattern — "acquire in constructor, release in destructor" — is called **RAII** (Resource Acquisition Is Initialization). It is the central design philosophy of modern C++. CUDA buffer management follows this pattern too.

### 8.3 Access Modifiers and Encapsulation

Private members can only be accessed through the class's own public functions. This is not just style — it has practical benefits:

1. **Invariant protection:** A setter can validate input before writing. A public member can be written to with any garbage.
2. **Change without breakage:** If you change a private member's type or name, only the class's own code needs updating — not every caller.
3. **Debugging:** When a private value is wrong, the bug is always in the class's own code. If it were public, it could be modified from anywhere.

### 8.4 Static Members

Static member variables live in the **data segment**, not inside any object. They're initialized once, before `main()` runs, and destroyed after `main()` returns.

```cpp
class Counter {
    static int count;  // declared inside class
};
int Counter::count = 0;  // defined outside — this is where the actual bytes are
```

This definition outside the class is not optional. Without it, the linker complains because `count` is declared but has no actual storage.

### 8.5 const Member Functions

```cpp
void display() const { ... }
```

The `const` here modifies the hidden `this` pointer from `Dog*` to `const Dog*`. This means the compiler will refuse to compile any code in `display()` that modifies a member variable. It's a compile-time guarantee — completely free at runtime.

---

## Chapter 9: Runtime Behaviour — What Actually Happens When You Run `./oop`

Let's trace exactly what happens when you run `./oop`, start to finish.

### 9.1 Before main()

1. The kernel loads the ELF binary from disk into virtual memory
2. Maps code segment, data segment, BSS segment into the process's address space
3. Sets up the initial stack
4. Hands control to the C++ runtime's entry point (`_start`, not `main`)
5. The C++ runtime:
   - Calls constructors for all global and static variables
   - `int Counter::count = 0;` is initialized here
   - Sets up `std::cin`, `std::cout`, `std::cerr`
   - Then calls your `main()`

### 9.2 Inside main()

Every local variable declaration allocates space on the current stack frame. Every `{` block can add more locals. Every `}` destroys locals declared in that block (in reverse order — last declared, first destroyed).

```cpp
int main() {
    Dog unnamed;           // stack: default constructor called
    Dog rex("Rex"...);     // stack: parameterized constructor called
    Dog rex_copy = rex;    // stack: copy constructor called
    ...
    return 0;
    // rex_copy destructor (last declared, first destroyed)
    // rex destructor
    // unnamed destructor
}
```

### 9.3 After main()

1. Your `main()` returns 0 to the C++ runtime
2. C++ runtime calls destructors for all global/static objects (reverse construction order)
3. C++ runtime calls `exit(0)` → system call to the kernel
4. Kernel reclaims the process's virtual memory
5. Kernel records exit status 0 (success)
6. Shell receives 0 — `echo $?` would print 0

**Exit codes:**
- `return 0` = success
- `return 1` (or any non-zero) = failure
- The kernel doesn't care what the number means — that convention is for shell scripts and build systems

### 9.4 What Happens When the Program Crashes

If your code does something illegal — accesses memory it doesn't own, divides by zero, calls a function through a null pointer:

1. The CPU raises a hardware exception (e.g., page fault for invalid memory)
2. The CPU hands control to the kernel's exception handler
3. The kernel sends a signal to your process (e.g., `SIGSEGV` for memory violations)
4. The default signal handler terminates the process
5. The kernel generates a "core dump" (a snapshot of the process's memory, if enabled)
6. You see "Segmentation fault (core dumped)" in the terminal

This is not a C++ error — it's the OS enforcing memory protection. Understanding this helps you debug crashes: a segfault almost always means a pointer is wrong (null, dangling, or pointing to invalid memory).

---

## Glossary

Terms defined in order of first appearance.

| Term | Definition |
|---|---|
| **Process** | A running program — has its own private memory, CPU time, file descriptors |
| **Kernel** | The core of the operating system. Manages hardware on behalf of processes |
| **System call** | A request from your program to the kernel (print, allocate memory, read file) |
| **Virtual memory** | The private address space the kernel gives each process; translated to physical RAM transparently |
| **ELF binary** | The format of compiled executables on Linux (Executable and Linkable Format) |
| **Compilation** | Translating C++ source into machine code (4 stages: preprocess, compile, assemble, link) |
| **Linker** | Combines compiled object files into a final executable |
| **Stack** | Automatic memory region — fast, managed by CPU, limited size, LIFO lifetime |
| **Heap** | Dynamic memory region — flexible lifetime, managed by you (`new`/`delete`) |
| **Stack frame** | The slice of stack memory reserved for one function call |
| **RAII** | Resource Acquisition Is Initialization — tie resource lifetime to object lifetime |
| **Segfault** | Segmentation fault — process accessed memory it's not allowed to (kernel kills it) |
| **Pointer** | A variable holding a memory address (`int*`) |
| **Reference** | An alias for another variable (`int&`) — same memory, different name |
| **Dereference** | Reading the value at a pointer's address (`*p`) |
| **Constructor** | A special function called when an object is created to initialize its state |
| **Destructor** | A special function called when an object is destroyed to release its resources |
| **Encapsulation** | Hiding internal data behind a controlled interface (private + public) |
| **Member initializer list** | The `: member(value)` syntax after a constructor — initializes directly, no extra work |
| **const member function** | A member function that cannot modify the object's state — compiler enforced |
| **Static member** | Belongs to the class, not any instance — lives in data segment for program lifetime |
| **`this` pointer** | Hidden pointer inside every member function pointing to the current object |
| **`nullptr`** | A pointer value meaning "points to nothing" — safe default for unused pointers |
| **Undefined behaviour** | Code that violates C++ rules — compiler may do anything (crash, corrupt, silently wrong) |
| **Memory leak** | Heap memory that is allocated but never freed — accumulates until process exits or runs out |

---

*Handbook version: Phase 1 — Fundamentals complete*
*Next update: when `oop_advanced.cpp` is created (inheritance, polymorphism, virtual functions)*
