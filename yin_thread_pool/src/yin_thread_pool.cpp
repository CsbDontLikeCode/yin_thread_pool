#include <thread>
#include <iostream>
#include <mutex>
#include <string>
#include <condition_variable>
#include <queue>
#include <functional>

class ThreadPool
{
public:
	ThreadPool(int threadNum)
	{
		stop = false;
		for (int i = 0; i < threadNum; i++)
		{
			threadVector.emplace_back([this] {
				while (1) 
				{
					std::unique_lock<std::mutex> lock(mtx);

					condition.wait(lock, [this] {
						return !taskQueue.empty() || stop;
						});

					if (stop && taskQueue.empty())
						return;

					std::function<void()> task(std::move(taskQueue.front()));
					taskQueue.pop();

					lock.unlock();

					task();
				}
			});
		}
	}

	~ThreadPool()
	{
		{
			std::unique_lock<std::mutex> lock(mtx);
			stop = true;
		}
		condition.notify_all();
		for (auto& t : threadVector)
		{
			t.join();
		}
	}

	template<class F, class... Args>
	void enqueue(F&& f, Args&&... args)
	{
		std::function<void()> task = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
		{
			std::unique_lock<std::mutex> lock(mtx);
			taskQueue.emplace(std::move(task));
		}
		condition.notify_one(); 
	}
private:
	std::vector<std::thread> threadVector;
	std::queue < std::function<void()>> taskQueue;

	std::mutex mtx;
	std::condition_variable condition;

	bool stop;
};



// Examples
void hello(std::string name) 
{
	for (int i = 0; i < 1000; i++)
		std::cout << "Hello, " << name << "!" << std::endl;
}

void print(int value)
{
	for (int i = 0; i < 1000; i++)
		std::cout << "Print-" << value << std::endl;
}

void fire()
{
	for(int i = 0; i < 1000; i++)
		std::cout << "Shot!" << std::endl;
}

int main(void)
{
	ThreadPool pool(4);

	pool.enqueue(hello, "Niko");
	pool.enqueue(hello, "Ganyu");
	pool.enqueue(print, 123);
	pool.enqueue(fire);

	return 0;
}