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

typedef std::array<typeid_t, typedescrlen> typedescr_t;

class AbstractType {
    public:
        virtual ~AbstractType() = default;

        virtual typeid_t getID() const = 0;
        virtual std::size_t getSize() const = 0;
        virtual std::string getName() const = 0;

        virtual dataptr_t createPtr(void* ptr) const = 0;
        virtual dataptrconst_t createPtr(const void* ptr) const = 0;

        virtual void generateDescriptor(typedescr_t::iterator& begin, typedescr_t::iterator end) const = 0;
        virtual void generateDescriptor(typedescr_t::iterator&& begin, typedescr_t::iterator end) const {
            generateDescriptor(begin, end);
        }
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

}

#endif

