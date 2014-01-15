#include "tuple.hpp"

#include <algorithm>
#include <bitset>
#include <limits>
#include <sstream>

#include "../config.hpp"

using namespace fluxcore;

class TuplePtr : public DataPtr {
    public:
        TuplePtr(void* ptr_, const std::list<typeptr_t>& basetypes_, std::size_t size_) :
            ptr(static_cast<byte_t*>(ptr_)),
            basetypes(basetypes_),
            size(size_) {}

        virtual dataref_t operator*() override;

        virtual dataptr_t operator+(std::size_t delta) override {
            return std::make_shared<TuplePtr>(ptr + delta * size, basetypes, size);
        }

        virtual dataptrconst_t operator+(std::size_t delta) const override {
            return std::make_shared<TuplePtr>(ptr + delta * size, basetypes, size);
        }

        virtual std::size_t operator-(const DataPtr& obj) const override {
            auto o = dynamic_cast<const TuplePtr&>(obj);
            return (ptr - o.ptr) / size;
        }

    private:
        byte_t* ptr;
        std::list<typeptr_t> basetypes;
        std::size_t size;
};

class TupleRef : public DataRef {
    public:
        TupleRef(void* ptr_, const std::list<typeptr_t>& basetypes_, std::size_t size_) :
            ptr(static_cast<byte_t*>(ptr_)),
            basetypes(basetypes_),
            size(size_) {}

        virtual dataptr_t operator&() override {
            return std::make_shared<TuplePtr>(ptr, basetypes, size);
        }

        virtual bool operator<(const DataRef& obj) override {
            auto o = dynamic_cast<const TupleRef&>(obj);
            auto iter1 = basetypes.cbegin();
            auto iter2 = o.basetypes.cbegin();

            for (std::size_t i = 0; i < std::min(basetypes.size(), o.basetypes.size()); ++i) {
                void* target1 = ptr + i * size * (*iter1)->getSize();
                void* target2 = o.ptr + i * size * (*iter2)->getSize();

                dataptr_t subptr1 = (*iter1)->createPtr(target1);
                dataptr_t subptr2 = (*iter2)->createPtr(target2);

                dataref_t subref1 = **subptr1;
                dataref_t subref2 = **subptr2;

                if (*subref1 < *subref2) {
                    return true;
                }

                ++iter1;
                ++iter2;
            }

            return false;
        }

        virtual bool operator==(const DataRef& obj) override {
            auto o = dynamic_cast<const TupleRef&>(obj);

            if (size != o.size) {
                return false;
            }

            auto iter1 = basetypes.cbegin();
            auto iter2 = o.basetypes.cbegin();

            for (std::size_t i = 0; i < size; ++i) {
                void* target1 = ptr + i * size * (*iter1)->getSize();
                void* target2 = o.ptr + i * size * (*iter2)->getSize();

                dataptr_t subptr1 = (*iter1)->createPtr(target1);
                dataptr_t subptr2 = (*iter2)->createPtr(target2);

                dataref_t subref1 = **subptr1;
                dataref_t subref2 = **subptr2;

                if (!(*subref1 == *subref2)) {
                    return false;
                }

                ++iter1;
                ++iter2;
            }

            return true;
        }

    private:
        byte_t* ptr;
        std::list<typeptr_t> basetypes;
        std::size_t size;
};

dataref_t TuplePtr::operator*() {
    return std::shared_ptr<DataRef>(new TupleRef(ptr, basetypes, size));
}

// I know this sucks, but I liked this construction :P
Tuple::Tuple(const std::list<typeptr_t>& basetypes_) :
        basetypes(basetypes_),
        size(std::accumulate(basetypes.cbegin(), basetypes.cend(), 0, [](std::size_t a, const typeptr_t& b){
                    return a + b->getSize();
                })) {
    if (basetypes.size() > std::numeric_limits<tupleSize_t>::max()) {
        throw std::runtime_error("Tuple is too large!");
    }
}

typeid_t Tuple::getID() const {
    return id;
}

std::size_t Tuple::getSize() const {
    return size;
}

std::string Tuple::getName() const {
    std::stringstream ss;
    ss << "tuple<";

    bool first = true;
    for (const auto& subtype : basetypes) {
        if (first) {
            first = false;
        } else {
            ss << ",";
        }

        ss << subtype->getName();
    }

    ss << ">";

    return ss.str();
}

dataptr_t Tuple::createPtr(void* ptr) const {
    return std::make_shared<TuplePtr>(ptr, basetypes, size);
}

dataptrconst_t Tuple::createPtr(const void* ptr) const {
    // black voodoo!
    return std::make_shared<TuplePtr>(const_cast<void*>(ptr), basetypes, size);
}

void Tuple::generateDescriptor(typedescr_t::iterator& begin, typedescr_t::iterator end) const {
    if (begin == end) {
        throw std::runtime_error("Typedescriptor is going to be too long!");
    }
    *begin = id;
    ++begin;

    tupleSize_t count = basetypes.size();
    std::bitset<sizeof(typeid_t) * 8> mask1;
    mask1.flip();
    auto mask2 = mask1.to_ulong();
    for (std::size_t i = sizeof(tupleSize_t) / sizeof(typeid_t); i > 0; --i) {
        if (begin == end) {
            throw std::runtime_error("Typedescriptor is going to be too long!");
        }

        *begin = (count >> (sizeof(typeid_t) * (i - 1))) & mask2;
        ++begin;
    }

    for (const auto& t : basetypes) {
        t->generateDescriptor(begin, end);
    }
}

typeptr_t Tuple::parseDescriptor(typedescr_t::const_iterator& begin, typedescr_t::const_iterator end) {
    if (begin == end) {
        throw std::runtime_error("Illegal type descriptor!");
    }
    if (*begin != id) {
        throw std::runtime_error("This typedescriptor does not belong to this type!");
    }
    ++begin;

    arraySize_t count = 0;
    for (std::size_t i = 0; i < sizeof(tupleSize_t) / sizeof(typeid_t); ++i) {
        if (begin == end) {
            throw std::runtime_error("Illegal typedescriptor!");
        }

        count = (count << sizeof(typeid_t)) | *begin;
        ++begin;
    }

    std::list<typeptr_t> basetypes;
    for (std::size_t i = 0; i < count; ++i) {
        if (begin == end) {
            throw std::runtime_error("Illegal typedescriptor!");
        }

        basetypes.push_back(Typeregistry::getRegistry().parseType(begin, end));
    }

    return std::make_shared<Tuple>(basetypes);
}


