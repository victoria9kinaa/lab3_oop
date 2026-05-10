#include "../Common/SysProg.h"

#include <future>
#include <tuple>
#include <algorithm>
#include <random>
#include <numeric>
#include <execution>


void start1()
{
	const int nThreads = 10;
	
	mutex evMutex;
	condition_variable cv;
	bool ready = false;

	for (int i = 0; i < nThreads; ++i)
	{
		thread([&](int id)
		{
			SafeWrite("Thread", id, "start");
			unique_lock<mutex> ul(evMutex);
			cv.wait(ul, [&] {SafeWrite("cv1 wait completed", id); return ready; });
//			cv.wait(ul);

			SafeWrite("Thread", id, "cv capture");
			Sleep(1000);
			SafeWrite("Thread", id, "end");

			cv.notify_one();
		}, i).detach();
	}
	SafeWrite("Threads created");
	_getch();
	{
		lock_guard ul(evMutex);
		ready = true;
	}
	cv.notify_one();
	_getch();
}

void start2()
{
	const int nThreads = 10;
	mutex evMutex;
	condition_variable cv;

	for (int i = 0; i < nThreads; ++i)
	{
		thread([&](int id)
		{
			SafeWrite("Thread", id, "start");
			unique_lock ul(evMutex);
			cv.wait(ul);

			SafeWrite("Thread", id, "cv capture");
			Sleep(1000);
			SafeWrite("Thread", id, "end");
		}, i).detach();
	}
	SafeWrite("Threads created");
	_getch();
	cv.notify_all();
	_getch();
}

void start3()
{
	const int nThreads = 10;
	shared_mutex evMutex;
	condition_variable_any cv;

	for (int i = 0; i < nThreads; ++i)
	{
		thread([&](int id)
		{
			SafeWrite("Thread", id, "start");
			shared_lock<shared_mutex> sl(evMutex);
			cv.wait(sl);

			SafeWrite("Thread", id, "cv capture");
			Sleep(1000);
			SafeWrite("Thread", id, "end");


		}, i).detach();
	}
	SafeWrite("Threads created");
	_getch();
	cv.notify_all();
	_getch();
}

template<typename T> void generate(vector<T>& v, int n)
{
	default_random_engine rnd;
	random_device dev;
	uniform_real_distribution<double> distr(0, 100);
	rnd.seed(dev());

	v.clear();
	for (int i = 0; i < n; i++)
	{
		v.push_back(distr(rnd));
	}
}

void sum(vector<double>::iterator begin, vector<double>::iterator end, promise<double> sump)
{
	sump.set_value(accumulate(begin, end, 0.0));
	//set_value_at_thread_exit
}

void sleeper(std::promise<void> ev)
{
	SafeWrite("sleeper start");
	std::this_thread::sleep_for(std::chrono::seconds(5));
	SafeWrite("sleeper done");
	ev.set_value();
}

void start4()
{
	vector<double> v;
	generate(v, 10000000);

	std::promise<double> sump;
	std::future<double> sumf = sump.get_future();
	std::thread sumt(sum, v.begin(), v.end(), std::move(sump));
	SafeWrite(sumf.get());

	sumt.join();

	std::promise<void> ev;
	std::future<void> evf = ev.get_future();
	std::jthread new_work_thread(sleeper, std::move(ev));
	SafeWrite("waiting");
	evf.wait();
	SafeWrite("waiting done");
}

// выборочное среднее для диапазона значений
template<typename T>
double mean(T begin, T end)
{
	return accumulate(begin, end, 0.0) / double(end - begin);
}


// выборочная дисперсия (несмещенная) для диапазона значений
tuple<double, double, size_t> mean_and_variance(vector<double>::const_iterator begin, vector<double>::const_iterator end)
{
	const double a = mean(begin, end);
	const size_t N = end - begin;

	return make_tuple(a,
		accumulate(begin, end, 0.0,				// расчет дисперсии
			[&a, &N](const double& accumulator, const double& y) -> double
			{
				return accumulator + double((y - a) * (y - a)) / double(N - 1);
			}),
		N);
}


void threaded_calc(const vector<double>& v, int nThreads)
{
	const auto t1 = chrono::steady_clock::now();
	vector<future<tuple<double, double, size_t>>> future_res(nThreads);
	double mean = 0;
	double variance = 0;

	cout << endl << nThreads << " threads: " << endl;

	size_t count = v.size() / nThreads;
	for (int i = 0; i < nThreads; i++)
	{
		if (i < nThreads - 1)
			future_res[i] = async(std::launch::async, mean_and_variance, v.begin() + i * count, v.begin() + (i + 1) * count);
		else
			future_res[i] = async(std::launch::async, mean_and_variance, v.begin() + i * count, v.end());
	}

	for (auto& r : future_res)
	{
		auto [m, v, n] = r.get();
		cout << m << "\t" << v << "\t" << n << endl;
		mean += m;
		variance += v * n;
	}

	mean /= nThreads;
	variance /= v.size();

	const auto t2 = chrono::steady_clock::now();
	const auto msec = chrono::duration_cast<chrono::milliseconds>(t2 - t1).count();
	cout << msec << " msec" << endl << mean << endl << variance << endl << endl;
}


void start5()
{
	int nMax = thread::hardware_concurrency();

	vector<double> v;
	generate(v, 10000000);

	auto [mean, variance, n] = mean_and_variance(v.begin(), v.end());
	cout << mean << ", " << variance << endl;
	for (int n = 1; n <= nMax * 2; n++)
	{
		threaded_calc(v, n);
	}

}

void foreach_calc(const vector<double>& v, int nThreads)
{
	const auto t1 = chrono::steady_clock::now();
	vector<tuple<tuple<double, double, size_t>, vector<double>::const_iterator, vector<double>::const_iterator>> res(nThreads);
	double mean = 0;
	double variance = 0;

	cout << endl << nThreads << " threads: " << endl;

	size_t count = v.size() / nThreads;

	for (int i = 0; i < nThreads; i++)
	{
		std::get<1>(res[i]) = v.begin() + i * count;
		if (i < nThreads - 1)
		{
			std::get<2>(res[i]) = v.begin() + (i + 1) * count;
		}
		else
		{
			std::get<2>(res[i]) = v.end();
		}
	}

	//	std::for_each(std::execution::seq, res.begin(), res.end(), [&](auto& r)
	std::for_each(std::execution::par, res.begin(), res.end(), [&](auto& r)
		{
			std::get<0>(r) = mean_and_variance(std::get<1>(r), std::get<2>(r));
		});

	for (auto& r : res)
	{
		auto [m, v, n] = std::get<0>(r);
		cout << m << "\t" << v << "\t" << n << endl;
		mean += m;
		variance += v * n;
	}

	mean /= nThreads;
	variance /= v.size();

	const auto t2 = chrono::steady_clock::now();
	const auto msec = chrono::duration_cast<chrono::milliseconds>(t2 - t1).count();
	cout << msec << " msec" << endl << mean << endl << variance << endl << endl;
}


void start6()
{
	int nMax = thread::hardware_concurrency();

	vector<double> v;
	generate(v, 10000000);

	auto [mean, variance, n] = mean_and_variance(v.begin(), v.end());
	cout << mean << ", " << variance << endl;
	for (int n = 1; n <= nMax * 2; n++)
	{
		foreach_calc(v, n);
	}

}

int main()
{
	start1();
//	start2();
//	start3();
//	start4();
//	start5();
//	start6();

	return 0;
}
