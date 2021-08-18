#pragma once

#include <type_traits>

template <typename UnderlyingType>
struct PlacementContainer
{
    alignas(UnderlyingType) unsigned char data[sizeof(UnderlyingType)] = { 0 };
    bool initialized = false;
};

template <typename UnderlyingType>
struct PlacementAccessor
{
    explicit PlacementAccessor(PlacementContainer<UnderlyingType>& container)
        : _container{ container }
    {
        if (container.initialized) {
            _ptr = reinterpret_cast<UnderlyingType*>(container.data);
        }
    }

    template <typename... Args>
    void construct(Args&&... args)
    {
        if (_container.initialized) {
            destroy();
        }

        _ptr = new (_container.data) UnderlyingType(std::forward<Args>(args)...);
    }

    void destroy()
    {
        if (_ptr) {
            _ptr->~UnderlyingType();
            _ptr = nullptr;
        }
    }

    bool empty() const
    {
        return _ptr == nullptr;
    }

    UnderlyingType& ref()
    {
        return *_ptr;
    }

    UnderlyingType* ptr()
    {
        return _ptr;
    }

    operator UnderlyingType&()
    {
        return ref();
    }

    operator UnderlyingType*()
    {
        return ptr();
    }

    UnderlyingType* operator->()
    {
        return ptr();
    }

private:
    PlacementContainer<UnderlyingType>& _container;
    UnderlyingType* _ptr = nullptr;
};