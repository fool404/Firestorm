
#include <libCore/libCore.h>
#include <libCore/RefPtr.h>
#include <libHarnessed/libHarnessed.h>
#include <functional>

using std::cout;
using std::cin;
using std::endl;

using namespace Elf;

struct TestObject
{
	TestObject(const Function<void()>& deletionCallback)
		: _callback(deletionCallback)
	{
	}
	virtual ~TestObject()
	{
		_callback();
	}
	Function<void()> _callback;
};

struct TestPolyObject : public TestObject
{
	TestPolyObject(const std::function<void()>& deletionCallback)
		: TestObject(deletionCallback)
	{
	}
};


int main(int argc, char** argv)
{
	TestHarness h("libCore Tests");

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// RefPtr Testing
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	h.It("RefPtr should delete the object when it falls out of scope", [](TestCase& t) {
		bool objectDeleted = false;
		{
			auto callback = [&objectDeleted]() { objectDeleted = true; };
			RefPtr<TestObject> object(new TestObject(callback));
			t.Assert(object.GetCount() == 1, "the ref count was not the expected value");
		}
		t.Assert(objectDeleted, "the object was not deleted");
	});

	h.It("weak pointers should not prolong the lifetime of objects managed by a shared ptr", [](TestCase& t) {
		bool objectDeleted = false;
		auto callback = [&objectDeleted]() { objectDeleted = true; };
		WeakPtr<TestObject> weak;
		{
			RefPtr<TestObject> object(new TestObject(callback));
			t.Assert(object.GetCount() == 1, "the ref count was not the expected value");
			weak = object;
			t.Assert(object.GetWeakCount() == 1, "the weak ref count was not the expected value");
			t.Assert(object.GetCount() == 1, "the ref count was not the expected value");
		}
		t.Assert(!weak, "the weak pointer still holds on to a valid object");
		t.Assert(objectDeleted, "the object was not deleted");
	});

	h.It("polymorphic types should be allowable by the RefPtr", [](TestCase& t) {
		bool objectDeleted = false;

		auto callback = [&objectDeleted]() { objectDeleted = true; };
		{
			RefPtr<TestObject> object(new TestPolyObject(callback));
			t.Assert(object.GetCount() == 1, "the ref count was not the expected value");
		}

		t.Assert(objectDeleted, "the object was not deleted");
	});

	h.It("weak pointers should also be allowed to hold polymorphic types", [](TestCase& t) {
		bool objectDeleted = false;
		auto callback = [&objectDeleted]() { objectDeleted = true; };
		WeakPtr<TestObject> weak;
		{

			RefPtr<TestPolyObject> object(new TestPolyObject(callback));
			t.Assert(object.GetCount() == 1, "the ref count was not the expected value");

			weak = object;

			t.Assert(object.GetWeakCount() == 1, "the weak ref count was not the expected value");
			t.Assert(object.GetCount() == 1, "the ref count was not the expected value");
		}
		t.Assert(!weak, "the weak pointer still holds on to a valid object");
		t.Assert(objectDeleted, "the object was not deleted");
	});

	

	uint32_t errorCount = h.Run();
	cout << "Tests Completed with [[" << errorCount << "]] errors." << endl;

	cout << "Press any key to close..." << endl;
	cin.get();

	return errorCount;
}