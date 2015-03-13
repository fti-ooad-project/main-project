#pragma once

#include <functional>
#include <utility>
#include <list>

#include <4u/la/vec.hpp>
#include <4u/util/op.hpp>

#include <iostream>

struct TreeKey
{
public:
	vec2 position;
	double size;
	TreeKey() {}
	TreeKey(const vec2 &p, double s) : position(p), size(s) {}
	bool isSuitable(const TreeKey &k) const
	{
		vec2 rel = k.position - position;
		return (size <= k.size) && (2*size > k.size) && (_max(_abs(rel.x()),_abs(rel.y())) <= size);
	}
	bool isSuitableDown(const TreeKey &k) const
	{
		vec2 rel = k.position - position;
		return (2*size > k.size) && (_max(_abs(rel.x()),_abs(rel.y())) <= size);
	}
};

template <typename T>
class Tree
{
public:
	typedef TreeKey Key;
	
private:
	class Branch
	{
	public:
		Key key;
		Branch *parent = nullptr;
		Branch *offspring = nullptr;
		Branch *neighbours[8] = {nullptr};
		std::list<std::pair<Key,T>> elements;
		bool empty = true;
	};
	
	Branch root;
	int depth;
	
	static void __create_tree(Branch *root, int level)
	{
		if(level > 0)
		{
			root->offspring = new Branch[4];
			const Key &key = root->key;
			for(int i = 0; i < 4; ++i)
			{
				root->offspring[i].parent = root;
				root->offspring[i].key = 
				    Key(key.position + key.size*vec2(i%2 - 0.5, i/2 - 0.5), 0.5*key.size);
				__create_tree(&root->offspring[i],level - 1);
			}
		}
	}
	
	static void __find_neighbours(Branch *root, Branch *b)
	{
		for(int i = 0; i < 8; ++i)
		{
			Key k = b->key;
			int j = i + (i > 3);
			k.position += 2.0*vec2(j%3 - 1, j/3 - 1)*k.size;
			Branch *nb = __find_branch(root,k);
			if(nb->key.isSuitable(k))
			{
				b->neighbours[i] = nb;
			}
		}
		if(b->offspring)
		{
			for(int i = 0; i < 4; ++i)
			{
				__find_neighbours(root,&b->offspring[i]);
			}
		}
	}
	
	static void __destroy_tree(Branch *root)
	{
		if(root->offspring)
		{
			for(int i = 0; i < 4; ++i)
			{
				__destroy_tree(&root->offspring[i]);
			}
			delete[] root->offspring;
			root->offspring = nullptr;
		}
	}
	
	static int __get_index(vec2 rel)
	{
		return int(rel.x() > 0.0) + 2*int(rel.y() > 0.0);
	}
	
	static Branch *__find_branch(Branch *root, const Key &key)
	{
		if(root->key.size >= key.size && root->offspring)
		{
			return __find_branch(
			    &root->offspring[__get_index(key.position - root->key.position)],
			    key
			    );
		}
		return root;
	}
	
	static void __empty_branch(Branch *b)
	{
		bool empty = true;
		if(b->offspring)
		{
			for(int i = 0; i < 4; ++i)
			{
				empty = empty && b->offspring[i].empty;
			}
		}
		if(empty)
		{
			b->empty = true;
			if(b->parent)
			{
				__empty_branch(b->parent);
			}
		}
	}
	
	static void __empty_branch_if_needed(Branch *b)
	{
		if(!b->elements.size())
		{
			__empty_branch(b);
		}
	}
	
	static void __fill_branch(Branch *b)
	{
		b->empty = false;
		if(b->parent)
		{
			__fill_branch(b->parent);
		}
	}
	
	static void __call_all(Branch *root, std::function<void(Branch*)> func)
	{
		if(!root->empty)
		{
			func(root);
			if(root->offspring)
			{
				for(int i = 0; i < 4; ++i)
				{
					__call_all(&root->offspring[i],func);
				}
			}
		}
	}
	
