
#include <libCore.h>
#include <libHarnessed.h>

int main(int argc, char** argv)
{
	Elf::TestHarness harness("Test");
	harness.It("should run the tests given to it", [](Elf::TestCase& t) {});

	harness.It("should report an error when an assertion is tripped", [](Elf::TestCase& t) {
		t.Assert(true, "This should not trigger an assertion");
		t.Assert(false, "Intentional assertion.");
	});

	uint32_t errorCount = harness.Run();
	std::cout << "Tests Completed with " << errorCount << " errors." << std::endl;

	std::cout << "Press any key to close..." << std::endl;
	std::cin.get();

	return errorCount;
}