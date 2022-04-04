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

clf;

exps = {
	"pareto-cpu-i5-4460";
	"pareto-cpu-i7-6700";
	"pareto-cpu-i7-12700K";
	"pareto-cpu-i9-9900";
};

mega = 1024 * 1024;
for limit = 0.7*mega:0.2*mega:1.9*mega
	clf;
	hold on;
	for i = 1:length(exps)
		exp = exps{i};
		filename = sprintf("%s/fixed-bitstream-size-%07d.dat", exp, round(limit));
		res = importdata(filename, "\t");
		plot(res.data(:,3)/10^3, res.data(:,1), "-o");
	endfor

	plot([10 ; 10], [70 ; 100], "k--");

	hold off;

	xlim([0 15])
	ylim([70 100])

	t = sprintf("Pareto front comparaison with fixed bitstream size at %2.01f Mo", limit / 2^20);
	title(t)
	xlabel("Encoding time (seconds)")
	ylabel("VMAF score")
	legend("i5-4460","i7-6700","i7-12700K","i9-9900", "location", "southeast")

	filename = sprintf("pareto-comparaison-bitstream-size-%07d.png", round(limit));
	saveas(1, filename);
endfor
