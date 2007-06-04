/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2005, ART+COM AG Berlin, Germany
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//
// Description:  A hybrid (reference counting & mark-sweep garbage collector
//
// Last Review: pavel 30.11.2005 
//
//  review status report: (perfect, ok, fair, poor, disaster)
//    usefullness            : 
//    formatting             : 
//    documentation          : 
//    test coverage          : 
//    names                  : 
//    style guide conformance: 
//    technical soundness    : 
//    dead code              : 
//    readability            : 
//    understandabilty       : 
//    interfaces             : 
//    confidence             : 
//    integration            : 
//    dependencies           : 
//    cheesyness             : 
//
//    overall review status  :
//
//    recommendations:
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#ifndef _included_asl_GarbageCollector_h_
#define _included_asl_GarbageCollector_h_

#include "settings.h"
#include "Exception.h"
#include "string_functions.h"
#include "Singleton.h"

#include "AtomicCount.h"
#ifdef WIN32
#include <stddef.h>
#endif

#include <assert.h>
#include <pthread.h>
#ifdef OSX
#include <malloc/malloc.h>
#else
#include <malloc.h>
#endif

#include <map>
#include <set>

#define DBP(x) // x
#define DB2(x) //  x
#define DBP2(x) //  x

namespace asl {

typedef unsigned long Size;

/*! \addtogroup aslbase */
/* @{ */

DEFINE_EXCEPTION(BadAllocSize,asl::Exception);
DEFINE_EXCEPTION(UnmanageableObject,asl::Exception);
DEFINE_EXCEPTION(InternalCorruption,asl::Exception);
 

    enum CollectorColor { MARK_UNUSED, MARK_UNKNOWN, MARK_IN_USE, IS_ROOT, MARK_LOCKED, MARK_LOCKED_DISPOSED };
    std::ostream & operator<<(std::ostream & os, CollectorColor theColor) {
        switch (theColor) {
            case MARK_UNUSED: os << "MARK_UNUSED"; break;
            case MARK_UNKNOWN: os << "MARK_UNKNOWN"; break;
            case MARK_IN_USE: os << "MARK_IN_USE"; break;
            case IS_ROOT: os << "IS_ROOT"; break;
            case MARK_LOCKED: os << "MARK_LOCKED"; break;
            case MARK_LOCKED_DISPOSED: os << "MARK_LOCKED_DISPOSED"; break;
            default: os << "#illegal"; break;
        }
        return os;
    }
#if 0
    // getNext(0) returns first element
    // getNext(p) return next element or 0 if p is last element
    template <class ThreadingModel, class Allocator>
    class RefInfo {
        bool addObject(void * theNativePtr, ObjectDescriptor<ThreadingModel, Allocator>* theDescriptor) {
        } 

        CollectingPtrBase<ThreadingModel, Allocator> * getNext(CollectingPtrBase<ThreadingModel, Allocator> *);
        bool isRef(CollectingPtrBase<ThreadingModel, Allocator> *);
        bool addRefs(CollectingPtrBase<ThreadingModel, Allocator> *, int theSize);
        bool deleteRefs(CollectingPtrBase<ThreadingModel, Allocator> *, int theSize);

        typedef std::map<void *, ObjectDescriptor<ThreadingModel, Allocator>*> ObjectDescriptorSet;
       
        ObjectDescriptorSet 
    };
#endif

    template <class ThreadingModel, class Allocator>
    struct CollectablePtrBase;
  
    template <class ThreadingModel, class Allocator>
    struct ObjectDescriptor {
        ObjectDescriptor(void * theNativePtr, 
                unsigned theSize,
                long theSmartCount = 1,
                long theWeakCount = 1,
                CollectorColor theColor = MARK_UNKNOWN)
            : myNativePtr(theNativePtr), mySize(theSize), myColor(theColor), smartCount(theSmartCount), weakCount(theWeakCount)
        {}
        void mark(CollectorColor theColor) {
            myColor=theColor;
            DBP2(std::cerr << "ObjectDescriptor::setMark(): myColor="<< myColor << ", this="<<(void*)this<< std::endl);
        }
        CollectorColor getMark() const {
            DBP2(std::cerr << "ObjectDescriptor::getMark(): myColor="<< myColor << ", this="<<(void*)this<< std::endl);
            return myColor;
        }
        unsigned getSize() const {
            DBP2(std::cerr << "ObjectDescriptor::getSize(): mySize="<< mySize << ", this="<<(void*)this<< std::endl);
            return mySize;
        }
        void * getNativePtr() const {
            return myNativePtr;
        }
         void * myNativePtr;
        unsigned mySize;
        typedef std::set<CollectablePtrBase<ThreadingModel, Allocator>*> PtrSet;
        PtrSet myPtrs;
        CollectorColor myColor;

        std::ostream & print(std::ostream & os) const {
            os << "ObjDesc@" << (void*)this<<" ->"<<myNativePtr<<"["<<mySize<<"] smart="<< smartCount
               <<" weak="<<weakCount<<" ptrs["<<myPtrs.size()<<"] c="<<myColor;
        }
        //  ObjectDescriptor<ThreadingModel, Allocator> * next;
        AtomicCount<ThreadingModel> smartCount;

        // weakCount is one higher than the number of weak pointers as long as the smart 
        // count is at least one. This allows an atomic test for destruction of the reference
        // counter: if the weak count is 0, no more references exist.
        AtomicCount<ThreadingModel> weakCount;

        ~ObjectDescriptor(){} 
   private:
        ObjectDescriptor() {}
        ObjectDescriptor & operator=(const ObjectDescriptor & theInfo) {}
        ObjectDescriptor(const ObjectDescriptor & theInfo) {}
        
