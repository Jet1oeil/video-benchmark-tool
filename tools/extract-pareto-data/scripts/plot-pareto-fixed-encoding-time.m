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
