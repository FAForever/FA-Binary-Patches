#pragma once

#include "global.h"

typedef int unk_t;
typedef int BOOL;
typedef int SOCKET;

struct FILE
{
	char *_ptr;
	int _cnt;
	char *_base;
	int _flag;
	int _file;
	int _charbuf;
	int _bufsiz;
	char *_tmpfname;
};

struct typeInfo
{ // 0x8+ bytes
	void *vtable;
	int zero;
	char name[];
};

template<class S, class T>
struct pair
{
	S first;
	T second;
};

struct _Container_base
{
	uint32_t gap;
};

template <typename T>
struct vector : _Container_base
{ // 0x10 bytes
	T *begin, *end, *capacity_end;

	T operator[](int i) { return begin[i]; }
	size_t size() { return begin ? end - begin : 0; }
};
VALIDATE_SIZE(vector<unk_t>, 0x10);

template<class T>
struct set : _Container_base
{
	struct _Node
	{
		_Node *_Left;
		_Node *_Parent;
		_Node *_Right;
		T _Myval;
		char _Color;
		char _Isnil;
	};

	_Node *_Myhead;
	size_t _Mysize;
};

template<class K, class V>
struct map : _Container_base {
    struct _Node {
        _Node *_Left;    // 0x00
        _Node *_Parent;  // 0x04
        _Node *_Right;   // 0x08
        struct {
            K first;     // 0x0C
            V second;    // 0x10
        } _Myval;
        char _Color;     // 0x14
        char _Isnil;     // 0x19
    };

    _Node *_Myhead;      // 0x04
    size_t _Mysize;      // 0x08

	size_t size() const {
		return _Mysize;
	}

    struct iterator {
        _Node* _Ptr;
        iterator(_Node* p) : _Ptr(p) {}
        auto* operator->() const { return &(_Ptr->_Myval); }
        bool operator==(const iterator& other) const { return _Ptr == other._Ptr; }
        bool operator!=(const iterator& other) const { return _Ptr != other._Ptr; }
    };
    iterator end() const { 
        return iterator(_Myhead); 
    }

	iterator find(const K& key) const {
        if (!_Myhead || _Mysize == 0) return end();

        _Node* node = _Myhead->_Parent;
        _Node* result = _Myhead;

        while (node && !node->_Isnil) {
            if (key < node->_Myval.first) {
                node = node->_Left;
            } 
            else if (key > node->_Myval.first) {
                node = node->_Right;
            } 
            else {
                result = node;
                break;
            }
        }

        return iterator(result);
    }
	bool contains(const K& key) const {
		return find(key) != end();
	}
};


template <typename T>
struct list : _Container_base
{ // 0xC bytes
	struct _Node
	{
		_Node *_Next;
		_Node *_Prev;
		T _Myval;
	};

	_Node *_Myhead;
	size_t _Mysize;
};
VALIDATE_SIZE(list<unk_t>, 0xC);

#define _DEQUESIZ	(sizeof (_Ty) <= 1 ? 16 \
	: sizeof (_Ty) <= 2 ? 8 \
	: sizeof (_Ty) <= 4 ? 4 \
	: sizeof (_Ty) <= 8 ? 2 : 1)	/* elements per block (a power of 2) */

template<class _Ty>
struct deque : _Container_base
{
	struct block
	{
  		_Ty vals[_DEQUESIZ];
	} * _Map;
	size_t _Mapsize;
	size_t _Myoff;
	size_t _Mysize;
};

template<class T, class _Container = deque<T>>
struct queue
{
	_Container c;
};
