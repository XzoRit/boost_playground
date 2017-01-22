#include <boost/spirit/include/karma.hpp>
// #define BOOST_SPIRIT_DEBUG
#include <boost/spirit/include/qi.hpp>

#define CATCH_CONFIG_RUNNER
#include <catch.hpp>

#include <string>
#include <fstream>
#include <iterator>

#if defined(BOOST_SPIRIT_DEBUG)
#define DEBUG_PARSER_RULE(r) r.name(#r); debug(r)
#else
#define DEBUG_PARSER_RULE(r) r.name(#r)
#endif


namespace workshop
{
namespace boost_spirit
{
namespace generating
{
template <typename OutIter>
class Generator : public boost::spirit::karma::grammar<OutIter, std::string()>
{
public:
  Generator()
    : Generator::base_type(key, "Generator")
  {
    using namespace boost::spirit::karma;

    key = +char_("a-zA-Z0-9");
  }

  boost::spirit::karma::rule<OutIter, std::string()> key;
};

namespace generator
{
typedef std::string OutType;
typedef std::back_insert_iterator<OutType> OutIter;
typedef Generator<OutIter> Type;
}

template<class Generator, class Attribute>
bool generate(const Generator& generator, Attribute& attr,
              generator::OutType& result);

template<class Generator, class Attribute>
inline bool generate(const Generator& generator, Attribute& attr,
                     generator::OutType& result)
{
  using boost::spirit::karma::generate;

  generator::OutType s;
  generator::OutIter outIter(s);
  const bool success = generate(outIter, generator, attr);
  result = s;

  return success;
}
}
}
}

namespace workshop
{
namespace boost_spirit
{
namespace generating
{
namespace test
{
TEST_CASE("generate string from key", "[generator][key]")
{
  const std::string expected  = "ThisIsAKey1";
  std::string actual;
  CHECK(generate(generator::Type().key, expected, actual));
  CHECK(expected == actual);
}
}
}
}
}

namespace workshop
{
namespace boost_spirit
{
namespace data
{
typedef std::string KeyType;
}
}
}

namespace workshop
{
namespace boost_spirit
{
namespace parsing
{
template<class InIter>
class Skipper : public boost::spirit::qi::grammar<InIter>
{
public:
  Skipper()
    : Skipper::base_type(skip, "Skipper")
  {
    using namespace boost::spirit::qi;

    skip = boost::spirit::ascii::space;
  }

  boost::spirit::qi::rule<InIter> skip;
};

template<class InIter>
class Parser
  : public boost::spirit::qi::grammar<InIter, data::KeyType()>
{
public:
  Parser()
    : Parser::base_type(key, "Parser")
  {
    using namespace boost::spirit::qi;

    key = *alnum;

    DEBUG_PARSER_RULE(key);
  }

  boost::spirit::qi::rule<InIter, data::KeyType()> key;
};

namespace parser
{
typedef std::string InType;
typedef InType::iterator InIter;
typedef Skipper<InIter> Skipper;
typedef Parser<InIter> Type;
}

template<class Pattern>
bool parse(parser::InType in, const Pattern& p);
template<class Pattern, class T>
bool parse(parser::InType in, const Pattern& p, T& attr);

template<class Pattern>
inline bool parse(parser::InType in, const Pattern& p)
{
  using boost::spirit::qi::phrase_parse;
  parser::InIter b = in.begin();
  parser::InIter e = in.end();
  return phrase_parse(b, e, p, parser::Skipper()) && (b == e);
}

template<class Pattern, class T>
inline bool parse(parser::InType in, const Pattern& p, T& attr)
{
  using boost::spirit::qi::phrase_parse;
  parser::InIter b = in.begin();
  parser::InIter e = in.end();
  return phrase_parse(b, e, p, parser::Skipper(), attr) && (b == e);
}
}
}
}

namespace workshop
{
namespace boost_spirit
{
namespace parsing
{
namespace test
{
TEST_CASE("match and extract valid key names", "[parser][key]")
{
  data::KeyType expected;
  data::KeyType actual;
  expected = "Abcd1234";
  CHECK(parse(expected, parser::Type().key, actual));
  CHECK(expected == actual);
}
}
}
}
}

namespace workshop
{
namespace boost_spirit
{
namespace app
{
struct ConfigData
{
public:
  ConfigData()
    : showHelp(false)
    , inputFile("")
  {}

  bool showHelp;
  std::string inputFile;
};
}
}
}

int main(int argc, char* const argv[])
{
  int result = Catch::Session().run(argc, argv);
  if(result) return result;

  try
  {
    using namespace workshop::boost_spirit::app;

    Catch::Clara::CommandLine<ConfigData> cmdLine;

    cmdLine["-h"]["--help"]
    .describe("display usage information")
    .bind(&ConfigData::showHelp);
    cmdLine["-i"]["--input-file"]
    .describe("path to file to be converted\n"
              "if the extension is .ini a input-file.json is created\n"
              "if the extension is .json a input-file.ini is created")
    .bind(&ConfigData::inputFile, "inputFile");

    cmdLine.setThrowOnUnrecognisedTokens(true);

    ConfigData configData;
    cmdLine.parseInto(Catch::Clara::argsToVector(argc, argv), configData);

    if(!configData.inputFile.empty())
    {
      const std::string inputFile = configData.inputFile;
      const std::string::size_type idxOfDot = inputFile.find_last_of(".");
      const std::string filename =
        std::string(inputFile.begin(),
                    inputFile.begin() + idxOfDot);
      const std::string fileExt =
        std::string(inputFile.begin() + idxOfDot,
                    inputFile.end());
      if(fileExt == ".ini")
      {
        const std::string outputFile = filename + ".json";
        // fromIniToJson(inputFile, outputFile);
      }
      else if(fileExt == ".json")
      {
        const std::string outputFile = filename + ".ini";
        // fromJsonToIni(inputFile, outputFile);
      }
      else
      {
        cmdLine.optUsage(std::cout);
      }
    }
    else if(configData.showHelp)
    {
      cmdLine.optUsage(std::cout);
    }
    else
    {
      cmdLine.optUsage(std::cout);
    }
  }
  catch(const std::exception& e)
  {
    std::cout << e.what();
    return 1;
  }
  catch(...)
  {
    std::cerr << "Unknown exception!\n";
    return 2;
  }
  return 0;
}
