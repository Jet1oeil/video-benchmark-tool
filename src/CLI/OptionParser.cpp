/* Copyright (c) 2022 Jet1oeil
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

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
