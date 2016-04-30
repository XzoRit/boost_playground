#pragma once

#include <boost/archive/basic_binary_oprimitive.hpp>
#include <boost/archive/basic_binary_oarchive.hpp>
#include <boost/archive/basic_archive.hpp>

namespace xzr
{
  namespace archive
  {
    template<class Archive>
    class bin_oarchive_impl
      : public boost::archive::basic_binary_oprimitive<Archive, char, std::char_traits<char> >
      , public boost::archive::basic_binary_oarchive<Archive>
    {
    protected:
      friend class boost::archive::save_access;
    public:
      typedef boost::archive::basic_binary_oprimitive<Archive, char, std::char_traits<char> > oprimitive;
      bin_oarchive_impl(std::ostream& str, unsigned int flags)
        : oprimitive(*str.rdbuf(), flags)
        , boost::archive::basic_binary_oarchive<Archive>(flags)
      {
      }

      void save_start(const char* name)
      {
        oprimitive::save(name);
      }
      
      void save_end(const char* name)
      {
        oprimitive::save(name);
      }
      
      template<class T>
      void save(const T& t)
      {
        oprimitive::save(t);
      }

      void save(const bool t)
      {
        oprimitive::save(t);
      }
      void save(const char t)
      {
        oprimitive::save(t);
      }
      void save(const signed char t)
      {
        oprimitive::save(t);
      }
      void save(const unsigned char t)
      {
        oprimitive::save(t);
      }
      void save(const wchar_t t)
      {
        oprimitive::save(t);
      }
      void save(const short t)
      {
        oprimitive::save(t);
      }
      void save(const unsigned short t)
      {
        oprimitive::save(t);
      }
      void save(const int t)
      {
        oprimitive::save(t);
      }
      void save(const unsigned int t)
      {
        oprimitive::save(t);
      }
      void save(const long t)
      {
        oprimitive::save(t);
      }
      void save(const unsigned long t)
      {
        oprimitive::save(t);
      }
      void save(const boost::long_long_type t)
      {
        oprimitive::save(t);
      }
      void save(const boost::ulong_long_type t)
      {
        oprimitive::save(t);
      }
      void save(const float t)
      {
        oprimitive::save(t);
      }
      void save(const double t)
      {
        oprimitive::save(t);
      }
      void save(const std::string & t)
      {
        oprimitive::save(t);
      }
      void save(const std::wstring & t)
      {
        oprimitive::save(t);
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
      }

      void save(const boost::archive::version_type t)
      {
        //oprimitive::save(t);
      }
      void save(const boost::archive::object_id_type t)
      {
        oprimitive::save(t);
      }
      void save(const boost::archive::object_reference_type t)
      {
        oprimitive::save(t);
      }
      void save(const boost::archive::class_id_type t)
      {
        oprimitive::save(t);
      }
      void save(const boost::archive::class_id_optional_type t)
      {
        oprimitive::save(t);
      }
      void save(const boost::archive::class_id_reference_type t)
      {
        oprimitive::save(t);
      }
      void save(const boost::archive::class_name_type & t)
      {
        oprimitive::save(t);
      }
      void save(const boost::archive::tracking_type t)
      {
        oprimitive::save(t);
      }

      template<class T>
      void save(const boost::serialization::nvp< T > & t)
      {
        oprimitive::save(t.name());
        oprimitive::save(t.value());
      }
    };
  
    class bin_oarchive
      : public bin_oarchive_impl<bin_oarchive>
    {
    public:
      bin_oarchive(std::ostream& str, unsigned int flags = 0)
        : bin_oarchive_impl<bin_oarchive>(str, flags)
      {}
    };
  }
}

#include <boost/archive/impl/basic_binary_oprimitive.ipp>
