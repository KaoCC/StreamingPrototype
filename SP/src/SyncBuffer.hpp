#ifndef _SP_SYNCBUFFER_HPP_
#define _SP_SYNCBUFFER_HPP_

#include <mutex>
#include <condition_variable>
#include <deque>
#include <memory>

namespace SP {


	template <typename T>
	class SyncBuffer {
	public:

		using DataPointer = std::unique_ptr<T>;

		SyncBuffer() = default;

		void insert(DataPointer dataPtr) {

			{
				std::unique_lock<std::mutex> lock(mu);

				cond.wait(lock, [this]() { return buffer.size() < kSize; });

				buffer.push_back(std::move(dataPtr));

			}

			cond.notify_all();
			return;
		}

		DataPointer remove() {


			DataPointer ptr = nullptr;

			{
				std::unique_lock<std::mutex> lock(mu);

				cond.wait(lock, [this]() {return buffer.size() > 0; });

				ptr = std::move(buffer.back());
				buffer.pop_back();
			}

			cond.notify_all();
			return ptr;
		}



	private:

		// for sync
		std::mutex mu;
		std::condition_variable cond;

		std::deque<DataPointer> buffer;
		const size_t kSize = 10;		// tmp
	};


}






#endif

