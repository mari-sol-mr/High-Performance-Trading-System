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
