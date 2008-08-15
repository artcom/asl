/**********************************************************************************************/

/*
**
** Copyright (C) 1993-2005, ART+COM AG Berlin, Germany
**
** These coded instructions, statements, and computer programs contain
** unpublished proprietary information of ART+COM AG Berlin, and
** are copy protected by law. They may not be disclosed to third parties
** or copied or duplicated in any form, in whole or in part, without the
** specific, prior written permission of ART+COM AG Berlin.
**
*/

/**********************************************************************************************/

/*!
** \file xpath/Path.cpp
**
** \brief XPath parsed paths
**
** \author Hendrik Schober
**
** \date 2008-07-11
**
*/

/**********************************************************************************************/
#include "Path.h"

/**********************************************************************************************/
#include <iostream>
#include <iterator>

/**********************************************************************************************/
#include <asl/Logger.h>

/**********************************************************************************************/
#include "PathParser.h"

/**********************************************************************************************/

namespace {

    DEFINE_EXCEPTION(UnknownParseError, xpath::ParseError);

    inline void parsePath_( xpath::detail::ParseInput& pi
                          , xpath::detail::LocationStepList& steps )
    {
        bool result = true;
        try {
            xpath::detail::ParseState ps(pi);
            if( !xpath::detail::parsePath( pi, steps ) ) {
                throw UnknownParseError( "Unknown parse error", PLUS_FILE_LINE );
            }
            ps.commit();
        } catch( const xpath::XPathError& x ) {
            AC_ERROR << "Parse error: Couldn't parse \"" << pi.getRemainingString() << '\"';
            AC_ERROR << x;
            AC_ERROR << pi.getString();
            AC_ERROR << std::string(pi.getErrorIndex(),' ') << '^';
            throw;
        }
    }

}

/**********************************************************************************************/

namespace xpath {

    //class Path {
    //private:
    class Path::Impl_ : public Path::StepList {
        public:
            unsigned long         references;
            Impl_()                                 : references(0), Path::StepList() {}
        private:
            Impl_(const Impl_&);
            Impl_& operator=(const Impl_&);
        };

    //public:
        Path::Path(const std::string& str)
            : myImpl_(new Impl_)
        {
            ++myImpl_->references;
            xpath::detail::ParseInput pi(str);
            parsePath_(pi,myImpl_->steps);
        }

        Path::Path(const std::string& str, std::string::size_type& idx)
            : myImpl_(new Impl_)
        {
            ++myImpl_->references;
            xpath::detail::ParseInput pi(str,idx);
            parsePath_(pi,myImpl_->steps);
            idx = pi.getCurrentIndex();
        }

        Path::Path(const StepList& stepList)
            : myImpl_(new Impl_)
        {
            ++myImpl_->references;
            myImpl_->steps = stepList.steps;
        }
        
        Path::Path(const Path& rhs)
            : myImpl_(rhs.myImpl_)
        {
            ++myImpl_->references;
        }

        Path::~Path()
        {
            if( 0 == --myImpl_->references ) {
                delete myImpl_;
            }
        }

        Path& Path::operator=(const Path& rhs)
        {
            if( 0 == --myImpl_->references ) {
                delete myImpl_;
            }
            myImpl_ = rhs.myImpl_;
            ++myImpl_->references;
            return *this;
        }

        std::ostream& Path::streamTo(std::ostream& os) const
        {
            typedef xpath::detail::LocationStepList::const_iterator
                                                          citer_t;
            citer_t it=myImpl_->steps.begin();
            while( it!=myImpl_->steps.end() ) {
                os << *it;
                const bool root = it->isRoot();
                ++it;
                if( it!=myImpl_->steps.end() && !root ) {
                    os << '/';
                }
            }
            return os;
        }

        std::string Path::toString() const
        {
            std::ostringstream oss;
            streamTo(oss);
            return oss.str();
        }

        int Path::compare(const Path& rhs) const
        {
            typedef detail::LocationStepList::const_iterator
                                                      const_iterator;
                                                      
                  const_iterator lhs_begin = myImpl_->steps.begin();
            const const_iterator lhs_end   = myImpl_->steps.end  ();
                  const_iterator rhs_begin = rhs.myImpl_->steps.begin();
            const const_iterator rhs_end   = rhs.myImpl_->steps.end  ();

            while( lhs_begin!=lhs_end && rhs_begin!=rhs_end ) {
                int cmp = lhs_begin->compare(*rhs_begin);
                if( cmp < 0 ) {
                    return -1;
                }
                if( cmp > 0 ) {
                    return 1;
                }
                ++lhs_begin;
                ++rhs_begin;
            }

            if( lhs_begin==lhs_end ) {
                if( rhs_begin==rhs_end ) {
                    return 0;
                }
                return -1;
            }
            assert( rhs_begin==rhs_end );
            return 1;
        }

        const Path::StepList& Path::getStepList() const
        {
            return *myImpl_;
        }
    //};

}

/**********************************************************************************************/
