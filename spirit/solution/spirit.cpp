#include <boost/spirit/include/karma.hpp>
// #define BOOST_SPIRIT_DEBUG
#include <boost/spirit/include/qi.hpp>

#define CATCH_CONFIG_RUNNER
#include <catch.hpp>
#include <clara.hpp>

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
typedef std::string StringValType;
typedef int IntValType;
typedef boost::variant<StringValType, IntValType> ValType;
typedef boost::fusion::vector<KeyType, ValType> KeyValType;
struct SectionType;
typedef std::vector<boost::variant<KeyValType, boost::recursive_wrapper<SectionType> > >
ContentType;
struct SectionType
{
  KeyType name;
  ContentType content;
};
typedef std::vector<SectionType> SectionsType;
}
}
}

BOOST_FUSION_ADAPT_STRUCT
(
  workshop::boost_spirit::data::SectionType,
  (workshop::boost_spirit::data::KeyType, name)
  (workshop::boost_spirit::data::ContentType, content)
)

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
  : public boost::spirit::qi::grammar<InIter, data::SectionsType(), Skipper<InIter> >
{
public:
  Parser()
    : Parser::base_type(sections, "Parser")
  {
    using namespace boost::spirit::qi;

    key = alnum >> *(alnum | char_("_-"));
    stringVal = lit("'") >> lexeme[*(char_ - lit("'"))] >> lit("'");
    intVal = int_;
    val = intVal | stringVal;
    keyVal = key >> lit('=') >> val;
    sectionBegin = key >> lit('{');
    sectionEnd = lit('}');
    content = *(keyVal | section);
    section = sectionBegin >> content >> sectionEnd;
    sections = +section;

    DEBUG_PARSER_RULE(key);
    DEBUG_PARSER_RULE(stringVal);
    DEBUG_PARSER_RULE(intVal);
    DEBUG_PARSER_RULE(val);
    DEBUG_PARSER_RULE(keyVal);
    DEBUG_PARSER_RULE(sectionBegin);
    DEBUG_PARSER_RULE(sectionEnd);
    DEBUG_PARSER_RULE(content);
    DEBUG_PARSER_RULE(section);
    DEBUG_PARSER_RULE(sections);
  }

  boost::spirit::qi::rule<InIter, data::KeyType(), Skipper<InIter> > key;
  boost::spirit::qi::rule<InIter, data::StringValType(), Skipper<InIter> >
  stringVal;
  boost::spirit::qi::rule<InIter, data::IntValType(), Skipper<InIter> > intVal;
  boost::spirit::qi::rule<InIter, data::ValType(), Skipper<InIter> > val;
  boost::spirit::qi::rule<InIter, data::KeyValType(), Skipper<InIter> > keyVal;
  boost::spirit::qi::rule<InIter, data::KeyType(), Skipper<InIter> > sectionBegin;
  boost::spirit::qi::rule<InIter, void(), Skipper<InIter> > sectionEnd;
  boost::spirit::qi::rule<InIter, data::ContentType(), Skipper<InIter> > content;
  boost::spirit::qi::rule<InIter, data::SectionType(), Skipper<InIter> > section;
  boost::spirit::qi::rule<InIter, data::SectionsType(), Skipper<InIter> >
  sections;
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
  SECTION("upper and lower case letters")
  {
    expected = "AbCd";
  }
  SECTION("numbers")
  {
    expected = "1234";
  }
  SECTION("_")
  {
    expected = "can_contain_underscore";
  }
  SECTION("-")
  {
    expected = "can-contain-minus";
  }
  data::KeyType actual;
  CHECK(parse(expected, parser::Type().key, actual));
  CHECK(expected == actual);
}

TEST_CASE("match string values", "[parser][stringVal]")
{
  data::StringValType expected;
  SECTION("letter and numbers")
  {
    expected = "Abcd1234";
  }
  SECTION("punctuations")
  {
    expected = "![](){}();";
  }
  SECTION("whitespace")
  {
    expected = "a  b  c ";
  }
  data::StringValType quoted = data::StringValType("'") + expected + "'";
  data::StringValType actual;
  CHECK(parse(quoted, parser::Type().stringVal, actual));
  CHECK(expected == actual);
}

TEST_CASE("match int values", "[parser][intVal]")
{
  data::IntValType expected = 1234;
  data::IntValType actual;
  CHECK(parse("1234", parser::Type().intVal, actual));
  CHECK(expected == actual);
}

