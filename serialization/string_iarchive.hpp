#pragma once

#include <boost/archive/basic_text_iprimitive.hpp>
#include <boost/archive/basic_text_iarchive.hpp>

namespace xzr
{
  namespace archive
  {
    template<class Archive>
    class string_iarchive_impl
      : public boost::archive::basic_text_iprimitive<std::istream>
      , public boost::archive::basic_text_iarchive<Archive>
    {
    protected:
      friend class boost::archive::load_access;
    public:
      string_iarchive_impl(std::istream& str, unsigned int flags = 0)
	: boost::archive::basic_text_iprimitive<std::istream>(str, true)
	, boost::archive::basic_text_iarchive<Archive>(flags)
      {
      }

      void load_start(const char* name)
      {
      }
      void load_end(const char* name)
      {
      }

      template<class T>
      void load(T& t)
      {
	basic_text_iprimitive<std::istream>::load(t);
      }

      void load(bool& t)
      {
	basic_text_iprimitive<std::istream>::load(t);
      }
      void load(char& t)
      {
	basic_text_iprimitive<std::istream>::load(t);
      }
      void load(signed char& t)
      {
	basic_text_iprimitive<std::istream>::load(t);
      }
      void load(unsigned char& t)
      {
	basic_text_iprimitive<std::istream>::load(t);
      }
      void load(wchar_t& t)
      {
	basic_text_iprimitive<std::istream>::load(t);
      }
      void load(short& t)
      {
	basic_text_iprimitive<std::istream>::load(t);
      }
      void load(unsigned short& t)
      {
	basic_text_iprimitive<std::istream>::load(t);
      }
      void load(int& t)
      {
	basic_text_iprimitive<std::istream>::load(t);
      }
      void load(unsigned int& t)
      {
	basic_text_iprimitive<std::istream>::load(t);
      }
      void load(long& t)
      {
	basic_text_iprimitive<std::istream>::load(t);
      }
      void load(unsigned long& t)
      {
	basic_text_iprimitive<std::istream>::load(t);
      }
      void load(boost::long_long_type& t)
      {
	basic_text_iprimitive<std::istream>::load(t);
      }
      void load(boost::ulong_long_type& t)
      {
	basic_text_iprimitive<std::istream>::load(t);
      }
      void load(float& t)
      {
	basic_text_iprimitive<std::istream>::load(t);
      }
      void load(double& t)
      {
	basic_text_iprimitive<std::istream>::load(t);
      }
      void load(std::string& t)
      {
	basic_text_iprimitive<std::istream>::load(t);
      }
      void load(std::wstring& t)
      {
	basic_text_iprimitive<std::istream>::load(t);
      }
      
      void load(boost::archive::version_type &t)
      {
	is >> t;
      }
      void load(boost::archive::object_id_type &t)
      {
	is >> t;
      }
      void load(boost::archive::class_id_type &t)
      {
	is >> t;
      }
      void load(boost::archive::class_id_optional_type &t)
      {
	is >> t;
      }
      void load(boost::archive::class_name_type &t)
      {
	is >> t;
      }
      void load(boost::archive::tracking_type &t)
      {
	is >> t;
      }
    };
  
    class string_iarchive
      : public string_iarchive_impl<string_iarchive>
    {
    public:
      string_iarchive(std::istream& str)
	: string_iarchive_impl<string_iarchive>(str)
	, m_stream(str)
      {}
    private:
      std::istream& m_stream;
    };
  }
}

#include <boost/archive/impl/basic_text_iarchive.ipp>
