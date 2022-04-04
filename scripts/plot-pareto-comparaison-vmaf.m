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

for j = 75:5:95
	clf;
	hold on;
	x_max = 0;
	for i = 1:length(exps)
		exp = exps{i};
		filename = sprintf("%s/fixed-vmaf-%d.dat", exp, j);
		res = importdata(filename, "\t");
		plot(res.data(:,2)/10^6, res.data(:,3)/10^3, "-o");
		local_max = max(res.data(:,2)/10^6);
		if x_max < local_max
			x_max = local_max;
		endif
	endfor

	plot([0 ; x_max], [10 ; 10], "k--");
	plot([1.3 ; 1.3], [0 ; 20], "k--");

	hold off;

	% xlim([0 5])
	ylim([0 20])

	t = sprintf("Pareto front comparaison with fixed vmaf at %d", j);
	title(t)
	xlabel("Bitstream size (Mo)")
	ylabel("Encoding time (seconds)")
	legend("i5-4460","i7-6700","i7-12700K","i9-9900")

	filename = sprintf("pareto-comparaison-vmaf-%d.png", j);
	saveas(1, filename);
endfor
