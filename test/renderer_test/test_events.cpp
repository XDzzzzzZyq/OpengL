#include "pch.h"
#include "Events.h"

// Minimal event types used only in these tests
struct EvA { int value; };
struct EvB { int value; };
struct EvC { int value; };

// ---------------------------------------------------------------------------
// Basic enqueue / process behaviour
// ---------------------------------------------------------------------------

TEST(EventPool, Event_Count)
{
	EventPool pool;
	int received = 0;

	pool.subscribe<EvA>([&](const EvA&) { received++; });

	pool.emit(EvA{ 1 });
	pool.emit(EvA{ 2 });

	// Nothing dispatched yet
	EXPECT_EQ(received, 0);

	pool.process();

	EXPECT_EQ(received, 2);
}

TEST(EventPool, Event_Order)
{
	EventPool pool;
	std::vector<int> order;

	pool.subscribe<EvA>([&](const EvA& e) { order.push_back(e.value); });

	pool.emit(EvA{ 10 });
	pool.emit(EvA{ 20 });
	pool.emit(EvA{ 30 });

	pool.process();

	ASSERT_EQ(order.size(), 3u);
	EXPECT_EQ(order[0], 10);
	EXPECT_EQ(order[1], 20);
	EXPECT_EQ(order[2], 30);
}

TEST(EventPool, Event_Order2)
{
	EventPool pool;
	std::vector<int> order;

	pool.subscribe<EvA>([&](const EvA& e) { order.push_back(e.value); });
	pool.subscribe<EvB>([&](const EvB& e) { order.push_back(e.value + 100); });

	pool.emit(EvA{ 1 });
	pool.emit(EvB{ 2 });
	pool.emit(EvA{ 3 });

	pool.process();

	ASSERT_EQ(order.size(), 3u);
	EXPECT_EQ(order[0], 1);
	EXPECT_EQ(order[1], 102);
	EXPECT_EQ(order[2], 3);
}

TEST(EventPool, Event_Empty)
{
	EventPool pool;
	int received = 0;

	pool.subscribe<EvA>([&](const EvA&) { received++; });

	// process() on an empty queue must not throw or crash
	pool.process();
	pool.process();

	EXPECT_EQ(received, 0);
}

// ---------------------------------------------------------------------------
// Event chain ordering — sub-events must be appended to end of queue
// ---------------------------------------------------------------------------

// When EvA is handled it emits EvB; when EvB is handled it emits EvC.
// Expected dispatch sequence: A → B → C  (breadth-first / FIFO).
TEST(EventPool, Event_Chain)
{
	EventPool pool;
	std::vector<std::string> order;

	pool.subscribe<EvA>([&](const EvA& e) {
		order.push_back("A");
		pool.emit(EvB{ e.value });
	});

	pool.subscribe<EvB>([&](const EvB& e) {
		order.push_back("B");
		pool.emit(EvC{ e.value });
	});

	pool.subscribe<EvC>([&](const EvC&) {
		order.push_back("C");
	});

	pool.emit(EvA{ 1 });
	pool.process();

	ASSERT_EQ(order.size(), 3u);
	EXPECT_EQ(order[0], "A");
	EXPECT_EQ(order[1], "B");
	EXPECT_EQ(order[2], "C");
}

// Two independent EvA events each trigger a chain.
// Expected order: A1 → A2 → B1 → B2 → C1 → C2  (breadth-first across chains).
TEST(EventPool, Event_MultiChain)
{
	EventPool pool;
	std::vector<std::string> order;

	pool.subscribe<EvA>([&](const EvA& e) {
		order.push_back("A" + std::to_string(e.value));
		pool.emit(EvB{ e.value });
	});

	pool.subscribe<EvB>([&](const EvB& e) {
		order.push_back("B" + std::to_string(e.value));
		pool.emit(EvC{ e.value });
	});

	pool.subscribe<EvC>([&](const EvC& e) {
		order.push_back("C" + std::to_string(e.value));
	});

	pool.emit(EvA{ 1 });
	pool.emit(EvA{ 2 });
	pool.process();

	// A1 and A2 were in queue first → dispatched first.
	// B1 and B2 are enqueued (in that order) while A1/A2 are processed.
	// C1 and C2 are enqueued while B1/B2 are processed.
	ASSERT_EQ(order.size(), 6u);
	EXPECT_EQ(order[0], "A1");
	EXPECT_EQ(order[1], "A2");
	EXPECT_EQ(order[2], "B1");
	EXPECT_EQ(order[3], "B2");
	EXPECT_EQ(order[4], "C1");
	EXPECT_EQ(order[5], "C2");
}
