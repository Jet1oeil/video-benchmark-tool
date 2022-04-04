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

hold on;
for vmaf_limit = 70:5:95
	filename = sprintf("fixed-vmaf-%02d.dat", vmaf_limit);
	exps = importdata(filename, "\t");
	plot(exps.data(:,2), exps.data(:,3), "-o");
endfor
hold off;

xlim([0 2])
ylim([0 1.5])

title("Pareto front with fixed vmaf")
xlabel("Bitstream size (ratio)")
ylabel("Encoding time (ratio)")
legend(
 "70 VMAF limit",
 "75 VMAF limit",
 "80 VMAF limit",
 "85 VMAF limit",
 "90 VMAF limit",
 "95 VMAF limit",
 "location", "northeast")

saveas(1, "pareto-fixed-vmaf.png");
