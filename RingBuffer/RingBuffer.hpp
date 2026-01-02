using namespace std;

template <typename T, size_t capacity_>
class RingBuffer {

    static_assert((capacity_ & (capacity_ - 1)) == 0,
                "Capacity must be power of two");
    static_assert(std::is_trivially_copyable_v<T>,
                "T must be trivially copyable for wait-free guarantees");
public:
    RingBuffer()
    : write_index_(0)
    , read_index_(0)
    {
        buffer = static_cast<T*>(malloc(capacity_*sizeof(T)));
        assert(buffer != nullptr);
    }

    ~RingBuffer()

    {
        auto noop = [](T&) {};
        while (consume_one(noop));
        free(buffer);
    }

    bool push(const T& item)
    {
        const size_t write_index = write_index_.load(memory_order_relaxed);  // only written from push thread
        
        if (write_index - read_index_.load(memory_order_acquire) == capacity_)
            return false;

        buffer[mask(write_index)] = item;
        new (buffer + mask(write_index)) T(item);

        write_index_.store(write_index_ + 1, memory_order_release);

        return true;
    }

    template <typename Functor>
    bool consume_one(Functor & func)
    {
        const size_t write_index = write_index_.load(memory_order_acquire);
        const size_t read_index  = read_index_.load(memory_order_relaxed);

        if (write_index == read_index)
            return false;

        T& elem = *reinterpret_cast<T*>(&buffer[mask(read_index)]);
        func(elem);
        elem.~T();

        read_index_.store(read_index + 1, memory_order_release);

        return true;
    }

private:
    size_t mask(size_t val) {
        return val & (capacity_ - 1);
    }

    alignas(std::hardware_destructive_interference_size) std::atomic<size_t> write_index_;
    alignas(std::hardware_destructive_interference_size) std::atomic<size_t> read_index_;
    T* buffer;
};