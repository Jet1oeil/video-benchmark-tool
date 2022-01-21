clf;

hold on;
for vmaf_limit = 70:5:95
	filename = sprintf("fixed-vmaf-%02d.dat", vmaf_limit);
	exps = importdata(filename, "\t");
	plot(exps.data(:,2)/10^6, exps.data(:,3)/10^3);
endfor
hold off;

xlim([0 5])
ylim([0 15])

title("Pareto front with fixed vmaf")
xlabel("Bitstream size (Mo)")
ylabel("Encoding time (seconds)")
legend("70 VMAF limit","75 VMAF limit","80 VMAF limit","85 VMAF limit","90 VMAF limit","95 VMAF limit")

saveas(1, "pareto-fixed-vmaf.png");