        inline void init() {
            smartCount.set(1);
            weakCount.set(1);
        }

    };     
    template <class ThreadingModel, class Allocator>
    std::ostream & operator<<(std::ostream & os, const ObjectDescriptor<ThreadingModel, Allocator> & theColor) {
        return theColor.print(os);
    }

    template <class Threading> class CollectablePtrAllocator;
  
    template <class ThreadingModel>
    class CollectablePtrHeapAllocator {
        public:
            typedef CollectablePtrAllocator<ThreadingModel> Allocator; 
            inline static ObjectDescriptor<ThreadingModel, Allocator> * allocate() {
                return new ObjectDescriptor<ThreadingModel, Allocator>;
            }
            inline static void free(ObjectDescriptor<ThreadingModel, Allocator> * anOldPtr) {
                delete anOldPtr;
            }
    };

    template <>
    class CollectablePtrAllocator<MultiProcessor> : public CollectablePtrHeapAllocator<MultiProcessor> {};
    template <>
    class CollectablePtrAllocator<SingleThreaded> : public CollectablePtrHeapAllocator<SingleThreaded> {};
    template <>
    class CollectablePtrAllocator<SingleProcessor> : public CollectablePtrHeapAllocator<SingleProcessor> {};
  
    // All collectable objects should be derived from collectableobject
    // to ensure the proper destructor can be called on finalizing
    template <class ThreadingModel, class Allocator>
    struct Collectable {
        Collectable() {
            DBP2(std::cerr << "Collectable() @" << (void*)this << std::endl);
        };
        virtual ~Collectable() {
            DBP2(std::cerr << "~Collectable() @" << (void*)this << std::endl);
        };
    };
    
    template <class ThreadingModel, class Allocator>
    struct CollectableAllocated;
  
    template <class T, class ThreadingModel, class Allocator=CollectablePtrAllocator<ThreadingModel> >
    struct MakeCollectable : public CollectableAllocated<ThreadingModel, Allocator> {
        MakeCollectable() {}
        MakeCollectable(const T & theType) : myType(theType) {}
        T & operator=(const T & theType) {myType=theType;}
        operator T& ()  {return myType;}
        operator const T& () const {return myType;}
        T myType;
    };

    template <class ThreadingModel, class Allocator>
    struct LessThanCollectablePtrBase
    {
        bool operator()(const CollectablePtrBase<ThreadingModel, Allocator>* s1,
                        const CollectablePtrBase<ThreadingModel, Allocator>* s2) const
        {
            return (char*)s1 < (char*)s2;
        }
    };
    template <class T,
              class ThreadingModel=MultiProcessor,
              class RefCountAllocator=CollectablePtrAllocator<ThreadingModel> >
    class CollectableWeakPtr;


    // This class does the collection work for all circular references
    template <class ThreadingModel, class Allocator>
    struct Collector : public Singleton<Collector<ThreadingModel, Allocator> > {
        typedef std::map<void *, ObjectDescriptor<ThreadingModel, Allocator>*> CollectableObjectMap;
        typedef std::set<CollectablePtrBase<ThreadingModel, Allocator>*,
                         LessThanCollectablePtrBase<ThreadingModel, Allocator> > RootedObjectMap;

        CollectableObjectMap ourCollectableMap;
        RootedObjectMap ourRootedObjectMap;
        unsigned numObjectsInUse;
       
       inline static Collectable<ThreadingModel, Allocator> * getObjectPtr(ObjectDescriptor<ThreadingModel, Allocator> * theInfoPtr) {
           return static_cast<Collectable<ThreadingModel, Allocator>*>(theInfoPtr.nativePtr);
       }    
       inline ObjectDescriptor<ThreadingModel, Allocator> * getInfoPtr(Collectable<ThreadingModel, Allocator> * theObjectPtr) {
           DBP2(std::cerr << "getInfoPtr: theObject="<< theObjectPtr <<  std::endl);
            typename CollectableObjectMap::iterator it = ourCollectableMap.find(theObjectPtr);
            if (it != ourCollectableMap.end() ) {
                DBP2(std::cerr << "getInfoPtr: theObject="<< theObjectPtr <<  " found" << std::endl);
                return it->second;
            }
            DBP2(std::cerr << "getInfoPtr: theObject="<< theObjectPtr <<  " not found" << std::endl);
            return 0;
       }
       static ObjectDescriptor<ThreadingModel, Allocator> & getInfo(typename CollectableObjectMap::iterator it) {
            return *it->second;
       }   
       static const ObjectDescriptor<ThreadingModel, Allocator> & getInfo(typename CollectableObjectMap::const_iterator it) {
            return *it->second;
       }   
 
 
        unsigned getRootPtrCount() const {
            return ourRootedObjectMap.size();
        }
        unsigned getManagedObjectCount() const {
            return ourCollectableMap.size();
        }
        unsigned calcManagedPtrCount() const {
            unsigned myPtrCount = 0;
            for (typename CollectableObjectMap::const_iterator it=ourCollectableMap.begin(); it!=ourCollectableMap.end(); ++it) {
                myPtrCount+=getInfo(it).myPtrs.size();
            }
            return myPtrCount;
        }
  
