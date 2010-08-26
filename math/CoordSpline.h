/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// This file is part of the ART+COM Standard Library (asl).
//
// It is distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//  Author: Axel Kilian
//
//  $Revision: 1.5 $
//
//  Description:
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#ifndef _included_asl_CoordSpline_
#define _included_asl_CoordSpline_

#include "asl_math_settings.h"

#include "Vector234.h"
#include "Matrix4.h"
#include "Quaternion.h"
#include <vector>


namespace asl {

    /*! @addtogroup aslmath */
    /* @{ */

    class Hermite;

    class Keyframe
    {
      public:
        Keyframe( asl::Vector3f & coord, float theTimeDelta, float theSpeed )
                    : xyz(coord), t(theTimeDelta), speed(theSpeed)  {}
        Keyframe( const Keyframe& theKeyframe) { xyz = theKeyframe.xyz; t = theKeyframe.t; speed = theKeyframe.speed;}
        Keyframe() {}
            Keyframe & operator=(const Keyframe & theKeyframe) {
                xyz = theKeyframe.xyz;
                t = theKeyframe.t;
                speed = theKeyframe.speed;
                return * this;
            }

        virtual ~Keyframe() {}
        const asl::Vector3f & getPosition() const { return xyz; }
        virtual void setPosition(const asl::Vector3f & thePosition) { xyz = thePosition; }

            const float getTime() const { return t; }
            void setTime(float theTimeDelta) { t = theTimeDelta; }
            const float getSpeed() const { return speed; }
            void setSpeed(float theSpeed) { speed = theSpeed; }
        protected:

            asl::Vector3f  xyz;
            float  t;
            float  speed;

    };
    class EulerKeyframe
    {
      public:
            // XXX don't know if here's actually ZYXRotating needed (gm)
        EulerKeyframe( asl::Vector3f & coord, asl::Vector3f & orientation, float theTimeDelta, float theSpeed )
                : rotation(orientation), xyz(coord), t(theTimeDelta), speed(theSpeed)
         {
                _myOrientationMatrix.makeZYXRotating(rotation);
         }

        EulerKeyframe( const EulerKeyframe& theKeyframe)
              : rotation(theKeyframe.rotation),
                xyz(theKeyframe.xyz),
                t(theKeyframe.t),
                speed (theKeyframe.speed)
            {
                _myOrientationMatrix.makeZYXRotating(rotation);
        }
            EulerKeyframe & operator=(const EulerKeyframe & theKeyframe) {
                rotation = theKeyframe.rotation;
                xyz = theKeyframe.xyz;
                t = theKeyframe.t;
                speed = theKeyframe.speed;
                return * this;
            }
        EulerKeyframe() {}
        virtual ~EulerKeyframe() {}

        const asl::Vector3f & getPosition() const { return xyz; }
        virtual void setPosition(const asl::Vector3f & thePosition) { xyz = thePosition; }

            const float getTime() const { return t; }
            void setTime(float theTimeDelta) { t = theTimeDelta; }
            const float getSpeed() const { return speed; }
            void setSpeed(float theSpeed) { speed = theSpeed; }


        const asl::Vector3f & getOrientation() const { return rotation; }
        void setOrientation(const asl::Vector3f & theOrientation) {
             rotation = theOrientation;
                _myOrientationMatrix.makeZYXRotating(rotation);
        }
        const asl::Matrix4f & getOrientationMatrix() const { return _myOrientationMatrix; }

        private:
            asl::Matrix4f   _myOrientationMatrix;
            asl::Vector3f  rotation;
            asl::Vector3f  xyz;
            float  t;
            float  speed;

    };

    class QuaternionKeyframe
    {
      public:
        QuaternionKeyframe( asl::Vector3f & coord, asl::Quaternionf & orientation, float theTimeDelta, float theSpeed )
                : _myQuaternion(orientation), xyz(coord), t(theTimeDelta), speed(theSpeed)
            {
                _myOrientationMatrix = asl::Matrix4f(_myQuaternion);
            }
        QuaternionKeyframe( const QuaternionKeyframe& theKeyframe) {
                _myQuaternion = theKeyframe._myQuaternion;
                _myOrientationMatrix = theKeyframe._myOrientationMatrix;
                xyz = theKeyframe.xyz;
                t = theKeyframe.t;
                speed = theKeyframe.speed;
        }
            QuaternionKeyframe & operator=(const QuaternionKeyframe & theKeyframe) {
                _myQuaternion = theKeyframe._myQuaternion;
                _myOrientationMatrix = theKeyframe._myOrientationMatrix;
                xyz = theKeyframe.xyz;
                t = theKeyframe.t;
                speed = theKeyframe.speed;
                return * this;
            }
        QuaternionKeyframe() {}
        virtual ~QuaternionKeyframe() {}

