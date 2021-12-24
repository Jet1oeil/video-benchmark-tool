#include "OptionParser.h"

#include <algorithm>
#include <stdexcept>
#include <vector>

namespace cli {
	namespace {
		inline std::string getPrefixedName(const std::string& name)
		{
			if (name.size() == 1) {
				return "-" + name;
			}

			return "--" + name;
		}
	}

	Option::Option(const std::string& optionName, const std::string& defaultValue)
	: name(getPrefixedName(optionName))
	, value(defaultValue)
	, needsParameter(!defaultValue.empty())
	, isSet(false)
	{

	}

	bool Option::operator==(const std::string& arg) const {
		return arg == name || getPrefixedName(arg) == name;
	}

	bool Option::operator!=(const std::string& arg) const {
		return !(*this == arg);
	}

	Option::operator bool() const
	{
		return isSet;
	}

	void OptionParser::registerOption(const std::string& name, const std::string& defaultValue)
	{
		m_options.emplace_back(Option(name, defaultValue));
	}

	void OptionParser::parse(int argc, char* argv[])
	{
		std::vector<char*> args(argv, argv + argc);

		for (std::size_t i = 1; i < args.size(); ++i) {
			auto& option = (*this)[args[i]];

			if (option.needsParameter && (i + 1) < args.size()) {
				option.value = args[++i];
			}
			option.isSet = true;
		}
	}

	const Option& OptionParser::operator[] (const std::string& name) const
	{
		auto it = findOption(name);
		if (it == m_options.end()) {
			throw std::runtime_error("Option '" + name + "' unkown");
		}

		return *it;
	}

	Option& OptionParser::operator[] (const std::string& name)
	{
		auto it = findOption(name);
		if (it == m_options.end()) {
			throw std::runtime_error("Option '" + name + "' unkown");
		}

		return *it;
	}

	std::vector<Option>::iterator OptionParser::findOption(const std::string& name) {
		return std::find(m_options.begin(), m_options.end(), name);
	}

	std::vector<Option>::const_iterator OptionParser::findOption(const std::string& name) const {
		return std::find(m_options.begin(), m_options.end(), name);
	}
}