TEST_CASE("value can be quoted string or int", "[parser][val]")
{
  SECTION("int value")
  {
    data::ValType expected = 1234;
    data::ValType actual;
    CHECK(parse("1234", parser::Type().val, actual));
    CHECK(expected == actual);
  }
  SECTION("quoted string")
  {
    data::ValType expected = "1234";
    data::ValType actual;
    CHECK(parse("'1234'", parser::Type().val, actual));
    CHECK(expected == actual);
  }
}

TEST_CASE("key value pair can be parsed", "[parser][keyVal]")
{
  SECTION("int value")
  {
    data::KeyValType expected("key_1", 1234);
    data::KeyValType actual;
    CHECK(parse("key_1 = 1234", parser::Type().keyVal, actual));
    CHECK(expected == actual);
  }
  SECTION("quoted string")
  {
    data::KeyValType expected("key_1", "1234");
    data::KeyValType actual;
    CHECK(parse("key_1 = '1234'", parser::Type().keyVal, actual));
    CHECK(expected == actual);
  }
}

TEST_CASE("key value pairs can be parsed", "[parser][content]")
{
  data::KeyValType val1("key_1", 1234);
  data::KeyValType val2("key_2", "1234");
  data::ContentType expected;
  expected.push_back(val1);
  expected.push_back(val2);
  data::ContentType actual;
  CHECK(parse("key_1 = 1234\n"
              "key_2 = '1234'", parser::Type().content, actual));
  CHECK(expected.size() == actual.size());
  REQUIRE(actual.size() == 2);
  CHECK(boost::get<data::KeyValType>(expected[0]) == boost::get<data::KeyValType>
        (actual[0]));
  CHECK(boost::get<data::KeyValType>(expected[1]) == boost::get<data::KeyValType>
        (actual[1]));
}

TEST_CASE("empty section can be parsed", "[parser][section]")
{
  data::ContentType content;
  data::SectionType expected;
  expected.name = "section";
  expected.content = content;
  data::SectionType actual;
  CHECK(parse("section {\n"
              "}", parser::Type().section, actual));
  CHECK(expected.name == actual.name);
  CHECK(expected.content.size() == 0);
}

TEST_CASE("section with key value pairs can be parsed", "[parser][section]")
{
  data::KeyValType val1("key_1", 1234);
  data::KeyValType val2("key_2", "1234");
  data::ContentType content;
  content.push_back(val1);
  content.push_back(val2);
  data::SectionType expected;
  expected.name = "section";
  expected.content = content;
  data::SectionType actual;
  CHECK(parse("section {\n"
              "key_1 = 1234\n"
              "key_2 = '1234'\n"
              "}", parser::Type().section, actual));
  CHECK(expected.name == actual.name);
}

TEST_CASE("sections can contain key-values and nested sections",
          "[parser][sections]")
{
  std::string const data =
    "section_1 {\n"
    "value_1 = 1\n"
    "value_2  = '2'\n"
    "section_1_1 {\n"
    "value_1_1 = 11\n"
    "value_1_2 = '12'\n"
    "}\n"
    "}";
  data::SectionsType actual;
  CHECK(parse(data, parser::Type().sections, actual));
  data::SectionsType const expected;
  CHECK(actual.size() == 1);
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

int main(int argc, char* argv[])
{
  int result = Catch::Session().run(argc, argv);
  if(result) return result;

  try
  {
    using namespace workshop::boost_spirit::app;

    ConfigData configData{};

    auto cmdLine
        = clara::Opt(configData.showHelp, "help")
        ["-h"]["--help"]
        ("display usage information")
        | clara::Opt(configData.showHelp, "path to file to be converted")
        ["-i"]["--input-file"]
        ("if the extension is .ini a input-file.json is created\n"
         "if the extension is .json a input-file.ini is created")
        ;
    auto parseResult = cmdLine.parse(clara::Args{argc, argv});
    if(!parseResult)
    {
        std::cout << "\ncould not parse command line\n"
                  << parseResult.errorMessage() << '\n';
    }

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
        std::cout << '\n' << cmdLine << '\n';
      }
    }
    else if(configData.showHelp)
    {
      std::cout << '\n' << cmdLine << '\n';
    }
    else
    {
      std::cout << '\n' << cmdLine << '\n';
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
