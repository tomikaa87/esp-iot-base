/*
    This file is part of esp-iot-base.

    esp-iot-base is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    esp-iot-base is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with esp-iot-base.  If not, see <http://www.gnu.org/licenses/>.
    
    Author: Tamas Karpati
    Created on 2020-08-13
*/

#pragma once

#include <cstring>

class Variant
{
public:
    enum class Type {
        Empty,
        Integer,
        Float,
        Double,
        String
    };

    Variant()
        : _type(Type::Empty)
    {
        memset(&_storage, 0, sizeof(Storage));
    }

    explicit Variant(const int value)
        : Variant(Type::Integer)
    {
        _storage.integerValue = value;
    }

    explicit Variant(const float value)
        : Variant(Type::Float)
    {
        _storage.floatValue = value;
    }

    explicit Variant(const double value)
        : Variant(Type::Double)
    {
        _storage.doubleValue = value;
    }

    explicit Variant(const char* value)
        : Variant(Type::String)
    {
        _storage.stringValue = value;
    }

    Type type() const
    {
        return _type;
    }

    explicit operator int() const
    {
        if (_type != Type::Integer) {
            return {};
        }

        return _storage.integerValue;
    }

    explicit operator float() const
    {
        if (_type != Type::Float) {
            return {};
        }

        return _storage.floatValue;
    }

    explicit operator double() const
    {
        if (_type != Type::Double) {
            return {};
        }

        return _storage.doubleValue;
    }

    explicit operator const char*() const
    {
        if (_type != Type::String) {
            return {};
        }

        return _storage.stringValue;
    }

    Variant& operator=(const int value)
    {
        _type = Type::Integer;
        _storage.integerValue = value;
        return *this;
    }

    Variant& operator=(const float value)
    {
        _type = Type::Float;
        _storage.floatValue = value;
        return *this;
    }

    Variant& operator=(const double value)
    {
        _type = Type::Double;
        _storage.doubleValue = value;
        return *this;
    }

    Variant& operator=(const char* value)
    {
        _type = Type::String;
        _storage.stringValue = value;
        return *this;
    }

    bool operator==(const int v) const
    {
        return _type == Type::Integer && _storage.integerValue == v;
    }

    bool operator==(const float v) const
    {
        return _type == Type::Float && _storage.floatValue == v;
    }

    bool operator==(const double v) const
    {
        return _type == Type::Double && _storage.doubleValue == v;
    }

    bool operator==(const char* v) const
    {
        // TODO replace pointer comparison if necessary
        return _type == Type::String && _storage.stringValue == v;
    }

    bool operator!=(const int v) const
    {
        return !(*this == v);
    }

    bool operator!=(const float v) const
    {
        return !(*this == v);
    }

    bool operator!=(const double v) const
    {
        return !(*this == v);
    }

    bool operator!=(const char* v) const
    {
        return !(*this == v);
    }

protected:
    explicit Variant(Type type)
        : _type(type)
    {
        memset(&_storage, 0, sizeof(Storage));
    }

private:
    union Storage {
        int integerValue;
        float floatValue;
        double doubleValue;
        const char* stringValue;
    };

    Storage _storage;
    Type _type;
};