        // called to find if a new CollectablePtr is part of a collectable container object
        ObjectDescriptor<ThreadingModel, Allocator> * findLocation(void * thePtrLocation)
        {
           if (ourCollectableMap.size() == 0) {
               DBP2(std::cerr << "findLocationInObject: empty list, obj not found" <<  std::endl);
               return 0;
           }
           typename CollectableObjectMap::iterator notLessThanThePtrLocation =
               ourCollectableMap.upper_bound(static_cast<Collectable<ThreadingModel, Allocator>*>(thePtrLocation));
           --notLessThanThePtrLocation;
           DBP2(std::cerr << "findLocationInObject: thePtrLocation="<< thePtrLocation <<  std::endl);
           DBP2(std::cerr << "findLocationInObject: notLessThanThePtrLocation="<< (void*)&*notLessThanThePtrLocation <<  std::endl);
           DBP2(std::cerr << "findLocationInObject: ourCollectableMap.end()="<< (void*)(&*ourCollectableMap.end()) <<  std::endl);
           if (notLessThanThePtrLocation != ourCollectableMap.end()) {
               Collectable<ThreadingModel, Allocator> * myObjectLocation = (Collectable<ThreadingModel, Allocator>*)(notLessThanThePtrLocation->first);
               if ((char*)thePtrLocation >= (char*)myObjectLocation &&
                   (char*)thePtrLocation <  (char*)myObjectLocation + getInfo(notLessThanThePtrLocation).mySize) {
                    DBP2(std::cerr << "findLocationInObject: found" <<  std::endl);
                   return notLessThanThePtrLocation->second;
               }
           }
           DBP2(std::cerr << "findLocationInObject: not found" <<  std::endl);
           return 0;
       }
        // called to find if a new CollectablePtr is part of a collectable container object
        bool findObject(Collectable<ThreadingModel, Allocator> * theObject) {
           DBP2(std::cerr << "findObject: theObject="<< theObject <<  std::endl);
            typename CollectableObjectMap::iterator it = ourCollectableMap.find(theObject);
            if (it != ourCollectableMap.end() ) {
                 DBP2(std::cerr << "findObject: theObject="<< theObject <<  " found" << std::endl);
                return true;
            }
             DBP2(std::cerr << "findObject: theObject="<< theObject <<  " not found" << std::endl);
            return false;
        }

        unsigned markAllUsedObjects() {
            for (typename CollectableObjectMap::iterator it=ourCollectableMap.begin(); it!=ourCollectableMap.end(); ++it) {
                getInfo(it).mark(MARK_UNUSED);
            }
            DBP(std::cerr << "markAllUsedObjects: marked ="<< ourCollectableMap.size() <<  " objects as unused" << std::endl);
            numObjectsInUse = 0;
            for (typename RootedObjectMap::iterator it=ourRootedObjectMap.begin(); it!=ourRootedObjectMap.end(); ++it) {
                DBP(std::cerr << "markAllUsedObjects: marking roots as used, root ="<< (void*)(*it) << std::endl);
                if ((*it)->getDescriptorPtr()) {
                    DBP(std::cerr << "markAllUsedObjects:  check color of "<< (void*)(*it) << std::endl);
                    if ((*it)->getDescriptorPtr()->getMark() != MARK_IN_USE) {
                        (*it)->getDescriptorPtr()->mark(MARK_IN_USE);
                        markUsedObjectsReachableFrom(*(*it)->getDescriptorPtr());
                        ++numObjectsInUse;
                    }
                } else {
                   DBP(std::cerr << "markAllUsedObjects: erase empty root = "<< (void*)(*it) << std::endl);
                   ourRootedObjectMap.erase(it); 
                }
            }
            return numObjectsInUse;
        }
        void markUsedObjectsReachableFrom(ObjectDescriptor<ThreadingModel, Allocator> & theInfo) {
             DBP(std::cerr << "markUsedObjectsReachableFrom: theInfo @ "<< (void*)(&theInfo) << std::endl);
             for (typename ObjectDescriptor<ThreadingModel, Allocator>::PtrSet::iterator it=theInfo.myPtrs.begin();
                  it!=theInfo.myPtrs.end();++it)
             {
                DBP(std::cerr << "markUsedObjectsReachableFrom: theInfo"<< (void*)(&theInfo) << std::endl);
                if ((*it)->getDescriptorPtr() &&
                        (*it)->getDescriptorPtr()->myNativePtr &&
                        (*it)->getDescriptorPtr()->getMark() != MARK_IN_USE) 
                {
                    (*it)->getDescriptorPtr()->mark(MARK_IN_USE);
                    ++numObjectsInUse;
                    DBP(std::cerr << "markUsedObjectsReachableFrom:recursing into ptr="<< (void*)(*it) << std::endl);
                    markUsedObjectsReachableFrom(*(*it)->getDescriptorPtr());
                }
             }
        }
        unsigned sweepUnused() {
            unsigned numSweptObjects = 0;
            typename CollectableObjectMap::iterator it=ourCollectableMap.begin();
            while (it != ourCollectableMap.end()) {
                void * myKey = it->first;
                
                DBP2(std::cerr << "sweepUnused: it->first="<< (void*)(it->first) << std::endl);
                DBP2(std::cerr << "sweepUnused: it->second="<< (void*)(it->second) << std::endl);
               
                // make sure to keep the weakCount to avoid premature disposal of current ObjectDescriptor 
				CollectableWeakPtr<Collectable<ThreadingModel, Allocator>,ThreadingModel, Allocator> myDescriptorSaver(it->second);
				//CollectableWeakPtr<Collectable<ThreadingModel, Allocator>,ThreadingModel, Allocator> myDescriptorSaver2(it->second);
                DBP2(std::cerr << "sweepUnused: saved it in weakptr "<< *myDescriptorSaver.getDescriptorPtr() << std::endl);
             
                typename ObjectDescriptor<ThreadingModel, Allocator>::PtrSet * myPtrs = &getInfo(it).myPtrs;
                typename ObjectDescriptor<ThreadingModel, Allocator>::PtrSet::iterator pit = myPtrs->begin();
                typename ObjectDescriptor<ThreadingModel, Allocator>::PtrSet::iterator myEnd = myPtrs->end();

                while (pit != myEnd) 
                {
                    typename ObjectDescriptor<ThreadingModel, Allocator>::PtrSet::iterator myNext = pit;
                    ++myNext;
                    DBP2(std::cerr << "-- sweepUnused: *pit="<< (void*)(*pit) << std::endl);
                    DBP2(std::cerr << "-- sweepUnused: &*pit="<< (void*)(&*pit) << std::endl);
                    DBP2(std::cerr << "-- sweepUnused:(*pit)->getDescriptorPtr()="<< (void*)(*pit)->getDescriptorPtr() << std::endl);
                    if ((*pit)->getDescriptorPtr() &&
                        (*pit)->getDescriptorPtr()->myNativePtr &&
                        (*pit)->getDescriptorPtr()->getMark() == MARK_UNUSED)
                    {
                        (*pit)->dispose(true);
                        ++numSweptObjects; 
                    } else {
                        DBP2(std::cerr << "-- sweepUnused: &*pit="<< (void*)(&*pit) << " , not disposing" << std::endl);
					}
                    DBP2(std::cerr << "-- sweepUnused: inner loop end *pit="<< (void*)(*pit) << std::endl);
                    DBP2(std::cerr << "sweepUnused: loop end it in weakptr "<< (void*)(myDescriptorSaver.getDescriptorPtr()) << std::endl);
                    pit = myNext;
                }
                DBP2(std::cerr << "- sweepUnused: inner loop done: (&*it)="<< (void*)(&*it) << std::endl);
                it = ourCollectableMap.upper_bound(myKey);
            }
			DBP2(std::cerr << "- sweepUnused: ready"<< std::endl);
            return numSweptObjects;
        }
        void printObjectMap() {
            std::cerr << "-- Collector Map @"<<(void*)(this) << ", size=" << ourCollectableMap.size()<< std::endl;
            for (typename CollectableObjectMap::iterator it=ourCollectableMap.begin(); it!=ourCollectableMap.end(); ++it) {
                std::cerr << "-- @"<<(void*)(it->second) << ", size=" << getInfo(it).mySize<< std::endl;
                for (typename ObjectDescriptor<ThreadingModel, Allocator>::PtrSet::iterator pit=getInfo(it).myPtrs.begin();
                        pit!=getInfo(it).myPtrs.end(); ++pit) {
                    std::cerr << "---- @"<< (void*)(*pit) << ", +"<< (char*)(*pit) - (char*)(it->first)<< std::endl;
                }
            }
        }
     };

