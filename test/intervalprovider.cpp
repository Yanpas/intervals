#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <iostream>

#define private public
#include "IntervalProvider.hpp"
#undef private

TEST_CASE("struct") {
	using Interval = IntervalProvider::Interval;
	auto res = Interval{0, 5}.split(0).first;
	REQUIRE(res.empty());
	REQUIRE((Interval{0, UINT32_MAX}.split(UINT32_MAX).second.empty()));
	auto res2 = Interval{0, 5}.split(2);
	REQUIRE((res2.first == Interval{0,1} && res2.second == Interval{3,5}));
	res2 = Interval{0, 5}.split(1);
	REQUIRE((res2.first == Interval{0,0} && res2.second == Interval{2,5}));
}

TEST_CASE("basic") {
	IntervalProvider p;
	for(auto i : {1,3,4,7}) {
		p.markAsUsed(i);
		//std::cout << "~~\n";
		//std::cout << p;
		REQUIRE_THROWS(p.markAsUsed(i));
	}
	for(auto i : {0,2,5,6}) {
		REQUIRE_THROWS(p.free(0));
		auto fv = p.requestFree();
		REQUIRE(fv == i);
	}
}

TEST_CASE("sequences") {
	IntervalProvider p, emptyone;
	auto capture_n_free = [&](uint32_t v){
		p.markAsUsed(v);
		p.free(v);
		p.markAsUsed(v);
		p.free(v);
	};
	auto capture = [&](uint32_t v) {
		p.markAsUsed(v);
	};
	auto free = [&](uint32_t v) {
		p.free(v);
	};
	for(uint32_t i = 0; i<40; ++i) {
		for(int j = 1; j <=4; j++) {
			//std::cerr << "BEF\n" << p;
			capture_n_free(i*j);
			//std::cerr << "AFTER\n" << p;
			bool free = p.free_ == emptyone.free_;
			bool used = p.used_ == emptyone.used_;
			if(!free) {
				FAIL("free i:" << i << " j:" << j);
			}
			if(!used) {
				FAIL("used i:" << i << " j:" << j);
			}
		}
	}
//	std::cerr << p;
	for(uint32_t i = 1; i<20; ++i) {
		for(int j = 1; j <=6; j++) {
			capture(i*j);
//			std::cerr << " CAPT " << i*j << "\n"  << p;
		}
		for(int j = 1; j <=6; j++) {
			free(i*j);
//			std::cerr << " FREE " << i*j << "\n" << p;
		}
		bool free = p.free_ == emptyone.free_;
		bool used = p.used_ == emptyone.used_;
		if(!free) {
			FAIL("free i:" << i );
		}
		if(!used) {
			FAIL("used i:" << i );
		}
	}
}