	static void __call_neighbours(Branch *root, const Key &key, std::function<void(Branch*)> func)
	{
		Branch *b = __find_branch(root,key);
		if(!b->empty)
		{
			func(b);
		}
		for(int i = 0; i < 8; ++i)
		{
			if(b->neighbours[i] && !b->neighbours[i]->empty)
			{
				func(b->neighbours[i]);
			}
		}
	}
	
	static void __call_nearest(Branch *root, const Key &key, std::function<void(Branch*)> func)
	{
		if(!root->key.isSuitable(key))
		{
			if(!root->empty)
			{
				func(root);
			}
			for(int i = 0; i < 8; ++i)
			{
				if(root->neighbours[i] && !root->neighbours[i]->empty)
				{
					func(root->neighbours[i]);
				}
			}
			if(root->offspring)
			{
				vec2 rel = key.position - root->key.position;
				__call_neighbours(&root->offspring[__get_index(rel)],key,func);
			}
		}
		else
		{
			__call_all(root,func);
			for(int i = 0; i < 8; ++i)
			{
				if(root->neighbours[i])
				{
					__call_all(root->neighbours[i],func);
				}
			}
		}
	}
	
public:
	Tree(const Key &key, int d)
	  : depth(d)
	{
		root.key = key;
		__create_tree(&root,depth);
		__find_neighbours(&root,&root);
	}

	~Tree()
	{
		__destroy_tree(&root);
	}

	/* Iterates all elements */
	void for_all(std::function<void(std::list<std::pair<Key,T>> &)> func)
	{
		__call_all(&root,[func](Branch *b)
		{
			func(b->elements);
		});
	}
	
	/* Iterates all elements which may interact with key element */
	void for_nearest(const Key &key, std::function<void(std::list<std::pair<Key,T>> &)> func)
	{
		__call_neighbours(&root,key,[func](Branch *b)
		{
			func(b->elements);
		});
	}
	
	void insert(const Key &key, T elem)
	{
		Branch *b = __find_branch(&root,key);
		b->elements.push_back(std::pair<Key,T>(key,elem));
		if(b->empty)
		{
			__fill_branch(b);
		}
	}
	
	void remove(const Key &key, T elem)
	{
		Branch *b = __find_branch(&root,key);
		b->elements.remove_if([elem](const std::pair<Key,T> &p)->bool{return p.second == elem;});
		__empty_branch_if_needed(b);
	}
	
	void remove(T elem)
	{
		__call_all([elem](Branch *b)
		{
			b->elements.remove_if([elem](const std::pair<Key,T> &p)->bool{return p.second == elem;});
			__empty_branch_if_needed(b);
		});
	}

	void replace(const Key &from, const Key &to, T elem)
	{
		Branch *b = __find_branch(&root,from);
		if((b->offspring && !b->key.isSuitable(to)) || (!b->offspring && !b->key.isSuitableDown(to)))
		{
			b->elements.remove_if([elem](const std::pair<Key,T> &p)->bool{return p.second == elem;});
			__empty_branch_if_needed(b);
			insert(to,elem);
		}
	}
	
	/* Replaces elements if their keys were changed */
	void update()
	{
		__call_all(&root,[this](Branch *b)
		{
			b->elements.remove_if([this,b](const std::pair<Key,T> &p)
			{
				if((b->offspring && !b->key.isSuitable(p.first)) || (!b->offspring && !b->key.isSuitableDown(p.first)))
				{
					insert(p.first,p.second);
					return true;
				}
				return false;
			});
			__empty_branch_if_needed(b);
		});
	}
	
	void _print() const
	{
		std::cout << root.elements.size() << std::endl;
		std::cout 
		    << root.offspring[0].elements.size()
		    << root.offspring[1].elements.size()
		    << root.offspring[2].elements.size()
		    << root.offspring[3].elements.size()
		    << std::endl;
	}
};
