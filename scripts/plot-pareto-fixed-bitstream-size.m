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

for limit = 0.6:0.2:1.4
	filename = sprintf("fixed-bitstream-size-%1.01fx.dat", limit);
	exps = importdata(filename, "\t");
	plot(exps.data(:,3), exps.data(:,1), "-o");
endfor
hold off;

ylim([65 100])
xlim([0 1.5])

title("Pareto front with fixed bitstream size")
xlabel("Encoding time (ratio)")
ylabel("VMAF score")
legend(
%  "0.5 times original size",
 "0.6 times original size",
%  "0.7 times original size",
 "0.8 times original size",
%  "0.9 times original size",
 "1.0 times original size",
%  "1.1 times original size",
 "1.2 times original size",
%  "1.3 times original size",
 "1.4 times original size",
%  "1.5 times original size",
"location", "southeast"
)

saveas(1, "pareto-fixed-bitstream-size.png");
