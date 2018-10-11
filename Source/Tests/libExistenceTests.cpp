#include <libCore/libCore.h>
#include <libCore/RefPtr.h>
#include <libHarnessed/libHarnessed.h>

#include <libExistence/libExistence.h>
#include <libExistence/Universe.h>
#include <libExistence/System.h>
#include <libExistence/Entity.h>
#include <libExistence/ComponentDefinition.h>

#include <libMath/Vector.h>
#include <libMath/Quaternion.h>
#include <libMath/Matrix.h>

#include <libCore/Logger.h>

#include <iomanip>

using namespace Firestorm;

class PosRotComponent : public MappedComponentDefinition<Vector3, Quaternion>
{
public:
	FIRE_SOA_INDEX(ENTITY, 0);
	FIRE_SOA_INDEX(POSITION, 1);

	PosRotComponent(EntityMgr& eMgr)
	: DefType(eMgr, true)
	{
	}

	const Vector3& GetPosition(Instance i) const
	{
		return _this[POSITION][i];
	}

	void SetPosition(Instance i, const Vector3& value)
	{
		_this[POSITION][i] = value;
	}
private:
};

RefPtr<TestHarness> libExistencePrepareHarness(int ac, char** av)
{
	RefPtr<TestHarness> h(new TestHarness("libExistence"));


	h->It("entity managers should register and unregister destructor callbacks as the definitions fall out of scope", 
		[&](TestCase& t) {
			UUIDMgr uuidMgr;
			EntityMgr eMgr(uuidMgr);
			{
				PosRotComponent componentMgr(eMgr);
				t.Assert(eMgr.GetNumRegisteredDestructors() == 1, "the destructor callback was not registered");
			}
			t.Assert(eMgr.GetNumRegisteredDestructors() == 0, "the destructor callback was not unregistered");
		});

	h->It("component definitions should be able to map entities to components", [&](TestCase& t) {
		UUIDMgr uuidMgr;
		EntityMgr eMgr(uuidMgr);
		PosRotComponent componentMgr(eMgr);

		Entity e = eMgr.SpawnEntity();
		IComponentDefinition::Instance eye = componentMgr.Assign(e);
		t.Assert(eye != FIRE_INVALID_COMPONENT, "received an invalid component for some reason");

		IComponentDefinition::Instance i = componentMgr.Lookup(e);
		IComponentDefinition::Instance ii = componentMgr.Lookup(e);
		t.Assert(eye == i == ii, "the component manager did not properly map the Entity to the component");
	});

	h->It("component definitions should never double-map entities",[&](TestCase& t) {
		UUIDMgr uuidMgr;
		EntityMgr eMgr(uuidMgr);
		PosRotComponent componentMgr(eMgr);
		size_t count = 10;

		Vector<Entity> entities;
		entities.reserve(count);
		Vector<IComponentDefinition::Instance> components;
		components.reserve(count);

		for(size_t i=0; i<count; ++i)
		{
			entities.push_back(eMgr.SpawnEntity());
			IComponentDefinition::Instance ii = componentMgr.Assign(entities.back());
			components.push_back(componentMgr.Lookup(entities.back()));
		}

		for(size_t i=0; i<count; ++i)
		{
			IComponentDefinition::Instance ii = components[i];
			for(size_t j=0; j<count; j++)
			{
				if(i == j) continue;
				IComponentDefinition::Instance ji = components[j];
				t.Assert(ii != ji, "a component was double-mapped");
			}
		}
	});


	h->It("component definitions should re-map to their entities once one has been despawned",[&](TestCase& t) {
		UUIDMgr uuidMgr;
		EntityMgr eMgr(uuidMgr);
		PosRotComponent componentMgr(eMgr);
		size_t count = 10;

		Vector<Entity> entities;
		entities.reserve(count);
		Vector<IComponentDefinition::Instance> components;
		components.reserve(count);

		for(size_t i=0; i<count; ++i)
		{
			entities.push_back(eMgr.SpawnEntity());
			IComponentDefinition::Instance ii = componentMgr.Assign(entities.back());
			components.push_back(ii);
		}

		IComponentDefinition::Instance i_last = componentMgr.Lookup(entities.back());
		componentMgr.SetPosition(i_last, { 9001.0f,9001.0f,9001.0f });
		{
			const Vector3& pos_test = componentMgr.GetPosition(i_last);
			t.Assert(pos_test.x == 9001.0f, "the component did not have the right values");
			t.Assert(pos_test.y == 9001.0f, "the component did not have the right values");
			t.Assert(pos_test.z == 9001.0f, "the component did not have the right values");
		}

		// erase a random entity in the middle of the buffer
		auto ent = entities.begin() + 4;
		eMgr.DespawnEntity(*ent);
		entities.erase(ent);

		Entity testEnt = entities.back();
		IComponentDefinition::Instance i_test = componentMgr.Lookup(testEnt);
		const Vector3& pos_test = componentMgr.GetPosition(i_test);
		t.Assert(pos_test.x == 9001.0f, "the component did not have the right values");
		t.Assert(pos_test.y == 9001.0f, "the component did not have the right values");
		t.Assert(pos_test.z == 9001.0f, "the component did not have the right values");
	});


	h->It("see above, only we're removing what would be the last component",[&](TestCase& t) {
		UUIDMgr uuidMgr;
		EntityMgr eMgr(uuidMgr);
		PosRotComponent componentMgr(eMgr);
		size_t count = 10;

		Vector<Entity> entities;
		entities.reserve(count);
		Vector<IComponentDefinition::Instance> components;
		components.reserve(count);

		for(size_t i=0; i<count; ++i)
		{
			entities.push_back(eMgr.SpawnEntity());
			IComponentDefinition::Instance ii = componentMgr.Assign(entities.back());
			components.push_back(ii);
		}

		IComponentDefinition::Instance i_last = componentMgr.Lookup(entities.back());
		componentMgr.SetPosition(i_last, { 9001.0f,9001.0f,9001.0f });
		{
			const Vector3& pos_test = componentMgr.GetPosition(i_last);
			t.Assert(pos_test.x == 9001.0f, "the component did not have the right values");
			t.Assert(pos_test.y == 9001.0f, "the component did not have the right values");
			t.Assert(pos_test.z == 9001.0f, "the component did not have the right values");
		}

		// erase a random entity in the middle of the buffer
		Entity ent = entities[entities.size()-1];
		eMgr.DespawnEntity(ent);
		entities.erase(entities.begin() + entities.size()-1);

		Entity testEnt = entities.back();
		IComponentDefinition::Instance i_test = componentMgr.Lookup(testEnt);
		const Vector3& pos_test = componentMgr.GetPosition(i_test);
		t.Assert(pos_test.x == 9001.0f, "the component did not have the right values");
		t.Assert(pos_test.y == 9001.0f, "the component did not have the right values");
		t.Assert(pos_test.z == 9001.0f, "the component did not have the right values");
	});

	return h;
}

	/*h->It("benchmark[OOP Complex]", [](TestCase&) {
		Vector<double> construction;
		Vector<double> constructors;
		Vector<double> flagSets;
		Vector<double> flagGets;
		Vector<double> destructions;
		for(size_t i=0;i<numRuns;++i)
		{
			//FIRE_LOG_DEBUG("+++++ Run %d +++++", i);
			auto begin = high_resolution_clock::now();
			{
				TupleBasedComponentMgr testMgr(benchmarkAllocations);
				log_duration("manager construction", begin, construction);
				begin = high_resolution_clock::now();

				for(size_t i = 0; i < benchmarkAllocations; ++i)
				{
					//if(i % 5000 == 0)
					//FIRE_LOG_DEBUG("%d allocations performed...", i);
					testMgr.MakeInstance();
				}

				log_duration("component additions", begin, constructors);

				//FIRE_LOG_DEBUG("+++++ beginning %d flag sets", benchmarkAllocations);
				begin = high_resolution_clock::now();
				for(size_t i = 0; i < benchmarkAllocations; ++i)
				{
					//if(i % 5000 == 0)
					//FIRE_LOG_DEBUG("%d flag sets performed...", i);
					testMgr.SetName(ents[i], "Slim Shady");
				}
				log_duration("component name sets", begin, flagSets);

				begin = high_resolution_clock::now();
				for(size_t i = 0; i < benchmarkAllocations; ++i)
				{
					//if(i % 5000 == 0)
					//FIRE_LOG_DEBUG("%d flag sets performed...", i);
					String value(testMgr.GetName(ents[i]));
				}
				log_duration("component name gets", begin, flagGets);
				begin = high_resolution_clock::now();
			}
			log_duration("manager destruction", begin, destructions);
		}
		FIRE_LOG_DEBUG("+++++ Average over %d Runs +++++", numRuns);
		double constructiont = 0, compCons = 0,flagt = 0, flaggt=0, destructiont = 0;
		for(size_t i = 0; i < numRuns; ++i)
		{
			constructiont += construction[i];
			compCons += constructors[i];
			flagt += flagSets[i];
			flaggt += flagGets[i];
			destructiont += destructions[i];
		}
		FIRE_LOG_DEBUG("%f%f+++++ constructions     = %d", std::fixed, std::setprecision(15), constructiont / numRuns);
		FIRE_LOG_DEBUG("%f%f+++++ comp constructors = %d", std::fixed, std::setprecision(15), compCons / numRuns);
		FIRE_LOG_DEBUG("%f%f+++++ name sets         = %d", std::fixed, std::setprecision(15), flagt / numRuns);
		FIRE_LOG_DEBUG("%f%f+++++ name gets         = %d", std::fixed, std::setprecision(15), flaggt / numRuns);
		FIRE_LOG_DEBUG("%f%f+++++ destructions      = %d", std::fixed, std::setprecision(15), destructiont / numRuns);
	});
}*/

	/*h->It("Entities should be instantiable and contain proper ids", [](Firestorm::TestCase& t) {
		EntityMgr eMgr;

		Entity e = eMgr.Create();
		t.Assert(e.Index() == 0, "index was incorrect");
		t.Assert(e.Generation() == 0, "generation was incorrect");
	});

	h->It("components should report back their proper size", [](Firestorm::TestCase& t) {

		static size_t expectedSize = 4;

		size_t componentSize = TestComponent::SizeOf();

		FIRE_LOG_DEBUG("TestComponent (IComponent<uint8_t, uint8_t, uint8_t, uint8_t>) reported: %d", componentSize);

		t.Assert(expectedSize == componentSize, "the component reported back the incorrect size");
	});

	h->It("component field offsets should be proper", [](TestCase& t) {
		auto& memberDetailList = TestComponent::MyMembers();

		const size_t expectedMemberSize = sizeof(uint8_t);

		size_t expectedOffset = 0;
		for(size_t i=0; i<memberDetailList.size(); ++i)
		{
			auto& memberInfo = memberDetailList[i];
			FIRE_LOG_DEBUG("[%d] size = %d, offset = %d", i, memberInfo.Size, memberInfo.OffsetTo);
			t.Assert(memberInfo.Size == expectedMemberSize, 
				Format("size of member [%d] was wrong. expected %d. got %d", i, expectedMemberSize, memberInfo.Size));
			t.Assert(memberInfo.OffsetTo == expectedOffset,
				Format("offset of member [%d] was wrong. expected %d. got %d.", i, expectedOffset, memberInfo.OffsetTo));
			expectedOffset += memberInfo.Size;
		}
	});

	h->It("complex components should have the expected offsets as well", [](TestCase& t) {
		auto& memberDetailList = TestComplexComponent::MyMembers();
		size_t expectedOffset = 0;
		for(size_t i=0; i<memberDetailList.size(); ++i)
		{
			auto& memberInfo = memberDetailList[i];
			FIRE_LOG_DEBUG("[%d] size = %d, offset = %d", i, memberInfo.Size, memberInfo.OffsetTo);
			t.Assert(memberInfo.OffsetTo == expectedOffset,
				Format("offset of member [%d] was wrong. expected %d. got %d.", i, expectedOffset, memberInfo.OffsetTo));
			expectedOffset += memberInfo.Size;
		}
	});

	h->It("the component manager should allocate space properly", [](TestCase& t) {
		TestComponentMgr test;
		t.Assert(test.GetCapacity() == 5,
			Format(
			"test mgr capacity was an unexpected value. expected %d. got %d.",
			5,
			test.GetCapacity()));
	});

	h->It("components managers should manage their data buffers properly", [](TestCase& t) {

		// ensure that the capacity of the data buffer is enlarged when it is expected to be.
		// data buffer should expand when Count == Capacity and someone attempts to add a new instance.
		// in this instance, the capacity of the internal data buffer should grow by a factor of 2.
		{
			TestComponentMgr testMgr;
			EntityMgr eMgr;

			Entity e = eMgr.Create();
			size_t capacityBefore = testMgr.GetCapacity();

			Vector<Component> handles(testMgr.AddNewInstances(5));

			t.Assert(capacityBefore == testMgr.GetCapacity(),
				Format("the testMgr's capacity was modified when it shouldn't have been. before = %d. after = %d",
				capacityBefore, testMgr.GetCapacity()));

			handles.push_back(testMgr.AddNewInstance());
			t.Assert(testMgr.GetCapacity() == (capacityBefore * 2),
				Format("the testMgr's capacity was not modified properly. expected %d. got %d.",
				(capacityBefore * 2), testMgr.GetCapacity()));
		}


		{

		}
	});

	h->It("component managers should be able to access the fields of components properly", [](TestCase& t) {
		// test of a simple 4 byte component.
		{
			TestComponentMgr testMgr;
			EntityMgr eMgr;

			Entity e = eMgr.Create();
			Component c = testMgr.AddNewInstance();
			testMgr.SetFlag1(e, 20);
			uint8_t flag1 = testMgr.GetFlag1(e);
			t.Assert(flag1 == 20, "the flag value was not set properly");
		}

		// test of a more complicated component
		{
			TestComplexComponentMgr testMgr;
			EntityMgr eMgr;
			Entity e = eMgr.Create();
			Component c = testMgr.AddNewInstance();

			String testName = "Test Name";

			testMgr.SetName(e, testName);
			String retrieveTestName = testMgr.GetName(e);
			FIRE_LOG_DEBUG("retrieved the test name '%s'", retrieveTestName);
			t.Assert(testName == retrieveTestName,
				Format("the retrieved test name was wrong. expected %s. got %s", testName, retrieveTestName));
		}
	});

	using namespace std::chrono;
	static size_t benchmarkAllocations = 100000;

	static auto log_duration = [](const String& tag, const time_point<high_resolution_clock>& begin, Vector<double>& times)
	{
		auto dur = duration<double, std::nano>(high_resolution_clock::now() - begin);
		auto cnt = duration_cast<nanoseconds>(dur).count();
		
		double c = (double)cnt / 1000000000.0;
		//FIRE_LOG_DEBUG("+++++%f[%s] -> Time = %d seconds", std::setprecision(10), tag, c);
		times.push_back(c);
	};

	static EntityMgr eMgr;

	FIRE_LOG_DEBUG("+++++ allocating entities");
	static Vector<Entity> ents;
	ents.reserve(benchmarkAllocations);
	std::unordered_set<uint32_t> createdIndices;
	for(size_t i = 0; i < benchmarkAllocations; ++i)
	{
		Entity e = eMgr.Create();
		FIRE_ASSERT(createdIndices.find(e.Index()) == createdIndices.end());
		createdIndices.insert(e.Index());
		ents.push_back(e);
	}

	static size_t numRuns = 50;

	h->It("benchmark[Single Buffer Allocator]", [](TestCase&) {
		Vector<double> construction;
		Vector<double> constructors;
		Vector<double> flagSets;
		Vector<double> flagGets;
		Vector<double> destructions;
		for(size_t i=0;i<numRuns;++i)
		{
			//FIRE_LOG_DEBUG("+++++ Run %d +++++", i);
			auto begin = high_resolution_clock::now();
			{
				TestComponentMgr testMgr(benchmarkAllocations);
				log_duration("manager construction", begin, construction);
				begin = high_resolution_clock::now();

				// not needed in the alternate version
				for(size_t i = 0; i < benchmarkAllocations; ++i)
				{
					//if(i % 5000 == 0)
					//FIRE_LOG_DEBUG("%d allocations performed...", i);
					testMgr.AddNewInstance();
				}
				log_duration("component additions", begin, constructors);

				//FIRE_LOG_DEBUG("+++++ beginning %d flag sets", benchmarkAllocations);
				begin = high_resolution_clock::now();
				for(size_t i = 0; i < benchmarkAllocations; ++i)
				{
					//if(i % 5000 == 0)
					//FIRE_LOG_DEBUG("%d flag sets performed...", i);
					testMgr.SetFlag1(ents[i], 101);
				}
				log_duration("component flag sets", begin, flagSets);

				begin = high_resolution_clock::now();
				for(size_t i = 0; i < benchmarkAllocations; ++i)
				{
					//if(i % 5000 == 0)
					//FIRE_LOG_DEBUG("%d flag sets performed...", i);
					uint8_t value = testMgr.GetFlag1(ents[i]);
				}
				log_duration("component flag gets", begin, flagGets);

				begin = high_resolution_clock::now();
			}
			log_duration("manager destruction", begin, destructions);
		}

		FIRE_LOG_DEBUG("+++++ Average over %d Runs +++++", numRuns);
		double constructiont = 0, compCons = 0,flagt = 0, flaggt=0, destructiont = 0;
		for(size_t i = 0; i < numRuns; ++i)
		{
			constructiont += construction[i];
			compCons += constructors[i];
			flagt += flagSets[i];
			flaggt += flagGets[i];
			destructiont += destructions[i];
		}
		FIRE_LOG_DEBUG("%f%f+++++ constructions     = %d", std::fixed, std::setprecision(15), constructiont / numRuns);
		FIRE_LOG_DEBUG("%f%f+++++ comp constructors = %d", std::fixed, std::setprecision(15), compCons / numRuns);
		FIRE_LOG_DEBUG("%f%f+++++ flag sets         = %d", std::fixed, std::setprecision(15), flagt / numRuns);
		FIRE_LOG_DEBUG("%f%f+++++ flag gets         = %d", std::fixed, std::setprecision(15), flaggt / numRuns);
		FIRE_LOG_DEBUG("%f%f+++++ destructions      = %d", std::fixed, std::setprecision(15), destructiont / numRuns);
	});


	h->It("benchmark[Multi Buffer Allocator]", [](TestCase&) {
		Vector<double> construction;
		Vector<double> constructors;
		Vector<double> flagSets;
		Vector<double> flagGets;
		Vector<double> destructions;
		for(size_t i=0;i<numRuns;++i)
		{
			//FIRE_LOG_DEBUG("+++++ Run %d +++++", i);
			auto begin = high_resolution_clock::now();
			{
				TestComponentMgr_Alt testMgr(benchmarkAllocations);
				log_duration("manager construction", begin, construction);
				begin = high_resolution_clock::now();

				for(size_t i = 0; i < benchmarkAllocations; ++i)
				{
					//if(i % 5000 == 0)
					//FIRE_LOG_DEBUG("%d allocations performed...", i);
					testMgr.MakeInstance();
				}

				log_duration("component additions", begin, constructors);

				//FIRE_LOG_DEBUG("+++++ beginning %d flag sets", benchmarkAllocations);
				begin = high_resolution_clock::now();
				for(size_t i = 0; i < benchmarkAllocations; ++i)
				{
					//if(i % 5000 == 0)
					//FIRE_LOG_DEBUG("%d flag sets performed...", i);
					testMgr.SetFlag1(ents[i], 101);
				}
				log_duration("component flag sets", begin, flagSets);

				begin = high_resolution_clock::now();
				for(size_t i = 0; i < benchmarkAllocations; ++i)
				{
					//if(i % 5000 == 0)
					//FIRE_LOG_DEBUG("%d flag sets performed...", i);
					uint8_t value = testMgr.GetFlag1(ents[i]);
				}
				log_duration("component flag gets", begin, flagGets);
				begin = high_resolution_clock::now();
			}
			log_duration("manager destruction", begin, destructions);
		}
		FIRE_LOG_DEBUG("+++++ Average over %d Runs +++++", numRuns);
		double constructiont = 0, compCons = 0,flagt = 0, flaggt=0, destructiont = 0;
		for(size_t i = 0; i < numRuns; ++i)
		{
			constructiont += construction[i];
			compCons += constructors[i];
			flagt += flagSets[i];
			flaggt += flagGets[i];
			destructiont += destructions[i];
		}
		FIRE_LOG_DEBUG("%f%f+++++ constructions     = %d", std::fixed, std::setprecision(15), constructiont / numRuns);
		FIRE_LOG_DEBUG("%f%f+++++ comp constructors = %d", std::fixed, std::setprecision(15), compCons / numRuns);
		FIRE_LOG_DEBUG("%f%f+++++ flag sets         = %d", std::fixed, std::setprecision(15), flagt / numRuns);
		FIRE_LOG_DEBUG("%f%f+++++ flag gets         = %d", std::fixed, std::setprecision(15), flaggt / numRuns);
		FIRE_LOG_DEBUG("%f%f+++++ destructions      = %d", std::fixed, std::setprecision(15), destructiont / numRuns);
	});






	h->It("benchmark[Single Buffer Allocator Complex]", [](TestCase&) {
		Vector<double> construction;
		Vector<double> constructors;
		Vector<double> flagSets;
		Vector<double> flagGets;
		Vector<double> destructions;
		for(size_t i=0;i<numRuns;++i)
		{
			//FIRE_LOG_DEBUG("+++++ Run %d +++++", i);
			auto begin = high_resolution_clock::now();
			{
				TestComplexComponentMgr testMgr(benchmarkAllocations);
				log_duration("manager construction", begin, construction);
				begin = high_resolution_clock::now();

				// not needed in the alternate version
				for(size_t i = 0; i < benchmarkAllocations; ++i)
				{
					//if(i % 5000 == 0)
					//FIRE_LOG_DEBUG("%d allocations performed...", i);
					testMgr.AddNewInstance();
				}

				log_duration("component additions", begin, constructors);

				//FIRE_LOG_DEBUG("+++++ beginning %d flag sets", benchmarkAllocations);
				begin = high_resolution_clock::now();
				for(size_t i = 0; i < benchmarkAllocations; ++i)
				{
					//if(i % 5000 == 0)
					//FIRE_LOG_DEBUG("%d flag sets performed...", i);
					testMgr.SetName(ents[i], "Slim Shady");
				}
				log_duration("component name sets", begin, flagSets);

				begin = high_resolution_clock::now();
				for(size_t i = 0; i < benchmarkAllocations; ++i)
				{
					//if(i % 5000 == 0)
					//FIRE_LOG_DEBUG("%d flag sets performed...", i);
					String value(testMgr.GetName(ents[i]));
				}
				log_duration("component name gets", begin, flagGets);
				begin = high_resolution_clock::now();
			}
			log_duration("manager destruction", begin, destructions);
		}
		FIRE_LOG_DEBUG("+++++ Average over %d Runs +++++", numRuns);
		double constructiont = 0, compCons = 0,flagt = 0, flaggt=0, destructiont = 0;
		for(size_t i = 0; i < numRuns; ++i)
		{
			constructiont += construction[i];
			compCons += constructors[i];
			flagt += flagSets[i];
			flaggt += flagGets[i];
			destructiont += destructions[i];
		}
		FIRE_LOG_DEBUG("%f%f+++++ constructions     = %d", std::fixed, std::setprecision(15), constructiont / numRuns);
		FIRE_LOG_DEBUG("%f%f+++++ comp constructors = %d", std::fixed, std::setprecision(15), compCons / numRuns);
		FIRE_LOG_DEBUG("%f%f+++++ name sets         = %d", std::fixed, std::setprecision(15), flagt / numRuns);
		FIRE_LOG_DEBUG("%f%f+++++ name gets         = %d", std::fixed, std::setprecision(15), flaggt / numRuns);
		FIRE_LOG_DEBUG("%f%f+++++ destructions      = %d", std::fixed, std::setprecision(15), destructiont / numRuns);
	});


	h->It("benchmark[Multi Buffer Allocator Complex]", [](TestCase&) {
		Vector<double> construction;
		Vector<double> constructors;
		Vector<double> flagSets;
		Vector<double> flagGets;
		Vector<double> destructions;
		for(size_t i=0;i<numRuns;++i)
		{
			//FIRE_LOG_DEBUG("+++++ Run %d +++++", i);
			auto begin = high_resolution_clock::now();
			{
				TestComplexComponentMgr_Alt testMgr(benchmarkAllocations);
				log_duration("manager construction", begin, construction);
				begin = high_resolution_clock::now();

				for(size_t i = 0; i < benchmarkAllocations; ++i)
				{
					//if(i % 5000 == 0)
					//FIRE_LOG_DEBUG("%d allocations performed...", i);
					testMgr.MakeInstance();
				}

				log_duration("component additions", begin, constructors);

				//FIRE_LOG_DEBUG("+++++ beginning %d flag sets", benchmarkAllocations);
				begin = high_resolution_clock::now();
				for(size_t i = 0; i < benchmarkAllocations; ++i)
				{
					//if(i % 5000 == 0)
					//FIRE_LOG_DEBUG("%d flag sets performed...", i);
					testMgr.SetName(ents[i], "Slim Shady");
				}
				log_duration("component name sets", begin, flagSets);

				begin = high_resolution_clock::now();
				for(size_t i = 0; i < benchmarkAllocations; ++i)
				{
					//if(i % 5000 == 0)
					//FIRE_LOG_DEBUG("%d flag sets performed...", i);
					String value(testMgr.GetName(ents[i]));
				}
				log_duration("component name gets", begin, flagGets);
				begin = high_resolution_clock::now();
			}
			log_duration("manager destruction", begin, destructions);
		}
		FIRE_LOG_DEBUG("+++++ Average over %d Runs +++++", numRuns);
		double constructiont = 0, compCons = 0,flagt = 0, flaggt=0, destructiont = 0;
		for(size_t i = 0; i < numRuns; ++i)
		{
			constructiont += construction[i];
			compCons += constructors[i];
			flagt += flagSets[i];
			flaggt += flagGets[i];
			destructiont += destructions[i];
		}
		FIRE_LOG_DEBUG("%f%f+++++ constructions     = %d", std::fixed, std::setprecision(15), constructiont / numRuns);
		FIRE_LOG_DEBUG("%f%f+++++ comp constructors = %d", std::fixed, std::setprecision(15), compCons / numRuns);
		FIRE_LOG_DEBUG("%f%f+++++ name sets         = %d", std::fixed, std::setprecision(15), flagt / numRuns);
		FIRE_LOG_DEBUG("%f%f+++++ name gets         = %d", std::fixed, std::setprecision(15), flaggt / numRuns);
		FIRE_LOG_DEBUG("%f%f+++++ destructions      = %d", std::fixed, std::setprecision(15), destructiont / numRuns);
	});

	struct ComplexComponent
	{
		String name;
		float value1;
		double value2;
	};
	struct ComponentManager
	{

		ComponentManager(size_t allocations = 5)
		{
			components.reserve(allocations);
		}
		void MakeInstance()
		{
			components.push_back(ComplexComponent());
		}

		void SetName(const Entity& e, const String& name)
		{
			components[e.Index()].name = name;
		}

		const String& GetName(const Entity& e) const
		{
			return components[e.Index()].name;
		}

		Vector<ComplexComponent> components;
	};

	h->It("benchmark[OOP Complex]", [](TestCase&) {
		Vector<double> construction;
		Vector<double> constructors;
		Vector<double> flagSets;
		Vector<double> flagGets;
		Vector<double> destructions;
		for(size_t i=0;i<numRuns;++i)
		{
			//FIRE_LOG_DEBUG("+++++ Run %d +++++", i);
			auto begin = high_resolution_clock::now();
			{
				ComponentManager testMgr(benchmarkAllocations);
				log_duration("manager construction", begin, construction);
				begin = high_resolution_clock::now();

				for(size_t i = 0; i < benchmarkAllocations; ++i)
				{
					//if(i % 5000 == 0)
					//FIRE_LOG_DEBUG("%d allocations performed...", i);
					testMgr.MakeInstance();
				}

				log_duration("component additions", begin, constructors);

				//FIRE_LOG_DEBUG("+++++ beginning %d flag sets", benchmarkAllocations);
				begin = high_resolution_clock::now();
				for(size_t i = 0; i < benchmarkAllocations; ++i)
				{
					//if(i % 5000 == 0)
					//FIRE_LOG_DEBUG("%d flag sets performed...", i);
					testMgr.SetName(ents[i], "Slim Shady");
				}
				log_duration("component name sets", begin, flagSets);

				begin = high_resolution_clock::now();
				for(size_t i = 0; i < benchmarkAllocations; ++i)
				{
					//if(i % 5000 == 0)
					//FIRE_LOG_DEBUG("%d flag sets performed...", i);
					String value(testMgr.GetName(ents[i]));
				}
				log_duration("component name gets", begin, flagGets);
				begin = high_resolution_clock::now();
			}
			log_duration("manager destruction", begin, destructions);
		}
		FIRE_LOG_DEBUG("+++++ Average over %d Runs +++++", numRuns);
		double constructiont = 0, compCons = 0,flagt = 0, flaggt=0, destructiont = 0;
		for(size_t i = 0; i < numRuns; ++i)
		{
			constructiont += construction[i];
			compCons += constructors[i];
			flagt += flagSets[i];
			flaggt += flagGets[i];
			destructiont += destructions[i];
		}
		FIRE_LOG_DEBUG("%f%f+++++ constructions     = %d", std::fixed, std::setprecision(15), constructiont / numRuns);
		FIRE_LOG_DEBUG("%f%f+++++ comp constructors = %d", std::fixed, std::setprecision(15), compCons / numRuns);
		FIRE_LOG_DEBUG("%f%f+++++ name sets         = %d", std::fixed, std::setprecision(15), flagt / numRuns);
		FIRE_LOG_DEBUG("%f%f+++++ name gets         = %d", std::fixed, std::setprecision(15), flaggt / numRuns);
		FIRE_LOG_DEBUG("%f%f+++++ destructions      = %d", std::fixed, std::setprecision(15), destructiont / numRuns);
	});

	return h;
}*/