#ifndef FLUXCORE_ABSTRACTTYPE_HPP
#define FLUXCORE_ABSTRACTTYPE_HPP

#include <memory>
#include <string>
#include <utility>

#include "../config.hpp"

namespace fluxcore {

class DataPtr;
class DataRef;

typedef std::shared_ptr<DataPtr> dataptr_t;
typedef std::shared_ptr<const DataPtr> dataptrconst_t;
typedef std::shared_ptr<DataRef> dataref_t;
typedef std::shared_ptr<const DataRef> datarefconst_t;

class AbstractType {
    public:
        virtual ~AbstractType() = default;

        virtual typeid_t getID() const = 0;
        virtual std::size_t getSize() const = 0;
        virtual std::string getName() const = 0;

        virtual dataptr_t createPtr(void* ptr) const = 0;
        virtual dataptrconst_t createPtr(const void* ptr) const = 0;
};

typedef std::shared_ptr<AbstractType> typeptr_t;

class DataPtr {
    public:
        virtual ~DataPtr() = default;

        virtual dataref_t operator*() = 0;

        virtual dataptr_t operator+(std::size_t delta) = 0;
        virtual dataptrconst_t operator+(std::size_t delta) const = 0;

        virtual std::size_t operator-(const DataPtr& ptr) const = 0;
};

class DataRef {
    public:
        virtual ~DataRef() = default;

        virtual dataptr_t operator&() = 0;
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

        virtual dataref_t operator*() override {
            return std::make_shared<DataRefTemplate<T>>(*ptr);
        }

        virtual dataptr_t operator+(std::size_t delta) override {
            return std::make_shared<DataPtrTemplate<T>>(ptr + delta);
        }

        virtual dataptrconst_t operator+(std::size_t delta) const override {
            return std::make_shared<DataPtrTemplate<T>>(ptr + delta);
        }

        virtual std::size_t operator-(const DataPtr& obj) const override {
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

        virtual dataptr_t operator&() override {
            return std::make_shared<DataPtrTemplate<T>>(&ref);
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

