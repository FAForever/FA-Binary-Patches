#pragma once
#include <cstdint>
#include <new>

template <typename T>
class LinkedList
{
private:
    struct Node
    {
        Node *next;
        Node *prev;
        T data;

        Node()
            : next(nullptr),
              prev(nullptr),
              data{}
        {
        }

        ~Node() = default;
    };

    Node *FindNode(T *data)
    {
        Node *node = head;
        while (node)
        {
            if (&node->data == data)
                return node;
            node = node->next;
        }
        return nullptr;
    }

public:
    LinkedList()
        : head(nullptr),
          tail(nullptr),
          size(0)
    {
    }

    ~LinkedList()
    {
        Clear();
    }

    T *AddFront()
    {
        Node *newNode = new (std::nothrow) Node();
        if (!newNode)
            return nullptr;

        if (!head)
        {
            head = newNode;
            tail = newNode;
        }
        else
        {
            newNode->next = head;
            head->prev = newNode;
            head = newNode;
        }
        ++size;

        return &newNode->data;
    }

    T *AddBack()
    {
        Node *newNode = new (std::nothrow) Node();
        if (!newNode)
            return nullptr;

        if (!tail)
        {
            head = newNode;
            tail = newNode;
        }
        else
        {
            tail->next = newNode;
            newNode->prev = tail;
            tail = newNode;
        }
        ++size;

        return &newNode->data;
    }

    void Remove(T *value)
    {
        Node *node = FindNode(value);
        if (!node)
            return;

        Node *next = node->next;
        Node *prev = node->prev;
        if (prev)
            prev->next = next;
        else
            head = next;

        if (next)
            next->prev = prev;
        else
            tail = prev;

        delete node;
        --size;
    }

    bool IsEmpty() const { return head == nullptr; }
    size_t Size() const { return size; }

    void Clear()
    {
        while (head)
        {
            Node *temp = head;
            head = head->next;
            delete temp;
        }
        tail = nullptr;
        size = 0;
    }

    class Iterator
    {
    public:
        Iterator(Node *node) : node(node) {}
        T &operator*() { return node->data; }
        Iterator &operator++()
        {
            node = node->next;
            return *this;
        }
        bool operator==(const Iterator &other) const { return node == other.node; }
        bool operator!=(const Iterator &other) const { return node != other.node; }

    private:
        Node *node;
    };

    class ConstIterator
    {
    public:
        ConstIterator(Node *node) : node(node) {}
        const T &operator*() { return node->data; }
        ConstIterator &operator++()
        {
            node = node->next;
            return *this;
        }
        bool operator==(const ConstIterator &other) const { return node == other.node; }
        bool operator!=(const ConstIterator &other) const { return node != other.node; }

    private:
        Node *node;
    };

    Iterator begin() { return Iterator(head); }
    Iterator end() { return Iterator(nullptr); }

    ConstIterator begin() const { return ConstIterator(head); }
    ConstIterator end() const { return ConstIterator(nullptr); }

private:
    Node *head;
    Node *tail;
    size_t size;
};