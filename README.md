# GyoUtils

Gyoutils is a simple C++ library used to simplify and speedup development while working in C/C++, with a focus on speed and simplicity.

Programming has become an increasingly complex, annoying and troublesome task. Meanwhile applications are becoming slower and buggier, each year requiring more and more powerful hardware. What in the past could be done by any programmer are now only possible by teams of hundreds or even thousands of persons.

We do not agree.

Programming used to be simple. Applications used to be fast. We want things to go back to the way they were.

Since hoping for things to get better is pure fantasy, we decided to create GyoUtils.

We know we won't be able to solve every problem for everyone, different projects will always have different requirements. Instead of trying to provide solutions for everything, we're only trying to cover the most common scenarios. That's why we're building this so you can, and should, edit the code to fit your needs.

We want you to be in control of the software. We hope this brings you in the right direction.


# How do we do that

Gyoutils is a collection of simple tools that can help you during development.

Here's a high level view of most of the things you can find in each file.
For a more comprehensive list you should look into each file.
For updates on recent changes you should look at CHANGELOG.txt

- a new simpler alternative to printf
- deactivatable assert macros with custom print messages
- defer macro, similar as defer in go lang, useful for when
- dynamic array, a simple replacement to std::vector
- str, a simple replacement to std::string
- str_builder, a simple structure to construct your strings and binary files
- a for macro which will help you during iteration over some of gyoutils structures
- simple deactivatable performance counters, for when you want to measure performance at low level
- simple profiling structure to help you in profiling nested/recursive functions
- simple benchmarking functionality to help you test functions in a variety of cases
- many common math constructs, among them you can find fast versions of vec2, vec3, vec4, mat4
- Rotors, a replacement for quaternions from a branch of mathematics called Geometric Algebra, making them easier to understand and producing faster code
- common functions for working with files
- many other things


# License

(C) 2023 Gyoshev Industries

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.
2. Any modification to the source must not be misrepresented as being 
   the original software.
3. A copy of the license and copyright notice must be included with the
   licensed material in source form, but is not required for binaries.
