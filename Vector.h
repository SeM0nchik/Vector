#ifndef BFH_C_VECTOR_H
#define BFH_C_VECTOR_H

#define VECTOR_MEMORY_IMPLEMENTED

#include <algorithm>
#include <iterator>
#include <new>
#include <stdexcept>
#include <type_traits>

template <typename T>
class Vector {
public:
  using ValueType = T;
  using Pointer = T *;
  using ConstPointer = const T *;
  using Reference = T &;
  using ConstReference = const T &;
  using SizeType = size_t;
  using Iterator = T *;
  using ConstIterator = const T *;
  using ReverseIterator = std::reverse_iterator<Iterator>;
  using ConstReverseIterator = std::reverse_iterator<ConstIterator>;

  Iterator begin() noexcept { // NOLINT
    return buffer_; //
  }
  ConstIterator begin() const noexcept { // NOLINT
    return buffer_;
  }
  ConstIterator cbegin() const noexcept { // NOLINT
    return buffer_;
  }

  Iterator end() noexcept { // NOLINT
    return buffer_ + count_;
  }
  ConstIterator end() const noexcept { // NOLINT
    return buffer_ + count_;
  }
  ConstIterator cend() const noexcept { // NOLINT
    return buffer_ + count_;
  }

  ReverseIterator rbegin() noexcept { // NOLINT
    return ReverseIterator(end());
  }
  ConstReverseIterator rbegin() const noexcept { // NOLINT
    return ConstReverseIterator(end());
  }
  ConstReverseIterator crbegin() const noexcept { // NOLINT
    return ConstReverseIterator(cend());
  }

  ReverseIterator rend() noexcept { // NOLINT
    return ReverseIterator(begin());
  }
  ConstReverseIterator rend() const noexcept { // NOLINT
    return ConstReverseIterator(begin());
  }
  ConstReverseIterator crend() const noexcept { // NOLINT
    return ConstReverseIterator(cbegin());
  }

  Vector() : count_{0}, reserved_{0}, buffer_{nullptr} {}

  explicit Vector(size_t sz) : count_{sz}, reserved_{sz}, buffer_{Allocate(sz)} {
    if (count_) {
      try {
        std::uninitialized_default_construct_n(buffer_, count_);
      } catch (...) {
        Release(buffer_);
        throw;
      }
    }
  }

  Vector(size_t sz, const T &val) : count_{sz}, reserved_{sz}, buffer_{Allocate(sz)} {
    if (count_) {
      try {
        std::uninitialized_fill_n(buffer_, count_, val);
      } catch (...) {
        Release(buffer_);
        throw;
      }
    }
  }

  template <typename InputIt, typename = std::enable_if_t<!std::is_integral_v<InputIt>>>
  Vector(InputIt first, InputIt last)
      : count_{static_cast<SizeType>(std::distance(first, last))},
        reserved_{count_},
        buffer_{Allocate(count_)} {
    if (count_) {
      try {
        std::uninitialized_copy(first, last, buffer_);
      } catch (...) {
        Release(buffer_);
        throw;
      }
    }
  }

  Vector(std::initializer_list<T> ilist)
      : count_{ilist.size()}, reserved_{ilist.size()}, buffer_{Allocate(count_)} {
    if (count_) {
      try {
        std::uninitialized_copy(ilist.begin(), ilist.end(), buffer_);
      } catch (...) {
        Release(buffer_);
        throw;
      }
    }
  }

  Vector(const Vector &other)
      : count_{other.count_}, reserved_{other.reserved_}, buffer_{Allocate(count_)} {
    if (count_) {
      try {
        std::uninitialized_copy(other.buffer_, other.buffer_ + count_, buffer_);
      } catch (...) {
        Release(buffer_);
        throw;
      }
    }
  }

  Vector(Vector &&other) noexcept
      : count_{other.count_}, reserved_{other.reserved_}, buffer_{other.buffer_} {
    other.count_ = 0;
    other.reserved_ = 0;
    other.buffer_ = nullptr;
  }

  Vector &operator=(const Vector &rhs) {
    if (this != &rhs) {
      Vector temp(rhs);
      Swap(temp);
    }
    return *this;
  }

  Vector &operator=(Vector &&rhs) noexcept {
    if (this != &rhs) {
      Clear();
      Release(buffer_);
      count_ = rhs.count_;
      reserved_ = rhs.reserved_;
      buffer_ = rhs.buffer_;
      rhs.count_ = 0;
      rhs.reserved_ = 0;
      rhs.buffer_ = nullptr;
    }
    return *this;
  }

  ~Vector() {
    Clear();
    Release(buffer_);
  }

  size_t Size() const { return count_; }
  size_t Capacity() const { return reserved_; }
  bool Empty() const { return count_ == 0; }

  T &At(size_t idx) {
    if (idx >= count_) {
      throw std::out_of_range("Index out of bounds");
    }
    return buffer_[idx];
  }

  const T &At(size_t idx) const {
    if (idx >= count_) {
      throw std::out_of_range("Index out of bounds");
    }
    return buffer_[idx];
  }

