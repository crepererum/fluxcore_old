#ifndef FLUXCORE_ABSTRACTTYPE_HPP
#define FLUXCORE_ABSTRACTTYPE_HPP

#include <string>
#include <utility>

#include "../config.hpp"

namespace fluxcore {

class DataPtr;
class DataRef;

class AbstractType {
    public:
        virtual ~AbstractType() = default;
        virtual typeid_t getID() const = 0;
        virtual std::size_t getSize() const = 0;
        virtual DataPtr getPtr(void* ptr) const = 0;
        virtual std::string getName() const = 0;
};

class DataPtr {
    public:
        virtual ~DataPtr() = default;
        virtual DataRef operator*() = 0;
        virtual DataPtr&& operator+(std::size_t delta) = 0;
        virtual std::size_t operator-(const DataPtr& ptr) = 0;
};

class DataRef {
    public:
        virtual ~DataRef() = default;
        virtual DataPtr operator&() = 0;
        virtual bool operator<(const DataRef& obj) = 0;
        virtual bool operator==(const DataRef& obj) = 0;
};

template <typename T>
class DataPtrTemplate;

template <typename T>
class DataRefTemplate;

template <typename T>
class DataPtrTemplate : public DataPtr {
    public:
        DataPtrTemplate(void* ptr_) : ptr(static_cast<T*>(ptr_)) {}
        DataPtrTemplate(T* ptr_) : ptr(ptr_) {}

        virtual DataRef operator*() override {
            return DataRefTemplate<T>(*ptr);
        }

        virtual DataPtr&& operator+(std::size_t delta) override {
            return std::move(DataPtrTemplate<T>(ptr + delta));
        }

        virtual std::size_t operator-(const DataPtr& obj) override {
            auto o = dynamic_cast<const DataPtrTemplate<T>&>(obj);
            return ptr - o.ptr;
        }

    private:
        T* ptr;
};

template <typename T>
class DataRefTemplate : public DataRef {
    public:
        DataRefTemplate(T& ref_) : ref(ref_) {}

        virtual DataPtr operator&() override {
            return DataPtrTemplate<T>(&ref);
        }

        virtual bool operator<(const DataRef& obj) override {
            auto o = dynamic_cast<const DataRefTemplate<T>&>(obj);
            return ref < o.ref;
        }

        virtual bool operator==(const DataRef& obj) override {
            auto o = dynamic_cast<const DataRefTemplate<T>&>(obj);
            return ref == o.ref;
        }

    private:
        T& ref;
};

}

#endif

