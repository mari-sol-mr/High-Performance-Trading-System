using namespace std;

template <typename T, size_t capacity_>
class RingBuffer {

    static_assert((capacity_ & (capacity_ - 1)) == 0,
                "Capacity must be power of two");
    static_assert(std::is_trivially_copyable_v<T>,
                "T must be trivially copyable for wait-free guarantees");
public:
    RingBuffer()
    : write_index(0)
    , read_index(0)
    {}

    bool write(const T& item)
    {
        if (full())
            return false;
        
        buffer[mask(write_index++)] = item;

        return true;
    }

    bool read(const T& item)
    {
        if (empty())
            return false;

        T& elem = *reinterpret_cast<T*>(&buffer[mask(read_index++)]);
        std::destroy_at(&elem);

        return true;
    }

private:
    size_t mask(size_t val) {
        return val & (capacity_ - 1);
    }

    bool full()
    {
        return write_index - read_index == capacity_;
    }

    bool empty()
    {
        return write_index == read_index;
    }

    alignas(std::hardware_destructive_interference_size) std::atomic<size_t> write_index;
    alignas(std::hardware_destructive_interference_size) std::atomic<size_t> read_index;
    T* buffer[capacity_];
};