// HarnessedUnitTests.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <libHarnessed.h>
#include <libHarnessed/TestHarness.h>
#include <libHarnessed/TestCase.h>

int main()
{
	Elf::TestHarness harness("Test Harness Tests");

	harness.It("should run all of the tests passed to it", [](Elf::TestCase& t) {
		uint32_t numRun = 0;
		Elf::TestHarness h("All Tests", true);

		h.It("should not report this as an error", [&numRun](Elf::TestCase&) {
			numRun++;
		});
		h.It("should not report this as an error either", [&numRun](Elf::TestCase&) {
			numRun++;
		});
		h.It("and finally this", [&numRun](Elf::TestCase&) {
			numRun++;
		});
		uint32_t results = h.Run();
		t.Assert(results == 0, "There were errors reported when there shouldn't have been.");
		t.Assert(numRun == 3, "There should have been 3 functions run. There were not.");
	});

	harness.It("should run all of the tests passed to it even when one has an error", [](Elf::TestCase& t) {
		uint32_t numRun = 0;
		Elf::TestHarness h("All Tests", true);

		h.It("should report this as an error", [&numRun](Elf::TestCase& tt) {
			numRun++;
			tt.Assert(false, "forcing an error");
		});

		h.It("should not report this as an error", [&numRun](Elf::TestCase&) {
			numRun++;
		});

		h.It("and finally this", [&numRun](Elf::TestCase&) {
			numRun++;
		});

		uint32_t results = h.Run();
		t.Assert(results == 1, "There was no error report when there should have been 1.");
		t.Assert(numRun == 3, "There should have been 3 functions run. There were not.");
	});

	harness.It("should report no errors when there are none", [](Elf::TestCase& t) {
		Elf::TestHarness h("No Errors Test!", true);
		h.It("should not report this as an error", [](Elf::TestCase&) {});
		h.It("should not report this as an error either", [](Elf::TestCase&) {});
		h.It("and finally this", [](Elf::TestCase&) {});
		uint32_t results = h.Run();
		t.Assert(results == 0, "There were errors reported when there shouldn't have been.");
	});
	
	uint32_t results = harness.Run();

	std::cout << "Completed with " << results << " errors." << std::endl << std::endl;
	std::cout << "Press 'Enter' to close..." << std::endl;
	std::cin.get();

    return 0;
}

