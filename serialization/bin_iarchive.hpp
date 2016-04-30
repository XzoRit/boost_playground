#pragma once

#include <boost/archive/basic_binary_iprimitive.hpp>
#include <boost/archive/basic_binary_iarchive.hpp>

namespace xzr
{
  namespace archive
  {
    template<class Archive>
    class bin_iarchive_impl
      : public boost::archive::basic_binary_iprimitive<Archive, char, std::char_traits<char> >
      , public boost::archive::basic_binary_iarchive<Archive>
    {
    protected:
      friend class boost::archive::load_access;
    public:
      typedef boost::archive::basic_binary_iprimitive<Archive, char, std::char_traits<char> > iprimitive;
      bin_iarchive_impl(std::istream& str, unsigned int flags = 0)
	: iprimitive(*str.rdbuf(), true)
	, boost::archive::basic_binary_iarchive<Archive>(flags)
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
	iprimitive::load(t);
      }

      void load(bool& t)
      {
	iprimitive::load(t);
      }
      void load(char& t)
      {
	iprimitive::load(t);
      }
      void load(signed char& t)
      {
	iprimitive::load(t);
      }
      void load(unsigned char& t)
      {
	iprimitive::load(t);
      }
      void load(wchar_t& t)
      {
	iprimitive::load(t);
      }
      void load(short& t)
      {
	iprimitive::load(t);
      }
      void load(unsigned short& t)
      {
	iprimitive::load(t);
      }
      void load(int& t)
      {
	iprimitive::load(t);
      }
      void load(unsigned int& t)
      {
	iprimitive::load(t);
      }
      void load(long& t)
      {
	iprimitive::load(t);
      }
      void load(unsigned long& t)
      {
	iprimitive::load(t);
      }
      void load(boost::long_long_type& t)
      {
	iprimitive::load(t);
      }
      void load(boost::ulong_long_type& t)
      {
	iprimitive::load(t);
      }
      void load(float& t)
      {
	iprimitive::load(t);
      }
      void load(double& t)
      {
	iprimitive::load(t);
      }
      void load(std::string& t)
      {
	iprimitive::load(t);
      }
      void load(std::wstring& t)
      {
	iprimitive::load(t);
      }
      
      void load(boost::archive::version_type &t)
      {
	//iprimitive::load(t);
      }
      void load(boost::archive::object_id_type &t)
      {
	iprimitive::load(t);
      }
      void load(boost::archive::class_id_type &t)
      {
	iprimitive::load(t);
      }
      void load(boost::archive::class_id_optional_type &t)
      {
	iprimitive::load(t);
      }
      void load(boost::archive::class_name_type &t)
      {
	iprimitive::load(t);
      }
      void load(boost::archive::tracking_type &t)
      {
	iprimitive::load(t);
      }
    };
  
    class bin_iarchive
      : public bin_iarchive_impl<bin_iarchive>
    {
    public:
      bin_iarchive(std::istream& str)
	: bin_iarchive_impl<bin_iarchive>(str)
	, m_stream(str)
      {}
    private:
      std::istream& m_stream;
    };
  }
}

#include <boost/archive/impl/basic_binary_iarchive.ipp>
#include <boost/archive/impl/basic_binary_iprimitive.ipp>
