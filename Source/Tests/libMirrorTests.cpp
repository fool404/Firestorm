////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  /!\ libMirror Tests /!\
//
//  These are the test cases for libMirror, the reflection utility library.
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libCore/libCore.h>
#include <libHarnessed/libHarnessed.h>
#include <libCore/RefPtr.h>

#include <libMirror/libMirror.h>
#include <libMirror/Object.h>
#include <libMirror/SerialTraits.h>
#include <libMirror/EventDispatcher.h>

#include <rttr/registration.h>
#include <rttr/registration_friend.h>

using namespace Firestorm;

class TestBasicObject : public Mirror::Object
{
	FIRE_MIRROR_DECLARE(TestBasicObject, Mirror::Object);
private:
public:
	TestBasicObject() {}
	virtual ~TestBasicObject()
	{
		std::cout << "TestBasicObject::~TestBasicObject" << std::endl;
	}
private:
};

class TestObjectWithFields : public Mirror::Object
{
	FIRE_MIRROR_DECLARE(TestObjectWithFields, Mirror::Object);
public:
	TestObjectWithFields() {}
	explicit TestObjectWithFields(const string& str) : testString(str) {}
	virtual ~TestObjectWithFields() 
	{
		std::cout << "TestObjectWithFields::~TestObjectWithFields" << std::endl;
	}

	string testString;
private:
};



struct TestEvent
{
	FIRE_MIRROR_DECLARE(TestEvent);
public:
	TestEvent(Firestorm::string s) : Message(s) {}
	string Message;
};

class TestEventObject
{
public:
	void DoRegistration(EventDispatcher& dispatcher)
	{
		using namespace std::placeholders;
		function<void(const TestEvent&)> func(std::bind(&TestEventObject::Invoke, this, _1));
		_receipt = dispatcher.Register<TestEvent>([this](const TestEvent& event) {
			Invoke(event);
		});
	}

	void Invoke(const TestEvent& arg) const
	{
		FIRE_LOG_DEBUG("TestEventObject says '%s'.", arg.Message);
	}
private:
	EventDispatcher::Receipt _receipt;
};

/*template<class T1, class T2>
void TestTypeUtils(T1 obj1, T2 obj2, TestCase& t)
{
	Mirror::Type t1 = Mirror::TypeUtils::get(obj1);
	Mirror::Type t2 = Mirror::TypeUtils::get(obj2);

	t.Assert(t1 == t2, "The types do not match.");
}*/

RefPtr<TestHarness> libMirrorPrepareHarness(int argc, char** argv)
{
	// TestBasicObject::RegisterReflection();
	// TestObjectWithFields::RegisterReflection();
	// TestEvent::RegisterReflection();

	RefPtr<TestHarness> h(new TestHarness("Test libMirror"));

	Library<libMirror>::Initialize(argc, argv);

	// h->It("TestBasicObject::MyType() should return the same type as instance->GetType()", [](TestCase& t) {
	// 	Mirror::Type tboStaticType = TestBasicObject::MyType();
	// 
	// 	RefPtr<TestBasicObject> tbo(new TestBasicObject());
	// 	t.Assert(tbo->GetType() == tboStaticType, "The two types were not the same.");
	// });

	// h->It("the properties in an object instance should be retrievable when passing in a RefPtr", [](TestCase& t) {
	// 	using namespace rttr;
	// 
	// 	std::string testStringValue("testing value!");
	// 	RefPtr<TestObjectWithFields> test(new TestObjectWithFields(testStringValue));
	// 	// test->testString = testStringValue;
	// 
	// 	Result<void, Error> result;
	// 	t.Assert(result.has_value(), "Firestorm::Result<void> does not contain a value...");
	// 
	// 	Mirror::Instance objInstance(test);
	// 	FireClassID testType = test->GetType();
	// 
	// 	Mirror::Property testStringProperty = testType.get_property("testString");
	// 	Mirror::Variant extractedValueVariant = testStringProperty.get_value(objInstance);
	// 
	// 	t.Assert(extractedValueVariant.get_value<std::string>() == testStringValue, "testStringValue != extractedValueVariant");
	// });


	h->It("the event dispatcher should allow the registration of events", [](TestCase& t) {
		std::unique_ptr<TestEventObject> object(std::make_unique<TestEventObject>());

		EventDispatcher dispatcher;
		EventDispatcher::Receipt receipt = dispatcher.Register<TestEvent>([](const TestEvent& str){
			FIRE_LOG_DEBUG("TEST EVENT: %s", str.Message);
		});

		TestEventObject o;
		EventDispatcher::Receipt objReceipt = dispatcher.Register<TestEvent>([&o](const TestEvent& e) {
			o.Invoke(e);
		});// &TestEventObject::Invoke, &o);

		t.Assert(dispatcher.GetNumRegisteredEvents() == 2, "There was an incorrect number of registered events.");

		dispatcher.Dispatch(TestEvent("Hello test suite!"));
	});

	h->It("the event dispatcher should unregister when receipts fall out of scope", [](TestCase& t) {
		std::unique_ptr<TestEventObject> object(std::make_unique<TestEventObject>());
		EventDispatcher dispatcher;

		{
			EventDispatcher::Receipt receipt = dispatcher.Register<TestEvent>([](const TestEvent& str) {
				FIRE_LOG_DEBUG("TEST EVENT: %s", str.Message);
			});
			t.Assert(dispatcher.GetNumRegisteredEvents() == 1, "There was an incorrect number of registered events.");
			t.Assert(dispatcher.GetNumRegisteredReceipts() == 1, "There was an incorrect number of registered receipts.");
			dispatcher.Dispatch(TestEvent("Hello test suite!"));
		}
		t.Assert(dispatcher.GetNumRegisteredEvents() == 0, "There was an incorrect number of registered events.");
		t.Assert(dispatcher.GetNumRegisteredReceipts() == 0, "There was an incorrect number of registered receipts.");
	});

	return h;
}