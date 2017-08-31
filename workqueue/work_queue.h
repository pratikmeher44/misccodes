#include <unordered_map>
//#include "misc/utils.h"
// need to include the required headers

template <class KeyType, class ItemsType>
struct KeyItems {
public:
  KeyType key;
  vector<ItemsType> Items;
  bool shuttingDown;
};

template <class Type1, class Type2>
class WorkQueue {

public:
WorkQueue() : shuttingDown(false) {
}

~WorkQueue() {
}

// Add function
void Add(Type1 key, Type2 item)
{
  boost::lock_guard<boost::recursive_mutex> lock_add(work_queue_lock_);
  if(shuttingDown)
    return;
  if (processing.find(key) != processing.end()) {
    // Key is under processing. Can not add it to the queue.
    toQueue.insert(pair<Type1, Type2>(key, item));
    return;
  }
  else {
    typename unordered_multimap<Type1, Type2>::iterator it = items.find(key);
    items.insert(pair<Type1, Type2>(key, item));
    if(it == items.end()) {
      // New key in the queue. Send signal.
      key_queue.push_back(key);
      // Notify any other threads waiting to execute processes
      cond_var_.notify_one();
    }
  }
}

// Get function
struct KeyItems<Type1, Type2>& Get()
{
  boost::lock_guard<boost::recursive_mutex> lock(work_queue_lock_);
  while((key_queue.size() == 0) && !shuttingDown) {
    cond_var_.wait(work_queue_lock_);
  }
  if(key_queue.size() == 0) {
    // We must be shutting down.
    get_key_items.key = (Type1)NULL;
    get_key_items.Items.push_back((Type2)NULL);
    get_key_items.shuttingDown = true;
    return get_key_items;
  }
  Type1 key = key_queue.front();
  get_key_items.key = key;
  key_queue.erase(key_queue.begin() + 0);
  // Add key to the processing set.
  processing.insert(key);
  auto key_items = items.equal_range(key);
  for(auto it = key_items.first; it != key_items.second; ++it) {
    get_key_items.Items.insert(get_key_items.Items.begin(), it->second);
  }
  items.erase(key);
  get_key_items.shuttingDown = false;
  return get_key_items;
}

// Done function
void Done(Type1 key)
{
  boost::lock_guard<boost::recursive_mutex> lock(work_queue_lock_);
  processing.erase(key);
  auto key_items = toQueue.equal_range(key);
  if(key_items.first != key_items.second) {
    key_queue.push_back(key);
    for (auto it = key_items.first; it != key_items.second; ++it) {
      items.insert(pair<Type1, Type2>(key, it->second));
    }
    toQueue.erase(key);
    cond_var_.notify_one();
  }
}

// ShutDown function
void ShutDown()
{
  boost::lock_guard<boost::recursive_mutex> lock(work_queue_lock_);
  shuttingDown = true;
  cond_var_.notify_all();
}

// ShuttingDown
bool ShuttingDown()
{
  boost::lock_guard<boost::recursive_mutex> lock(work_queue_lock_);
  return shuttingDown;
}

void print_queue() {
  LOG(INFO) << "KEY QUEUE:";
  for(auto it = key_queue.begin(); it != key_queue.end(); ++it) {
    LOG(INFO) << "Key = " << *it;
  }

  LOG(INFO) << "ITEMS QUEUE:";
  for(auto it = items.begin(); it != items.end(); ++it) {
    LOG(INFO) << it->first << " : " << it->second;
  }

  LOG(INFO) << "TOQUEUE:";
  for(auto it = toQueue.begin(); it != toQueue.end(); ++it) {
    LOG(INFO) << it->first << ":" << it->second;
  }
  LOG(INFO) << "\n";
}

private:
  boost::recursive_mutex work_queue_lock_;
  boost::condition_variable_any cond_var_;
  vector<Type1> key_queue;
  unordered_set<Type1> processing;
  bool shuttingDown;
  unordered_multimap<Type1, Type2> items;
  unordered_multimap<Type1, Type2> toQueue;
  struct KeyItems<Type1, Type2> get_key_items;
};

