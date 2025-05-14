#pragma once
#include "types.h"
#include <type_traits>

namespace ema::network {
	class packet {
	   public:
		packet(const u64 size) { _malloc(size); }
		packet(const packet& lvalue) { operator=(lvalue); }
		packet(packet&& rvalue) { operator=(std::move(rvalue)); }
		~packet() {}

		inline packet& operator=(const packet& lvalue) noexcept {
			if (this == &lvalue) return *this;
			if (!lvalue.size_) { free(); return *this; }
			else if (!lvalue.offset_) { clear(); return *this; }
			else if (lvalue.size_ > size_) _realloc(lvalue.size_);
			std::memcpy(data_, lvalue.data_, lvalue.offset_);
			if (lvalue.offset_ < size_) std::memset(data_ + lvalue.offset_, 0, size_ - lvalue.offset_);
			offset_ = lvalue.offset_;
			return *this;
		}

		inline packet& operator=(packet&& rvalue) noexcept {
			data_ = rvalue.data_;
			size_ = rvalue.size_;
			offset_ = rvalue.offset_;
			rvalue.data_ = nullptr;
			rvalue.size_ = rvalue.offset_ = 0;
			return *this;
		}

		inline bool empty() const { return !size_; }
		inline u64 size() const { return size_; }
		inline u64 length() const { return offset_; }
		inline u64 remaining() const { return size_ > offset_ ? size_ - offset_ : 0; }
		inline void reset_pos() { read_pos_ = 0; }
		inline u64 get_pos() const { return read_pos_; }
		inline void set_pos(u64 pos) { read_pos_ = pos < offset_ ? pos : offset_; }
		inline u64 read_remaining() const { return offset_ > read_pos_ ? offset_ - read_pos_ : 0; }

		template<typename T>
		bool write(const T& value) {
			static_assert(std::is_trivially_copyable_v<T>, "Type must be trivially copyable");
			const u64 data_size = sizeof(T);
			if (offset_ + data_size > size_) _realloc(std::max(size_ * 2, offset_ + data_size));
			std::memcpy(data_ + offset_, &value, data_size);
			offset_ += data_size;
			return true;
		}

		bool write(const void* data, u64 data_size) {
			if (!data || !data_size) return false;
			if (offset_ + data_size > size_) _realloc(std::max(size_ * 2, offset_ + data_size));
			std::memcpy(data_ + offset_, data, data_size);
			offset_ += data_size;
			return true;
		}

		template<typename T>
		bool read(T& value) {
			static_assert(std::is_trivially_copyable_v<T>, "Type must be trivially copyable");
			const u64 data_size = sizeof(T);
			if (read_pos_ + data_size > offset_) return false;
			std::memcpy(&value, data_ + read_pos_, data_size);
			read_pos_ += data_size;
			return true;
		}

		bool read(void* buffer, u64 buffer_size) {
			if (!buffer || !buffer_size || read_pos_ + buffer_size > offset_) return false;
			std::memcpy(buffer, data_ + read_pos_, buffer_size);
			read_pos_ += buffer_size;
			return true;
		}

		inline void clear() {
			if (empty() || !offset_) return;
			std::memset(data_, 0, offset_);
			offset_ = 0;
			read_pos_ = 0;
		}

		inline void free() {
			if (empty() || !data_) return;
			std::free(data_);
			data_ = nullptr;
			size_ = offset_ = read_pos_ = 0;
		}

	   private:
		void _malloc(const u64 length) {
			if (!length) return;
			data_ = (char*)std::malloc(length);
			if (!data_) throw std::runtime_error("out of memory");
			size_ = length;
		}

		void _realloc(const u64 length) {
			if (!length || length <= size_) return;
			if (!data_) return _malloc(length);
			char* new_data = (char*)std::realloc(data_, length);
			if (!new_data) throw std::runtime_error("out of memory");
			data_ = new_data;
			size_ = length;
		}

	   private:
		bytes data_{nullptr};
		u64 size_{0};
		u64 offset_{0};
		u64 read_pos_{0};
	};
}  // namespace enigma::network
