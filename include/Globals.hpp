/**
 * @file include/Types.hpp
 *
 * @author Ramin Raeisi <rr@epen.io>
 */
#ifndef TYPES_HPP
#define TYPES_HPP

#include <cstdint>
#include <vector>
#include <algorithm>
#include <functional>
#include <iostream>

#define CHECK_AND_REPORT_ERROR_IF(VAR, OP, VAL, MESSAGE) \
    if (VAR OP  VAL) {\
        reportErrorLine(MESSAGE); \
        return(false); \
    }

namespace cdp {
	namespace globals {
		/// Type alias for the IDs of the packets
		using PacketIDType = uint8_t;

		/// Enum class type for the target endianness
		enum class Endianness { LittleEndian, BigEndian };

		/// Function for retrieving the target endianness
		inline Endianness getEndianness() {
			unsigned int i = 1;
			char* c = (char*)&i;
			if (*c)
				return(cdp::globals::Endianness::LittleEndian);
			else
				return(cdp::globals::Endianness::BigEndian);
		}

		/// Defines a custom hash for std::pair
		struct hash_pair {
			template <class T1, class T2>
			size_t operator()(const std::pair<T1, T2>& p) const
			{
				auto hash1 = std::hash<T1>{}(p.first);
				auto hash2 = std::hash<T2>{}(p.second);
				return hash1 ^ hash2;
			}
		};

		/// Calculates sum of size of a tuple elements
		template <typename ... Args>
		constexpr std::size_t sizeOfTupleElements(std::tuple<Args...> const&) {
			return (sizeof(std::remove_pointer_t<Args>) + ...);
		}

		/// Template function which perform an operation on two variables with possible different types
		template<typename Op, typename T1, typename T2>
		auto foldOperation(Op&& op, T1 arg1, T2 arg2)
		{
			return op(*arg1, *arg2);
		}

		/// Template function which perform an operation on multiple variables with possible different types
		/// using fold expressions 
		template<typename Op, typename T, typename ... Ts>
		auto foldOperation(Op&& op, T head, Ts ... rest)
		{
			return op(*head, fold1(std::forward<Op>(op), rest...));
		}

		template<typename RangesType, typename Operator, typename TFirst, typename ... TRest>
		size_t findRangeHelper(const std::vector<std::pair<RangesType, RangesType>>& ranges, Operator&& op, TFirst first, TRest ... rest)
		{
			auto finalValue = foldOperation(op, first, rest...);
			auto it = std::find_if(ranges.begin(), ranges.end(),
				[&](const std::pair<size_t, size_t>& element) { return (finalValue >= element.first && finalValue <= element.second); }
			);
			size_t index = it - ranges.begin();
			return index;
		}

		template<typename ... Args>
		std::string stringFormat(const std::string& format, Args ... args)
		{
			int size = snprintf(nullptr, 0, format.c_str(), args ...) + 1;
			if (size <= 0) {
				throw std::runtime_error("Error during formatting.");
			}
			std::vector<char> buf(size);
			snprintf(&buf[0], size, format.c_str(), args ...);
			return(std::string(buf.begin(), buf.end() - 1));
		}

	} // namespace general
} // namespace cdp

#endif // !TYPES_HPP


