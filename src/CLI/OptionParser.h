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
