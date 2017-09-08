#include "IntervalProvider.hpp"

#include <stdexcept>

IntervalProvider::IntervalProvider() {
	free_.push_back({0, UINT32_MAX});
}

uint32_t IntervalProvider::requestFree() {
	for(auto it = free_.begin(); it!=free_.end(); ++it) {
		auto retval = it->from++;
		if(it->empty() || it->from == 0/*overflow*/)
			free_.erase(it);
		return retval;
	}
	throw std::runtime_error("out of free keys");
}

void IntervalProvider::markAsUsed(uint32_t val) {
	for(auto it = free_.begin(); it!=free_.end(); ++it) {
		if(it->contains(val)) {
			move(val, it, free_, used_);
			return;
		}
	}
	throw std::logic_error("Id is already used");
}

void IntervalProvider::free(uint32_t val) {
	for(auto it = used_.begin(); it!=used_.end(); ++it) {
		if(it->contains(val)) {
			move(val, it, used_, free_);
			return;
		}
	}
	throw std::logic_error("No such used id");
}

void IntervalProvider::move(uint32_t val, IntervalSeq::iterator pos, IntervalSeq& from, IntervalSeq& to) {
	assert(pos->contains(val));
	removeFromSeq(val, from, pos);
	insertToSeq(val, to);
}

void IntervalProvider::removeFromSeq(uint32_t val, IntervalSeq& from, IntervalSeq::iterator pos) {
	assert(pos->contains(val));
	auto slices = pos->split(val);
	pos = from.erase(pos);
	if(!slices.second.empty()) {
		pos = from.insert(pos, slices.second);
	}
	if(!slices.first.empty()) {
		from.insert(pos, slices.first);
	}
}

void IntervalProvider::insertToSeq(uint32_t val, IntervalSeq& to) {
	Interval newinter{val,val};
	if(to.empty()) {
		to.push_back(newinter);
		return;
	}
	Interval* max_inter = (to.front() >= newinter ? &to.front() : &newinter);
	for(auto it = to.begin(); it != to.end(); ++it) {
		assert(!it->contains(val));
		if(*it >= *max_inter) {
			max_inter = &*it;
		}
		if(max_inter == &*it) {
			if(*it <= val) {
				it->to++;
			} else if (*it < newinter) {
				it = to.insert(++it, newinter);
			} else { // insert newinter to the beginning
				it = to.insert(it, newinter);
			}
			auto it2 = it;
			if((++it2) != to.end() && *it2 >= it->to) {
				it->to = it2->to;
				to.erase(it2);
			}
			it2=it;
			if(it2-- != to.begin() && (*it2) <= it->from) {
				it->from = it2->from;
				to.erase(it2);
			}
			return;
		}
	}
	if(max_inter == &newinter) {
		auto it = to.end();
		if(to.size() && *(--it) <= val) {
			it->to = val;
		} else {
			to.push_back(newinter);
		}
		return;
	}
}

std::ostream& operator<<(std::ostream& s, const IntervalProvider& ip) {
	s << "Free:";
	for(auto& i : ip.free_)
		s << " [" << i.from << ',' << i.to << "]";
	s << '\n';
	s << "Used:";
	for(auto& i : ip.used_)
		s << " [" << i.from << ',' << i.to << "]";
	s << '\n';
	return s;
}