        const asl::Vector3f & getPosition() const { return xyz; }
        virtual void setPosition(const asl::Vector3f & thePosition) { xyz = thePosition; }

            const float getTime() const { return t; }
            void setTime(float theTimeDelta) { t = theTimeDelta; }
            const float getSpeed() const { return speed; }
            void setSpeed(float theSpeed) { speed = theSpeed; }

        void setOrientation(const asl::Quaternionf & theOrientation) {
                _myOrientationMatrix = asl::Matrix4f(_myQuaternion);
        }
        const asl::Quaternionf & getOrientation() const { return _myQuaternion; }
        const asl::Matrix4f & getOrientationMatrix() const { return _myOrientationMatrix; }

        private:
            asl::Quaternionf _myQuaternion;
            asl::Matrix4f   _myOrientationMatrix;
            asl::Vector3f  xyz;
            float  t;
            float  speed;

    };

    class QuaternionSpline {
            public:
              typedef std::vector<asl::Quaternionf> VectorOfQuaternionf;
              typedef std::map<float,Quaternionf> OrientationKeys;
              QuaternionSpline( const VectorOfQuaternionf & quaternionList, const std::vector<float> & timeList){
                for(VectorOfQuaternionf::size_type i = 0; i < quaternionList.size(); i++) {
                  _orientationKeys.insert(std::make_pair(timeList[i], quaternionList[i]));
                }
              };
              asl::Quaternionf getOrientation(float s) const{
                                //pos points on next keyframe with timestamp >= s
                                OrientationKeys::const_iterator next = _orientationKeys.lower_bound(s);
                                if(next == _orientationKeys.begin()) {
                                   return next->second;
                                }
                                if(next == _orientationKeys.end()) {
                                  next--;
                                }

                                OrientationKeys::const_iterator previous(next);
                                    --previous;

                                Quaternionf q;
                  float progress = (s - previous->first) / (next->first - previous->first);
                  AC_TRACE << "slerp params: " << previous->second << next->second << progress;

                  q = slerp(previous->second, next->second, progress);
                  return q;
                            };
            private:
              OrientationKeys _orientationKeys;
        };


    class ASL_MATH_DECL CoordSpline
    {
      public:
          CoordSpline        ( void );
          ~CoordSpline      ( void );
        void    print        ( void );
        bool    init        ( const std::vector<asl::QuaternionKeyframe>& keyframes, float& total_path, bool planet_mode );
        asl::Vector3f xyz          ( float t );
        asl::Quaternionf getQuaternion ( float s );
//        asl::Vector3f getHPR   ( float t );
        float  s          ( float t );
        float  sPlanet        ( float t );
        float  v          ( float t );
//        float  vPlanet        ( float t );
        float  arcElementTrans      ( float t );
//        float  arcElementRot      ( float t );
        float  getPath        ( int i );
        float  getTime        ( int i );
        QuaternionSpline  * quaternionSpline;

    //  private:

        typedef std::vector<Hermite*> Spline;
        Spline  _spline;
        bool    _planet_mode;
        int      _error_count;
        int      _error_maxnum;

//        asl::Vector3f vFront        ( float s );
//        asl::Vector3f vRight        ( float s );
//        asl::Vector3f vCamera        ( float s );
        void    avoidEarthCollision      ( int segment );
        float  rad23        ( float t );
        float   minRad2        ( float ax, float bx, float cx, float tol, float *xmin );

    };

#if 0
    class v_planet {
        public:
      v_planet ( CoordSpline& c    ) : _c(c   ) {}
      v_planet ( const v_planet& c ) : _c(c._c) {}

      float operator()  (float x) { return _c.vPlanet(x); }

        private:
      CoordSpline& _c;
    };

#endif
    /* @} */

}
#endif // _included_asl_CoordSpline_
