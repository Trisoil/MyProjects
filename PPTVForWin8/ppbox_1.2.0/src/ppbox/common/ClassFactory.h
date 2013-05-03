// Create.h

#ifndef _PPBOX_COMMON_CLASS_FACTORY_H_
#define _PPBOX_COMMON_CLASS_FACTORY_H_

#include "ppbox/common/Create.h"
#include "ppbox/common/Call.h"

#include <boost/function.hpp>

namespace ppbox
{
    namespace common
    {

        template <
            typename ClassType, 
            typename KeyType, 
            typename CreateProto
        >
        class ClassFactory
        {
        protected:
            typedef KeyType key_type;
            typedef ClassType * result_type;
            typedef boost::function<CreateProto> creator_type;
            typedef std::map<KeyType, creator_type> creator_map_type;

        protected:
            typedef ClassFactory factory_type;

        public:
            static void register_creator(
                key_type const & key, 
                creator_type creator)
            {
                creator_map().insert(std::make_pair(key, creator));
            }

            static result_type create(
                key_type const & key)
            {
                typename creator_map_type::const_iterator iter = 
                    creator_map().find(key);
                if (iter == creator_map().end())
                    return result_type();
                return iter->second();
            }

            template <
                typename Arg1
            >
            static result_type create(
                key_type const & key, 
                Arg1 & arg1)
            {
                typename creator_map_type::const_iterator iter = 
                    creator_map().find(key);
                if (iter == creator_map().end())
                    return result_type();
                return iter->second(arg1);
            }

            template <
                typename Arg1, 
                typename Arg2
            >
            static result_type create(
                key_type const & key, 
                Arg1 & arg1, 
                Arg2 & arg2)
            {
                typename creator_map_type::const_iterator iter = 
                    creator_map().find(key);
                if (iter == creator_map().end())
                    return result_type();
                return iter->second(arg1, arg2);
            }

            template <
                typename Arg1, 
                typename Arg2, 
                typename Arg3
            >
            static result_type create(
                key_type const & key, 
                Arg1 & arg1, 
                Arg2 & arg2, 
                Arg3 & arg3)
            {
                typename creator_map_type::const_iterator iter = 
                    creator_map().find(key);
                if (iter == creator_map().end())
                    return result_type();
                return iter->second(arg1, arg2, arg3);
            }

            static void destroy(
                result_type & obj)
            {
                delete obj;
                obj = result_type();
            }

        protected:
            static creator_map_type & creator_map()
            {
                static creator_map_type smap;
                return smap;
            }
        };

    } // namespace commom
} // namespace ppbox

#ifdef PPBOX_ENABLE_REGISTER_CLASS
#define PPBOX_REGISTER_CLASS_NAME(cls) BOOST_PP_CAT(reg_class_, BOOST_PP_CAT(cls, __LINE__))
#  define PPBOX_REGISTER_CLASS(key, cls) \
    static ppbox::common::Call PPBOX_REGISTER_CLASS_NAME(cls)(cls::register_creator, key, ppbox::common::Creator<cls>())
#  define PPBOX_REGISTER_CLASS_FUNC(key, cls, func) \
    static ppbox::common::Call PPBOX_REGISTER_CLASS_NAME(cls)(cls::register_creator, key, func)
#  define PPBOX_REGISTER_CLASS_FACTORY(key, fact, cls) \
    static ppbox::common::Call PPBOX_REGISTER_CLASS_NAME(cls)(fact::register_creator, key, ppbox::common::Creator<cls>())
#  define PPBOX_REGISTER_CLASS_FACTORY_FUNC(key, fact, func) \
    static ppbox::common::Call PPBOX_REGISTER_CLASS_NAME(cls)(fact::register_creator, key, func)
#else
#  define PPBOX_REGISTER_CLASS(key, cls)
#  define PPBOX_REGISTER_CLASS_FUNC(key, cls, func)
#  define PPBOX_REGISTER_CLASS_FACTORY(key, fact, cls)
#  define PPBOX_REGISTER_CLASS_FACTORY_FUNC(key, fact, func)
#endif

#endif // _PPBOX_COMMON_CLASS_FACTORY_H_
