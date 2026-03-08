#pragma once

#include <type_traits>
#include <cstdint>
#include <vector>
#include <string>
#include <stdexcept>

namespace bacon
{
	class ByteStream
	{
		public:
			ByteStream() = default;
			explicit ByteStream(std::vector<uint8_t> raw) : m_raw(std::move(raw)) {}
			~ByteStream() = default;

			const std::vector<uint8_t>& raw() const noexcept { return m_raw; }
			size_t size() const noexcept { return m_raw.size(); }
			bool empty() const noexcept { return m_raw.empty(); }

			size_t read_pos() const noexcept { return m_read_pos; }
			void reset_read() noexcept { m_read_pos = 0; }

			template <typename T,
				typename = std::enable_if_t<std::is_arithmetic_v<T>>>
			ByteStream& operator<<(T value)
			{
				const auto* ptr = reinterpret_cast<const uint8_t*>(&value);
				m_raw.insert(m_raw.end(), ptr, ptr + sizeof(T));
				return *this;
			}

			ByteStream& operator<<(const std::string& str)
			{
				size_t len = str.size();
				*this << len;
				const uint8_t* bytes = reinterpret_cast<const uint8_t*>(str.data());
				m_raw.insert(m_raw.end(), bytes, bytes + len);
				return *this;
			}

			template <typename T,
				typename = std::enable_if<std::is_arithmetic_v<T>>>
			ByteStream& operator<<(const std::vector<T>& vec)
			{
				size_t count = static_cast<size_t>(vec.size());
				*this << count;
				for (const auto& item : vec)
					*this << item;
				return *this;
			}

			template <typename T,
			    typename = std::enable_if<std::is_arithmetic_v<T>>>
			ByteStream& operator>>(T& value)
			{
				check_remaining(sizeof(T));
				auto* ptr = reinterpret_cast<uint8_t*>(&value);
				std::copy(m_raw.begin() + m_read_pos, m_raw.begin() + m_read_pos + sizeof(T), ptr);
				m_read_pos += sizeof(T);
				return *this;
			}

			ByteStream& operator>>(std::string& str)
			{
				size_t len;
				*this >> len;

				check_remaining(len);
				str.assign(reinterpret_cast<const char*>(m_raw.data() + m_read_pos), len);
				m_read_pos += len;

				return *this;
			}

			template <typename T,
              typename = std::enable_if_t<std::is_arithmetic_v<T>>>
			ByteStream& operator>>(std::vector<T>& vec)
			{
				size_t len;
				*this >> len;
				vec.resize(len);
				for (auto& v : vec)
					*this >> v;
				return *this;
			}

		private:
			std::vector<uint8_t> m_raw;
			size_t m_read_pos = 0;

			void check_remaining(size_t needed) const
			{
				if (m_read_pos + needed > m_raw.size())
					throw std::out_of_range("ByteStream: read past end");
			}
	};

	template <typename T,
		typename = std::enable_if<std::is_arithmetic_v<T>>>
	T read_value(ByteStream& bytes)
	{
		T value;
		bytes >> value;
		return value;
	}

	template <typename T,
		typename = std::enable_if<std::is_arithmetic_v<T>>>
	std::vector<T> read_vector(ByteStream& bytes)
	{
		uint32_t count;
		bytes >> count;
		std::vector<T> vec(count);
		for (auto& item : vec)
			bytes >> item;
		return vec;
	}

	inline std::string read_string(ByteStream& bytes)
	{
		std::string out;
		bytes >> out;
		return out;
	}

	inline int8_t read_int8(ByteStream& bytes) { return read_value<int8_t>(bytes); }
	inline int16_t read_int16(ByteStream& bytes) { return read_value<int16_t>(bytes); }
	inline int32_t read_int32(ByteStream& bytes) { return read_value<int32_t>(bytes); }
	inline int64_t read_int64(ByteStream& bytes) { return read_value<int64_t>(bytes); }
	inline uint8_t read_uint8(ByteStream& bytes) { return read_value<uint8_t>(bytes); }
	inline uint16_t read_uint16(ByteStream& bytes) { return read_value<uint16_t>(bytes); }
	inline uint32_t read_uint32(ByteStream& bytes) { return read_value<uint32_t>(bytes); }
	inline uint64_t read_uint64(ByteStream& bytes) { return read_value<uint64_t>(bytes); }
	inline float read_float(ByteStream& bytes) { return read_value<float>(bytes); }
	inline double read_double(ByteStream& bytes) { return read_value<double>(bytes); }
	inline bool read_bool(ByteStream& bytes) { return read_value<bool>(bytes); }
} // namespace bacon