    template <class ThreadingModel, class Allocator>
    class CollectableAllocated : public Collectable<ThreadingModel, Allocator> {
    public:
       typedef CollectableAllocated<ThreadingModel, Allocator> ObjectAllocator;
       void * operator new(size_t theSize) {
           ++getAllocatedObjectsCounter();
           void * myMemory = ::operator new(theSize);
           DBP(std::cerr << "CollectableAllocator::new: (orig) @"<< myMemory << ", size=" << theSize << std::endl);
           getTotalAllocatedSizeField()+=theSize; 
           typename Collector<ThreadingModel, Allocator>::CollectableObjectMap & myMap =
                Collector<ThreadingModel, Allocator>::get().ourCollectableMap;
           ObjectDescriptor<ThreadingModel, Allocator> * myObjectDescriptor =
                new ObjectDescriptor<ThreadingModel, Allocator>(myMemory, theSize, 0, 1);
           myMap[myMemory]=myObjectDescriptor;
           DBP(std::cerr << "CollectableAllocator::new descriptor: @"<< (void*)myObjectDescriptor << ", size=" << theSize << std::endl);
           DBP2((Collector<ThreadingModel, Allocator>::get().printObjectMap()));
           return myMemory; 
       }
	   // note: delete is not entirely symmetric with new; new also allocates an ObjectDescriptor
       void operator delete(void * theDoomed, size_t theSize) {
           ++getFreedObjectsCounter();
           DBP(std::cerr << "CollectableAllocator::delete: @"<< theDoomed << ", size=" << theSize << std::endl);
           typename Collector<ThreadingModel, Allocator>::CollectableObjectMap & myMap =
               Collector<ThreadingModel, Allocator>::get().ourCollectableMap;
           typename Collector<ThreadingModel, Allocator>::CollectableObjectMap::iterator mySelf =
                 myMap.find(static_cast<Collectable<ThreadingModel, Allocator>*>(theDoomed)); 
           if (mySelf != myMap.end()) {
			   // if (mySelf->second->weakCount == 0)
               myMap.erase(mySelf);
               DBP(std::cerr << "CollectableAllocator::delete: mySelf deleted" << std::endl);
           } else {
               DBP(std::cerr << "CollectableAllocator::delete: mySelf not in map" << std::endl);
           }
           ::operator delete(theDoomed);
           getTotalAllocatedSizeField()-=theSize; 
           DBP(std::cerr << "CollectableAllocator::delete: done: @"<< theDoomed << ", size=" << theSize << std::endl);
       }
       static unsigned long getAllocatedObjectsCount() {
           return getAllocatedObjectsCounter();
       }    
       static unsigned long getFreedObjectsCount() {
           return getFreedObjectsCounter();
       }
       static unsigned long getTotalAllocatedSize() {
           return getTotalAllocatedSizeField();
       }
    private:
       static unsigned long & getAllocatedObjectsCounter() {
           static unsigned long ourAllocatedObjectsCounter = 0;
           return ourAllocatedObjectsCounter;
       }
       static unsigned long & getFreedObjectsCounter() {
           static unsigned long ourFreedObjectsCounter = 0;
           return ourFreedObjectsCounter;
       }   
       static unsigned long & getTotalAllocatedSizeField() {
           static unsigned long totalAllocatedSize = 0;
           return totalAllocatedSize;
       }   
    };


