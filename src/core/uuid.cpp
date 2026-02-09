#include "uuid.h"

#include <random>

namespace bacon
{
    static std::random_device _random_device;
    static std::mt19937_64 _random_engine(_random_device());
    static std::uniform_int_distribution<uint64_t> _uniform_dist;

    UUID::UUID()
    {
        m_p1 = _uniform_dist(_random_engine);
        m_p2 = _uniform_dist(_random_engine);
    }

    UUID::UUID(std::string uuid)
    {
        size_t pos = uuid.find("_");
        std::string p1 = uuid.substr(0, pos);
        std::string p2 = uuid.substr(pos + 1);

        m_p1 = std::stoull(p1);
        m_p2 = std::stoull(p2);
    }

    bool UUID::operator==(UUID uuid) const
    {
        return ((m_p1 == uuid.get_left()) && (m_p2 == uuid.get_right()));
    }

    std::string UUID::get_uuid() const
    {
        return std::to_string(m_p1) + "_" + std::to_string(m_p2);
    }

    uint64_t UUID::get_left() const
    {
        return this->m_p1;
    }

    uint64_t UUID::get_right() const
    {
        return this->m_p2;
    }
} // namespace bacon
