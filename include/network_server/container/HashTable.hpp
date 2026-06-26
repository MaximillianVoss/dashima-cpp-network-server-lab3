#pragma once

#include "network_server/container/AdditiveHash.hpp"

#include <cstddef>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace network_server::container {

/**
 * @brief Template hash table with additive mixing and separate chaining.
 *
 * The table does not use STL containers internally. Buckets are stored in a
 * dynamic array, and collisions are resolved by singly linked chains.
 *
 * @tparam Key key type.
 * @tparam Value stored value type.
 * @tparam Hasher hash functor, defaulting to AdditiveHash.
 * @tparam KeyEqual equality predicate.
 */
template <
    typename Key,
    typename Value,
    typename Hasher = AdditiveHash<Key>,
    typename KeyEqual = std::equal_to<Key>>
class HashTable {
public:
    /**
     * @brief Key-value pair stored in the table.
     */
    struct Entry {
        Key key;
        Value value;
    };

private:
    struct Node {
        Entry entry;
        std::unique_ptr<Node> next;

        Node(Key newKey, Value newValue)
            : entry{std::move(newKey), std::move(newValue)}, next(nullptr) {}
    };

    struct Bucket {
        std::unique_ptr<Node> head;
    };

    template <bool IsConst>
    class BasicIterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = Entry;
        using difference_type = std::ptrdiff_t;
        using reference = std::conditional_t<IsConst, const Entry&, Entry&>;
        using pointer = std::conditional_t<IsConst, const Entry*, Entry*>;
        using TablePointer = std::conditional_t<IsConst, const HashTable*, HashTable*>;
        using NodePointer = std::conditional_t<IsConst, const Node*, Node*>;

        /**
         * @brief Creates an end iterator.
         */
        BasicIterator() noexcept : table_(nullptr), bucketIndex_(0), node_(nullptr) {}

        /**
         * @brief Returns the current entry.
         */
        reference operator*() const {
            if (node_ == nullptr) {
                throw std::out_of_range("Cannot dereference end iterator");
            }
            return node_->entry;
        }

        /**
         * @brief Returns a pointer to the current entry.
         */
        pointer operator->() const {
            return &(**this);
        }

        /**
         * @brief Advances the iterator to the next entry.
         */
        BasicIterator& operator++() {
            advance();
            return *this;
        }

        /**
         * @brief Advances the iterator and returns the previous value.
         */
        BasicIterator operator++(int) {
            BasicIterator previous = *this;
            advance();
            return previous;
        }

        /**
         * @brief Compares iterators for equality.
         */
        friend bool operator==(const BasicIterator& left, const BasicIterator& right) noexcept {
            return left.table_ == right.table_ && left.bucketIndex_ == right.bucketIndex_ &&
                   left.node_ == right.node_;
        }

        /**
         * @brief Compares iterators for inequality.
         */
        friend bool operator!=(const BasicIterator& left, const BasicIterator& right) noexcept {
            return !(left == right);
        }

    private:
        friend class HashTable;

        BasicIterator(TablePointer table, std::size_t bucketIndex, NodePointer node) noexcept
            : table_(table), bucketIndex_(bucketIndex), node_(node) {}

        void advance() {
            if (table_ == nullptr || node_ == nullptr) {
                return;
            }

            if (node_->next != nullptr) {
                node_ = node_->next.get();
                return;
            }

            ++bucketIndex_;
            while (bucketIndex_ < table_->capacity_) {
                if (table_->buckets_[bucketIndex_].head != nullptr) {
                    node_ = table_->buckets_[bucketIndex_].head.get();
                    return;
                }
                ++bucketIndex_;
            }

            node_ = nullptr;
        }

        TablePointer table_;
        std::size_t bucketIndex_;
        NodePointer node_;
    };