  // _myRefCountPtr remains
  template <class ThreadingModel, class Allocator>
    struct CollectablePtrBase { // non-template base for all CollectablePtr
        CollectablePtrBase(ObjectDescriptor<ThreadingModel, Allocator> * theDescriptor, void * theNativePtr)
            : myDescriptorPtr(theDescriptor)
        {
		    DBP(std::cerr << "CollectablePtrBase::CollectablePtrBase: this="<< (void*)this << " ,myDescriptorPtr ="<< (void*)myDescriptorPtr <<std::endl);
            if (myDescriptorPtr) {
		        DBP(std::cerr << "CollectablePtrBase::CollectablePtrBase: reference() this="<< (void*)this << " ,myDescriptorPtr ="<< (void*)myDescriptorPtr <<std::endl);
				reference();
			} else {
				if (theNativePtr) {
					myDescriptorPtr =
						 Collector<ThreadingModel, Allocator>::get().getInfoPtr((Collectable<ThreadingModel, Allocator> *)theNativePtr);
		            DBP(std::cerr << "CollectablePtrBase::CollectablePtrBase: reference2() this="<< (void*)this << " ,myDescriptorPtr ="<< (void*)myDescriptorPtr <<std::endl);
					if (myDescriptorPtr) {
						myDescriptorPtr->smartCount.increment();
						DBP(std::cerr << "CollectablePtrBase::CollectablePtrBase: this="<< (void*)this << " ,myDescriptorPtr ="<< *myDescriptorPtr <<std::endl);
					} else {
						DBP(std::cerr << "CollectablePtrBase::CollectablePtrBase: this="<< (void*)this << std::endl);
						throw UnmanageableObject("Target Pointer not a Collectable object, use proper allocator", PLUS_FILE_LINE);
					}
				}
            }
        }

        ObjectDescriptor<ThreadingModel, Allocator> * myDescriptorPtr;
        
        inline ObjectDescriptor<ThreadingModel, Allocator> * getDescriptorPtr() const {
            return myDescriptorPtr;
        }
  
        // find out if this pointer resides in a managed container 
        void registerWithContainer() {
           DBP(std::cerr << "CollectablePtrBase::registerWithContainer: this="<< (void*)this << std::endl);
            ObjectDescriptor<ThreadingModel, Allocator> * myContainer = Collector<ThreadingModel, Allocator>::get().findLocation(this);
            if (myContainer) {
                DBP(std::cerr << "CollectablePtrBase::registerWithContainer: this="<< (void*)this << 
                    " in descr @"<<(void*)myContainer<< 
                    " in obj @"<<(void*)myContainer->myNativePtr << std::endl);
                typename ObjectDescriptor<ThreadingModel, Allocator>::PtrSet::iterator it = myContainer->myPtrs.find(this);
                if (it == myContainer->myPtrs.end())  {
                    DBP(std::cerr << "CollectablePtrBase::registerWithContainer: this="<< (void*)this << " inserted in obj @"<<(void*)myContainer<< std::endl);
                    myContainer->myPtrs.insert(this);
                }
            } else {
                typename Collector<ThreadingModel, Allocator>::RootedObjectMap & ourRootedObjectMap =
                    asl::Singleton<Collector<ThreadingModel, Allocator> >::get().ourRootedObjectMap;
                ourRootedObjectMap.insert(this);
                DBP(std::cerr << "CollectablePtrBase::registerWithContainer: this="<< (void*)this << " registered as root" << std::endl);
             } 
        }
    
        void unregisterWithContainer() {
           DBP(std::cerr << "CollectablePtrBase::unregisterWithContainer: this="<< (void*)this << std::endl);
            ObjectDescriptor<ThreadingModel, Allocator> * myContainer = Collector<ThreadingModel, Allocator>::get().findLocation(this);
            if (myContainer) {
                DBP(std::cerr << "CollectablePtrBase::unregisterWithContainer: this="<< (void*)this << 
                    " in container descr @"<<(void*)myContainer<<
                    " in obj @"<<(void*)myContainer->getNativePtr() << std::endl);
                typename ObjectDescriptor<ThreadingModel, Allocator>::PtrSet::iterator it = myContainer->myPtrs.find(this);
                if (it != myContainer->myPtrs.end())  {
                    myContainer->myPtrs.erase(it);
                    DBP(std::cerr << "CollectablePtrBase::unregisterWithContainer: this="<< (void*)this <<
                                 " erased from obj @"<<(void*)myContainer<< std::endl);
                } else {
                    DBP(std::cerr << "CollectablePtrBase::unregisterWithContainer: this="<< (void*)this <<
                                 " not erased (not found) from obj @"<<(void*)myContainer<< std::endl);
				}
            } else {
                typename Collector<ThreadingModel, Allocator>::RootedObjectMap & ourRootedObjectMap =
                    asl::Singleton<Collector<ThreadingModel, Allocator> >::get().ourRootedObjectMap;
                ourRootedObjectMap.erase(this);
                DBP(std::cerr << "CollectablePtrBase::unregisterAsRootCheck: this="<< (void*)this << " unregistered" << std::endl);
            } 
        }
    
