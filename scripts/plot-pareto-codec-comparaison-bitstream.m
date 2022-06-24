# Copyright (c) 2022 Jet1oeil
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.


codec_list = {
	"openh264-baseline";
	"openh264-high";
	"x264-baseline";
	"x264-main";
	"x264-high";
	"x265-main";
};

for limit = 0.010:0.002:0.050
	clf;

	hold on;
	for it = 1:length(codec_list)
		codec = codec_list{it};

		filename = sprintf("%s-fixed-bitstream-size-%0.3fx.dat", codec, limit);

		if (exist(filename, "file") == 0)
			printf("%s doesn\'t exists\n", filename);
			continue
		endif

		res = importdata(filename, "\t");
		plot(res.data(:,3), res.data(:,1), "-o", 'DisplayName', codec);
	endfor
	% plot([1 1], [50 100], '--k', 'DisplayName', "Original video time");

	hold off;

	xlim([0 0.5])
	ylim([70 100])

	t = sprintf("Comparaison between codecs with fixed compression ratio at %0.3f", limit);
	title(t);
	xlabel("Encoding time (ratio)")
	ylabel("VMAF score")
	legend('show');

	filename = sprintf("pareto-codec-comparaison-bitstream-size-%0.3f.png", limit);
	saveas(1, filename);
endfor
