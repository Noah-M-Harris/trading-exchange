// queue that handles where next to write and read data: used by multiple threads, needs to be atomic

#pragma once
#include <iostream>
#include <vector>
#include <atomic>

namespace Common {
    template<typename T>
    class LFQueue final {

        private:
            std::vector<T> store;
            std::atomic<size_t> next_write_index_ = {0};
            std::atomic<size_t> next_read_index_ = {0};
            std::atomic<size_t> num_elements_ = {0};


        public:
            LFQueue(atd::size_t num_elems) : store_(num_elems, T()) {
                
            }

            auto getNextToWriteTo() noexcept {
                return &store_[next_write_index_];
            }

            auto updateWriteIndex() noexcept {
                next_write_index_ = (next_write_index_ + 1) % store_.size();
                num_elements_++;
            }

            auto getNextToRead() noexcept {
                return (next_read_index_ == next_write_index_) ? nullptr : &store_[next_read_index_];
            }

            auto updateReadIndex() noexcept {
                next_read_index_ = (next_read_index_ + 1) % store_.size();
                ASSERT(num_elements_ != 0, "Read an invalid element in:" + std::to_string(pthread_self()));
                num_elements_--;
            }

            auto size() const noexcept {
                return num_elements_.load();
            }

            LFQueue() = delete;
            LFQueue(const LFQueue&) = delete;
            LFQueue(const LFQueue&&) = delete;
            LFQueue& operator = (const LFQueue&) = delete;
            LFQueue& operator = (const LFQueue&&) = delete;

    };
}
