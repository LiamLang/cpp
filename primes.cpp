#include <iostream>
#include <fstream>
#include <chrono>
#include <vector>
#include <thread>

int main()
{
	static constexpr long numBlocks = 1'000;
	static constexpr long blockSize = 1'000'000;

	const auto startTime = std::chrono::system_clock::now();

	std::vector<std::vector<long>> primes;

	for (int i = 0; i < numBlocks; i++)
	{
		primes.push_back(std::vector<long>());
		primes.back().reserve(blockSize / 10);
	}

	const auto calculate = [&](const long block) {

		const long start = block == 0 ? 2 : block * blockSize;
		const long end = (block + 1) * blockSize;

		for (long i = start; i < end; i++)
		{
			long j = 0;
			auto it = primes.at(0).cbegin();

			while (it != primes.at(0).cend())
			{
				j = *it;

				if (j * j > i || i % j == 0)
				{
					break;
				}

				it++;
			}

			if (j == 0 || j * j > i)
			{
				primes.at(block).push_back(i);
			}
		}

		std::cout << std::to_string(start) << "-" << std::to_string(end) << std::endl;
	};

	calculate(0);

	const unsigned int hardwareConcurrency = std::thread::hardware_concurrency();

	std::vector<std::thread> threads;
	threads.reserve(hardwareConcurrency);

	long block = 1;

	while(block < numBlocks)
	{
		while (block < numBlocks && threads.size() < hardwareConcurrency)
		{
			threads.emplace_back(calculate, block++);
		}

		threads.front().join();
		threads.erase(threads.begin());
	}

	for (auto& thread : threads)
	{
		thread.join();
	}

	std::ofstream file;
	file.open("primes.txt");

	for (const auto& blockResults : primes)
	{
		for (const auto prime : blockResults)
		{
			file << std::to_string(prime) << "\n";
		}
	}
	file.close();

	const auto endTime = std::chrono::system_clock::now();
	const auto timeDiff = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
	std::cout << std::to_string(timeDiff.count()) << " ms\n" << std::endl;

	return 0;
}