public:
    using iterator = BasicIterator<false>;
    using const_iterator = BasicIterator<true>;

    /**
     * @brief Creates a table with the requested bucket count.
     */
    explicit HashTable(std::size_t bucketCount = 17)
        : buckets_(std::make_unique<Bucket[]>(bucketCount)),
          capacity_(bucketCount),
          size_(0),
          hasher_(Hasher{}),
          equal_(KeyEqual{}) {
        if (bucketCount == 0) {
            throw std::invalid_argument("Bucket count must be positive");
        }
    }

    HashTable(const HashTable&) = delete;
    HashTable& operator=(const HashTable&) = delete;
    HashTable(HashTable&&) noexcept = default;
    HashTable& operator=(HashTable&&) noexcept = default;
    ~HashTable() = default;

    /**
     * @brief Inserts a new entry or replaces an existing entry with the same key.
     *
     * @return true if a new key was inserted, false if an existing key was replaced.
     */
    bool insert(Key key, Value value) {
        ensureCapacityForInsert();
        const auto index = bucketIndex(key);
        Node* current = buckets_[index].head.get();
        while (current != nullptr) {
            if (equal_(current->entry.key, key)) {
                current->entry.value = std::move(value);
                return false;
            }
            current = current->next.get();
        }

        auto node = std::make_unique<Node>(std::move(key), std::move(value));
        node->next = std::move(buckets_[index].head);
        buckets_[index].head = std::move(node);
        ++size_;
        return true;
    }

    /**
     * @brief Removes an entry by key.
     *
     * @return true if an entry was removed.
     */
    bool erase(const Key& key) {
        const auto index = bucketIndex(key);
        Node* current = buckets_[index].head.get();
        Node* previous = nullptr;

        while (current != nullptr) {
            if (equal_(current->entry.key, key)) {
                if (previous == nullptr) {
                    buckets_[index].head = std::move(current->next);
                } else {
                    previous->next = std::move(current->next);
                }
                --size_;
                return true;
            }
            previous = current;
            current = current->next.get();
        }
        return false;
    }

    /**
     * @brief Finds an entry value by key.
     */
    Value* find(const Key& key) noexcept {
        Node* node = findNode(key);
        return node == nullptr ? nullptr : &node->entry.value;
    }

    /**
     * @brief Finds an entry value by key.
     */
    const Value* find(const Key& key) const noexcept {
        const Node* node = findNode(key);
        return node == nullptr ? nullptr : &node->entry.value;
    }

    /**
     * @brief Checks whether the table contains a key.
     */
    bool contains(const Key& key) const noexcept {
        return find(key) != nullptr;
    }

    /**
     * @brief Removes all entries from the table.
     */
    void clear() noexcept {
        buckets_ = std::make_unique<Bucket[]>(capacity_);
        size_ = 0;
    }

    /**
     * @brief Returns the number of stored entries.
     */
    std::size_t size() const noexcept {
        return size_;
    }

    /**
     * @brief Returns true if the table is empty.
     */
    bool empty() const noexcept {
        return size_ == 0;
    }

    /**
     * @brief Returns the number of buckets.
     */
    std::size_t bucketCount() const noexcept {
        return capacity_;
    }

    /**
     * @brief Returns an iterator to the first entry.
     */
    iterator begin() noexcept {
        return makeBegin<iterator>(this);
    }

    /**
     * @brief Returns an iterator past the last entry.
     */
    iterator end() noexcept {
        return iterator(this, capacity_, nullptr);
    }

    /**
     * @brief Returns a const iterator to the first entry.
     */
    const_iterator begin() const noexcept {
        return makeBegin<const_iterator>(this);
    }

    /**
     * @brief Returns a const iterator past the last entry.
     */
    const_iterator end() const noexcept {
        return const_iterator(this, capacity_, nullptr);
    }

    /**
     * @brief Returns a const iterator to the first entry.
     */
    const_iterator cbegin() const noexcept {
        return begin();
    }

    /**
     * @brief Returns a const iterator past the last entry.
     */
    const_iterator cend() const noexcept {
        return end();
    }

private:
    template <typename IteratorType, typename TablePointer>
    static IteratorType makeBegin(TablePointer table) noexcept {
        for (std::size_t i = 0; i < table->capacity_; ++i) {
            if (table->buckets_[i].head != nullptr) {
                return IteratorType(table, i, table->buckets_[i].head.get());
            }
        }
        return IteratorType(table, table->capacity_, nullptr);
    }

    Node* findNode(const Key& key) noexcept {
        Node* current = buckets_[bucketIndex(key)].head.get();
        while (current != nullptr) {
            if (equal_(current->entry.key, key)) {
                return current;
            }
            current = current->next.get();
        }
        return nullptr;
    }

    const Node* findNode(const Key& key) const noexcept {
        const Node* current = buckets_[bucketIndex(key)].head.get();
        while (current != nullptr) {
            if (equal_(current->entry.key, key)) {
                return current;
            }
            current = current->next.get();
        }
        return nullptr;
    }

    std::size_t bucketIndex(const Key& key) const noexcept {
        return hasher_(key) % capacity_;
    }

    void ensureCapacityForInsert() {
        if ((size_ + 1) * 4 <= capacity_ * 3) {
            return;
        }
        rehash(capacity_ * 2 + 1);
    }

    void rehash(std::size_t newCapacity) {
        auto oldBuckets = std::move(buckets_);
        const auto oldCapacity = capacity_;

        buckets_ = std::make_unique<Bucket[]>(newCapacity);
        capacity_ = newCapacity;
        size_ = 0;

        for (std::size_t i = 0; i < oldCapacity; ++i) {
            auto current = std::move(oldBuckets[i].head);
            while (current != nullptr) {
                auto next = std::move(current->next);
                insertExistingNode(std::move(current));
                current = std::move(next);
            }
        }
    }

    void insertExistingNode(std::unique_ptr<Node> node) {
        const auto index = bucketIndex(node->entry.key);
        node->next = std::move(buckets_[index].head);
        buckets_[index].head = std::move(node);
        ++size_;
    }

    std::unique_ptr<Bucket[]> buckets_;
    std::size_t capacity_;
    std::size_t size_;
    Hasher hasher_;
    KeyEqual equal_;
};

}  // namespace network_server::container
