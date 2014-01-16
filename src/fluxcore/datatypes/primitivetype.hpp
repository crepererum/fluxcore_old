#ifndef FLUXCORE_PRIMITIVETYPE_HPP
#define FLUXCORE_PRIMITIVETYPE_HPP

#include <stdexcept>

#include "abstracttype.hpp"
#include "typeregistry.hpp"

namespace fluxcore {

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

        virtual ptrdiff_t operator-(const DataPtr& obj) const override {
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

template <typename H>
class PrimitiveType : public AbstractType {
    typedef typename H::type type;

    public:
        static constexpr typeid_t id = H::id;

        virtual ~PrimitiveType() override = default;

        virtual typeid_t getID() const override {
            return id;
        }

        virtual std::size_t getSize() const override {
            return sizeof(type);
        }

        virtual std::string getName() const override {
            return H::getName();
        }

        virtual dataptr_t createPtr(void* ptr) const override {
            return std::make_shared<DataPtrTemplate<type>>(ptr);
        }

        virtual dataptrconst_t createPtr(const void* ptr) const override {
            // black voodoo!
            return std::make_shared<DataPtrTemplate<type>>(const_cast<void*>(ptr));
        }

        virtual void generateDescriptor(typedescr_t::iterator& begin, typedescr_t::iterator end) const {
            if (begin == end) {
                throw std::runtime_error("Typedescriptor is going to be too long!");
            }
            *begin = id;
            ++begin;
        }

        static typeptr_t parseDescriptor(typedescr_t::const_iterator& begin, typedescr_t::const_iterator end) {
            if (begin == end) {
                throw std::runtime_error("Illegal type descriptor!");
            }
            if (*begin != id) {
                throw std::runtime_error("This typedescriptor does not belong to this type!");
            }
            ++begin;

            return std::make_shared<PrimitiveType<H>>();
        }
};

}

#endif

