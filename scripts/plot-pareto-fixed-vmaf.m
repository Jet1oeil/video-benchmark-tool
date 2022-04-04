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
