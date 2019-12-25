# Ebic Regex

Yeah idk, just some weird thing

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