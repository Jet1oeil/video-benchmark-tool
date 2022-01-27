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
	for i = 1:length(exps)
		exp = exps{i};
		filename = sprintf("%s/fixed-vmaf-%d.dat", exp, j);
		res = importdata(filename, "\t");
		plot(res.data(:,2)/10^6, res.data(:,3)/10^3, "-o");
	endfor

	plot([0 ; 3], [10 ; 10], "k--");
	plot([1.3 ; 1.3], [0 ; 20], "k--");

	hold off;

	% xlim([0 5])
	ylim([0 20])

	t = sprintf("Pareto front comparaison with fixed vmaf at %d", j)
	title(t)
	xlabel("Bitstream size (Mo)")
	ylabel("Encoding time (seconds)")
	legend("i5-4460","i7-6700","i7-12700K","i9-9900")

	filename = sprintf("pareto-comparaison-fixed-vmaf-%d.png", j)
	saveas(1, filename);
endfor
