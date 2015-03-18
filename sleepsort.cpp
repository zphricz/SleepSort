#include <iostream>
#include <unistd.h>
#include <thread>
#include <condition_variable>
#include <vector>

using namespace std;

void do_sleep(unsigned int val, bool &run, condition_variable &sig,
              unsigned int *&loc, mutex &m) {
  /* Wait until all sleepers are spawned */
  if (!run) {
    unique_lock<mutex> lock(m);
    while (!run) {
      sig.wait(lock);
    }
  }
  usleep(val * 10000);

  /* Place value at destination */
  unsigned int *destination;
  {
    lock_guard<mutex> lock(m);
    destination = loc++;
  }
  *destination = val;
}

void sleep_sort(unsigned int *arr, int size) {
  mutex m;
  condition_variable sig;
  bool run = false;
  vector<thread> threads;

  /* Spawn the sleepers */
  for (int i = 0; i < size; ++i) {
    threads.push_back(
        thread(do_sleep, arr[i], ref(run), ref(sig), ref(arr), ref(m)));
  }
  {
    lock_guard<mutex> lock(m);
    run = true;
    sig.notify_all();
  }

  /* Wait for the sleepers */
  for (auto &t : threads) {
    t.join();
  }
}

int check_sorted(unsigned int *arr, int size) {
  int cnt = 0;
  for (int i = 0; i < size - 1; ++i) {
    if (arr[i] > arr[i + 1]) {
      cout << "FAILURE ON: [" << arr[i] << ", " << arr[i + 1] << ']' << endl;
      cnt++;
    }
  }
  return cnt;
}

int main(int argc, char *argv[]) {

  if (argc != 2) {
    cout << "USAGE: " << argv[0] << " <array size>" << endl;
    return 1;
  }
  int size = atoi(argv[1]);
  unsigned int *arr = new unsigned int[size];

  srand(time(NULL));
  for (int i = 0; i < size; ++i) {
    arr[i] = rand() % 100;
  }

  sleep_sort(arr, size);

  /* Pretty print it */
  cout << "[";
  for (int i = 0; i < size - 1; ++i) {
    cout << arr[i] << ", ";
  }
  cout << arr[size - 1] << "]" << endl;

  int num_failures = check_sorted(arr, size);
  if (num_failures == 0) {
    cout << "SUCCESS" << endl;
  } else {
    cout << "There were " << num_failures << " failures" << endl;
  }

  return 0;
}
