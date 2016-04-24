#pragma once

#include <boost/archive/basic_text_oprimitive.hpp>
#include <boost/archive/basic_text_oarchive.hpp>

namespace xzr
{
  namespace archive
  {
    template<class Archive>
    class string_oarchive_impl
      : public boost::archive::basic_text_oprimitive<std::ostream>
      , public boost::archive::basic_text_oarchive<Archive>
    {
    protected:
      friend class boost::archive::save_access;
    public:
      string_oarchive_impl(std::ostream& str, unsigned int flags)
        : boost::archive::basic_text_oprimitive<std::ostream>(str, flags)
        , boost::archive::basic_text_oarchive<Archive>(flags)
      {
      }

      void save_start(const char* name)
      {
        basic_text_oprimitive<std::ostream>::save(name);
      }
      
      void save_end(const char* name)
      {
        basic_text_oprimitive<std::ostream>::save(name);
      }
      
      template<class T>
      void save(const T& t)
      {
        basic_text_oprimitive<std::ostream>::save(t);
      }

      void save(const bool t)
      {
        basic_text_oprimitive<std::ostream>::save(t);
      }
      void save(const char t)
      {
        basic_text_oprimitive<std::ostream>::save(t);
      }
      void save(const signed char t)
      {
        basic_text_oprimitive<std::ostream>::save(t);
      }
      void save(const unsigned char t)
      {
        basic_text_oprimitive<std::ostream>::save(t);
      }
      void save(const wchar_t t)
      {
        basic_text_oprimitive<std::ostream>::save(t);
      }
      void save(const short t)
      {
        basic_text_oprimitive<std::ostream>::save(t);
      }
      void save(const unsigned short t)
      {
        basic_text_oprimitive<std::ostream>::save(t);
      }
      void save(const int t)
      {
        basic_text_oprimitive<std::ostream>::save(t);
      }
      void save(const unsigned int t)
      {
        basic_text_oprimitive<std::ostream>::save(t);
      }
      void save(const long t)
      {
        basic_text_oprimitive<std::ostream>::save(t);
      }
      void save(const unsigned long t)
      {
        basic_text_oprimitive<std::ostream>::save(t);
      }
      void save(const boost::long_long_type t)
      {
        basic_text_oprimitive<std::ostream>::save(t);
      }
      void save(const boost::ulong_long_type t)
      {
        basic_text_oprimitive<std::ostream>::save(t);
      }
      void save(const float t)
      {
        basic_text_oprimitive<std::ostream>::save(t);
      }
      void save(const double t)
      {
        basic_text_oprimitive<std::ostream>::save(t);
      }
      void save(const std::string & t)
      {
        basic_text_oprimitive<std::ostream>::save(t);
      }
      void save(const std::wstring & t)
      {
        basic_text_oprimitive<std::ostream>::save(t);
      }
      void save_null_pointer()
      {
      }
      void register_basic_serializer(const boost::archive::detail::basic_oserializer & bos)
      {
      }
      boost::archive::detail::helper_collection & get_helper_collection()
      {
      }
      void end_preamble()
      {
	os << ' ';
      }

      void save(const boost::archive::version_type t)
      {
        basic_text_oprimitive<std::ostream>::save(t);
      }
      void save(const boost::archive::object_id_type t)
      {
        basic_text_oprimitive<std::ostream>::save(t);
      }
      void save(const boost::archive::object_reference_type t)
      {
        basic_text_oprimitive<std::ostream>::save(t);
      }
      void save(const boost::archive::class_id_type t)
      {
        basic_text_oprimitive<std::ostream>::save(t);
      }
      void save(const boost::archive::class_id_optional_type t)
      {
        basic_text_oprimitive<std::ostream>::save(t);
      }
      void save(const boost::archive::class_id_reference_type t)
      {
        basic_text_oprimitive<std::ostream>::save(t);
      }
      void save(const boost::archive::class_name_type & t)
      {
        basic_text_oprimitive<std::ostream>::save(t);
      }
      void save(const boost::archive::tracking_type t)
      {
        basic_text_oprimitive<std::ostream>::save(t);
      }

      template<class T>
      void save(const boost::serialization::nvp< T > & t)
      {
        basic_text_oprimitive<std::ostream>::save(t.name());
        basic_text_oprimitive<std::ostream>::save(t.value());
      }
    };
  
    class string_oarchive
      : public string_oarchive_impl<string_oarchive>
    {
    public:
      string_oarchive(std::ostream& str, unsigned int flags = 0)
        : string_oarchive_impl<string_oarchive>(str, flags)
      {}
    };
  }
}

#include <boost/archive/impl/basic_text_oarchive.ipp>