        inline bool reference() {
            DBP2(std::cerr<<"CollectablePtrBase::reference myDescriptorPtr = "<<myDescriptorPtr<<std::endl);
            if (myDescriptorPtr) {
                DBP2(std::cerr<<"CollectablePtrBase::reference before = "<<*myDescriptorPtr<<std::endl);
                if (myDescriptorPtr->smartCount==0) {
                    // we are now a weak ptr
                    myDescriptorPtr->weakCount.conditional_increment();
                    DBP2(std::cerr<<"CollectablePtrBase::(acting weak) middle = "<<*myDescriptorPtr<<std::endl);
                }
                if (!myDescriptorPtr || !myDescriptorPtr->smartCount.conditional_increment()) {
                    DBP2(std::cerr<<"CollectablePtrBase::reference after(1) = "<<*myDescriptorPtr<<std::endl);
                    return false;
                }
                DBP2(std::cerr<<"CollectablePtrBase::reference after(2) = "<<*myDescriptorPtr<<std::endl);
                return true;
            }
            return false;
        }

        inline void dispose(bool forced) {
			DBP(std::cerr<<"CollectablePtrBase::dispose() this = "<<(void*)this<<std::endl);
            DBP2(std::cerr<<"CollectablePtrBase::dispose() myDescriptorPtr = "<<myDescriptorPtr<<std::endl);
 			if (myDescriptorPtr) {
                DBP2(std::cerr<<"CollectablePtrBase::dispose before = "<<*myDescriptorPtr<<std::endl);
                if (myDescriptorPtr->myNativePtr) {
                    Collectable<ThreadingModel, Allocator>* myNativeSavePtr = (Collectable<ThreadingModel, Allocator>*)myDescriptorPtr->myNativePtr;
                    myDescriptorPtr->myNativePtr = 0;
                    delete myNativeSavePtr;
                }
                if (forced) {
                    // dispose has been forced by sweep and not by refcount
                    // our destructor will run later so we will not dispose the descriptor yet
                    DBP2(std::cerr<<"CollectablePtrBase::dispose (weakening1) = "<<*myDescriptorPtr<<std::endl);
                    while (myDescriptorPtr->smartCount) { // turn other eventually existing smart refs into weak refs
                        myDescriptorPtr->smartCount.decrement_and_test();
                        DBP2(std::cerr<<"CollectablePtrBase::dispose (weakening2) = "<<*myDescriptorPtr<<std::endl);
                        myDescriptorPtr->weakCount.increment();
                        DBP2(std::cerr<<"CollectablePtrBase::dispose (weakening3) = "<<*myDescriptorPtr<<std::endl);
                    }
                } else {
                    DBP2(std::cerr<<"CollectablePtrBase::dispose middle = "<<*myDescriptorPtr<<std::endl);
                    if (myDescriptorPtr->weakCount.decrement_and_test()) {
                        DBP2(std::cerr<<"CollectablePtrBase::dispose free myDescriptorPtr: "<<*myDescriptorPtr<<std::endl);
                        // play nice & thread-safe in case another thread will call
                        // reference() while we are unreference
                        ObjectDescriptor<ThreadingModel, Allocator> * savePtr = myDescriptorPtr;
                        myDescriptorPtr = 0;
                        delete savePtr;
                        //Allocator::free(savePtr);
                    } else {
                        DBP2(std::cerr<<"CollectablePtrBase::dispose after = "<<*myDescriptorPtr<<std::endl);
                    }
                }
            }
        }

        inline void unreference() {
            DBP2(std::cerr<<"CollectablePtrBase::unreference myDescriptorPtr = "<<myDescriptorPtr<<std::endl);
            if (myDescriptorPtr) {
                 DBP2(std::cerr<<"CollectablePtrBase::unreference before: "<<*myDescriptorPtr<<std::endl);
                if (!myDescriptorPtr->smartCount || myDescriptorPtr->smartCount.decrement_and_test()) {
                    DBP2(std::cerr<<"CollectablePtrBase::unreference after1: "<<*myDescriptorPtr<<std::endl);
                    dispose(false);
                } else {
                    DBP2(std::cerr<<"CollectablePtrBase::unreference after2: "<<*myDescriptorPtr<<std::endl);
                }
            }
        }
        ~CollectablePtrBase() {
            DBP(std::cerr<<"~CollectablePtrBase this = "<<(void*)this<<std::endl);
            unregisterWithContainer();
			unreference();

        }
    };

       // Use this class almost exactly as you would use a pointer
    template<class C,
             class ThreadingModel=MultiProcessor,
             class RefCountAllocator=CollectablePtrAllocator<ThreadingModel> >
    class CollectablePtr : public CollectablePtrBase<ThreadingModel, RefCountAllocator> {
        
    public:
        typedef ThreadingModel ThreadingModelType;
        typedef RefCountAllocator RefCountAllocatorType;
        typedef CollectablePtrBase<ThreadingModel, RefCountAllocator> Base;

        explicit CollectablePtr(C * nativePtr = 0) : CollectablePtrBase<ThreadingModel, RefCountAllocator>(0, nativePtr) {
            Base::registerWithContainer();
        }
        
        explicit CollectablePtr(ObjectDescriptor<ThreadingModel, RefCountAllocator> * theDescriptorPtr, void *) // strange signature to avoid ambiguity
            : CollectablePtrBase<ThreadingModel, RefCountAllocator>(theDescriptorPtr, 0)
        {
            if (!Base::myDescriptorPtr || !Base::myDescriptorPtr->smartCount) {
                throw BadWeakPtrException(JUST_FILE_LINE);
            }
        }
        
        template <class D>
        CollectablePtr(const CollectablePtr<D,ThreadingModel,RefCountAllocator> & otherType)
            : CollectablePtrBase<ThreadingModel, RefCountAllocator>(otherType.getDescriptorPtr(), 0)
        {
            Base::registerWithContainer();
        }

