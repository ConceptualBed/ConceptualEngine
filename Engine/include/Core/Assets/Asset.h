#pragma once

#include <string>

class Asset
{
public:
    virtual ~Asset() = default;

const std::string& GetPath() const
    {
        return path;
    }

protected:
    std::string path;
};