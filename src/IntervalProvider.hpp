#ifndef INTERVALPROVIDER_HPP_
#define INTERVALPROVIDER_HPP_

#include <utility>
#include <cinttypes>
#include <cassert>
#include <list>
#include <ostream>

class IntervalProvider {
public:
	struct Interval {
		uint32_t from, to;
		bool operator==(const Interval& r) const {
			return from == r.from && to == r.to;
		}
		bool operator<(const Interval& other) {
			return to < (int64_t)other.from - 1;
		}
		bool operator>(const Interval& other) {
			return from > (int64_t)other.to + 1;
		}
		bool empty() const {
			return from > to;
		}
		bool contains(uint32_t val) const {
			return val >= from && val <= to;
		}
		bool operator<=(const Interval& other) const {
			return to <= (int64_t)other.from - 1;
		}
		bool operator>=(const Interval& other) const {
			return from >= (int64_t)other.to + 1;
		}
		bool operator<=(uint32_t val) const {
			return to == (int64_t)val - 1;
		}
		bool operator>=(uint32_t val) const {
			return from == (int64_t)val + 1;
		}
		std::pair<Interval, Interval> split(uint32_t val) const {
			assert(this->contains(val));
			if(val - 1 == UINT32_MAX)
				return {{1,  0}, {val + 1, to}};
			else if(val + 1 == 0)
				return {{from,  val -1}, {1, 0}};
			return {{from,  val -1}, {val + 1, to}};
		}
	};
	
	friend std::ostream& operator<<(std::ostream& s, const IntervalProvider& ip);
	
	IntervalProvider();
	uint32_t requestFree();
	void markAsUsed(uint32_t val);
	void free(uint32_t val);
private:
	typedef std::list<Interval> IntervalSeq;
	
	IntervalSeq free_, used_;
	
	static void move(uint32_t val, IntervalSeq::iterator left,IntervalSeq& from, IntervalSeq& to);
	static void removeFromSeq(uint32_t val, IntervalSeq& from, IntervalSeq::iterator pos);
	static void insertToSeq(uint32_t val, IntervalSeq& to);
};


#endif /* INTERVALPROVIDER_HPP_ */
