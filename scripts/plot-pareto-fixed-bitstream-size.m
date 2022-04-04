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
