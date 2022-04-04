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

#ifndef CLI_OPTION_PARSER_H_
#define CLI_OPTION_PARSER_H_

#include <string>
#include <vector>

namespace cli {
	struct Option {
		Option(const std::string& optionName, const std::string& defaultValue = std::string());

		bool operator==(const std::string& arg) const;
		bool operator!=(const std::string& arg) const;

		operator bool() const;

		std::string name;
		std::string value;
		bool needsParameter;
		bool isSet;
	};

	class OptionParser {
	public:
		void registerOption(const std::string& name, const std::string& defaultValue = std::string());
		void parse(int argc, char* argv[]);

		const Option& operator[] (const std::string& name) const;
		Option& operator[] (const std::string& name);

	private:
		std::vector<Option>::iterator findOption(const std::string& name);
		std::vector<Option>::const_iterator findOption(const std::string& name) const;

	private:
		std::vector<Option> m_options;
	};
}

#endif // CLI_OPTION_PARSER_H_
