#pragma once

#include <array>

template <typename ValueType, std::size_t Size>
class RingBuffer
{
    static_assert(Size > 0, "Size must be greater than 0");

    std::array<ValueType, Size> _buffer{};
    std::size_t _inIndex{};
    std::size_t _outIndex{};
    bool _empty{ true };

public:
    bool push(const ValueType& value)
    {
        // The buffer is full
        if (!_empty && _inIndex == _outIndex) {
            return false;
        }

        _buffer[_inIndex] = value;
        _inIndex = (_inIndex + 1) % Size;
        _empty = false;

        return true;
    }

    bool pop()
    {
        if (_empty) {
            return false;
        }

        _outIndex = (_outIndex + 1) % Size;

        if (_inIndex == _outIndex) {
            _empty = true;
        }

        return true;
    }

    bool peek(ValueType& output)
    {
        if (_empty) {
            return false;
        }

        output = _buffer[_outIndex];

        return true;
    }

    [[nodiscard]] bool empty() const
    {
        return _empty;
    }
};