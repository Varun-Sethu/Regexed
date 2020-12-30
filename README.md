# Regexed
Really simple Regex engine for C++. Recognises the entire class of regular languages.

## Compilation
```bash
    cd build
    cmake ..
    make
```


## Usage
```cpp
    #include "include/Regex/regex.h"

    Regex r;
    r.parse("(ab|c*)*");
    std::cout << r.matches("ababababccababab");
```
