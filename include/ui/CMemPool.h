/* Copyright(C) 2025 guillaume.taze@proton.me

This program is free software : you can redistribute it and /or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.If not, see < https://www.gnu.org/licenses/>.
*/

#ifndef CUIMEMPOOL_h
#define CUIMEMPOOL_h

#include <forward_list>
#include <vector>

template<class ClassType, unsigned int PoolSize>
class CPoolAllocatorBlock
{
public:
	CPoolAllocatorBlock() { Pool = new ClassType[PoolSize]; for (int Index = 0; Index < PoolSize; ++Index) { FreeMem.push_front(&Pool[Index]); } }
	~CPoolAllocatorBlock() { delete[] Pool; }

	ClassType* New() { if (FreeMem.empty()) return nullptr; ClassType* NewObj = FreeMem.front(); FreeMem.pop_front(); new (NewObj) ClassType(); return NewObj; }
	void Delete(ClassType* Obj) { if (Obj) { Obj->~ClassType(); FreeMem.push_front(Obj); } }

	bool HasSlot() const { return !FreeMem.empty(); }
	bool Contains(const ClassType* Obj) const { return Obj >= Pool && Obj <= Pool + PoolSize * sizeof(ClassType); }

	ClassType* Pool;
	std::forward_list<ClassType*> FreeMem;
};

template<class ClassType, unsigned int PoolBlockSize>
class CPoolAllocator
{
public:
	std::vector<CPoolAllocatorBlock<ClassType, PoolBlockSize>*> Blocks;

	CPoolAllocator() { Blocks.push_back(new CPoolAllocatorBlock<ClassType, PoolBlockSize>()); }
	~CPoolAllocator() { while (!Blocks.empty()) { delete Blocks.back(); Blocks.pop_back(); } }

	ClassType* New() 
	{
		for (int Index = 0; Index < Blocks.size(); ++Index)
		{
			if (Blocks[Index]->HasSlot())
			{
				return Blocks[Index]->New();
			}
		}
		Blocks.push_back(new CPoolAllocatorBlock<ClassType, PoolBlockSize>());
		return Blocks.back()->New();
	}

	void Delete(ClassType* Obj) 
	{ 
		for (int Index = 0; Index < Blocks.size(); ++Index)
		{
			if (Blocks[Index]->Contains(Obj))
			{
				Blocks[Index]->Delete(Obj);
				return;
			}
		}
	}
};

class CReferenced
{
public:
	CReferenced() : ReferenceCount(0) { }
	virtual ~CReferenced() { }

	void IncrementRefCount() { ++ReferenceCount; }
	void DecrementRefCount() { --ReferenceCount; if (ReferenceCount < 0) ReferenceCount = 0; }
	int GetRefCount() const { return ReferenceCount; }

	virtual void DeleteItself() { delete this; }

private:
	int ReferenceCount;
};

template<class ClassType>
class TSharedPtr
{
public:
	TSharedPtr() : Ptr(0) { }
	TSharedPtr(ClassType* InPtr) : Ptr(InPtr) { Reference(); }
	~TSharedPtr() { Dereference(); }

	TSharedPtr(const TSharedPtr<ClassType>& Rhs)
	{
		Ptr = Rhs.Ptr;
		Reference();
	}

	TSharedPtr<ClassType>& operator=(const TSharedPtr<ClassType>& Rhs)
	{
		if (Ptr == Rhs.Ptr)
			return *this;

		Dereference();
		Ptr = Rhs.Ptr;
		Reference();
		return *this;
	}

	bool operator==(const TSharedPtr<ClassType>& Rhs) const { return Ptr == Rhs.Ptr; }
	bool operator==(const ClassType* Rhs) const { return Ptr == Rhs; }
	bool operator!=(const TSharedPtr<ClassType>& Rhs) const { return Ptr != Rhs.Ptr; }
	bool operator!=(const ClassType* Rhs) const { return Ptr != Rhs; }

	void Reset() { Dereference(); }

	ClassType* Get() { return Ptr; }
	const ClassType* Get() const { return Ptr; }
	ClassType* operator->() { return Ptr; }
	ClassType* operator*() { return Ptr; }

	operator bool() const { return Ptr != 0; }

protected:
	void Reference()
	{
		if (Ptr)
		{
			Ptr->IncrementRefCount();
		}
	}

	void Dereference()
	{
		if (Ptr) 
		{ 
			Ptr->DecrementRefCount(); 
			if (Ptr->GetRefCount() == 0)
			{
				Ptr->DeleteItself();
			}
		}
		Ptr = 0;
	}

private:
	ClassType* Ptr;
};

template<class ForClass>
class CStaticPoolAllocator
{
public:
	static CPoolAllocator<ForClass, 512> Allocator;

	static ForClass* New() { return Allocator.New(); }
};

template<class ForClass>
CPoolAllocator<ForClass, 512> CStaticPoolAllocator<ForClass>::Allocator;

#endif
