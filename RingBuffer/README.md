# Design Choices
## Infinite atomic indeces
After every read or write, the write and read indeces will not be updated so that their value is in the "correct range" (0 - size of the buffer). Instead, they will be unsigned integers that grow unbounded and eventually wrap around to 0. 

This avoids the wasted slot of other implementations and simplifies the logic.

Since both producer and consumer will access the read and write indeces, they will be atomic variables to avoid data races.

## Size of the buffer
Must be a power of two.   
### This improves the performance of the modulo operation.
When you have:  
`x % size`  
if size is a power of 2, it is the same as:  
`x & (size - 1)`  

`(size - 1)` acts as a mask of the lower bits of `size`.  
For example, if size is 8 (1000), the mask is 7 (0111).

Intuitively, modulo gives you the remainder of x after taking away any values larger than `size`. In the same way, the mask removes any values larger than `size` and leaves only values less than `size`.

Modulo is a very slow arithmetic operation (up to 90 CPU cycles compared to 1 cycle for addition). So creating the mask and then applying bitwise AND, instead of explicitly using the modulo operator, will save a lot of time.

### This allows for correct wraparound of the head and tail.

Some sizes that are not powers of 2 have the problematic quality that when an index wrap arounds, index % `size` results in the same number.

For example, say we have a buffer of size 17 and the write index is 0xffffffff (it's about to wrap around).  
The producer will write to:  
0xffffffff % 17 = 0  
But, after the write index wraps around to 0, it will write to the same place:  
0x00000000 % 17 = 0  

Setting the size to a power of 2 will guarantee this problem will never arise.  
0xffffffff % 64 = 63   
0x00000000 % 64 = 0 


## Storage of atomic variables
Must be in separate cache lines to avoid false-sharing.  

**False-sharing** could occur when two threads use the same cache line to store variables. For example, Thread A could be reading variable A and Thread B could be writing to variable B. The cache policy may force Thread A to update its value of variable A every time that variable B is written to, even though the value of variable A has not changed.  

We align the atomic variables by the hardware_constructive_interference_size (the minimum offset between two objects in memory to avoid false sharing).

## Memory Ordering

Since the consumer and producer threads will be writing to and reading from shared variables (write_index and read_index), 
it's important to ensure that out-of-order execution does not cause incorrect results. 

The producer must write the new data to the buffer before incrementing write_index, or two issues might arise:
1. we may write to the incorrect location (by writing to write_index + 1).
2. The consumer thread will use the incremented write_index and try to read new data that hasn't been written yet!

# API
```
write(val) {      
    if !full()
        buffer[mask(write_index++)] = val;
}
```

```
read(val) {      
    if !empty()
        return buffer[mask(read_index++)];
}
```

```
mask(val) {
    return val & (capacity_ - 1);
}
```

```
full() {
    return write_index - read_index == buffer.capacity();
}
```

```
empty() {
    return write_index == read_index;
}
```