  T &Front() { return buffer_[0]; }
  const T &Front() const { return buffer_[0]; }

  T &Back() { return buffer_[count_ - 1]; }
  const T &Back() const { return buffer_[count_ - 1]; }

  T *Data() { return buffer_; }
  const T *Data() const { return buffer_; }

  void Swap(Vector &other) {
    std::swap(count_, other.count_);
    std::swap(reserved_, other.reserved_);
    std::swap(buffer_, other.buffer_);
  }

  void Clear() {
    std::destroy(buffer_, buffer_ + count_);
    count_ = 0;
  }

  void Resize(size_t new_size) {
    if (new_size == count_) {
      return;
    }

    if (new_size < count_) {
      std::destroy(buffer_ + new_size, buffer_ + count_);
      count_ = new_size;
    } else {
      T *tmp = Allocate(new_size - count_);
      try {
        std::uninitialized_default_construct(tmp, tmp + new_size - count_);
      } catch (...) {
        Release(tmp);
        throw;
      }
      if (new_size > reserved_) {
        Reserve(new_size);
      }
      std::uninitialized_move(tmp, tmp + new_size - count_, buffer_ + count_);
      std::destroy(tmp, tmp + new_size - count_);
      Release(tmp);
      count_ = new_size;
    }
  }

  void Resize(size_t new_size, const T &value) {
    if (new_size == count_) {
      return;
    }

    if (new_size < count_) {
      std::destroy(buffer_ + new_size, buffer_ + count_);
      count_ = new_size;
    } else {
      T *tmp = Allocate(new_size - count_);
      try {
        std::uninitialized_fill(tmp, tmp + new_size - count_, value);
      } catch (...) {
        Release(tmp);
        throw;
      }
      if (new_size > reserved_) {
        Reserve(new_size);
      }
      std::uninitialized_move(tmp, tmp + new_size - count_, buffer_ + count_);
      std::destroy(tmp, tmp + new_size - count_);
      Release(tmp);
      count_ = new_size;
    }
  }

  void Reserve(size_t new_capacity) {
    if (new_capacity > reserved_) {
      T *tmp = Allocate(new_capacity);
      try {
        std::uninitialized_move(buffer_, buffer_ + count_, tmp);
      } catch (...) {
        Release(tmp);
        throw;
      }
      std::destroy(buffer_, buffer_ + count_);
      Release(buffer_);
      buffer_ = tmp;
      reserved_ = new_capacity;
    }
  }

  void ShrinkToFit() {
    if (reserved_ > count_) {
      if (count_ != 0) {
        T *tmp = Allocate(count_);
        std::uninitialized_move(buffer_, buffer_ + count_, tmp);
        std::destroy(buffer_, buffer_ + count_);
        Release(buffer_);
        buffer_ = tmp;
      } else {
        std::destroy(buffer_, buffer_ + count_);
        Release(buffer_);
        buffer_ = nullptr;
      }
      reserved_ = count_;
    }
  }

  void PushBack(const T &el) {
    T tmp = el;
    if (count_ == reserved_) {
      Reserve(reserved_ ? reserved_ * 2 : 1);
    }
    new (&buffer_[count_]) T(std::move(tmp));
    ++count_;
  }

  void PushBack(T &&el) {
    if (count_ == reserved_) {
      Reserve(reserved_ ? reserved_ * 2 : 1);
    }
    new (&buffer_[count_]) T(std::move(el));
    ++count_;
  }

  template <typename... Args>
  void EmplaceBack(Args &&... args) {
    T tmp(std::forward<Args>(args)...);
    if (count_ == reserved_) {
      Reserve(reserved_ ? reserved_ * 2 : 1);
    }
    new (&buffer_[count_]) T(std::move(tmp));
    ++count_;
  }

  void PopBack() {
    if (count_ > 0) {
      std::destroy_at(buffer_ + count_ - 1);
      --count_;
    }
  }

  T &operator[](size_t i) { return buffer_[i]; }
  const T &operator[](size_t i) const { return buffer_[i]; }

  bool operator==(const Vector &rhs) const {
    if (count_ != rhs.count_) {
      return false;
    }
    return std::equal(buffer_, buffer_ + count_, rhs.buffer_);
  }
  bool operator!=(const Vector &rhs) const { return !(*this == rhs); }
  bool operator<(const Vector &rhs) const {
    return std::lexicographical_compare(buffer_, buffer_ + count_, rhs.buffer_, rhs.buffer_ + rhs.count_);
  }
  bool operator>(const Vector &rhs) const { return rhs < *this; }
  bool operator<=(const Vector &rhs) const { return !(rhs < *this); }
  bool operator>=(const Vector &rhs) const { return !(*this < rhs); }

private:
  T *Allocate(size_t n) {
    return n ? static_cast<T *>(::operator new(n * sizeof(T))) : nullptr;
  }

  void Release(T *ptr) {
    if (ptr) {
      ::operator delete(ptr);
    }
  }

private:
  size_t count_;
  size_t reserved_;
  T *buffer_;
};

#endif // BFH_C_VECTOR_H
