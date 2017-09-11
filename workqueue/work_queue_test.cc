

#include <gtest/gtest.h>
#include "work_queue.h"

class WorkQueueTest : public ::testing::Test {
protected:
  WorkQueueTest() {
    // You can do initial set-up work for each test here.
  }

  virtual ~WorkQueueTest() {
    // You can do clean-up work that doesn't throw exceptions here.
  }

  virtual void SetUp() {
    // Code here will be called immediately after the constructor (right
    // before each test).
  }

  virtual void TearDown() {
    // Code here will be called immediately after each test (right
    // before the destructor).
  }

};

TEST_F(WorkQueueTest, QueueTest) {
  WorkQueue<string, string> test_queue;
  test_queue.Add("10", "Value10");
  test_queue.Add("3", "Value3");
  test_queue.Add("2", "Value2");
  test_queue.Add("4", "Value4");
  //test_queue.Add("1", "Value11");
  //test_queue.Add("1", "Value111");
  test_queue.Add("10", "Value1010");
  test_queue.Add("10", "Value101010");
  test_queue.print_queue();
  struct KeyItems<string, string> get_key_items = test_queue.Get();
  cout << "key = " << get_key_items.key << endl;
  cout << "values: " << endl;
  for(auto it = get_key_items.Items.begin(); it != get_key_items.Items.end(); ++it)
    cout << *it << " " << endl;
  test_queue.print_queue();
  test_queue.Add("1", "Value111");
  test_queue.print_queue();
  test_queue.Done("2");
  test_queue.print_queue();
}


int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
