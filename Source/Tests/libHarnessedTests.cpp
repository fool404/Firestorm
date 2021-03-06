// HarnessedUnitTests.cpp : Defines the entry point for the console application.
//

#include <libCore/libCore.h>

#include <libHarnessed/libHarnessed.h>
#include <libHarnessed/TestHarness.h>
#include <libHarnessed/TestCase.h>

#include <libCore/RefPtr.h>

using namespace Firestorm;

template<class... Fields>
struct SOA : std::tuple<Fields...>
{
};

RefPtr<TestHarness> libHarnessedPrepareHarness(int argc, char** argv)
{
	std::tuple<uint8_t, uint8_t, uint8_t, uint8_t> t(0,0,0,0);
	FIRE_LOG_DEBUG("size of tuple<uint8_t * 4> = %d", sizeof(t));

	SOA<uint8_t, uint8_t, uint8_t, uint8_t> def;
	FIRE_LOG_DEBUG("size of ComponentDefinition<uint8_t * 4> = %d", sizeof(def));


	RefPtr<TestHarness> harness(new TestHarness("Test Harness Tests"));

	harness->It("should run all of the tests passed to it", [](Firestorm::TestCase& t) {
		size_t numRun = 0;
		Firestorm::TestHarness h("All Tests", true);

		h.It("should not report this as an error", [&numRun](Firestorm::TestCase&) {
			numRun++;
		});
		h.It("should not report this as an error either", [&numRun](Firestorm::TestCase&) {
			numRun++;
		});
		h.It("and finally this", [&numRun](Firestorm::TestCase&) {
			numRun++;
		});
		size_t results = h.Run();
		t.Assert(results == 0, "There were errors reported when there shouldn't have been.");
		t.Assert(numRun == 3, "There should have been 3 functions run. There were not.");
	});

	harness->It("should run all of the tests passed to it even when one has an error", [](Firestorm::TestCase& t) {
		size_t numRun = 0;
		Firestorm::TestHarness h("All Tests", true);

		h.It("should report this as an error", [&numRun](Firestorm::TestCase& tt) {
			numRun++;
			tt.AssertIsTrue(false, "forcing an error");
		});

		h.It("should not report this as an error", [&numRun](Firestorm::TestCase&) {
			numRun++;
		});

		h.It("and finally this", [&numRun](Firestorm::TestCase&) {
			numRun++;
		});

		size_t results = h.Run();
		t.Assert(results == 1, "There was no error report when there should have been 1.");
		t.Assert(numRun == 3, "There should have been 3 functions run. There were not.");
	});

	harness->It("should continue through multiple asserts in a given test condition", [](Firestorm::TestCase& t) {
		size_t numRun = 0;
		Firestorm::TestHarness h("All Tests", true);

		h.It("should report this as an error", [&numRun](Firestorm::TestCase& tt) {
			numRun++;
			tt.AssertIsTrue(false, "forcing an error");
			tt.AssertIsTrue(false, "forcing another error");
			tt.AssertIsTrue(false, "forcing one more error");
		});

		size_t results = h.Run();
		t.Assert(numRun == 1, "There should have been 1 function invoked. got more than that somehow.");
		t.Assert(results == 3, "expected 3 errors. did not get that number.");
	});

	harness->It("should report no errors when there are none", [](Firestorm::TestCase& t) {
		Firestorm::TestHarness h("No Errors Test!", true);
		h.It("should not report this as an error", [](Firestorm::TestCase&) {});
		h.It("should not report this as an error either", [](Firestorm::TestCase&) {});
		h.It("and finally this", [](Firestorm::TestCase&) {});
		size_t results = h.Run();
		t.Assert(results == 0, "There were errors reported when there shouldn't have been.");
	});
	
	return harness;
}

