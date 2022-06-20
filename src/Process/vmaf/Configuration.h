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

#ifndef VMAF_CONFIGURATION_H_
#define VMAF_CONFIGURATION_H_

#include <vector>

#include "Types/Codec.h"

namespace vmaf {
	struct Configuration : public types::EncoderParameters {
		bool operator<(const Configuration& other) const;
	};

	bool updateConfigurationListFile(const std::vector<Configuration> &listConfigurations);
	bool writeConfigurationList(const std::string& videoFile, const std::vector<Configuration> &listConfigurations);
	bool readConfigurationList(std::string& szVideoFileName, std::vector<Configuration> &listConfigurations);
}

#endif // VMAF_CONFIGURATION_H_