        CollectablePtr(const CollectablePtr & otherPtr)
            : CollectablePtrBase<ThreadingModel, RefCountAllocator>(otherPtr.getDescriptorPtr(), 0)
        {
            Base::registerWithContainer();
        }

        template<class D>
        const CollectablePtr<C,ThreadingModel,RefCountAllocator> & operator=(const CollectablePtr<D,ThreadingModel,RefCountAllocator> & otherType) 
        {
            if (static_cast<const void *>(&otherType) != static_cast<const void*>(this)) {
                Base::unreference();
                Base::myDescriptorPtr = otherType.myDescriptorPtr;
                Base::reference();
            }
            return *this;
        }

        inline const CollectablePtr & operator=(const CollectablePtr & otherPtr) {
            if (&otherPtr != this) {
                Base::unreference();
                Base::myDescriptorPtr = otherPtr.myDescriptorPtr;
                Base::reference();
            }
            return *this;
        }

		inline bool operator==(const CollectablePtr& otherPtr) const {
            return Base::myDescriptorPtr == otherPtr.myDescriptorPtr;
        }

        inline bool operator!=(const CollectablePtr& otherPtr) const {
            return !operator==(otherPtr);
        }

        inline operator bool() const {
            return (Base::myDescriptorPtr && Base::myDescriptorPtr->myNativePtr);
        }

        inline C & operator*() {
            return *static_cast<C*>(Base::myDescriptorPtr->myNativePtr);
        }

        inline const C & operator*() const {
            return *static_cast<const C *>(Base::myDescriptorPtr->myNativePtr);
        }

        inline C * operator->() {
            return static_cast<C*>(Base::myDescriptorPtr->myNativePtr);
        }

        inline const C * operator->() const {
            return static_cast<const C *>(Base::myDescriptorPtr->myNativePtr);
        }

        inline C * getNativePtr() const {
            return static_cast<C*>(Base::myDescriptorPtr ? Base::myDescriptorPtr->myNativePtr : 0);
        }

      
        inline long getRefCount() const {
            if (Base::myDescriptorPtr) {
                return Base::myDescriptorPtr->smartCount;
            }
            return 0;
        }
        
    public:
        bool operator==(const CollectableWeakPtr<C, ThreadingModel, RefCountAllocator> & otherPtr) const {
            return Base::myDescriptorPtr == otherPtr.myDescriptorPtr;
        }

        bool operator!=(const CollectableWeakPtr<C, ThreadingModel, RefCountAllocator> & otherPtr) const {
            return Base::myDescriptorPtr != otherPtr.myDescriptorPtr;
        }
    
    private:
        // do not allow comparison with other types and forbid comparison of bool results
        template<class D>
        bool operator==(const CollectableWeakPtr<D, ThreadingModel, RefCountAllocator> & otherPtr) const {
            throw Forbidden(JUST_FILE_LINE);
        }

        template<class D>
        bool operator!=(const CollectableWeakPtr<D, ThreadingModel, RefCountAllocator> & otherPtr) const {
            throw Forbidden(JUST_FILE_LINE);
        }
    };

    template <class T,
              class ThreadingModel,
              class RefCountAllocator>
    class CollectableWeakPtr {
        friend class CollectablePtr<T, ThreadingModel, RefCountAllocator>;
        public:
            CollectableWeakPtr() :
                myDescriptorPtr(0)
            {};

            CollectableWeakPtr(const CollectablePtr<T, ThreadingModel, RefCountAllocator> & theSmartPtr) :
                myDescriptorPtr(theSmartPtr.getDescriptorPtr())
            {
                reference();
            };


            CollectableWeakPtr(const CollectableWeakPtr & otherWeakPtr) :
                myDescriptorPtr(otherWeakPtr.myDescriptorPtr)
            {
                reference();
            }
            explicit CollectableWeakPtr(const CollectablePtrBase<ThreadingModel, RefCountAllocator> & otherPtr) :
                myDescriptorPtr(otherPtr.myDescriptorPtr)
            {
                reference();
            }
             explicit CollectableWeakPtr(ObjectDescriptor<ThreadingModel, RefCountAllocator> * theDescriptorPtr) :
                myDescriptorPtr(theDescriptorPtr)
            {
                reference();
            }
            ~CollectableWeakPtr() {
                DBP2(std::cerr<<"~CollectableWeakPtr() = "<<(void*)this<<std::endl);
                unreference();
            }

            inline const CollectableWeakPtr & operator=(const CollectableWeakPtr & otherPtr) {
                if (this != & otherPtr) {
                    unreference();
                    myDescriptorPtr = otherPtr.myDescriptorPtr;
                    reference();
                }
                return *this;
            }

            operator bool() const {
                return isValid();
            }

            inline ObjectDescriptor<ThreadingModel, RefCountAllocator> * getDescriptorPtr() const {
                return myDescriptorPtr;
            }

            inline CollectablePtr<T, ThreadingModel, RefCountAllocator> lock() {
                if (isValid()) {
                    try {
                        return CollectablePtr<T, ThreadingModel, RefCountAllocator>(myDescriptorPtr, 0);
                    } catch (BadWeakPtrException&) {
                        std::cerr << "BadWeakPtrException: refCount=" << getRefCount() << 
                                ", weakCount=" << getWeakCount() << std::endl;
                    }
                }
                return CollectablePtr<T, ThreadingModel, RefCountAllocator>(0);
            }

