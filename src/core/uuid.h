#pragma once

#include <cstdint>
#include <string>

namespace bacon
{
class UUID
{
     public:
    UUID();
    UUID(std::string uuid);
    bool operator==(UUID uuid) const;
    std::string get_uuid() const;
    uint64_t get_left() const;
    uint64_t get_right() const;

     private:
    uint64_t m_p1;
    uint64_t m_p2;
};
} // namespace bacon
