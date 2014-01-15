#include "array.hpp"

#include <bitset>
#include <sstream>

using namespace fluxcore;

class ArrayPtr : public DataPtr {
    public:
        ArrayPtr(void* ptr_, typeptr_t basetype_, arraySize_t size_) :
            ptr(static_cast<byte_t*>(ptr_)),
            basetype(basetype_),
            size(size_) {}

        virtual dataref_t operator*() override;

        virtual dataptr_t operator+(std::size_t delta) override {
            return std::make_shared<ArrayPtr>(ptr + delta * size * basetype->getSize(), basetype, size);
        }

        virtual dataptrconst_t operator+(std::size_t delta) const override {
            return std::make_shared<ArrayPtr>(ptr + delta * size * basetype->getSize(), basetype, size);
        }

        virtual std::size_t operator-(const DataPtr& obj) const override {
            auto o = dynamic_cast<const ArrayPtr&>(obj);
            return (ptr - o.ptr) / (size * basetype->getSize());
        }

    private:
        byte_t* ptr;
        typeptr_t basetype;
        arraySize_t size;
};

class ArrayRef : public DataRef {
    public:
        ArrayRef(void* ptr_, typeptr_t basetype_, arraySize_t size_) :
            ptr(static_cast<byte_t*>(ptr_)),
            basetype(basetype_),
            size(size_) {}

        virtual dataptr_t operator&() override {
            return std::make_shared<ArrayPtr>(ptr, basetype, size);
        }

        virtual bool operator<(const DataRef& obj) override {
            auto o = dynamic_cast<const ArrayRef&>(obj);

            for (std::size_t i = 0; i < std::min(size, o.size); ++i) {
                void* target1 = ptr + i * size * basetype->getSize();
                void* target2 = o.ptr + i * size * basetype->getSize();

                dataptr_t subptr1 = basetype->createPtr(target1);
                dataptr_t subptr2 = basetype->createPtr(target2);

                dataref_t subref1 = **subptr1;
                dataref_t subref2 = **subptr2;

                if (*subref1 < *subref2) {
                    return true;
                }
            }

            return false;
        }

        virtual bool operator==(const DataRef& obj) override {
            auto o = dynamic_cast<const ArrayRef&>(obj);

            if (size != o.size) {
                return false;
            }

            for (std::size_t i = 0; i < size; ++i) {
                void* target1 = ptr + i * size * basetype->getSize();
                void* target2 = o.ptr + i * size * basetype->getSize();

                dataptr_t subptr1 = basetype->createPtr(target1);
                dataptr_t subptr2 = basetype->createPtr(target2);

                dataref_t subref1 = **subptr1;
                dataref_t subref2 = **subptr2;

                if (!(*subref1 == *subref2)) {
                    return false;
                }
            }

            return true;
        }

    private:
        byte_t* ptr;
        typeptr_t basetype;
        arraySize_t size;
};

dataref_t ArrayPtr::operator*() {
    return std::shared_ptr<DataRef>(new ArrayRef(ptr, basetype, size));
}

Array::Array(typeptr_t& basetype_, arraySize_t size_) :
        basetype(basetype_),
        size(size_) {}

typeid_t Array::getID() const {
    return id;
}

std::size_t Array::getSize() const {
    return basetype->getSize() * size;
}

std::string Array::getName() const {
    std::stringstream ss;
    ss << "array<"
        << size
        << ","
        << basetype->getName()
        << ">";

    return ss.str();
}

dataptr_t Array::createPtr(void* ptr) const {
    return std::make_shared<ArrayPtr>(ptr, basetype, size);
}

dataptrconst_t Array::createPtr(const void* ptr) const {
    // black voodoo!
    return std::make_shared<ArrayPtr>(const_cast<void*>(ptr), basetype, size);
}

void Array::generateDescriptor(typedescr_t::iterator& begin, typedescr_t::iterator end) const {
    if (begin == end) {
        throw std::runtime_error("Typedescriptor is going to be too long!");
    }
    *begin = id;
    ++begin;

    basetype->generateDescriptor(begin, end);

    std::bitset<sizeof(typeid_t) * 8> mask1;
    mask1.flip();
    auto mask2 = mask1.to_ulong();
    for (std::size_t i = sizeof(arraySize_t) / sizeof(typeid_t); i > 0; --i) {
        if (begin == end) {
            throw std::runtime_error("Typedescriptor is going to be too long!");
        }

        *begin = (size >> (sizeof(typeid_t) * 8 * (i - 1))) & mask2;
        ++begin;
    }
}

typeptr_t Array::parseDescriptor(typedescr_t::const_iterator& begin, typedescr_t::const_iterator end) {
    if (begin == end) {
        throw std::runtime_error("Illegal type descriptor!");
    }
    if (*begin != id) {
        throw std::runtime_error("This typedescriptor does not belong to this type!");
    }
    ++begin;

    auto basetype = Typeregistry::getRegistry().parseType(begin, end);

    arraySize_t size = 0;
    for (std::size_t i = 0; i < sizeof(arraySize_t) / sizeof(typeid_t); ++i) {
        if (begin == end) {
            throw std::runtime_error("Illegal typedescriptor!");
        }

        size = (size << (sizeof(typeid_t) * 8)) | *begin;
        ++begin;
    }

    return std::make_shared<Array>(basetype, size);
}

