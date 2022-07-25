# slotmap
Simple C++ implementation of a slotmap, a fast and efficient container with constant erase, insert and lookup complexity.

# Cloning, Building and dependencies
To clone:
```
git clone https://github.com/pixelcluster/slotmap.git
```
To include the slotmap structure, simply
```c++
#include <Slotmap.hpp>
```  
There are no dependencies outside of the C++ STL.
# Example
```c++
#include <Slotmap.hpp>
#include <iostream>

int main() {
    Slotmap<int> slotmap;
    SlotmapHandle handle = slotmap.addElement(1);
    std::cout << "The slotmap handle refers to the number " << slotmap[handle] << "\n";
    SlotmapHandle otherHandle = slotmap.addElement(2);
    std::cout << "The other slotmap handle refers to the number " << slotmap[otherHandle] << "\n";
    slotmap.removeElement(handle);
    std::cout << "The other slotmap handle still refers to the number " << slotmap[otherHandle] << "\n";
}
```  
The output could look like:
```
The slotmap handle refers to the number 1
The other slotmap handle refers to the number 2
The other slotmap handle still refers to the number 2
```