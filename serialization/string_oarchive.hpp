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

      template<class T>
      void save(const T& t){
	this->newline();
	this->newtoken();
	basic_text_oprimitive<std::ostream>::save(t);
      }
    };
  
    class string_oarchive
      : public string_oarchive_impl<string_oarchive>
    {
    public:
      string_oarchive(std::ostream& str, unsigned int flags = 0)
	: string_oarchive_impl<string_oarchive>(str, flags)
	, m_stream(str)
      {}
    private:
      std::ostream& m_stream;
    };
  }
}

#include <boost/archive/impl/basic_text_oarchive.ipp>
