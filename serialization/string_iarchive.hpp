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
