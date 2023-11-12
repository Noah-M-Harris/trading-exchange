// memory pool represented as a single struct that signifies open/free memory
#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include "macros.h"

namespace Common {
    template<typename T>
    class MemPool final {
        private:
            struct ObjectBlock {
                T object_;
                bool is_free_ = true;
            };

            std::vector<ObjectBlock> store_;
            size_t next_free_index_ = 0;

            auto updateNextFreeIndex() noexcept {
                const auto initial_free_index = next_free_index_;
                while (!store_[next_free_index_].is_free) {
                    ++next_free_index_;
                    if (UNLIKELY(next_free_index_ == store_.size())) {
                        // loop back around to 0
                        next_free_index_ = 0;

                    }

                    if (UNLIKELY(initial_free_index == next_free_index_)) {
                        ASSERT(initial_free_index != next_free_index_, "Mem Pool out of space!");
                    }
                }
            }

        public:
            explicit MemPool(std::size_t num elems) : store_(num_elems, {T(), true}) {
                ASSERT(reinterpret_cast<const ObjectBlock *> (&(store_[0].object)) == &(store_[0]), "T object should be first member of ObjectBlock.");
            }

            // prohibit accidental calls
            MemPool() = delete;
            MemPool(const MemPool&) = delete;
            MemPool(cosnt MemPool&&) = delete;
            MemPool& operator = (const MemPool&) = delete;
            MemPool& operator = (const MemPool&&) = delete;

            template<typename... Args>
            T *allocate(Args... args) noexcept {
                auto obj_block = &(store_[next_free_index_]);
                ASSERT(obj_block->is_free_, "Expected free ObjectBlock @ idx:" + std::to_string(next_free_index_));
                T *ret = &(obj_block->object_);
                ret = new(ret) T(args...);
                obj_block->is_free = false;
                updateNextFreeIndex();
                return ret;
            }

    };
}