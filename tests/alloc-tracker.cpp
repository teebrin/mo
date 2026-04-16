#include <gtest/gtest.h>
#include <dlfcn.h>

namespace {
thread_local size_t count{0};
};

size_t get_alloc_count() { return count; }

extern "C" {
void* malloc(size_t size) {
    ++count;
    static void* (* real_malloc)(size_t) = (void*(*)(size_t))dlsym(RTLD_NEXT, "malloc");
    return real_malloc(size);
}

void free(void* ptr) {
    static void (* real_free)(void*) = (void(*)(void*))dlsym(RTLD_NEXT, "free");
    real_free(ptr);
}
}

void* operator new(std::size_t size) {
    ++count;
    return std::malloc(size);
}

void operator delete(void* ptr) noexcept {
    std::free(ptr);
}