            inline const CollectablePtr<T, ThreadingModel, RefCountAllocator> lock() const {
                if (isValid()) {
                    try {
                        return CollectablePtr<T, ThreadingModel, RefCountAllocator>(myDescriptorPtr, 0);
                    } catch (BadWeakPtrException&) {
                        std::cerr << "BadWeakPtrException: refCount=" << getRefCount() << 
                                ", weakCount=" << getWeakCount() << std::endl;
                    }
                }
                return CollectablePtr<T, ThreadingModel, RefCountAllocator>(0);
            }

            // only use for debugging and tests
            inline long getRefCount() const {
                if (myDescriptorPtr) {
                    return myDescriptorPtr->smartCount;
                }
                return 0;
            }
            inline long getWeakCount() const {
                if (myDescriptorPtr) {
                    return myDescriptorPtr->weakCount;
                }
                return 0;
            }

        private:
            ObjectDescriptor<ThreadingModel, RefCountAllocator> * myDescriptorPtr;

            inline bool isValid() const {
                if (myDescriptorPtr && myDescriptorPtr->smartCount != 0) {
                    return true;
                }
                return false;
            }


            inline void reference() {
                DBP2(std::cerr<<"CollectableWeakPtr::reference myDescriptorPtr = "<<myDescriptorPtr<<std::endl);
                if (myDescriptorPtr) {
                    DBP2(std::cerr<<"CollectableWeakPtr::reference before = "<<*myDescriptorPtr<<std::endl);
                    myDescriptorPtr->weakCount.increment();
                    DBP2(std::cerr<<"CollectableWeakPtr::reference after = "<<*myDescriptorPtr<<std::endl);
                }
            }

            inline void unreference() {
                DBP2(std::cerr<<"CollectableWeakPtr::unreference myDescriptorPtr = "<<myDescriptorPtr<<std::endl);
                if (myDescriptorPtr) {
                    DBP2(std::cerr<<"CollectableWeakPtr::unreference  before = "<<*myDescriptorPtr<<std::endl);
                    if (myDescriptorPtr->weakCount == 0) {
                        throw InternalCorruption("weakCount already 0 on weakPtr unreferencing", PLUS_FILE_LINE);
                    }
                    if (myDescriptorPtr->weakCount.decrement_and_test())
                    {
                        DBP2(std::cerr<<"CollectableWeakPtr::unreference  after(1) = "<<*myDescriptorPtr<<std::endl);
                        DBP2(std::cerr<<"Ptr: CollectableWeakPtr calling free on myDescriptorPtr"<<std::endl);
                        //RefCountAllocator::free(myDescriptorPtr);
                        delete myDescriptorPtr;
                    } else {
                        DBP2(std::cerr<<"CollectableWeakPtr::unreference  after(2) = "<<*myDescriptorPtr<<std::endl);
                    }
					myDescriptorPtr = 0;
                }
            }

           public:
                bool operator==(const CollectableWeakPtr & otherPtr) const {
                    return myDescriptorPtr == otherPtr.myDescriptorPtr;
                }
                bool operator!=(const CollectableWeakPtr & otherPtr) const {
                    return myDescriptorPtr != otherPtr.myDescriptorPtr;
                }
                bool operator==(const Ptr<T, ThreadingModel, RefCountAllocator> & otherPtr) const {
                    return myDescriptorPtr == otherPtr.myDescriptorPtr;
                }
                bool operator!=(const Ptr<T, ThreadingModel, RefCountAllocator> & otherPtr) const {
                    return myDescriptorPtr != otherPtr.myDescriptorPtr;
                }
           private:
                // do not allow comparison with other types and forbid comparison of bool results
                template<class D>
                bool operator==(const CollectableWeakPtr<D, ThreadingModel, RefCountAllocator> & otherPtr) const {
                    throw Forbidden(JUST_FILE_LINE);
                }
                template<class D>
                bool operator!=(const CollectableWeakPtr<D, ThreadingModel, RefCountAllocator> & otherPtr) const {
                    throw Forbidden(JUST_FILE_LINE);
                }
                template<class D>
                bool operator==(const Ptr<D, ThreadingModel, RefCountAllocator> & otherPtr) const {
                    throw Forbidden(JUST_FILE_LINE);
                }
                template<class D>
                bool operator!=(const Ptr<D, ThreadingModel, RefCountAllocator> & otherPtr) const {
                    throw Forbidden(JUST_FILE_LINE);
                }
    };


} //namespace

template<typename T, typename U, class ThreadingModel, class RefCountAllocator>
asl::CollectablePtr<T,ThreadingModel,RefCountAllocator> static_cast_CollectablePtr(asl::CollectablePtr<U,ThreadingModel,RefCountAllocator> const & r)
{
    return asl::CollectablePtr<T,ThreadingModel,RefCountAllocator>(static_cast<T*>(r.getDescriptorPtr()));
}

template<typename T, typename U, class ThreadingModel, class RefCountAllocator>
asl::CollectablePtr<T,ThreadingModel,RefCountAllocator> dynamic_cast_CollectablePtr(asl::CollectablePtr<U,ThreadingModel,RefCountAllocator> const & r)
{
    try {
        T * myCastedNative = dynamic_cast<T*>(r.getNativePtr());
        if (myCastedNative) {
             DBP2(std::cerr<<"dynamic_cast_CollectablePtr myDescriptorPtr = "<<r.myDescriptorPtr<<std::endl);
            return asl::CollectablePtr<T,ThreadingModel,RefCountAllocator>(r.getDescriptorPtr(), 0);
        } else {
            return asl::CollectablePtr<T,ThreadingModel,RefCountAllocator>(0);
        }
    } catch (asl::BadWeakPtrException&) {
        return asl::CollectablePtr<T,ThreadingModel,RefCountAllocator>(0);
    }
}


#undef DB2
#undef DBP
#undef DBP2


/* @} */

#endif