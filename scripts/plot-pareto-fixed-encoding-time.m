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
for limit = 0.2:0.2:1.2
	filename = sprintf("fixed-encoding-time-%1.01fx.dat", limit);
	exps = importdata(filename, "\t");
	plot(exps.data(:,2), exps.data(:,1), "-o");
endfor
hold off;

xlim([0 2])
ylim([70 100])

title("Pareto front with fixed encoding time")
xlabel("Bitstream size (ratio)")
ylabel("VMAF score")
legend(
 "0.2 times original time",
 "0.4 times original time",
 "0.6 times original time",
 "0.8 times original time",
 "1.0 times original time",
 "1.2 times original time",
"location", "southeast"
)

saveas(1, "pareto-fixed-encoding-time.png");
