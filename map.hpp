/**
 * implement a container like std::map
 */
#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

// only for std::less<T>
#include <functional>
#include <cstddef>
#include "utility.hpp"
#include "exceptions.hpp"

namespace sjtu {

template<
	class Key,
	class T,
	class Compare = std::less<Key>
> class map {
public:
	typedef pair<const Key, T> value_type;
	Compare key_compare;
	int max(int a, int b) { return (a > b) ? a : b; }
	struct node
	{
		value_type *data;
		int height;
		node *parent;
		node *left;
		node *right;
		node *prev;
		node *next;
		node() :data(NULL), height(1), parent(NULL), left(NULL), right(NULL), prev(NULL), next(NULL) {}
		node(const value_type &d, int c = 1, node *p = NULL, node *l = NULL, node *r = NULL, node *pre = NULL, node *n = NULL)
			:height(c), parent(p), left(l), right(r), prev(pre), next(n)
		{
			data = new value_type(d);
		}
		node(value_type *d, int c = 1, node *p = NULL, node *l = NULL, node *r = NULL, node *pre = NULL, node *n = NULL)
			:height(c), parent(p), left(l), right(r), prev(pre), next(n)
		{
			if (d != NULL)
				data = new value_type(*d);
		}
		node(Key *key, int c = 1, node *p = NULL, node *l = NULL, node *r = NULL, node *pre = NULL, node *n = NULL)
			:height(c), parent(p), left(l), right(r), prev(pre), next(n)
		{
			Key *k = new Key(*key);
			T *s = new T;
			data = new value_type(*k, *s);
			delete k;
			delete s;
		}
		node(const Key &key, int c = 1, node *p = NULL, node *l = NULL, node *r = NULL, node *pre = NULL, node *n = NULL)
			:height(c), parent(p), left(l), right(r), prev(pre), next(n)
		{
			Key *k = new Key(key);
			T *s = new T;
			data = new value_type(*k, *s);
			delete k;
			delete s;
		}
		~node()
		{
			if (data != NULL)
				delete data;
		}
	};
	class const_iterator;
	class iterator {
	public:
		node * pos;
		node *head;
		iterator() :pos(NULL), head(NULL) {}
		iterator(const iterator &other) {
			pos = other.pos;
			head = other.head;
		}
		void init()
		{
			head = new node;
			pos = head;
			head->left = head;
			head->right = head;
		}
		void _copy(node *t,node *objT)
		{
			if (objT == NULL)
				return;
			if (objT->left != NULL)
			{
				t->left = new node(objT->left->data, objT->left->height, t);
				_copy(t->left, objT->left);
			}
			if (objT->right != NULL)
			{
				t->right = new node(objT->right->data, objT->right->height, t);
				_copy(t->right, objT->right);
			}
		}
		void _mark(node *h, node *t,node *&cur)
		{
			if (t == NULL)
				return;
			/*if (t->left == NULL && h->left == h)
				h->left = t;
			if (t->right == NULL)
				h->right = t;*/
			_mark(h, t->left, cur);
			cur->next = t;
			t->prev = cur;
			cur = t;
			_mark(h, t->right, cur);
		}
		void copy(const iterator &rhs)
		{
			if (rhs.head->parent == NULL)
			{
				pos = head;
				head->left = head;
				head->right = head;
			}
			else
			{
				head->parent = new node(rhs.head->parent->data, rhs.head->parent->height, head);
				_copy(head->parent, rhs.head->parent);
				node *t = head;
				_mark(head, head->parent,t);
				head->prev = t;
				t->next = head;
				pos = head;
			}
		}
		void clear()
		{
			if (head == NULL)
				return;
			if (head->parent == NULL)
				return;
			node *tmp1;
			pos = pos->next;
			while (pos != head)
			{
				tmp1 = pos;
				pos = pos->next;
				delete tmp1;
			}
			delete head;
			pos = NULL;
			head = NULL;
		}
		iterator operator++(int)
		{
			iterator res = *this;
			++*this;
			return res;
		}
		iterator & operator++()
		{
			if (head->prev == NULL)
			{
				index_out_of_bound e;
				throw exception(e);
			}
			pos = pos->next;
			return *this;
		}
		iterator operator--(int)
		{
			iterator res = *this;
			--*this;
			return res;
		}
		iterator & operator--()
		{
			if (head->next == NULL || pos == head->next)
			{
				index_out_of_bound e;
				throw exception(e);
			}
			if (pos->prev == head->prev)
			{
				pos = pos->prev;
				return *this;
			}
			pos = pos->prev;
			return *this;
		}
		value_type & operator*() const
		{
			if (pos->prev == head->prev)
			{
				index_out_of_bound e;
				throw exception(e);
			}
			if (pos == head)
				throw index_out_of_bound();
			return *(pos->data);
		}
		bool operator==(const iterator &rhs) const { return pos == rhs.pos; }
		bool operator==(const const_iterator &rhs) const { return pos == rhs.pos; }
		bool operator!=(const iterator &rhs) const { return !(*this == rhs); }
		bool operator!=(const const_iterator &rhs) const { return !(*this == rhs); }
		value_type* operator->() const noexcept { return &(operator*()); }
	};
	class const_iterator {
		public:
			node * pos;
			node *head;
			const_iterator() :pos(NULL), head(NULL) {}
			const_iterator(const const_iterator &other) {
				pos = other.pos;
				head = other.head;
			}
			const_iterator(const iterator &other) {
				pos = other.pos;
				head = other.head;
			}
			const_iterator operator++(int)
			{
				const_iterator res = *this;
				++*this;
				return res;
			}
			const_iterator & operator++()
			{
				if (head->prev == NULL)
				{
					index_out_of_bound e;
					throw exception(e);
				}
				pos = pos->next;
				return *this;
			}
			const_iterator operator--(int)
			{
				const_iterator res = *this;
				--*this;
				return res;
			}
			const_iterator & operator--()
			{
				if (head->next == NULL || pos == head->next)
				{
					index_out_of_bound e;
					throw exception(e);
				}
				if (pos->prev == head->prev)
				{
					pos = pos->prev;
					return *this;
				}
				pos = pos->prev;
				return *this;
			}
			value_type & operator*() const
			{
				if (pos->prev == head->prev)
				{
					index_out_of_bound e;
					throw exception(e);
				}
				if (pos == head)
					throw index_out_of_bound();
				return *(pos->data);
			}
			bool operator==(const iterator &rhs) const { return pos == rhs.pos; }
			bool operator==(const const_iterator &rhs) const { return pos == rhs.pos; }
			bool operator!=(const iterator &rhs) const { return !(*this == rhs); }
			bool operator!=(const const_iterator &rhs) const { return !(*this == rhs); }
			value_type* operator->() const noexcept { return &(operator*()); }
	};
	iterator sign;
	size_t length;
	map(const Compare& comp = Compare()) :key_compare(comp), length(0)
	{
		sign.init();
	}
	map(const map &other) :key_compare(other.key_compare)
	{
		if (this == &other)
			return;
		length = other.length;
		sign.init();
		sign.copy(other.sign);
	}
	map & operator=(const map &other)
	{
		if (this == &other)
			return *this;
		length = other.length;
		if (sign.head != NULL)
			sign.clear();
		sign.init();
		sign.copy(other.sign);
		return *this;
	}
	int height(node *t)
	{
		if (t == NULL)
			return 0;
		return t->height;
	}
	/**
	 * TODO Destructors
	 */
	~map()
	{
		sign.clear();
	}
	node* _find(const Key &key) const
	{
		if (sign.head->parent == NULL)
			return NULL;
		node *tmp = sign.head->parent;
		while (tmp != NULL)
		{
			if (key_compare((*(tmp->data)).first, key))
				tmp = tmp->right;
			else if (key_compare(key, (*(tmp->data)).first))
				tmp = tmp->left;
			else
			{
				return tmp;
			}
		}
		return NULL;
	}
	void LL(node *t)
	{
		node *t1 = t->left, *t2 = t->parent;
		if (t2 == sign.head)
		{
			t2->parent = t1;
			t1->parent = t2;
			t->left = t1->right;
			if (t1->right != NULL)
				t1->right->parent = t;
			t1->right = t;
			t->parent = t1;
			t->height = max(height(t->left), height(t->right)) + 1;
			t1->height = max(height(t1->left), height(t)) + 1;
			return;
		}
		if (t2->left == t)
		{
			t2->left = t1;
			t1->parent = t2;
			t->left = t1->right;
			if (t1->right != NULL)
				t1->right->parent = t;
			t1->right = t;
			t->parent = t1;
			t->height = max(height(t->left), height(t->right)) + 1;
			t1->height = max(height(t1->left), height(t)) + 1;
			t2->height = max(height(t2->right), height(t2->left)) + 1;
			return;
		}
		else
		{
			t2->right = t1;
			t1->parent = t2;
			t->left = t1->right;
			if (t1->right != NULL)
				t1->right->parent = t;
			t1->right = t;
			t->parent = t1;
			t->height = max(height(t->left), height(t->right)) + 1;
			t1->height = max(height(t1->left), height(t)) + 1;
			t2->height = max(height(t2->right), height(t2->left)) + 1;
			return;
		}

	}
	void RR(node *t)
	{
		node *t1 = t->right, *t2 = t->parent;
		if (t2 == sign.head)
		{
			t2->parent = t1;
			t1->parent = t2;
			t->right = t1->left;
			if (t1->left != NULL)
				t1->left->parent = t;
			t1->left = t;
			t->parent = t1;
			t->height = max(height(t->left), height(t->right)) + 1;
			t1->height = max(height(t1->right), height(t)) + 1;
			return;
		}
		if (t2->left == t)
		{
			t2->left = t1;
			t1->parent = t2;
			t->right = t1->left;
			if (t1->left != NULL)
				t1->left->parent = t;
			t1->left = t;
			t->parent = t1;
			t->height = max(height(t->left), height(t->right)) + 1;
			t1->height = max(height(t1->right), height(t)) + 1;
			t2->height = max(height(t2->right), height(t2->left)) + 1;
			return;
		}
		else
		{
			t2->right = t1;
			t1->parent = t2;
			t->right = t1->left;
			if (t1->left != NULL)
				t1->left->parent = t;
			t1->left = t;
			t->parent = t1;
			t->height = max(height(t->left), height(t->right)) + 1;
			t1->height = max(height(t1->right), height(t)) + 1;
			t2->height = max(height(t2->right), height(t2->left)) + 1;
			return;
		}
	}
	void LR(node *t)
	{
		node *t1 = t->left;
		RR(t1);
		LL(t);
	}
	void RL(node *t)
	{
		node *t1 = t->right;
		LL(t1);
		RR(t);
	}

	/**
	 * TODO
	 * access specified element with bounds checking
	 * Returns a reference to the mapped value of the element with key equivalent to key.
	 * If no such element exists, an exception of type `index_out_of_bound'
	 */
	T & at(const Key &key)
	{
		node *t = _find(key);
		if (t == NULL)
			throw index_out_of_bound();
		return (*(t->data)).second;
	}
	const T & at(const Key &key) const
	{
		node *t = _find(key);
		if (t == NULL)
			throw index_out_of_bound();
		return (*(t->data)).second;
	}
	/**
	 * TODO
	 * access specified element
	 * Returns a reference to the value that is mapped to a key equivalent to key,
	 *   performing an insertion if such key does not already exist.
	 */
	void _check(const Key &key, node *t, node *&nNode, int &flag)
	{
		if (t == NULL)
		{
			flag = 1;
			return;
		}
		else if (key_compare(key, (*(t->data)).first))
		{
			if (flag != 2 || flag != -1)
				_check(key, t->left, nNode, flag);
			if (flag == -1)
				return;
			else if (flag == 1)
			{
				++flag;
				t->left = new node(key, 1, t);
				nNode = t->left;
				t->left->next = t;
				t->left->prev = t->prev;
				t->prev->next = t->left;
				t->prev = t->left;
				if (sign.head->left == t)
					sign.head->left = nNode;
			}
			if (height(t->left) - height(t->right) == 2)
			{
				if (key_compare(key, (*(t->left)).data->first))
					LL(t);
				else
					LR(t);
				t = t->parent;
			}
		}
		else if (key_compare((*(t->data)).first, key))
		{
			if (flag != 2 || flag != -1)
				_check(key, t->right, nNode, flag);
			if (flag == -1)
				return;
			else if (flag == 1)
			{
				++flag;
				t->right = new node(key, 1, t);
				nNode = t->right;
				nNode->prev = t;
				nNode->next = t->next;
				t->next = nNode;
				nNode->next->prev = nNode;
				if (sign.head->right == t)
					sign.head->right = nNode;
			}
			if (height(t->right) - height(t->left) == 2)
			{
				if (key_compare((*(t->right->data)).first, key))
					RR(t);
				else
					RL(t);
				t = t->parent;
			}
		}
		else
		{
			flag = -1;
			nNode = t;
			return;
		}
		t->height = max(height(t->left), height(t->right)) + 1;
	}
	/**
	* return a iterator to the beginning
	*/
	T & operator[](const Key &key)
	{
		node *t = _find(key);
		if (t == NULL)
		{
			++length;
			if (sign.head->parent == NULL)
			{
				sign.head->parent = new node(key, 1, sign.head);
				sign.head->left = sign.head->parent;
				sign.head->right = sign.head->parent;
				sign.head->next = sign.head->parent;
				sign.head->prev = sign.head->parent;
				sign.head->parent->next = sign.head;
				sign.head->parent->prev = sign.head;
 				return (*(sign.head->parent->data)).second;
			}
			int flag = 0;
			node *nNode = NULL;
			_check(key, sign.head->parent, nNode, flag);
			return (*(nNode->data)).second;
		}
		return (*(t->data)).second;
	}
	/**
	 * behave like at() throw index_out_of_bound if such key does not exist.
	 */
	const T & operator[](const Key &key) const
	{
		node *t = _find(key);
		if (t == NULL)
			throw index_out_of_bound();
		return *(t->data->second);
	}
	iterator begin()
	{
		if (length == 0)
			return sign;
		iterator res = sign;
		return ++res;
	}
	const_iterator cbegin() const
	{
		if (length == 0)
			return sign;
		const_iterator res = sign;
		return ++res;
	}
	/**
	 * return a iterator to the end
	 * in fact, it returns past-the-end.
	 */
	iterator end()
	{
		return sign;
	}
	const_iterator cend() const
	{
		const_iterator res = sign;
		return res;
	}
	/**
	 * checks whether the container is empty
	 * return true if empty, otherwise false.
	 */
	bool empty() const { return length == 0; }
	/**
	 * returns the number of elements.
	 */
	size_t size() const { return length; }
	/**
	 * clears the contents
	 */
	void clear() { sign.clear(); length = 0; }
	/**
	 * insert an element.
	 * return a pair, the first of the pair is
	 *   the iterator to the new element (or the element that prevented the insertion),
	 *   the second one is true if insert successfully, or false.
	 */
	void _ins(const value_type &value, node *t, node *&nNode, int &flag)
	{
		if (t == NULL)
		{
			flag = 1;
			return;
		}
		else if (key_compare(value.first, (*(t->data)).first))
		{
			if (flag != 2 || flag != -1)
				_ins(value, t->left, nNode, flag);
			if (flag == -1)
				return;
			else if (flag == 1)
			{
				++flag;
				t->left = new node(value, 1, t);
				nNode = t->left;
				t->left->next = t;
				t->left->prev = t->prev;
				t->prev->next = t->left;
				t->prev = t->left;
				if (sign.head->left == t)
					sign.head->left = nNode;
			}
			if (height(t->left) - height(t->right) == 2)
			{
				if (key_compare(value.first, (*(t->left->data)).first))
					LL(t);
				else
					LR(t);
				t = t->parent;
			}
		}
		else if (key_compare((*(t->data)).first, value.first))
		{
			if (flag != 2 || flag != -1)
				_ins(value, t->right, nNode, flag);
			if (flag == -1)
				return;
			else if (flag == 1)
			{
				++flag;
				t->right = new node(value, 1, t);
				nNode = t->right;
				nNode->prev = t;
				nNode->next = t->next;
				t->next = nNode;
				nNode->next->prev = nNode;
				if (sign.head->right == t)
					sign.head->right = nNode;
			}
			if (height(t->right) - height(t->left) == 2)
			{
				if (key_compare((*(t->right->data)).first, value.first))
					RR(t);
				else
					RL(t);
				t = t->parent;
			}
		}
		else
		{
			flag = -1;
			nNode = t;
			return;
		}
		t->height = max(height(t->left), height(t->right)) + 1;
	}
	pair<iterator, bool> insert(const value_type &value)
	{
		if (length == 0)
		{
			if (sign.head == NULL)
				sign.init();
			sign.head->parent = new node(value, 1, sign.head);
			sign.head->left = sign.head->parent;
			sign.head->right = sign.head->parent;
			sign.head->next = sign.head->parent;
			sign.head->prev = sign.head->parent;
			sign.head->parent->prev = sign.head;
			sign.head->parent->next = sign.head;
			iterator res = sign;
			res.pos = sign.head->parent;
			pair<iterator, bool> result(res, true);
			++length;
			return result;
		}
		node *nNode = NULL;
		int flag = 0;
		_ins(value, sign.pos->parent,nNode,flag);
		if (flag == -1)
		{
			iterator res = sign;
			res.pos = nNode;
			pair<iterator, bool> result(res, false);
			return result;
		}
		iterator res = sign;
		res.pos = nNode;
		pair<iterator, bool> result(res, true);
		++length;
		return result;
	}
	/**
	 * erase the element at pos.
	 *
	 * throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
	 */
	bool _adjust(node *&t,bool subTree)//0-左子树，1-右子树
	{
		t->height = max(height(t->left), height(t->right)) + 1;
		if (subTree)
		{
			if (height(t->left) - height(t->right) == 1)return true;
			if (height(t->left) == height(t->right))
			{
				return false;
			}
			if (t->left != NULL)
			{
				if (height(t->left->right) > height(t->left->left))
				{
					LR(t);
					t = t->parent;
					t->height = max(height(t->left), height(t->right)) + 1;
					return false;
				}
			}
			LL(t);
			t = t->parent;
			t->height = max(height(t->left), height(t->right)) + 1;
			if (height(t->left) == height(t->right))
				return false;
			else
				return true;
		}
		else
		{
			if (height(t->right) - height(t->left) == 1)return true;
			if (height(t->right) == height(t->left))
			{
				return false;
			}
			if (t->right != NULL)
			{
				if (height(t->right->left) > height(t->right->right))
				{
					RL(t);
					t = t->parent;
					t->height = max(height(t->left), height(t->right)) + 1;
					return false;
				}
			}
			RR(t);
			t = t->parent;
			t->height = max(height(t->left), height(t->right)) + 1;
			if (height(t->right) == height(t-> left))
				return false;
			else
				return true;
		}
	}
	void adjust(node *t,bool subTree)//0-zuo,1-you
	{
		if (t == sign.head)
			return;
		if (_adjust(t, subTree))
			return;
		if (t->parent == sign.head)
			return;
		if (t->parent->left == t)
			adjust(t->parent, 0);
		else
			adjust(t->parent, 1);
	}
	void remove(node *t,bool flag)//1-正常
	{
		if (t->left == NULL && t->right == NULL)
		{
			if (t->parent == sign.head)
			{
				delete t;
				sign.head->parent = NULL;
				sign.head->left = sign.head;
				sign.head->right = sign.head;
				sign.head->prev = NULL;
				sign.head->next = NULL;
			}
			else if (t->parent->left == t)
			{
				t->parent->left = NULL;
				if (flag)
				{
					t->prev->next = t->next;
					t->next->prev = t->prev;
					if (sign.head->left == t)
						sign.head->left = t->next;
				}
				node *m = t->parent;
				delete t;
				adjust(m, 0);
			}
			else if (t->parent->right == t)
			{
				t->parent->right = NULL;
				if (flag)
				{
					t->prev->next = t->next;
					t->next->prev = t->prev;
					if (sign.head->right == t)
						sign.head->right = t->prev;
				}
				node *m = t->parent;
				delete t;
				adjust(m, 1);
			}
		}
		else if (t->left == NULL && t->right != NULL)
		{
			if (t->parent == sign.head)
			{
				sign.head->parent = t->right;
				sign.head->left = t->right;
				sign.head->right = t->right;
				sign.head->prev = t->right;
				sign.head->next = t->right;
				t->right->prev = sign.head;
				t->right->parent = sign.head;
				delete t;
			}
			else if (t->parent->left == t)
			{
				t->parent->left = t->right;
				t->right->parent = t->parent;
				t->parent->height = max(height(t->parent->left), height(t->parent->right)) + 1;
				{
					t->prev->next = t->next;
					t->next->prev = t->prev;
					if (sign.head->left == t)
						sign.head->left = t->next;
				}
				node *m = t->parent;
				delete t;
				adjust(m, 0);
			}
			else if (t->parent->right == t)
			{
				t->parent->right = t->right;
				t->right->parent = t->parent;
				t->parent->height = max(height(t->parent->left), height(t->parent->right)) + 1;
				if (flag)
				{
					t->prev->next = t->next;
					t->next->prev = t->prev;
				}
				node *m = t->parent;
				delete t;
				adjust(m, 1);
			}
		}
		else if (t->left != NULL && t->right == NULL)
		{
			if (t->parent == sign.head)
			{
				sign.head->parent = t->left;
				sign.head->left = t->left;
				sign.head->right = t->left;
				sign.head->prev = t->left;
				sign.head->next = t->left;
				t->left->next = sign.head;
				t->left->parent = sign.head;
				delete t;
			}
			else if (t->parent->left == t)
			{
				t->parent->left = t->left;
				t->left->parent = t->parent;
				t->parent->height = max(height(t->parent->left), height(t->parent->right)) + 1;
				t->prev->next = t->next;
				t->next->prev = t->prev;
				node *m = t->parent;
				delete t;
				adjust(m, 0);
			}
			else if (t->parent->right == t)
			{
				t->parent->right = t->left;
				t->left->parent = t->parent;
				t->parent->height = max(height(t->parent->left), height(t->parent->right)) + 1;
				t->prev->next = t->next;
				t->next->prev = t->prev;
				if (sign.head->right == t)
					sign.head->right = sign.head->prev;
				node *m = t->parent;
				delete t;
				adjust(m, 1);
			}
		}
		else
		{
			node *tmp = t->right;
			while (tmp->left != NULL)
				tmp = tmp->left;
			if (t->parent == sign.head)
			{
				if (tmp == t->right)
				{
					node *m = tmp;
					sign.head->parent = tmp;
					tmp->parent = sign.head;
					tmp->left = t->left;
					t->left->parent = tmp;
					t->prev->next = tmp;
					tmp->prev = t->prev;
					delete t;
					adjust(m, 1);
				}
				else
				{
					node *m = tmp->parent;
					tmp->parent->left = tmp->right;
					if (tmp->right != NULL)
						tmp->right->parent = tmp->parent;
					sign.head->parent = tmp;
					tmp->parent = sign.head;
					t->left->parent = tmp;
					t->right->parent = tmp;
					tmp->left = t->left;
					tmp->right = t->right;
					t->prev->next = tmp;
					tmp->prev = t->prev;
					tmp->height = t->height;
					delete t;
					if (m != NULL)
						adjust(m, 0);
				}
			}
			else if (t->parent->left == t)
			{
				if (tmp == t->right)
				{
					node *m = tmp;
					t->parent->left = tmp;
					tmp->parent = t->parent;
					tmp->left = t->left;
					t->left->parent = tmp;
					t->prev->next = tmp;
					tmp->prev = t->prev;
					delete t;
					adjust(m, 1);
				}
				else
				{
					node *m = tmp->parent;
					tmp->parent->left = tmp->right;
					if (tmp->right != NULL)
						tmp->right->parent = tmp->parent;
					t->parent->left = tmp;
					tmp->parent = t->parent;
					t->left->parent = tmp;
					t->right->parent = tmp;
					tmp->left = t->left;
					tmp->right = t->right;
					t->prev->next = tmp;
					tmp->prev = t->prev;
					tmp->height = t->height;
					delete t;
					if (m != NULL)
						adjust(m, 0);
				}
			}
			else if (t->parent->right == t)
			{
				if (tmp == t->right)
				{
					node *m = tmp;
					t->parent->right = tmp;
					tmp->parent = t->parent;
					tmp->left = t->left;
					t->left->parent = tmp;
					t->prev->next = tmp;
					tmp->prev = t->prev;
					delete t;
					adjust(m, 1);
				}
				else
				{
					node *m = tmp->parent;
					tmp->parent->left = tmp->right;
					if (tmp->right != NULL)
						tmp->right->parent = tmp->parent;
					t->parent->right = tmp;
					tmp->parent = t->parent;
					t->left->parent = tmp;
					t->right->parent = tmp;
					tmp->left = t->left;
					tmp->right = t->right;
					t->prev->next = tmp;
					tmp->prev = t->prev;
					tmp->height = t->height;
					delete t;
					if (m != NULL)
						adjust(m, 0);
				}
			}
		}
	}
	void erase(iterator p)
	{
		if (p.head != sign.head)
			throw invalid_iterator();
		if (length == 0 || p.pos == p.head)
			throw index_out_of_bound();
		--length;
		remove(p.pos,1);
	}
	/**
	 * Returns the number of elements with key
	 *   that compares equivalent to the specified argument,
	 *   which is either 1 or 0
	 *     since this container does not allow duplicates.
	 * The default method of check the equivalence is !(a < b || b > a)
	 */
	size_t count(const Key &key) const
	{
		if (sign.head==NULL||sign.head->parent == NULL)
			return 0;
		node *tmp = sign.head->parent;
		while (tmp != NULL)
		{
			if (key_compare((*(tmp->data)).first, key))
				tmp = tmp->right;
			else if (key_compare(key, (*(tmp->data)).first))
				tmp = tmp->left;
			else
			{
				return 1;
			}
		}
		return 0;
	}
	/**
	 * Finds an element with key equivalent to key.
	 * key value of the element to search for.
	 * Iterator to an element with key equivalent to key.
	 *   If no such element is found, past-the-end (see end()) iterator is returned.
	 */
	iterator find(const Key &key)
	{
		if (sign.head->parent == NULL)
			return end();
		node *tmp = sign.head->parent;
		while (tmp != NULL)
		{
			if (key_compare((*(tmp->data)).first, key))
				tmp = tmp->right;
			else if (key_compare(key, ((*(tmp->data)).first)))
				tmp = tmp->left;
			else
			{
				iterator res = sign;
				res.pos = tmp;
				return res;
			}
		}
		return end();
	}
	const_iterator find(const Key &key) const
	{
		if (sign.head->parent == NULL)
			return cend();
		node *tmp = sign.head->parent;
		while (tmp != NULL)
		{
			if (key_compare(tmp->data->first, key))
				tmp = tmp->right;
			else if (key_compare(key, tmp->data->first))
				tmp = tmp->left;
			else
			{
				iterator res = sign;
				res.pos = tmp;
				return res;
			}
		}
		return cend();
	}
	void sss(node *t)
	{
		if (t == NULL)
			return;
		if (t->height != max(height(t->left), height(t->right)) + 1)
			throw runtime_error();
		sss(t->left);
		sss(t->right);
	}
	bool www()
	{
		if (sign.head == NULL || sign.head->parent == NULL)
			return true;
		try
		{
			sss(sign.head->parent);
		}
		catch (...)
		{
			std::cout << 2;
		}
		return true;
	}
};

}

#endif
