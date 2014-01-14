#include "array.hpp"

#include <sstream>

using namespace fluxcore;

class ArrayPtr : public DataPtr {
    public:
        ArrayPtr(void* ptr_, typeptr_t basetype_, std::size_t size_) :
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
        std::size_t size;
};

class ArrayRef : public DataRef {
    public:
        ArrayRef(void* ptr_, typeptr_t basetype_, std::size_t size_) :
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
        std::size_t size;
};

dataref_t ArrayPtr::operator*() {
    return std::shared_ptr<DataRef>(new ArrayRef(ptr, basetype, size));
}

Array::Array(typeptr_t& basetype_, std::size_t size_) :
        basetype(basetype_),
        size(size_) {}

typeid_t Array::getID() const {
    return 3;
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

