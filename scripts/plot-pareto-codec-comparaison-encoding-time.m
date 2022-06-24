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

for limit = 0.2:0.2:1.6
	clf;

	hold on;
	for it = 1:length(codec_list)
		codec = codec_list{it};

		filename = sprintf("%s-fixed-encoding-time-%0.1fx.dat", codec, limit);

		if (exist(filename, "file") == 0)
			printf("%s doesn\'t exists\n", filename);
			continue
		endif

		res = importdata(filename, "\t");
		plot(res.data(:,2), res.data(:,1), "-o", 'DisplayName', codec);
	endfor
	plot([0.010 0.010], [70 100], '--k', 'DisplayName', "Original ratio (H.265)");

	hold off;

	xlim([0 0.2])
	ylim([70 100])

	t = sprintf("Comparaison between codecs with fixed encoding time ratio at %0.1f", limit);
	title(t);
	xlabel("Encoding time (ratio)");
	xlabel("Compression (ratio)");
	legend('show');

	filename = sprintf("pareto-codec-comparaison-encoding-time-%0.1f.png", limit);
	saveas(1, filename);
endfor
