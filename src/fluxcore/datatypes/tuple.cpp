#include "tuple.hpp"

#include <algorithm>
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
                })) {}

typeid_t Tuple::getID() const {
    return 7;
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

