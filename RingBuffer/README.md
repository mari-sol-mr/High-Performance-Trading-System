# Design Choices

## Size of the buffer
Must be a power of two. This improves the performance of the modulo operation.  
When you have:  
`x % size`  
if size is a power of 2, it is the same as:  
`x & (size - 1)`  

`(size - 1)` acts as a mask of the lower bits of `size`.  
For example, if size is 8 (1000), the mask is 7 (0111).

Intuitively, modulo gives you the remainder of x after taking away any values larger than `size`. In the same way, the mask removes any values larger than `size` and leaves only values less than `size`.

## Storage of atomic variables
Must be in separate cache lines to avoid false-sharing.  
**False-sharing** could occur when two threads use the same cache line to store variables. For example, Thread A could be reading variable A and Thread B could be writing to variable B. The cache policy may force Thread A to update its value of variable A every time that variable B is written to, even though the value of variable A has not changed.  

We align the atomic variables by the hardware_constructive_interference_size (the minimum offset between two objects in memory to avoid false sharing).
