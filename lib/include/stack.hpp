/*
Copyright (c) 2014, Matteo Bertello
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.
* The names of its contributors may not be used to endorse or promote
products derived from this software without specific prior written
permission.
*/


#pragma once

#include <type_traits>
#include <vector>
#include <cstdint>
#include <cassert>
#include <limits>
#include <tuple>
#include <utility>
#include <iterator>

namespace LTL
{
namespace detail
{

static constexpr size_t STARTING_SIZE = 1000;

template<typename T, size_t BlockN>
class stack
{
	using indices_t = std::pair<uint64_t, uint64_t>;
	template<typename Ptr> class iterator_t;
	friend class iterator_t<T*>;
	friend class iterator_t<const T*>;

public:
	using iterator = iterator_t<T*>;
	using const_iterator = iterator_t<const T*>;
	using reverse_iterator = std::reverse_iterator<iterator>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;

	stack() : _blocks(STARTING_SIZE), _top(EMPTY_IDX) {}
	~stack() = default;

	stack(const stack&) = default;
	stack(stack&&) = default;

	stack& operator=(const stack&) = default;
	stack& operator=(stack&&) = default;

	T& top()
	{
		// Scott Meyers approves!
		return const_cast<T&>(static_cast<const stack*>(this)->top());
	}

	const T& top() const
	{
		uint64_t block_idx = 0;
		uint64_t elem_idx = 0;
		std::tie(block_idx, elem_idx) = indices();

		assert(elem_idx < BlockN);
		assert(block_idx < _blocks.size());

		return (*_blocks[block_idx])[elem_idx];
	}

	bool empty() const
	{
		return _top == EMPTY_IDX;
	}

	size_t size() const
	{
		return static_cast<size_t>(_top + 1);
	}

	void push(const T& value)
	{
		uint64_t block_idx = 0;
		uint64_t elem_idx = 0;
		std::tie(block_idx, elem_idx) = increase_size();

		_blocks[block_idx]->insert(elem_idx, value);
	}

	void push(T&& value)
	{
		uint64_t block_idx = 0;
		uint64_t elem_idx = 0;
		std::tie(block_idx, elem_idx) = increase_size();
		
		_blocks[block_idx]->insert(elem_idx, value);
	}

	template<typename... Args>
	void emplace(Args&&... args)
	{
		uint64_t block_idx = 0;
		uint64_t elem_idx = 0;
		std::tie(block_idx, elem_idx) = increase_size();

		_blocks[block_idx]->emplace(elem_idx, std::forward<Args>(args)...);
	}

	void pop()
	{
		assert(_top != EMPTY_IDX);

		uint64_t block_idx = 0;
		uint64_t elem_idx = 0;
		std::tie(block_idx, elem_idx) = indices();

		_block[block_idx]->erase(elem_idx);
		--_top;
	}

	void swap(stack& other)
	{
		std::swap(_blocks, other._blocks);
		std::swap(_top, other._top);
	}

	iterator begin()
	{
		return iterator(*this, 0);
	}

	iterator end()
	{
		return iterator(*this, _top + 1);
	}

	const_iterator cbegin()
	{
		return const_iterator(*this, 0);
	}

	const_iterator cend()
	{
		return const_iterator(*this, _top + 1);
	}

	reverse_iterator rbegin()
	{
		return std::make_reverse_iterator(end());
	}

	reverse_iterator rend()
	{
		return std::make_reverse_iterator(begin());
	}

	const_reverse_iterator crbegin()
	{
		return std::make_reverse_iterator(cend());
	}

	const_reverse_iterator crend()
	{
		return std::make_reverse_iterator(cbegin());
	}

	template<typename Ptr>
	class iterator_t
	{
		friend class stack;

		using pointed_type = typename std::remove_pointer<Ptr>::type;

	public:
		iterator_t() = delete;
		~iterator_t() = default;

		// TODO: missing constructor, assignment operators, and full RandomAccessIterator interface

		Ptr operator*()
		{
			uint64_t block_idx = 0;
			uint64_t elem_idx = 0;
			std::tie(block_idx, elem_idx) = _stack.indices(elem);

			return &(_stack._blocks[block_idx][elem_idx]);
		}

		pointed_type& operator[](size_t n)
		{
			assert(elem + n < _stack._top);

			uint64_t block_idx = 0;
			uint64_t elem_idx = 0;
			std::tie(block_idx, elem_idx) = _stack.indices(elem + n);

			return _stack[block_idx][elem_idx];
		}

	private:
		iterator_t(stack& s, uint64_t elem) : _stack(s), _elem(elem) {}

		stack& _stack;
		uint64_t elem;
	};

private:
	class Block
	{
	public:
		Block() = default;
		~Block() = default;

		Block(const Block&) = delete;
		Block(Block&&) = delete;

		Block& operator=(const Block&) = delete;
		Block& operator=(Block&&) = delete;

		void insert(size_t pos, const T& elem)
		{
			assert(pos < BlockN);
			new(_data + pos) T(elem);
		}

		void insert(size_t pos, T&& elem)
		{
			assert(pos < BlockN);
			new(_data + pos) T(elem);
		}

		template<typename ...Args>
		void emplace(size_t pos, Args&&... args)
		{
			assert(pos < BlockN);
			new(_data + pos) T(std::forward<Args>(args)...);
		}

		void erase(size_t pos)
		{
			assert(pos < BlockN);
			reinterpret_cast<const T*>(_data + pos)->~T();
		}

		const T& operator[](size_t pos) const
		{
			assert(pos < BlockN);
			return *reinterpret_cast<const T*>(_data + pos);
		}

		T& operator[](size_t pos)
		{
			assert(pos < BlockN);
			return *reinterpret_cast<T*>(_data + pos);
		}

	private:
		typename std::aligned_storage<sizeof(T), alignof(T)>::type _data[BlockN];
	};
	
	std::vector<Block*> _blocks;
	uint64_t _top;
	
	indices_t indices() const
	{
		return indices(_top);
	}

	indices_t indices(uint64_t elem) const
	{
		return { elem / BlockN, elem % BlockN };
	}

	indices_t increase_size()
	{
		++_top;
		assert(_top != std::numeric_limits<uint64_t>::max());

		uint64_t block_idx = 0;
		uint64_t elem_idx = 0;
		std::tie(block_idx, elem_idx) = indices();

		if (block_idx == _blocks.size())
		{
			_blocks.push_back(new Block());
			assert(_block_idx == _blocks.size() + 1);
		}

		assert(elem_idx < BlockN);
		assert(block_idx < _blocks.size());

		return { block_idx, elem_idx };
	}

	static constexpr uint64_t EMPTY_IDX = std::numeric_limits<uint64_t>::max();
};

template<typename T, uint64_t BlockN>
void swap(stack<T, BlockN>& lhs,
		  stack<T, BlockN>& rhs)
{
	lhs.swap(rhs);
}

}
}
